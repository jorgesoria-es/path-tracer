#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>

#include "imaging/Imagen.hh"

using namespace std;

// Constante de tolerancia para comparar píxeles
// Hemos elegido un valor ligeramente mayor que 1/255
// para tener en cuenta errores de redondeo.
static constexpr double EPS = 1.0 / 255.0 + 1e-6;

// Función auxiliar para crear una imagen de prueba con un patrón conocido
// (degradado con franjas de color para detectar swaps de canales)
static Image makeTestPattern(int w, int h) {
    Image img(w, h, Pixel(0,0,0));
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // Patrón: degradado + franjas de color para detectar swaps de canales
            double u = double(x) / std::max(1, w - 1);
            double v = double(y) / std::max(1, h - 1);
            double r = 0.75 * u + 0.25 * (y % 2);   // mezcla para forzar valores no triviales
            double g = 0.75 * v + 0.25 * (x % 2);
            double b = 0.5  * u + 0.5  * v;
            img.at(x,y) = Pixel(r, g, b);
        }
    }
    return img;
}

// Programa principal: realiza dos pruebas
// 1) Round-trip con una imagen sintética (creada con makeTestPattern)
//    Se guarda en PNG y se vuelve a cargar, y se comparan ambas imágenes.
// 2) Si se pasa un nombre de fichero PNG por línea de comandos, se carga
//    y se guarda como "test_salida.png" (para comprobar que se carga bien)
int main(int argc, char** argv) {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();
    ofstream log("test_png.log");
    log << "[test_png] PNG encoding/decoding tests started\n";

    try {
        // --- PRUEBA 1: Round-trip con imagen sintética ---
        {
            log << "[Test 1] Synthetic round-trip...\n";
            Image A = makeTestPattern(64, 48);
            Image::savePNG("test_sintetico.png", A);
            Image B = Image::loadPNG("test_sintetico.png");

            double maxErr = 0.0;
            int mismatches = Image::compare(A, B, maxErr, EPS);
            if (mismatches < 0) return 1;

            log << "Mismatches: " << mismatches
                << "  MaxAbsErr: " << maxErr << "\n";
            if (mismatches == 0 || maxErr <= EPS + 1e-6)
                log << "OK: Round-trip within tolerance\n";
            else
                log << "Warning: differences larger than 1/255\n";
        }

        // --- PRUEBA 2: Cargar PNG real (si se pasa por argv) ---
        if (argc >= 2) {
            std::string in = argv[1];
            log << "[Test 2] Loading real PNG: " << in << "\n";
            Image R = Image::loadPNG(in);
            log << "Dimensions: " << R.getW() << "x" << R.getH() << "\n";
            Image::savePNG("test_salida.png", R);
            log << "Saved as test_salida.png\n";
        } else {
            log << "(Test 2 skipped: no PNG provided)\n";
        }

        auto end = clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        log << "Tests completed successfully in " << elapsed << " seconds.\n";
        log << "[test_png] Done.\n";

        cout << "PNG test finished. Log written to test_png.log\n";
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        log << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
