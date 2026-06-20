#include "imaging/Imagen.hh"

#include "imaging/ToneMapping.hh"
#include <algorithm>
#include <stdexcept>

// libpng
#include <png.h>

// openEXR
#include <OpenEXR/ImfBoxAttribute.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfFrameBuffer.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfHeader.h>
#include <Imath/ImathBox.h>

/** Constructores */
Image::Image(const int w, const int h, const Pixel& val)
    : _w(w), _h(h), _pixels(w * h, val) {}

Image::Image(const int w, const int h, const std::vector<Pixel>& pixels)
    : _w(w), _h(h), _pixels(pixels) {}

/** Getter del ancho de la imagen */
int Image::getW() const {
    return this->_w;
}

/** Getter del alto de la imagen */
int Image::getH() const {
    return this->_h;
}

/** Getter de matriz de píxeles */
std::vector<Pixel>& Image::getPixels() {
    return this->_pixels;
}

/** Getter de matriz de píxeles si la imagen en constante */
const std::vector<Pixel>& Image::getPixels() const {
    return this->_pixels;
}

/** Acceder al pixel de la imagen en la posición [x, y] */
Pixel& Image::at(int x, int y) {
    return this->_pixels[y * _w + x];
}

/** Acceder al pixel de la imagen en la posición [x, y] si la imagen es constante */
const Pixel& Image::at(int x, int y) const {
    return this->_pixels[y * _w + x];
}

 /** Acceder a una fila */
Pixel* Image::row(int y) noexcept {
    return _pixels.data() + size_t(y) * _w;
}

/** Acceder a una fila si la imagen es constante */
const Pixel* Image::row(int y) const noexcept {
    return _pixels.data() + size_t(y) * _w;
}

//---------------------------------------------------------------------------------------
/** Métodos estáticos para cargar y guardar imágenes */

Image Image::loadPNG(const std::string& fichero) {
    FILE* fp = std::fopen(fichero.c_str(), "rb");
    if(!fp) throw std::runtime_error("No se puede abrir: \"" + fichero + "\"");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png) { std::fclose(fp); throw std::runtime_error("Fallo en png_create_read_struct"); }

    png_infop info = png_create_info_struct(png);
    if(!info) { png_destroy_read_struct(&png, nullptr, nullptr); std::fclose(fp); throw std::runtime_error("Fallo en png_create_info_struct"); }

    if(setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        std::fclose(fp);
        throw std::runtime_error("Error leyendo PNG");
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    png_uint_32 w, h;
    int bit_depth, color_type, interlace, comp, filter;
    png_get_IHDR(png, info, &w, &h, &bit_depth, &color_type, &interlace, &comp, &filter);

    if(bit_depth == 16) png_set_strip_16(png);
    if(color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if(png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);
    png_set_strip_alpha(png);

    png_read_update_info(png, info);

    const int channels = 3;
    std::vector<png_bytep> rows(h);
    const png_size_t rowbytes = png_get_rowbytes(png, info);
    std::vector<unsigned char> buffer(h * rowbytes);
    for(size_t y = 0; y < h; ++y) rows[y] = buffer.data() + y * rowbytes;

    png_read_image(png, rows.data());
    png_read_end(png, nullptr);

    png_destroy_read_struct(&png, &info, nullptr);
    std::fclose(fp);

    Image out((int) w, (int) h);
    std::vector<Pixel>& px = out.getPixels();
    for(size_t y = 0; y < h; ++y) {
        unsigned char* row = buffer.data() + y * rowbytes;
        for(size_t x = 0; x < w; ++x) {
            const unsigned char* p = row + x * channels;
            px[y * out.getW() + x] = Pixel(p[0] / 255.0, p[1] / 255.0, p[2] / 255.0);
        }
    }
    return out;
}

void Image::savePNG(const std::string& fichero, const Image& img) {
    Image img_ = clamp(img);

    FILE* fp = std::fopen(fichero.c_str(), "wb");
    if(!fp) throw std::runtime_error("No se puede abrir: \"" + fichero + "\"");

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png) { std::fclose(fp); throw std::runtime_error("Fallo en png_create_write_struct"); }

    png_infop info = png_create_info_struct(png);
    if(!info) { png_destroy_write_struct(&png, nullptr); std::fclose(fp); throw std::runtime_error("Fallo en png_create_info_struct"); }

    if(setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        std::fclose(fp);
        throw std::runtime_error("Error escribiendo PNG");
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, img_.getW(), img_.getH(), 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png, info);

    const int channels = 3;
    const png_size_t rowbytes = img_.getW() * channels;
    std::vector<unsigned char> buffer(img_.getH() * rowbytes);
    auto toUC = [](double v) -> unsigned char {
        if (v < 0.0) v = 0.0;
        else if (v > 1.0) v = 1.0;
        return static_cast<unsigned char>(std::lround(v * 255.0));
    };
    for(int y = 0; y < img_.getH(); ++y) {
        for(int x = 0; x < img_.getW(); ++x) {
            const Pixel& p = img_.at(x, y);
            unsigned char* dst = buffer.data() + y * rowbytes + x * channels;
            dst[0] = toUC(p.r);
            dst[1] = toUC(p.g);
            dst[2] = toUC(p.b);
        }
    }

    std::vector<png_bytep> rows(img_.getH());
    for(int y = 0; y < img_.getH(); ++y) rows[y] = buffer.data() + y * rowbytes;
    png_write_image(png, rows.data());
    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    std::fclose(fp);
}

