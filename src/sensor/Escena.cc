#include "sensor/Escena.hh"

#include <limits>
#include <fstream>
#include "geometry/Triangulo.hh"
#include "geometry/transformaciones.hh"

void Escena::add(std::shared_ptr<Objeto> o) {
    _objs.push_back(std::move(o));
}

void Escena::add(std::shared_ptr<LuzPuntual> o) {
    _lucesPuntuales.push_back(std::move(o));
}

const std::vector<std::shared_ptr<LuzPuntual>>& Escena::getLucesPuntuales() const { 
    return _lucesPuntuales; 
}

bool Escena::intersectar(const Rayo& r, Pixel& color, Punto& hit, std::shared_ptr<Objeto>& obj) const {
    bool any = false;
    double bestT = std::numeric_limits<double>::infinity();
    Pixel bestC(0,0,0);
    std::shared_ptr<Objeto> bestObj;

    for(const auto& o : _objs) {
        double t;
        if(o->intersect_t(r, t)) {
            if(t < bestT) {
                bestT = t;
                bestC = o->emision();
                bestObj = o;
                any = true;
            }
        }
    }

    if(any) {
        hit = r.origen + r.direccion * bestT;
        color = bestC;
        obj = bestObj;
    }
    return any;
}

bool Escena::intersectar(const Rayo& r, double maxDist) const {
    for (const auto& o : _objs) {
        /*Punto h;
        if (o->intersect(r, h)) {
            double t = (h - r.origen).getModulo();
            if (t > 1e-6 && t < maxDist) return true;
        }*/

        // más eficiente
        double t;
        if(o->intersect_t(r, t)) {
            if(t > 1e-6 && t < maxDist) return true;
        }
    }
    return false;
}

bool Escena::cargarObj(
    const std::string& rutaObj,
    const Pixel& emision, const Pixel& kd, const Pixel& ke, const Pixel& kt,
    const Eigen::Matrix4d& M
) {
    std::ifstream archivo(rutaObj);
    if(!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo \"" << rutaObj << "\"." << std::endl;
        return false;
    }

    std::vector<Punto> vertices;
    std::string linea;

    while(std::getline(archivo, linea)) {
        std::istringstream iss(linea);
        std::string tipo;
        iss >> tipo;

        if(tipo == "v") { // Vértice
            double x, y, z;
            if(iss >> x >> y >> z) {
                vertices.emplace_back(M * Punto(Eigen::Vector3d(x, y, z)));
            }
        }
        
        else if(tipo == "f") { // Cara
            std::vector<int> indicesVertices;
            std::string token;

            while(iss >> token) {
                // Las caras pueden ser: v/vt/vn o v//vn o v
                // Nos interesa solo el primer número (índice del vértice)

                size_t pos = token.find('/');
                if(pos != std::string::npos) {
                    token = token.substr(0, pos);
                }

                try {
                    int indice = std::stoi(token);
                    indicesVertices.push_back(indice - 1);
                } catch(const std::exception& e) {
                    continue;
                }
            }

            // para formar una cara necesitamos al menos tres vértices
            if(indicesVertices.size() >= 3) {
                if(indicesVertices[0] < vertices.size() &&
                   indicesVertices[1] < vertices.size() &&
                   indicesVertices[2] < vertices.size()) {

                    this->add(std::make_shared<Triangulo>(
                        vertices[indicesVertices[0]],
                        vertices[indicesVertices[1]],
                        vertices[indicesVertices[2]],
                        emision, kd, ke, kt
                    ));
                }
                // si hay más de tres vértices, triangulación
                // por ejemplo un cuadrado se puede formar con dos triángulos
                for(size_t i = 3; i < indicesVertices.size(); ++i) {
                    if(indicesVertices[0] < vertices.size() &&
                       indicesVertices[i - 1] < vertices.size() &&
                       indicesVertices[i] < vertices.size()) {
                    
                            this->add(std::make_shared<Triangulo>(
                                vertices[indicesVertices[0]],
                                vertices[indicesVertices[i - 1]],
                                vertices[indicesVertices[i]],
                                emision, kd, ke, kt
                            ));
                       }
                }
            }
        }
    }

    archivo.close();
    return true;
}