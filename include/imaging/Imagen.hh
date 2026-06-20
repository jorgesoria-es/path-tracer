#pragma once

#include "imaging/Pixel.hh"
#include <vector>
#include <string>

class Image {
    public:
        /** Constructor por defecto */
        Image() = default;

        /** Constructores */
        Image(const int w, const int h, const Pixel& val = Pixel());
        Image(const int w, const int h, const std::vector<Pixel>& pixels);

        /** Getter del ancho de la imagen */
        int getW() const;

        /** Getter del alto de la imagen */
        int getH() const;

        /** Getter de matriz de píxeles */
        std::vector<Pixel>& getPixels();

        /** Getter de matriz de píxeles si la imagen en constante */
        const std::vector<Pixel>& getPixels() const;

        /** Acceder al pixel de la imagen en la posición [x, y] */
        Pixel& at(int x, int y);

        /** Acceder al pixel de la imagen en la posición [x, y] si la imagen es constante */
        const Pixel& at(int x, int y) const;

        /** Acceder a una fila */
        Pixel* row(int y) noexcept;

         /** Acceder a una fila si la imagen es constante */
        const Pixel* row(int y) const noexcept;

        /** Métodos estáticos para cargar y guardar imágenes */
        static Image loadPNG(const std::string& fichero);
        static Image loadEXR(const std::string& fichero);
        static void savePNG(const std::string& fichero, const Image& img);
        static void saveEXR(const std::string& fichero, const Image& img);

        /** Comparar imagen píxel a píxel con cierta tolerancia */
        static int compare(const Image& A, const Image& B, double& maxAbsErr, const double tol = 1.0 / 255.0 + 1e-6);

    private:
        int _w, _h;
        std::vector<Pixel> _pixels;
};