Image Image::loadEXR(const std::string& fichero) {
    using namespace OPENEXR_IMF_NAMESPACE;
    using namespace IMATH_NAMESPACE;

    try {
        InputFile file(fichero.c_str());
        const Header& hdr = file.header();

        const Box2iAttribute* dwAttr  = hdr.findTypedAttribute<Box2iAttribute>("dataWindow");
        const Box2iAttribute* dswAttr = hdr.findTypedAttribute<Box2iAttribute>("displayWindow");
        if (!dwAttr && !dswAttr) throw std::runtime_error("EXR sin dataWindow/displayWindow en el header");

        const Box2i dw = dwAttr ? dwAttr->value() : dswAttr->value();
        const int w = dw.max.x - dw.min.x + 1;
        const int h = dw.max.y - dw.min.y + 1;

        std::vector<float> r(w * h), g(w * h), b(w * h);

        FrameBuffer fb;
        fb.insert("R", Slice(FLOAT,
                            (char*)(&r[0] - dw.min.x - dw.min.y * w),
                            sizeof(float), sizeof(float) * w, 1, 1, 0.0f));
        fb.insert("G", Slice(FLOAT,
                            (char*)(&g[0] - dw.min.x - dw.min.y * w),
                            sizeof(float), sizeof(float) * w, 1, 1, 0.0f));
        fb.insert("B", Slice(FLOAT,
                            (char*)(&b[0] - dw.min.x - dw.min.y * w),
                            sizeof(float), sizeof(float) * w, 1, 1, 0.0f));

        file.setFrameBuffer(fb);
        file.readPixels(dw.min.y, dw.max.y);

        std::vector<Pixel> pixels;
        pixels.reserve(w * h);
        for (int i = 0; i < w * h; ++i) pixels.emplace_back((double)r[i], (double)g[i], (double)b[i]);
        return Image(w, h, pixels);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error al leer EXR \"") + fichero + "\": " + e.what());
    }
}

void Image::saveEXR(const std::string& fichero, const Image& img) {
    using namespace OPENEXR_IMF_NAMESPACE;
    using namespace IMATH_NAMESPACE;

    try {
        const int w = img.getW();
        const int h = img.getH();

        std::vector<float> r(w * h), g(w * h), b(w * h);
        const auto& src = img.getPixels();
        for (int i = 0; i < w * h; ++i) {
            r[i] = (float)src[i].r;
            g[i] = (float)src[i].g;
            b[i] = (float)src[i].b;
        }

        Header header(w, h);
        header.channels().insert("R", Channel(FLOAT));
        header.channels().insert("G", Channel(FLOAT));
        header.channels().insert("B", Channel(FLOAT));

        OutputFile file(fichero.c_str(), header);

        FrameBuffer fb;
        fb.insert("R", Slice(FLOAT, (char*)r.data(), sizeof(float), sizeof(float) * w, 1, 1, 0.0f));
        fb.insert("G", Slice(FLOAT, (char*)g.data(), sizeof(float), sizeof(float) * w, 1, 1, 0.0f));
        fb.insert("B", Slice(FLOAT, (char*)b.data(), sizeof(float), sizeof(float) * w, 1, 1, 0.0f));

        file.setFrameBuffer(fb);
        file.writePixels(h);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error al escribir EXR \"") + fichero + "\": " + e.what());
    }
}

//---------------------------------------------------------------------------------------
/** Comparar dos imágenes */
int Image::compare(const Image& A, const Image& B, double& maxAbsErr, const double tol) {
    if (A.getW() != B.getW() || A.getH() != B.getH()) {
        std::cerr << "Dimensiones distintas: "
                  << A.getW() << "x" << A.getH() << " vs "
                  << B.getW() << "x" << B.getH() << "\n";
        return -1;
    }
    int mismatches = 0;
    maxAbsErr = 0.0;
    for (int y = 0; y < A.getH(); ++y) {
        for (int x = 0; x < A.getW(); ++x) {
            const Pixel& dif = A.at(x, y) - B.at(x, y);
            double emax = std::max({std::fabs(dif.r), std::fabs(dif.g), std::fabs(dif.b)});
            if(emax > tol) mismatches++;
            maxAbsErr = std::max(maxAbsErr, emax);
        }
    }
    return mismatches;
}
