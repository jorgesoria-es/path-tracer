#include "geometry/operaciones.hh"
#include <random>
#include <cmath>

Punto obtenerPuntoDePlaneta(
    const Punto& centro,
    const double radio,
    const double inclinacion,
    const double acimut
) {
    return centro + radio * Eigen::Vector3d(
        sin(inclinacion) * sin(acimut),
        sin(inclinacion) * cos(acimut),
        cos(inclinacion)
    );
}


