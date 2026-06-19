#pragma once

#include <iostream>
#include <eigen3/Eigen/Dense>

class Geometria {
    public:
        Geometria(const int flag);
        Geometria(const Eigen::Vector3d& coords, const int flag);

        Eigen::Vector3d getCoords() const;
        Eigen::Vector4d getHomogeneousCoords() const;
        friend std::ostream& operator<<(std::ostream& os, const Geometria& g);

    private:
        Eigen::Vector4d _coords;
};
