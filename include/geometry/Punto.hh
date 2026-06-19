#pragma once

#include "geometry/Geometria.hh"
#include "geometry/Direccion.hh"

class Punto : public Geometria {
    public:
        Punto();
        Punto(const Eigen::Vector3d& coords);

        Direccion operator-(const Punto& other) const;
        Punto operator+(const Direccion& dir) const;
        Punto operator+(const Eigen::Vector3d& dir) const;  
        Punto operator/(double a) const;
};
