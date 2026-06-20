#include "imaging/Pixel.hh"
#include <algorithm>
#include <stdexcept>

Pixel::Pixel(double r_, double g_, double b_)
    : r(r_), g(g_), b(b_) {}

Pixel::Pixel(const Eigen::Vector3d& rgb)
    : r(rgb.x()), g(rgb.y()), b(rgb.z()) {}

double Pixel::max() const {
    return std::max(this->r, std::max(this->g, this->b));
}

Pixel& Pixel::operator+=(const Pixel& p) noexcept {
    r += p.r; g += p.g; b += p.b;
    return *this;
}

Pixel Pixel::operator+(const Pixel& p) const {
    return Pixel(
        this->r + p.r,
        this->g + p.g,
        this->b + p.b
    );
}

Pixel Pixel::operator+(const double k) const {
    return Pixel(
        this->r + k,
        this->g + k,
        this->b + k
    );
}

Pixel Pixel::operator-(const Pixel& p) const {
    return Pixel(
        this->r - p.r,
        this->g - p.g,
        this->b - p.b
    );
}

Pixel Pixel::operator-(const double k) const {
    return Pixel(
        this->r - k,
        this->g - k,
        this->b - k
    );
}

Pixel& Pixel::operator*=(const Pixel& p) noexcept {
    r *= p.r; g *= p.g; b *= p.b;
    return *this;
}

Pixel Pixel::operator*(const Pixel& p) const {
    return Pixel(
        this->r * p.r,
        this->g * p.g,
        this->b * p.b
    ); 
}

Pixel Pixel::operator*(const double k) const {
    return Pixel(
        this->r * k,
        this->g * k,
        this->b * k
    );
}

Pixel Pixel::operator/(const Pixel& p) const {
    return Pixel(
        p.r == 0 ? 0.0 : this->r / p.r,
        p.g == 0 ? 0.0 : this->g / p.g,
        p.b == 0 ? 0.0 : this->b / p.b
    );
}

Pixel Pixel::operator/(const double k) const {
    if(k == 0.0)
        throw std::invalid_argument("División por cero en Pixel::operator/");
    return Pixel(
        this->r / k,
        this->g / k,
        this->b / k
    );
}
