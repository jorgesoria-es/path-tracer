#pragma once

#include "geometry/Direccion.hh"
#include "geometry/Objeto.hh"
#include "geometry/Punto.hh"
#include "geometry/Rayo.hh"
#include "imaging/Pixel.hh"

class Esfera : public Objeto {
    public:
        Esfera(
            const Punto& centro, const double radio,
            const Pixel& emision, const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion,
            double indRefraccion = 1.0
        );

        Esfera(const Punto& centro, const double radio, double indRefraccion = 1.0);

        bool intersect(const Rayo& r, Punto& hit) const override;
        
        bool intersect_t(const Rayo& r, double& t) const override;

        Direccion normalAt(const Punto& p) const override;

    private:
        Punto _centro;
        double _radio;
};