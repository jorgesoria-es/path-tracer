#pragma once

#include "geometry/Direccion.hh"
#include "geometry/Punto.hh"
#include "geometry/Rayo.hh"

class PinholeCamara {
    public:
        PinholeCamara(
            const Punto& origen,
            const Direccion& u,
            const Direccion& l,
            const Direccion& f,
            int width,
            int height
        );

        PinholeCamara(
            const Punto& origen,
            const int width,
            const int height
        );

        PinholeCamara(
            const Punto& origen,
            const int width,
            const int height,
            const double fovYdeg  
        );

        int getW() const;
        int getH() const; 
        Rayo generarRayo(const int x, const int y) const;

    private:
        Punto _origen;
        Direccion _u, _l, _f;
        int _W, _H;

        double _wPixel, _hPixel;
        Eigen::Matrix4d _toGlobalMatrix;    // Matriz para transformar a coordenadas globales

        // Si usamos FOVy
        double _f_len = 1.0, _u_len = 1.0, _l_len = 1.0;
};