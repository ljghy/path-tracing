#ifndef BITMAP_H
#define BITMAP_H
#include <string>
#include <cstdint>
#include "path_tracer/Math.h"

class Bitmap
{
protected:
    uint16_t m_w;
    uint16_t m_h;
    float *m_buffer;

public:
    Bitmap(uint16_t _w, uint16_t _h);
    Bitmap(const std::string &, bool = false);
    Bitmap(uint16_t _w, uint16_t _h, const glm::vec3 &color);
    Bitmap(const Bitmap &);
    Bitmap &operator=(const Bitmap &) = delete;
    virtual ~Bitmap();

    uint16_t w() const { return m_w; }
    uint16_t h() const { return m_h; }
    const glm::vec3 getPixel(uint16_t r, uint16_t c) const;
    void setPixel(uint16_t r, uint16_t c, const glm::vec3 &color);
    void addPixel(uint16_t r, uint16_t c, const glm::vec3 &color);

    void bloom(uint16_t kernelSize, float luminanceThreshold);
    void hdr(float exposure = 1.f);

    static void hdrMapping(float &c, float exposure = 1.f) { c = 1.f - exp(-c * exposure); }
    static void hdrMapping(glm::vec3 &c, float exposure = 1.f) { c = glm::vec3(1.f) - glm::exp(-c * exposure); }

    glm::vec3 sample(float u, float v) const;

    void outputPNG(const std::string &, float scale = 1.0, bool gamma = false) const;
};
#endif