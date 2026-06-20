#pragma once
#include <eigen3/Eigen/Dense>

class Pixel {
    public:
        // Campos
        double r, g, b;

        // Constructor y métodos de clase
        Pixel(double r_ = 0.0, double g_ = 0.0, double b_ = 0.0);
        Pixel(const Eigen::Vector3d& rgb);

        // Operadores sobrecargados para la clase Pixel
        double max() const; 

        Pixel& operator+=(const Pixel& p) noexcept;
        Pixel operator+(const Pixel& p) const;
        Pixel operator+(const double k) const;

        Pixel operator-(const Pixel& p) const;
        Pixel operator-(const double k) const;

        Pixel& operator*=(const Pixel& p) noexcept;
        Pixel operator*(const Pixel& p) const;
        Pixel operator*(const double k) const;

        Pixel operator/(const Pixel& p) const;
        Pixel operator/(const double k) const;
};
