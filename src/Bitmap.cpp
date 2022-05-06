#include "path_tracer/Bitmap.h"
#include <cassert>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi/stb_image.h"
#include "stbi/stb_image_write.h"
Bitmap::Bitmap(uint16_t _w, uint16_t _h)
    : m_w(_w), m_h(_h), m_buffer(nullptr)
{
    if (_w * _h > 0)
        m_buffer = new float[_w * _h * 3];
}

Bitmap::Bitmap(const std::string &filename, bool SRGB)
    : m_buffer(nullptr)
{
    int x, y, c;
    stbi_uc *buf = stbi_load(filename.c_str(), &x, &y, &c, 3);
    assert(buf != nullptr && c == 3);
    m_w = x;
    m_h = y;

    size_t s = m_w * m_h * 3;
    m_buffer = new float[s];
    if (SRGB)
    {
        float table[256];
        for (uint16_t i = 0; i < 256; ++i)
            table[i] = powf(i / 255.f, 2.2f);
        for (size_t i = 0; i < s; ++i)
            m_buffer[i] = table[buf[i]];
    }
    else
    {
        float table[256];
        for (uint16_t i = 0; i < 256; ++i)
            table[i] = i / 255.f;
        for (size_t i = 0; i < s; ++i)
            m_buffer[i] = table[buf[i]];
    }
    stbi_image_free(buf);
}

Bitmap::Bitmap(const Bitmap &other)
    : m_w(other.m_w), m_h(other.m_h)
{
    size_t s = m_w * m_h * 3;
    m_buffer = new float[s];
    std::copy(other.m_buffer, other.m_buffer + s, m_buffer);
}

Bitmap::Bitmap(uint16_t _w, uint16_t _h, const glm::vec3 &color)
    : m_w(_w), m_h(_h), m_buffer(nullptr)
{
    if (_w * _h > 0)
        m_buffer = new float[_w * _h * 3];
    for (size_t i = 0; i < _w * _h * 3; i += 3)
    {
        m_buffer[i] = color.r;
        m_buffer[i + 1] = color.g;
        m_buffer[i + 2] = color.b;
    }
}

Bitmap::~Bitmap()
{
    delete[] m_buffer;
}

const glm::vec3 Bitmap::getPixel(uint16_t r, uint16_t c) const
{
    size_t i = 3 * (r * m_w + c);
    return glm::vec3(m_buffer[i], m_buffer[i + 1], m_buffer[i + 2]);
}

void Bitmap::setPixel(uint16_t r, uint16_t c, const glm::vec3 &color)
{
    size_t i = 3 * (r * m_w + c);
    m_buffer[i] = color.r;
    m_buffer[i + 1] = color.g;
    m_buffer[i + 2] = color.b;
}

void Bitmap::addPixel(uint16_t r, uint16_t c, const glm::vec3 &color)
{
    size_t i = 3 * (r * m_w + c);
    m_buffer[i] += color.r;
    m_buffer[i + 1] += color.g;
    m_buffer[i + 2] += color.b;
}

void Bitmap::outputPNG(const std::string &filename, float scale, bool gamma) const
{
    size_t bufSize = m_w * m_h * 3;
    if (bufSize)
    {
        stbi_uc *tmpBuf = new stbi_uc[bufSize];
        if (gamma)
        {
            for (size_t i = 0; i < bufSize; ++i)
                tmpBuf[i] = (stbi_uc)(255 * CLAMP(std::pow(scale * m_buffer[i], 1.f / 2.2f), 0.f, 1.f));
        }
        else
        {
            for (size_t i = 0; i < bufSize; ++i)
                tmpBuf[i] = (stbi_uc)(255 * CLAMP(scale * m_buffer[i], 0.f, 1.f));
        }
        stbi_write_png(filename.c_str(), m_w, m_h, 3, tmpBuf, 3 * m_w);
        delete[] tmpBuf;
    }
}

glm::vec3 Bitmap::sample(float u, float v) const
{
    uint16_t r = CLAMP(int(m_h * v + 0.5f), 0, m_h - 1);
    uint16_t c = CLAMP(int(m_w * u + 0.5f), 0, m_w - 1);
    size_t i = 3 * (r * m_w + c);
    return glm::vec3(m_buffer[i], m_buffer[i + 1], m_buffer[i + 2]);
}

