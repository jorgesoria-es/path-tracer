#include "geometry/transformaciones.hh"
#include <cmath>

Eigen::Matrix4d M_RotateX(double angle) {
    Eigen::Matrix4d M;
    double c = cos(angle), s = sin(angle);

    M <<  1,  0,  0,  0,
          0,  c, -s,  0,
          0,  s,  c,  0,
          0,  0,  0,  1;

    return M;
}

Eigen::Matrix4d M_RotateY(double angle) {
    Eigen::Matrix4d M;
    double c = cos(angle), s = sin(angle);

    M <<  c,  0,  s,  0,
          0,  1,  0,  0,
         -s,  0,  c,  0,
          0,  0,  0,  1;

    return M;
}

Eigen::Matrix4d M_RotateZ(double angle) {
    Eigen::Matrix4d M;
    double c = cos(angle), s = sin(angle);

    M <<  c, -s,  0,  0,
          s,  c,  0,  0,
          0,  0,  1,  0,
          0,  0,  0,  1;

    return M;
}

Eigen::Matrix4d M_Translate(const Eigen::Vector3d& t) {
    Eigen::Matrix4d M = Eigen::Matrix4d::Identity();
    M(0, 3) = t.x();
    M(1, 3) = t.y();
    M(2, 3) = t.z();
    return M;
}

Eigen::Matrix4d M_Scale(const Eigen::Vector3d& s) {
    Eigen::Matrix4d M = Eigen::Matrix4d::Identity();
    M(0, 0) = s.x();
    M(1, 1) = s.y();
    M(2, 2) = s.z();
    return M;
}

Eigen::Matrix4d M_Base(
    const Eigen::Vector3d& u,
    const Eigen::Vector3d& v,
    const Eigen::Vector3d& w,
    const Eigen::Vector3d& o
) {
    Eigen::Matrix4d M = Eigen::Matrix4d::Identity();
    M(0, 0) = u.x(); M(0, 1) = v.x(); M(0, 2) = w.x(); M(0, 3) = o.x();
    M(1, 0) = u.y(); M(1, 1) = v.y(); M(1, 2) = w.y(); M(1, 3) = o.y();
    M(2, 0) = u.z(); M(2, 1) = v.z(); M(2, 2) = w.z(); M(2, 3) = o.z();
    return M;
}

Punto operator*(const Eigen::Matrix4d& m, const Punto& p) {
    Eigen::Vector4d aux = m * p.getHomogeneousCoords();
    return Punto(Eigen::Vector3d(
        aux.x() / aux.w(),
        aux.y() / aux.w(),
        aux.z() / aux.w()
    ));
}

Direccion operator*(const Eigen::Matrix4d& m, const Direccion& d) {
    Eigen::Vector4d aux = m * d.getHomogeneousCoords();
    return Direccion(aux.head<3>());
}
