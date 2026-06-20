#pragma once

#include "geometry/Direccion.hh"
#include "geometry/Punto.hh"

Eigen::Matrix4d M_RotateX(double angle);

Eigen::Matrix4d M_RotateY(double angle);

Eigen::Matrix4d M_RotateZ(double angle);

Eigen::Matrix4d M_Translate(const Eigen::Vector3d& t);

Eigen::Matrix4d M_Scale(const Eigen::Vector3d& s);

Eigen::Matrix4d M_Base(
    const Eigen::Vector3d& u,
    const Eigen::Vector3d& v,
    const Eigen::Vector3d& w,
    const Eigen::Vector3d& o
);

Punto operator*(const Eigen::Matrix4d& m, const Punto& p);

Direccion operator*(const Eigen::Matrix4d& m, const Direccion& d);
