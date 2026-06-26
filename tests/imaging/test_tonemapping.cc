#include "imaging/Imagen.hh"
#include "imaging/ToneMapping.hh"
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

/**
 * Aplica operaciones de ToneMapping a un EXR existente y guarda PNGs.
 */
int main(int argc, char** argv) {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();
    std::ofstream log("test_tonemapping.log");

    const char* kDefaultEXR =
        "/home/javier/Documentos/uni/2025-2026/practicas/ig/data/images/example_landscape.exr";
    const std::string ruta = (argc >= 2) ? argv[1] : kDefaultEXR;

    try {
        log << "[test_tonemapping] ToneMapping operations on: " << ruta << "\n";
        Image img = Image::loadEXR(ruta);

        log << "Applying clamp...\n";
        Image::savePNG("test_clamp.png", clamp(img));

        log << "Applying equalize...\n";
        Image::savePNG("test_equalize.png", equalize(img));

        log << "Applying equalize + clamp (1.5)...\n";
        Image::savePNG("test_equalize_clamp.png", equalizeAndClamp(img, 1.5));

        log << "Applying gamma (2.4)...\n";
        Image::savePNG("test_gamma.png", gamma(img, 2.4));

        log << "Applying gamma + clamp (2.4, 1.5)...\n";
        Image::savePNG("test_gamma_clamp.png", gammaAndClamp(img, 2.4, 1.5));

        auto end = clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        log << "PNG files generated successfully in " << elapsed << " seconds.\n";
        log << "[test_tonemapping] Done.\n";

        std::cout << "ToneMapping test finished. Log written to test_tonemapping.log\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        log << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
