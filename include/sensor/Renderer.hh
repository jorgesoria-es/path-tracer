#pragma once

#include "sensor/PinholeCamara.hh"
#include "imaging/Imagen.hh"
#include "sensor/Escena.hh"

#define USE_THREADS 1

enum class RenderMode { NONE, DIRECT, INDIRECT };

class Renderer {
    public:
        Renderer(const PinholeCamara& camara, const Escena& escena, int spp = 16);

        void render(Image& hdrOut, Image& ldrOut, RenderMode mode) const;

    private:
        /** Métodos de renderizado específicos */
        void renderNone(Image& hdrOut, Image& ldrOut) const;
        void renderDirect(Image& hdrOut, Image& ldrOut) const;
        void renderIndirect(Image& hdrOut, Image& ldrOut) const;

        /** Maneja la iteración de píxeles. PixelFunc es una lamba (x, y) -> Pixel */
        template<typename PixelFunc>
        void renderImage(Image& img, PixelFunc obtenerPixel) const;

        /** Path Tracing */
        Pixel contribucionDirecta(const Direccion& wo, const Punto& hit, std::shared_ptr<Objeto> obj) const;
        Pixel contribucion(const Rayo& r) const;

        /** Atributos */
        const PinholeCamara& _camara;
        const Escena& _escena;
        int _spp;
};