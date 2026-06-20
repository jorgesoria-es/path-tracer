#include "geometry/Objeto.hh"
#include <algorithm>
#include <random>
#include <math.h>
#include "geometry/transformaciones.hh"
#include "geometry/operaciones.hh"

Objeto::Objeto(double indRefraccion)
    : Objeto(Pixel(), Pixel(), Pixel(), Pixel(), indRefraccion) {}

Objeto::Objeto(
    const Pixel& emision,
    const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion,
    double indRefraccion
) : _emision(emision),
    _coefDifuso(coefDifuso),
    _coefEspecular(coefEspecular),
    _coefRefraccion(coefRefraccion),
    _indRefraccion(indRefraccion)
{}

Pixel Objeto::pathContribution(double rand, const Punto& hit) const {
    
    Pixel kd = kDifuso(hit), ks = kEspecular(hit), kt = kRefraccion(hit);
    double maxDifuso = kd.max(), maxEspecular = ks.max(), maxRefraccion = kt.max();

    double s = maxDifuso + maxEspecular + maxRefraccion;
    if(s <= 0.0) return Pixel();

    double pSurvive = std::min(s, 0.90);
    double pKill = 1.0 - pSurvive;
    double pDifuso = (maxDifuso / s) * pSurvive;
    double pEspecular = (maxEspecular / s) * pSurvive;
    double pRefraccion = (maxRefraccion / s) * pSurvive; 

    if(rand < pKill) {
        return Pixel();
    }
    if(rand < pKill + pDifuso) {
        return kd / pDifuso;
    }
    else if(rand < pKill + pDifuso + pEspecular) {
        return ks / pEspecular;
    }
    else if(rand < pKill + pDifuso + pEspecular + pRefraccion) {
        return kt / pRefraccion;
    }
    return Pixel();
}

Direccion Objeto::generarDirRayoSampleCoseno(const Direccion& w0, const Punto& hit) const {
    Direccion normal = normalAt(hit);
    
    Direccion aux(Eigen::Vector3d(1.0, 0.0, 0.0));
    if(std::fabs(aux.dot(normal)) > 1.0 - 1e-6) aux = Direccion(Eigen::Vector3d(0.0, 1.0, 0.0));

    Direccion z = normal.unitario();
    Direccion x = Direccion::perpendicular(z, aux).unitario();
    Direccion y = Direccion::perpendicular(x, z).unitario();

    Eigen::Matrix4d cambioAGlobal = M_Base(x.getCoords(), y.getCoords(), z.getCoords(), hit.getCoords());

    double sqrtRand = std::sqrt(rand01());
    double inclinacion = std::asin(sqrtRand);
    double acimut = 2 * M_PI * rand01();

    return cambioAGlobal * Direccion(Eigen::Vector3d(
        sqrtRand * std::sin(acimut),
        sqrtRand * std::cos(acimut),
        std::cos(inclinacion)
    ));
}

Direccion Objeto::generarDirRayoReflejado(const Direccion& w0, const Punto& hit) const {
    Direccion normal = normalAt(hit);
    if(w0.dot(normal) > 0.0) normal = normal * -1.0;
    Direccion ws = w0 - normal * (2.0 * w0.dot(normal));
    return ws;
}

Direccion Objeto::generarDirRayoRefractado(const Direccion& w0, const Punto& hit, bool* tir) const {
    Direccion normal = normalAt(hit), ws;
    double nIncidente, nSaliente;
    double cosIncidente = -w0.dot(normal); // w0 y normal unitarios

    if(cosIncidente < 0.0) { // el rayo está saliendo
        cosIncidente = -cosIncidente;
        normal = normal * -1.0;
        nIncidente = this->_indRefraccion, nSaliente = 1.0;
    } else { nIncidente = 1.0, nSaliente = this->_indRefraccion; }
    
    double eta = nIncidente / nSaliente;

    double sinSaliente2 = eta * eta * (1 - cosIncidente * cosIncidente);
    if(sinSaliente2 > 1.0) { // reflejamos en lugar de refractar (reflexión interna total)
        if(tir) *tir = true;
        ws = generarDirRayoReflejado(w0, hit);
    } else {
        if(tir) *tir = false;
        double cosSaliente = std::sqrt(1.0 - sinSaliente2);
        ws = ((w0 * eta) + (normal * (eta * cosIncidente - cosSaliente))).unitario();
    }
    return ws;
}

bool Objeto::crearSecundario(double rand, const Direccion& w0, const Punto& hit, Direccion& ws, EventoBSDF* ev) const {
    
    Pixel kd = kDifuso(hit), ks = kEspecular(hit), kt = kRefraccion(hit);
    double maxDifuso = kd.max(), maxEspecular = ks.max(), maxRefraccion = kt.max();

    double s = maxDifuso + maxEspecular + maxRefraccion;
    if(s <= 0.0) {
        if(ev) *ev = EventoBSDF::Kill;
        return false;
    }

    double pSurvive = std::min(s, 0.90);
    double pKill = 1.0 - pSurvive;
    double pDifuso = maxDifuso / s * pSurvive;
    double pEspecular = maxEspecular / s * pSurvive;
    double pRefraccion = maxRefraccion / s * pSurvive; 

    if(rand < pKill) {
        return false;
    }
    else if(rand < pKill + pDifuso) {
        ws = generarDirRayoSampleCoseno(w0, hit);
        if(ev) *ev = EventoBSDF::Difuso;
        return true;
    }
    else if (rand < pKill + pDifuso + pEspecular) { // ESPECULAR: reflejo perfecto
        ws = generarDirRayoReflejado(w0, hit);
        if(ev) *ev = EventoBSDF::Especular;
        return true;
    }
    else if(rand < pKill + pDifuso + pEspecular + pRefraccion) { // REFRACCIÓN
        bool tir = false;
        ws = generarDirRayoRefractado(w0, hit, &tir);
        if(ev) *ev = tir ? EventoBSDF::Especular : EventoBSDF::Refraccion;
        return true;
    }
    if(ev) *ev = EventoBSDF::Kill;
    return false;
}

bool Objeto::intersect_t(const Rayo& r, double& t) const {
    Punto h;
    if(!this->intersect(r, h)) return false;
    t = (h - r.origen).dot(r.direccion); // r.direccion normalizado
    return true;
}

Pixel Objeto::kDifuso(const Punto& hit) const {
    return _coefDifuso;
}

Pixel Objeto::kEspecular(const Punto& hit) const {
    return _coefEspecular;
}

Pixel Objeto::kRefraccion(const Punto& hit) const {
    return _coefRefraccion;
}