#include "geometry/Punto.hh"
#include "geometry/Direccion.hh"

Punto::Punto()
    : Geometria(1) {}

Punto::Punto(const Eigen::Vector3d& coords)
    : Geometria(coords, 1) {}

Direccion Punto::operator-(const Punto& other) const {
    return Direccion(this->getCoords() - other.getCoords());
}

Punto Punto::operator+(const Direccion& dir) const {
    return Punto(this->getCoords() + dir.getCoords());
}

Punto Punto::operator+(const Eigen::Vector3d& dir) const {
    return *this + Direccion(dir);
}

Punto Punto::operator/(double a) const {
    return Punto(this->getCoords() / a);
}
