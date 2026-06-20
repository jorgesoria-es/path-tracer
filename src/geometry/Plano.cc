#include "geometry/Plano.hh"
#include <cmath>

Plano::Plano(
    const Direccion& normal, const double distanciaOrigen,
    const Pixel& emision, const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion,
    double indRefraccion
)
    : _normal(normal.unitario()), _distanciaOrigen(distanciaOrigen), Objeto(emision, coefDifuso, coefEspecular, coefRefraccion, indRefraccion) {}

Plano::Plano(const Direccion& normal, const double distanciaOrigen, double indRefraccion)
    : _normal(normal.unitario()), _distanciaOrigen(distanciaOrigen), Objeto(indRefraccion) {}
        
bool Plano::intersect(const Rayo& r, Punto& hit) const {
    double t;
    if(!intersect_t(r, t)) return false;
    hit = r.at(t);
    return true;
}

bool Plano::intersect_t(const Rayo& r, double& t) const {
    // t = - (c + o·n) / (d·n)      c = -distanciaOrigen
    const double eps = 1e-6;
    
    double dn = r.direccion.dot(_normal);
    if(std::fabs(dn) < eps) return false;
    
    double on = r.origen.getCoords().dot(_normal.getCoords());
    t = (_distanciaOrigen - on) / dn;

    return t > eps;
}

Direccion Plano::normalAt(const Punto& p) const {
    return _normal;
}
