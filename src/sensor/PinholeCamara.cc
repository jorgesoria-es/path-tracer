#include "sensor/PinholeCamara.hh"

#include "geometry/transformaciones.hh"
#include "geometry/operaciones.hh"
#include <utility>
#include <random>
#include <thread>
#include <limits>

PinholeCamara::PinholeCamara(
    const Punto& origen,
    const Direccion& u,
    const Direccion& l,
    const Direccion& f,
    int width,
    int height
) : _origen(origen), _u(u), _l(l), _f(f), _W(width), _H(height) {
    
    _toGlobalMatrix = M_Base(
        l.getCoords(),
        u.getCoords(),
        f.getCoords(),
        origen.getCoords()
    );

    _wPixel = 2.0 / width;
    _hPixel = 2.0 / height;
}

PinholeCamara::PinholeCamara(
    const Punto& origen,
    const int width,
    const int height
) : _origen(origen), _W(width), _H(height) {

    if(width > height) {
        _l = Direccion(Eigen::Vector3d(-1.0, 0.0, 0.0));
        _u = Direccion(Eigen::Vector3d(0.0, (double) height / width, 0.0));
    } else {
        _l = Direccion(Eigen::Vector3d((double) -width / height, 0, 0));
        _u = Direccion(Eigen::Vector3d(0.0, 1.0, 0.0));
    }
    _f = Direccion(Eigen::Vector3d(0.0, 0.0, 1.0));

    _toGlobalMatrix = M_Base(
        _l.getCoords(),
        _u.getCoords(),
        _f.getCoords(),
        origen.getCoords()
    );

    _wPixel = 2.0 / width;
    _hPixel = 2.0 / height;
}

PinholeCamara::PinholeCamara(
    const Punto& origen,
    const int width,
    const int height,
    const double fovYdeg  
) : _origen(origen), _W(width), _H(height) {

    Direccion lhat = Direccion(Eigen::Vector3d(-1.0, 0.0, 0.0));
    Direccion uhat = Direccion(Eigen::Vector3d(0.0, 1.0, 0.0));
    Direccion fhat = Direccion(Eigen::Vector3d(0.0, 0.0, 1.0));

    _toGlobalMatrix = M_Base(
        lhat.getCoords(),
        uhat.getCoords(),
        fhat.getCoords(),
        origen.getCoords()
    );

    _wPixel = 2.0 / width;
    _hPixel = 2.0 / height;

    double aspect = double(width) / double(height);
    double fovY = fovYdeg * M_PI / 180.0;

    _f_len = 1.0;
    _u_len = std::tan(0.5 * fovY) * _f_len;
    _l_len = aspect * _u_len;
}

Rayo PinholeCamara::generarRayo(const int x, const int y) const {
        Direccion dir(
            Eigen::Vector3d(
                (1.0 - (_wPixel * (x + rand01()))) * _l_len,
                (1.0 - (_hPixel * (y + rand01()))) * _u_len,
                1.0 * _f_len
            )
        );
        return Rayo(
            _origen,
            (_toGlobalMatrix * dir).unitario()
        );
}

int PinholeCamara::getW() const { return _W; }
int PinholeCamara::getH() const { return _H; } 