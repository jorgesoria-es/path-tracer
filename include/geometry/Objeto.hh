#pragma once

#include "geometry/Punto.hh"
#include "geometry/Rayo.hh"
#include "imaging/Pixel.hh"

#include "imaging/Imagen.hh"

enum class EventoBSDF { Difuso, Especular, Refraccion, Kill };

class Objeto {
public:
    virtual ~Objeto() = default;

    //----------Constructores
    Objeto(double indRefraccion = 1.0);
    Objeto(
        const Pixel& emision,
        const Pixel& coefDifuso, const Pixel& coefEspecular, const Pixel& coefRefraccion, double indRefraccion = 1.0
    );

    //----------Para-Pathtracer
    virtual bool intersect(const Rayo& r, Punto& hit) const = 0;

    virtual bool intersect_t(const Rayo& r, double& t) const;

    inline virtual Direccion normalAt(const Punto& p) const = 0;

    Pixel pathContribution(double rand, const Punto& hit) const;

    bool crearSecundario(double rand, const Direccion& w0, const Punto& hit, Direccion& ws, EventoBSDF* ev = nullptr) const;

    //----------Getters
    const Pixel& emision() const { return _emision; }
    double indRefraccion() const { return _indRefraccion; }

    Pixel kDifuso(const Punto& hit) const;
    Pixel kEspecular(const Punto& hit) const;
    Pixel kRefraccion(const Punto& hit) const;

    //----------Setter
    void setIndRefraccion(double i) { _indRefraccion = i; }

private:
    Pixel _emision, _coefDifuso, _coefEspecular, _coefRefraccion;
    
    double _indRefraccion;

    //----------Funciones-auxiliares
    Direccion generarDirRayoRefractado(const Direccion& w0, const Punto& hit, bool* tir = nullptr) const;
    Direccion generarDirRayoReflejado(const Direccion& w0, const Punto& hit) const;
    Direccion generarDirRayoSampleCoseno(const Direccion& w0, const Punto& hit) const;
};
