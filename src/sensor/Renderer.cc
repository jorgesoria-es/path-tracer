#include "sensor/Renderer.hh"

#include "geometry/operaciones.hh"
#include "imaging/ToneMapping.hh"
#include <algorithm>
#include <utility>
#include <thread>
#include <atomic>
#include <vector>
#include <math.h>
#include <random>

/** Constructor */
Renderer::Renderer(const PinholeCamara& camara, const Escena& escena, int spp)
    : _camara(camara), _escena(escena), _spp(spp) {}

/** 
 * Gestionar que estrategia se usa para renderizar
 * 1. NONE          Sin iluminación, coloreado plano en base a emisión
 * 2. DIRECT        Iluminación directa
 * 3. INDIRECT      Iluminación indirecta
 */
void Renderer::render(Image& hdrOut, Image& ldrOut, RenderMode Mode) const {
    switch (Mode) {
        case RenderMode::NONE:
            renderNone(hdrOut, ldrOut); break;
        case RenderMode::DIRECT:
            renderDirect(hdrOut, ldrOut); break;
        case RenderMode::INDIRECT:
            renderIndirect(hdrOut, ldrOut); break;
        default:
            break;
    }
}

/** Maneja la iteración de píxeles. PixelFunc es una lamba (x, y) -> Pixel */
template<typename PixelFunc>
void Renderer::renderImage(Image& img, PixelFunc obtenerPixel) const {
    const int W = _camara.getW();
    const int H = _camara.getH();
    img = Image(W, H);

#if defined(USE_THREADS) && USE_THREADS == 0

    for(int y = 0; y < H; ++y) {
        Pixel* row = img.row(y);
        for(int x = 0; x < W; ++x) {
            row[x] = obtenerPixel(x, y);
        }
    }

#else

    struct Tile { int y0, y1, x0, x1; };
    const int TILE_SIZE = 16;

    std::vector<Tile> tiles;
    tiles.reserve(((H + TILE_SIZE - 1) / TILE_SIZE) * ((W + TILE_SIZE - 1) / TILE_SIZE));

    for(int y = 0; y < H; y += TILE_SIZE)
        for(int x = 0; x < W; x += TILE_SIZE)
            tiles.push_back({y, std::min(y + TILE_SIZE, H), x, std::min(x + TILE_SIZE, W)});

    std::atomic<size_t> nextTileId{0};
    const unsigned num_threads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> threads;

    auto worker = [&]() {
        while(true) {
            size_t id = nextTileId.fetch_add(1, std::memory_order_relaxed);
            if(id >= tiles.size()) break;
            const Tile& t = tiles[id];

            for(int y = t.y0; y < t.y1; ++y) {
                Pixel* row = img.row(y);
                for(int x = t.x0; x < t.x1; ++x) {
                    row[x] = obtenerPixel(x, y);
                }
            }
        }
    };

    for(unsigned i = 0; i < num_threads; ++i) threads.emplace_back(worker);
    for(auto& t : threads) t.join();

#endif
}

/** Implementaciones de render */

void Renderer::renderNone(Image& hdrOut, Image& ldrOut) const {
    const double invSpp = 1.0 / _spp;

    renderImage(hdrOut, [&](int x, int y) -> Pixel {
        Pixel acc;
        for(int i = 0; i < _spp; ++i) {
            Rayo r = _camara.generarRayo(x, y);

            Pixel Le; Punto hit; std::shared_ptr<Objeto> obj;
            if(_escena.intersectar(r, Le, hit, obj)) acc += Le;
        }
        return acc * invSpp;
    });

    ldrOut = gammaAndClamp(hdrOut, 2.2, 0.75);
}


void Renderer::renderDirect(Image& hdrOut, Image& ldrOut) const {
    const double invSpp = 1.0 / _spp;

    renderImage(hdrOut, [&](int x, int y) -> Pixel {
        Pixel acc;
        for(int i = 0; i < _spp; ++i) {
            Rayo r = _camara.generarRayo(x, y);

            Pixel Le; Punto hit; std::shared_ptr<Objeto> obj;
            if(_escena.intersectar(r, Le, hit, obj)) {
                acc += Le + this->contribucionDirecta(r.direccion, hit, obj);
            }
        }
        return acc * invSpp; 
    });

    ldrOut = gammaAndClamp(hdrOut, 2.2, 0.75);
}


void Renderer::renderIndirect(Image& hdrOut, Image& ldrOut) const {
    const double invSpp = 1.0 / _spp;

    renderImage(hdrOut, [&](int x, int y) -> Pixel {
        Pixel acc;
        for(int i = 0; i < _spp; ++i) {
            Rayo r = _camara.generarRayo(x, y);
            acc += contribucion(r);
        }
        return acc * invSpp;
    });

    ldrOut = gammaAndClamp(hdrOut, 2.2, 0.75);
}

/** Lógica trazado rayos - Path Tracer */

Pixel Renderer::contribucionDirecta(const Direccion& wo, const Punto& hit, std::shared_ptr<Objeto> obj) const {
    Pixel res;

    for(const auto& luz : _escena.getLucesPuntuales()) {
        Direccion wi = luz->posicion() - hit;
        double distLuz = wi.getModulo();
        Rayo rayoALuz(hit, wi);

        if(_escena.intersectar(rayoALuz, distLuz)) continue;

        Pixel Li = luz->potencia() / (distLuz * distLuz); // p / |c-x|²

        Pixel Fr = obj->kDifuso(hit) / M_PI;

        double Geometria = std::fabs(obj->normalAt(hit).dot(rayoALuz.direccion));

        res += Li * Fr * Geometria;
    }

    return res;
}

/** Versión recursiva
Pixel Renderer::contribucion(const Rayo& r) const {
    Pixel acc;

    Pixel Le; Punto hit; std::shared_ptr<Objeto> obj;
    if(_escena.intersectar(r, Le, hit, obj)) {
        acc += Le;
        acc += this->contribucionDirecta(r.direccion, hit, obj);

        Direccion ws; double rand = rand01();
        if(obj->crearSecundario(rand, r.direccion, hit, ws)) {
            Rayo rayoSecundario(hit, ws); // rayo secundario
            acc += obj->pathContribution(rand) * contribucion(rayoSecundario);
        }
    }
    return acc;
}
*/

Pixel Renderer::contribucion(const Rayo& r) const { // Iterativo: más rápido
    Pixel acc;
    Pixel throughput(1.0, 1.0, 1.0);

    Rayo rayo = r; bool seguir = true;
    while(seguir) {
        
        Pixel Le; Punto hit; std::shared_ptr<Objeto> obj;
        if(_escena.intersectar(rayo, Le, hit, obj)) {
            acc += throughput * Le;
            if(obj->kDifuso(hit).max() > 0.0) acc += throughput * this->contribucionDirecta(rayo.direccion, hit, obj);

            Direccion ws; double rand = rand01();
            if(obj->crearSecundario(rand, rayo.direccion, hit, ws)) {
                throughput = throughput * obj->pathContribution(rand, hit);
                rayo = Rayo(hit, ws);
            } else {
                seguir = false;
            }
        } else {
            seguir = false;
        }
    }
    return acc;
}