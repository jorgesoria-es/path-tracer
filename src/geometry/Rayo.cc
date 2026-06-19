#include "geometry/Rayo.hh"

Rayo::Rayo(const Punto& o, const Direccion& d)
    : origen(o), direccion(d.unitario()) {}

Punto Rayo::at(double t) const {
    return origen + direccion * t;
}
