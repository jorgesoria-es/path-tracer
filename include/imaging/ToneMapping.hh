#pragma once

#include "imaging/Imagen.hh"
#include <utility>

// Clamp a [0, V]: cada componente del píxel se limita al rango [0, V]
Image clamp(const Image& img);

// Equalize: se aplica la ecualización del histograma a cada canal de color
Image equalize(const Image& img);

// Equalize + Clamp a [0, V]: se aplica la ecualización del histograma y 
// luego se limita cada componente del píxel al rango [0, V]
Image equalizeAndClamp(const Image& img, double V);

// Gamma correction: se aplica la corrección gamma a cada componente del píxel
Image gamma(const Image& img, double gammaVal);

// Gamma correction + Clamp a [0, V]: se aplica la corrección gamma y 
// luego se limita cada componente del píxel al rango [0, V]
Image gammaAndClamp(const Image& img, double gammaVal, double V);
