#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "geometry/Objeto.hh"
#include "sensor/LuzPuntual.hh"

class Escena {
    public:
        void add(std::shared_ptr<Objeto> o);

        void add(std::shared_ptr<LuzPuntual> o);

        const std::vector<std::shared_ptr<LuzPuntual>>& getLucesPuntuales() const;

        bool cargarObj(
            const std::string& rutaObj,
            const Pixel& emision, const Pixel& kd, const Pixel& ke, const Pixel& kt,
            const Eigen::Matrix4d& M = Eigen::Matrix4d::Identity()
        );

        // Devuelve true si hay impacto; color=emisión del más cercano y deja el hit
        bool intersectar(const Rayo& r, Pixel& color, Punto& hit, std::shared_ptr<Objeto>& obj) const;
        bool intersectar(const Rayo& r, double maxDist) const;
            
    private:
        std::vector<std::shared_ptr<Objeto>>_objs;
        std::vector<std::shared_ptr<LuzPuntual>> _lucesPuntuales;
};