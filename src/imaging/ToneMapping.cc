#include "imaging/ToneMapping.hh"

#include <stdexcept>
#include <algorithm>
#include <limits>
#include <cmath>

static inline double luminancia(const Pixel& p) {
    return 0.2126 * p.r + 0.7152 * p.g + 0.0722 * p.b;
}

static double maxLuminancia(const Image& img) {
    const auto& px = img.getPixels();
    if(px.empty()) throw std::runtime_error("Imagen vacía");
    
    double max = 0.0;
    for(const Pixel& p : px) max = std::max(max, luminancia(p));
    return (max > 0.0) ? max : 1.0;
}

Image clamp(const Image& img) {
    Image out(img.getW(), img.getH(), img.getPixels());
    for(Pixel& p : out.getPixels()) {
        p.r = std::clamp(p.r, 0.0, 1.0);
        p.g = std::clamp(p.g, 0.0, 1.0);
        p.b = std::clamp(p.b, 0.0, 1.0);
    }
    return out;
}

Image equalize(const Image& img) {
    const double Lmax = maxLuminancia(img);
    const double s = 1.0 / Lmax;

    Image out(img.getW(), img.getH(), img.getPixels());
    for(Pixel& p : out.getPixels()) {
        p.r = std::clamp(p.r * s, 0.0, 1.0);
        p.g = std::clamp(p.g * s, 0.0, 1.0);
        p.b = std::clamp(p.b * s, 0.0, 1.0);
    }
    return out;
}

Image equalizeAndClamp(const Image& img, double V) {
    if(V <= 0.0) throw std::invalid_argument("V debe ser > 0");

    Image out = equalize(img);
    const double invV = 1.0 / V;
    for(Pixel& p : out.getPixels()) {
        p.r = std::clamp(p.r, 0.0, V) * invV;
        p.g = std::clamp(p.g, 0.0, V) * invV;
        p.b = std::clamp(p.b, 0.0, V) * invV;
    }
    return out;
}

Image gamma(const Image& img, double gammaVal) {
    if(gammaVal <= 0.0) throw std::invalid_argument("gamma debe ser > 0");

    Image out = equalize(img);
    const double invG = 1.0 / gammaVal;
    for(Pixel& p : out.getPixels()) {
        p.r = std::pow(p.r, invG);
        p.g = std::pow(p.g, invG);
        p.b = std::pow(p.b, invG);
    }
    return out;
}

Image gammaAndClamp(const Image& img, double gammaVal, double V) {
    if(gammaVal <= 0.0 || V <= 0.0) throw std::invalid_argument("V y gamma deben ser > 0");

    Image out(img.getW(), img.getH(), img.getPixels());
    const double invG = 1.0 / gammaVal;
    const double invV = 1.0 / V;

    for(Pixel& p : out.getPixels()) {
        p.r = std::pow( std::clamp(p.r * invV, 0.0, 1.0), invG);
        p.g = std::pow( std::clamp(p.g * invV, 0.0, 1.0), invG);
        p.b = std::pow( std::clamp(p.b * invV, 0.0, 1.0), invG);
    }
    return out;
}
