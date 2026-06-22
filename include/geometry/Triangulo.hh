#pragma once

#include "geometry/Direccion.hh"
#include "geometry/Objeto.hh"
#include "geometry/Punto.hh"
#include "geometry/Rayo.hh"
#include "imaging/Pixel.hh"

class Triangulo : public Objeto {
    public:
        Triangulo(const Punto& v0, const Punto& v1, const Punto& v2,
                  const Pixel& emision, const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion,
                  double indRefraccion = 1.0);

        Triangulo(const Punto& v0, const Punto& v1, const Punto& v2, double indRefraccion = 1.0);
        
        bool intersect(const Rayo& r, Punto& hit) const override;
        
        bool intersect_t(const Rayo& r, double& t) const override;

        Direccion normalAt(const Punto& p) const override;

        //----------Getter-Vértices
        const Punto& v0() const { return _v0; }
        const Punto& v1() const { return _v1; }
        const Punto& v2() const { return _v2; }

    private:
        Punto _v0, _v1, _v2; // vértices
        Direccion _e1, _e2;  // aristas (v1-v0, v2-v0)
        Direccion _normal;   // normal
};