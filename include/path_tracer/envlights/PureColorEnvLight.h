#ifndef PURE_COLOR_ENV_LIGHT_H
#define PURE_COLOR_ENV_LIGHT_H

#include "path_tracer/EnvLight.h"
class PureColorEnvLight : public EnvLight
{
protected:
    glm::vec3 m_color;

public:
    PureColorEnvLight(const glm::vec3 &c) : m_color(c) {}
    virtual glm::vec3 operator()(const glm::vec3 &) const override { return m_color; }
};

#endif