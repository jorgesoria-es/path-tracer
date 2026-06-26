#include "geometry/Punto.hh"
#include "random"

Punto obtenerPuntoDePlaneta(
    const Punto& centro,
    const double radio,
    const double inclinacion,
    const double acimut
);

inline double rand01() {
    thread_local std::mt19937 rng { std::random_device{}() };
    return std::generate_canonical<double, 53>(rng);
}
