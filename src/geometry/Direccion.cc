#include "geometry/Direccion.hh"

Direccion::Direccion()
    : Geometria(0) {}

Direccion::Direccion(const Eigen::Vector3d& coords)
    : Geometria(coords, 0) {}

double Direccion::getModulo() const {
    return this->getCoords().norm();
}

Direccion Direccion::unitario() const {
    return Direccion(this->getCoords().normalized());
}

Direccion Direccion::operator-() const {
    return Direccion(-this->getCoords());
}

Direccion Direccion::perpendicular(const Direccion& a, const Direccion& b) {
    return Direccion(a.getCoords().cross(b.getCoords()));
}

double Direccion::dot(const Direccion& d) const {
    return this->getCoords().dot(d.getCoords());
}

Direccion Direccion::operator*(double a) const {
    return Direccion(this->getCoords()*a);
}

Direccion Direccion::operator+(double a) const {
    return Direccion(this->getCoords()+Eigen::Vector3d(a, a, a));
}

Direccion Direccion::operator+(const Direccion& o) const {
    return Direccion(this->getCoords() + o.getCoords());
}

Direccion Direccion::operator-(const Direccion& o) const {
    return Direccion(this->getCoords() - o.getCoords());
}

Direccion Direccion::operator/(double a) const {
    return Direccion(this->getCoords() / a);
}
