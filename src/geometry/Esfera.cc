#include "geometry/Esfera.hh"
#include <cmath>

Esfera::Esfera(
    const Punto& centro, const double radio,
    const Pixel& emision, const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion,
    double indRefraccion
)
    : _centro(centro), _radio(radio), Objeto(emision, coefDifuso, coefEspecular, coefRefraccion, indRefraccion) {}

Esfera::Esfera(const Punto& centro, const double radio, double indRefraccion)
    : _centro(centro), _radio(radio), Objeto(indRefraccion) {}
        
bool Esfera::intersect(const Rayo& r, Punto& hit) const {
    double t;
    if(!intersect_t(r, t)) return false;
    hit = r.at(t);
    return true;
}

bool Esfera::intersect_t(const Rayo& r, double& t) const {
    // t²|d|² + 2td·(o - C) + |o - c|² - r² = 0    
    const double eps = 1e-6;

    Direccion oc = r.origen - _centro;
    
    double a = r.direccion.dot(r.direccion);
    double b = r.direccion.dot(oc) * 2.0;
    double c = (oc.dot(oc)) - (_radio * _radio);

    double discriminante = b*b - 4*a*c;
    if(discriminante < 0.0) return false;

    double sqrtD = std::sqrt(discriminante);
    double inv2a = 0.5 / a;
    double t1 = (-b - sqrtD) * inv2a;
    double t2 = (-b + sqrtD) * inv2a;

    t = (t1 > eps) ? t1 : (t2 > eps ? t2 : -1.0);
    return t > 0.0;
}

Direccion Esfera::normalAt(const Punto& p) const {
    return (p - _centro).unitario();
}
