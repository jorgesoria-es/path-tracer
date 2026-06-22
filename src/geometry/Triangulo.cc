#include "geometry/Triangulo.hh"
#include <cmath>

Triangulo::Triangulo(
    const Punto& v0, const Punto& v1, const Punto& v2,
    const Pixel& emision, const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion,
    double indRefraccion
) : _v0(v0), _v1(v1), _v2(v2), Objeto(emision, coefDifuso, coefEspecular, coefRefraccion, indRefraccion)
{
    _e1 = _v1 - _v0;
    _e2 = _v2 - _v0;
    _normal = Direccion::perpendicular(_e1, _e2).unitario();
}

Triangulo::Triangulo(const Punto& v0, const Punto& v1, const Punto& v2, double indRefraccion)
    : _v0(v0), _v1(v1), _v2(v2), Objeto(indRefraccion)
{
    _e1 = _v1 - _v0;
    _e2 = _v2 - _v0;
    _normal = Direccion::perpendicular(_e1, _e2).unitario();
}

bool Triangulo::intersect(const Rayo& r, Punto& hit) const {
    double t;
    if(!intersect_t(r, t)) return false;
    hit = r.at(t);
    return true;
}

bool Triangulo::intersect_t(const Rayo& r, double& t) const {
    // Algoritmo Möller-Trumbore

    const double eps = 1e-8;
    Direccion pvec = Direccion::perpendicular(r.direccion, _e2);
    double det = _e1.dot(pvec);

    if (std::fabs(det) < eps) return false; // rayo paralelo o muy cercano al plano del triángulo

    double invDet = 1.0 / det;

    Direccion tvec = r.origen - _v0;
    double u = tvec.dot(pvec) * invDet;
    if (u < 0.0 || u > 1.0) return false;

    Direccion qvec = Direccion::perpendicular(tvec, _e1);
    double v = r.direccion.dot(qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return false;

    t = _e2.dot(qvec) * invDet;
    return t > eps;
}
        
Direccion Triangulo::normalAt(const Punto& p) const {
    return _normal;
}
