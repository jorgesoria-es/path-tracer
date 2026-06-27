#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "sensor/PinholeCamara.hh"
#include "sensor/Renderer.hh"
#include "sensor/Escena.hh"

#include "geometry/Direccion.hh"
#include "geometry/Punto.hh"

#include "geometry/Esfera.hh"
#include "geometry/Plano.hh"

#include "imaging/ToneMapping.hh"
#include "imaging/Imagen.hh"
#include "imaging/Pixel.hh"

int main(int argc, char** argv) {
    const std::string nomTest = "test_t1-05-mezcla";
    std::ofstream log(nomTest);
    log << '[' << nomTest << "] Render iniciado.\n";
    
    int W = 512;
    int H = 512;
    int spp = 128;
    std::string nom = "output";

#if !RUNNING_AS_TEST
    std::cout << "Introduzca el ancho y alto separado por espacios: "; std::cin >> W >> H;
    std::cout << "Introduzca el nº de rayos por píxel: "; std::cin >> spp;
    std::cin.ignore();
    std::cout << "Introduzca un nombre para la imagen (sin extensión): "; std::getline(std::cin, nom); 
#endif

    log << "Resolución: " << W << 'x' << H << ", samples por píxel: " << spp << ".\n";

    //----------Escena
    Escena escena;

    // Pared izquierda (roja)
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(1.0, 0.0, 0.0)), -1.0,    // normal y distancia al origen
        Pixel(), Pixel(1.0, 0.0, 0.0), Pixel(), Pixel()     // emisión, difuso, especular, transmitancia
    ));

    // Pared derecha (verde)
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(-1.0, 0.0, 0.0)), -1.0,   // normal y distancia al origen
        Pixel(), Pixel(0.0, 1.0, 0.0), Pixel(), Pixel()     // emisión, difuso, especular, transmitancia
    ));

    // Suelo
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(0.0, 1.0, 0.0)), -1.0,    // normal y distancia al origen
        Pixel(), Pixel(1.0, 1.0, 1.0), Pixel(), Pixel()     // emisión, difuso, especular, transmitancia
    ));

    // Techo
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(0.0, -1.0, 0.0)), -1.0,   // normal y distancia al origen
        Pixel(), Pixel(1.0, 1.0, 1.0), Pixel(), Pixel()     // emisión, difuso, especular, transmitancia
    ));

    // Fondo
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(0.0, 0.0, -1.0)), -1.0,   // normal y distancia al origen
        Pixel(), Pixel(1.0, 1.0, 1.0), Pixel(), Pixel()     // emisión, difuso, especular, transmitancia
    ));

    // Esfera izquierda (reflejante)
    escena.add(std::make_shared<Esfera>(
        Punto(Eigen::Vector3d(-0.5, -0.7, 0.25)),               // centro
        0.3,                                                    // radio
        Pixel(), Pixel(.0, .5, .5), Pixel(.5, .5, .5), Pixel()  // emisión, difuso, especular, transmitancia
    ));

    // Esfera derecha (refractante)
    escena.add(std::make_shared<Esfera>(
        Punto(Eigen::Vector3d(0.5, -0.7, -0.25)),               // centro
        0.3,                                                    // radio
        Pixel(), Pixel(.5, .3, .3), Pixel(), Pixel(.5, .5, .5), // emisión, difuso, especular, transmitancia
        1.5                                                     // índice de refracción
    ));

    //----------Luces
    escena.add(std::make_shared<LuzPuntual>(
        Punto(Eigen::Vector3d(0.0, 0.8, 0.0)),              // centro
        Pixel(0.9, 0.9, 0.9)                                // potencia
    ));

    //----------Cámara
    Punto origenCamara(Eigen::Vector3d(0.0, 0.0, -3.5));
    PinholeCamara camara(origenCamara, W, H, 40.0);

    //----------Renderizar imágenes
    Renderer renderer(camara, escena, spp);
    Image hdr(W, H);
    Image ldr(W, H);

    auto tinicio = std::chrono::high_resolution_clock::now();
    
    renderer.render(hdr, ldr, RenderMode::INDIRECT);
    
    double duracion = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - tinicio).count();

    Image::savePNG(nom + ".png", ldr);
    Image::saveEXR(nom + ".exr", hdr);

    log << "Render completado en " << duracion << " segundos.\n";
    log << '[' << nomTest << "] Render finalizado.\n";
    return 0;
}