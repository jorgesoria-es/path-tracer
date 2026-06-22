#include "sensor/LuzPuntual.hh"

LuzPuntual::LuzPuntual(const Punto& posicion, const Pixel& potencia)
    : _posicion(posicion), _potencia(potencia) {}

Pixel LuzPuntual::potencia() const { return this->_potencia; }
        
Punto LuzPuntual::posicion() const { return this->_posicion; }