#pragma once

#include "geometry/Geometria.hh"

class Direccion : public Geometria {
    public:
        Direccion();
        Direccion(const Eigen::Vector3d& coords);

        double getModulo() const;
        Direccion unitario() const;
        Direccion operator-() const;
        double dot(const Direccion& d) const;
        Direccion operator*(double a) const;
        Direccion operator+(double a) const;
        Direccion operator+(const Direccion& o) const;
        Direccion operator-(const Direccion& o) const;
        static Direccion perpendicular(const Direccion& a, const Direccion& b);
        Direccion operator/(double a) const;
};
