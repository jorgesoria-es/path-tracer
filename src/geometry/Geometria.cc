#include "geometry/Geometria.hh"

Geometria::Geometria(const int flag)
    : _coords(Eigen::Vector4d(0, 0, 0, (double)flag)) {}

Geometria::Geometria(const Eigen::Vector3d& coords, const int flag)
    : _coords(Eigen::Vector4d(coords.x(), coords.y(), coords.z(), flag)) {}

Eigen::Vector3d Geometria::getCoords() const {
    return this->_coords.head<3>();
}

Eigen::Vector4d Geometria::getHomogeneousCoords() const {
    return this->_coords;
}

std::ostream& operator<<(std::ostream& os, const Geometria& g) {
    os << g.getCoords().transpose();
    return os;
}
