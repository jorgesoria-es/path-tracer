#pragma once

#include "geometry/Punto.hh"
#include "geometry/Direccion.hh"

class Rayo {
public:
    Punto origen;
    Direccion direccion;
    Rayo(const Punto& o, const Direccion& d);
    Punto at(double t) const;
};
