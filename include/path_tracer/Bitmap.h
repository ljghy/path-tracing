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
    uint8_t m_channels;
    float *m_buffer;

public:
    Bitmap(uint16_t _w, uint16_t _h, uint8_t _c = 3);
    Bitmap(const std::string &, int = 0, bool = false);
    Bitmap(uint16_t _w, uint16_t _h, const glm::vec3 &color);
    Bitmap(const Bitmap &) = delete;
    Bitmap &operator=(const Bitmap &) = delete;
    virtual ~Bitmap();
    uint16_t w() const { return m_w; }
    uint16_t h() const { return m_h; }
    void setPixel(uint16_t r, uint16_t c, const glm::vec3 &color);
    void addPixel(uint16_t r, uint16_t c, const glm::vec3 &color);

    glm::vec3 sample(float u, float v) const;

    void outputPNG(const std::string &, float scale = 1.0, bool gamma = false) const;
};
#endif