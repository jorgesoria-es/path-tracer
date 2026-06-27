#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#include "sensor/PinholeCamara.hh"
#include "sensor/Renderer.hh"
#include "sensor/Escena.hh"
#include "sensor/LuzPuntual.hh"

#include "geometry/Direccion.hh"
#include "geometry/Punto.hh"
#include "geometry/Esfera.hh"
#include "geometry/Plano.hh"

#include "imaging/Imagen.hh"
#include "imaging/Pixel.hh"

int main() {

    // ---------------- Configuración general ----------------
    int W = 512;
    int H = 512;
    std::string nom = "";

#if !RUNNING_AS_TEST
    std::cout << "Introduzca el ancho y alto separado por espacios: "; std::cin >> W >> H;
    std::cin.ignore();
    std::cout << "Introduzca un nombre para la imagen (sin extensión): "; std::getline(std::cin, nom); 
#endif

    // Caminos por píxel a analizar
    const std::vector<int> spps = {2, 8, 32, 128, 512};

    std::ofstream log("T1-07-convergenciaArea.log");
    log << "[T1-07] Análisis de convergencia (luces de área) iniciado\n";
    log << "Resolución: " << W << "x" << H << "\n";

    // ---------------- Escena: Cornell Box ----------------
    Escena escena;

    // Pared izquierda (roja)
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(1.0, 0.0, 0.0)), -1.0,
        Pixel(), Pixel(1.0, 0.0, 0.0), Pixel(), Pixel()
    ));

    // Pared derecha (verde)
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(-1.0, 0.0, 0.0)), -1.0,
        Pixel(), Pixel(0.0, 1.0, 0.0), Pixel(), Pixel()
    ));

    // Suelo
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(0.0, 1.0, 0.0)), -1.0,
        Pixel(), Pixel(1.0, 1.0, 1.0), Pixel(), Pixel()
    ));

    // Techo
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(0.0, -1.0, 0.0)), -1.0,
        Pixel(), Pixel(1.0, 1.0, 1.0), Pixel(), Pixel()
    ));

    // Fondo
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(0.0, 0.0, -1.0)), -1.0,
        Pixel(), Pixel(1.0, 1.0, 1.0), Pixel(), Pixel()
    ));

    // Esfera izquierda (difusa + especular)
    escena.add(std::make_shared<Esfera>(
        Punto(Eigen::Vector3d(-0.5, -0.7, 0.25)),
        0.3,
        Pixel(), Pixel(0.0, 0.5, 0.5), Pixel(0.5, 0.5, 0.5), Pixel()
    ));

    // Esfera derecha (refractante)
    escena.add(std::make_shared<Esfera>(
        Punto(Eigen::Vector3d(0.5, -0.7, -0.25)),
        0.3,
        Pixel(), Pixel(0.5, 0.3, 0.3), Pixel(), Pixel(0.5, 0.5, 0.5),
        1.5
    ));

    // ---------------- Luz puntual ----------------
    escena.add(std::make_shared<Plano>(
        Direccion(Eigen::Vector3d(0.0, -1.0, 0.0)), -0.99,
        Pixel(0.5, 0.5, 0.5),  // emisión
        Pixel(), Pixel(), Pixel()
    ));


    // ---------------- Cámara ----------------
    Punto origenCamara(Eigen::Vector3d(0.0, 0.0, -3.5));
    PinholeCamara camara(origenCamara, W, H, 40.0);

    // ---------------- Render por cada spp ----------------
    for (int spp : spps) {

        Renderer renderer(camara, escena, spp);
        Image hdr(W, H);
        Image ldr(W, H);

        auto t0 = std::chrono::high_resolution_clock::now();
        renderer.render(hdr, ldr, RenderMode::INDIRECT);
        double tiempo = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - t0
        ).count();

        std::string base = nom + "_cornell_spp_" + std::to_string(spp);
        Image::savePNG(base + ".png", ldr);
        Image::saveEXR(base + ".exr", hdr);

        log << "spp=" << spp << " tiempo=" << tiempo << " s\n";
        std::cout << "[T1-06] spp=" << spp
                  << " render completado (" << tiempo << " s)\n";
    }

    log << "[T1-07] Análisis finalizado\n";
    return 0;
}
