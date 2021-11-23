#include "path_tracer/Bitmap.h"
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi/stb_image.h"
#include "stbi/stb_image_write.h"
Bitmap::Bitmap(uint16_t _w, uint16_t _h, uint8_t _c)
	: m_w(_w), m_h(_h), m_channels(_c), m_buffer(nullptr)
{
	assert(_c > 0 && _c < 5);
	if (_w * _h > 0)
		m_buffer = new float[_w * _h * _c];
}

Bitmap::Bitmap(const std::string &filename, int channels, bool degamma)
	: m_buffer(nullptr)
{
	int x, y, c;
	stbi_uc *buf = stbi_load(filename.c_str(), &x, &y, &c, channels);
	assert(buf != nullptr);
	m_w = x;
	m_h = y;
	m_channels = 3;

	size_t s = m_w * m_h * 3;
	m_buffer = new float[s];
	if (degamma)
	{
		float table[256];
		for (uint16_t i = 0; i < 256; ++i)
			table[i] = powf(i / 255.f, 2.2f);
		for (size_t i = 0; i < s; ++i)
			m_buffer[i] = table[buf[i]];
	}
	else
	{
		for (size_t i = 0; i < s; ++i)
			m_buffer[i] = buf[i] / 255.f;
	}
	stbi_image_free(buf);
}

Bitmap::Bitmap(uint16_t _w, uint16_t _h, const glm::vec3 &color)
	: m_w(_w), m_h(_h), m_channels(3), m_buffer(nullptr)
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

void Bitmap::setPixel(uint16_t r, uint16_t c, const glm::vec3 &color)
{
	assert(m_channels == 3);
	size_t i = 3 * (r * m_w + c);
	m_buffer[i] = color.r;
	m_buffer[i + 1] = color.g;
	m_buffer[i + 2] = color.b;
}

void Bitmap::addPixel(uint16_t r, uint16_t c, const glm::vec3 &color)
{
	assert(m_channels == 3);
	size_t i = 3 * (r * m_w + c);
	m_buffer[i] += color.r;
	m_buffer[i + 1] += color.g;
	m_buffer[i + 2] += color.b;
}

void Bitmap::outputPNG(const std::string &filename, float scale, bool gamma) const
{
	size_t bufSize = m_w * m_h * m_channels;
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
		stbi_write_png(filename.c_str(), m_w, m_h, m_channels, tmpBuf, m_channels * m_w);
		delete[] tmpBuf;
	}
}

glm::vec3 Bitmap::sample(float u, float v) const
{
	assert(m_channels == 3);
	uint16_t r = CLAMP(int(m_h * v + 0.5f), 0, m_h - 1);
	uint16_t c = CLAMP(int(m_w * u + 0.5f), 0, m_w - 1);
	size_t i = 3 * (r * m_w + c);
	return glm::vec3(m_buffer[i], m_buffer[i + 1], m_buffer[i + 2]);
}