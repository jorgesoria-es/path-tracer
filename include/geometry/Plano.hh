#pragma once

#include "geometry/Direccion.hh"
#include "geometry/Objeto.hh"
#include "geometry/Punto.hh"
#include "geometry/Rayo.hh"
#include "imaging/Pixel.hh"

class Plano : public Objeto {
    public:
        Plano(
            const Direccion& normal, const double distanciaOrigen,
            const Pixel& emision, const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion,
            double indRefraccion = 1.0
        );

        Plano(const Direccion& normal, const double distanciaOrigen, double indRefraccion = 1.0);

        bool intersect(const Rayo& r, Punto& hit) const override;

        bool intersect_t(const Rayo& r, double& t) const override;

        Direccion normalAt(const Punto& p) const override;

    private:
        Direccion _normal;
        double _distanciaOrigen;
};