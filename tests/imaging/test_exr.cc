#include "imaging/Imagen.hh"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

/**
 * Genera un EXR de prueba y valida round-trip (load→save→load).
 */
int main(int argc, char** argv) {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

    const char* kDefaultEXR =
        "/home/javier/Documentos/uni/2025-2026/practicas/ig/data/images/example_landscape.exr";
    const string ruta = (argc >= 2) ? argv[1] : kDefaultEXR;

    try {
        cout << "[test_exr_generate] Round-trip EXR: " << ruta << '\n';
        Image img = Image::loadEXR(ruta);
        cout << "Dimensions: " << img.getW() << "x" << img.getH() << '\n';

        Image::saveEXR("test_salida.exr", img);
        Image img2 = Image::loadEXR("test_salida.exr");

        double maxErr = 0.0;
        int errores = Image::compare(img, img2, maxErr);
        if (errores == 0)
            cout << "Identical copy (MaxAbsErr=" << maxErr << ")\n";
        else if (errores > 0)
            cout << "Differences detected (" << errores << " pixels, MaxAbsErr=" << maxErr << ")\n";
        else
            cout << "Images have different dimensions\n";

        auto end = clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        cout << "Completed in " << elapsed << " seconds\n";
        cout << "[test_exr_generate] Done.\n";
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
