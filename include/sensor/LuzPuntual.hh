#pragma once

#include "geometry/Punto.hh"
#include "imaging/Pixel.hh"

class LuzPuntual {
    public:
        LuzPuntual(const Punto& posicion, const Pixel& potencia);

        Pixel potencia() const;

        Punto posicion() const;

    private:
        Pixel _potencia;
        Punto _posicion;
};