void Bitmap::bloom(uint16_t kernelSize, float luminanceThreshold)
{
    Bitmap bloomMap(*this);
    Bitmap tb(m_w, m_h);
    size_t s = m_w * m_h;
    float *alpha = new float[s];
    float *ta = new float[s];

    for (size_t i = 0; i < s; ++i)
        if (0.299f * m_buffer[3 * i] + 0.587f * m_buffer[3 * i + 1] + 0.114f * m_buffer[3 * i + 2] < luminanceThreshold)
            alpha[i] = bloomMap.m_buffer[3 * i] = bloomMap.m_buffer[3 * i + 1] = bloomMap.m_buffer[3 * i + 2] = 0.f;
        else
            alpha[i] = 1.f;

    float isigma2 = sqr(3.f / kernelSize);

    for (uint16_t w = 0; w < m_w; ++w)
        for (uint16_t h = 0; h < m_h; ++h)
        {
            float weights = 0.f;
            glm::vec3 color(0.f, 0.f, 0.f);
            float a = 0.f;
            for (int16_t i = std::clamp(w - kernelSize, 0, static_cast<int>(m_w) - 1); i <= std::clamp(w + kernelSize, 0, static_cast<int>(m_w) - 1); ++i)
            {
                float weight = sqrt(0.5f * INV_PI * isigma2) * exp(-(i - w) * (i - w) * 0.5f * isigma2);
                weights += weight;
                color += weight * bloomMap.getPixel(h, i);
                a += weight * alpha[m_w * h + i];
            }
            tb.setPixel(h, w, color / weights);
            ta[m_w * h + w] = a;
        }

    for (uint16_t w = 0; w < m_w; ++w)
        for (uint16_t h = 0; h < m_h; ++h)
        {
            float weights = 0.f;
            glm::vec3 color(0.f, 0.f, 0.f);
            float a = 0.f;
            for (int16_t i = std::clamp(h - kernelSize, 0, static_cast<int>(m_h) - 1); i <= std::clamp(h + kernelSize, 0, static_cast<int>(m_h) - 1); ++i)
            {
                float weight = sqrt(0.5f * INV_PI * isigma2) * exp(-(i - h) * (i - h) * 0.5f * isigma2);
                weights += weight;
                color += weight * tb.getPixel(i, w);
                a += weight * ta[m_w * i + w];
            }
            bloomMap.setPixel(h, w, color / weights);
            alpha[m_w * h + w] = a;
        }

    for (uint16_t w = 0; w < m_w; ++w)
        for (uint16_t h = 0; h < m_h; ++h)
        {
            addPixel(h, w, alpha[h * m_w + w] * bloomMap.getPixel(h, w));
        }

    delete[] ta;
    delete[] alpha;
}

void Bitmap::hdr(float exposure)
{
    size_t s = m_w * m_h * 3;
    for (size_t i = 0; i < s; ++i)
        hdrMapping(m_buffer[i], exposure);
}

void Bitmap::blur(uint16_t kernelSize)
{
    Bitmap tb(m_w, m_h);
    size_t s = m_w * m_h;

    float isigma2 = sqr(3.f / kernelSize);

    for (uint16_t w = 0; w < m_w; ++w)
        for (uint16_t h = 0; h < m_h; ++h)
        {
            float weights = 0.f;
            glm::vec3 color(0.f, 0.f, 0.f);
            float a = 0.f;
            for (int16_t i = std::clamp(w - kernelSize, 0, static_cast<int>(m_w) - 1); i <= std::clamp(w + kernelSize, 0, static_cast<int>(m_w) - 1); ++i)
            {
                float weight = sqrt(0.5f * INV_PI * isigma2) * exp(-(i - w) * (i - w) * 0.5f * isigma2);
                weights += weight;
                color += weight * getPixel(h, i);
            }
            tb.setPixel(h, w, color / weights);
        }

    for (uint16_t w = 0; w < m_w; ++w)
        for (uint16_t h = 0; h < m_h; ++h)
        {
            float weights = 0.f;
            glm::vec3 color(0.f, 0.f, 0.f);
            float a = 0.f;
            for (int16_t i = std::clamp(h - kernelSize, 0, static_cast<int>(m_h) - 1); i <= std::clamp(h + kernelSize, 0, static_cast<int>(m_h) - 1); ++i)
            {
                float weight = sqrt(0.5f * INV_PI * isigma2) * exp(-(i - h) * (i - h) * 0.5f * isigma2);
                weights += weight;
                color += weight * tb.getPixel(i, w);
            }
            setPixel(h, w, color / weights);
        }
}