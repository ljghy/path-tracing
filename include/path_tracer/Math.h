#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>

#define PI 3.14159265359f
#define INV_PI 0.3183098861838f
#define EPSILON 1e-4f
#define ISZERO(x) (std::fabs(x) < EPSILON)
#define CLAMP(x, floor, ceil) std::min((std::max((x), (floor))), (ceil))
#define ERR_COLOR glm::vec3(1.f, 0.f, 1.f)

inline float randf(float a = 0.f, float b = 1.f) { return rand() * (b - a) / RAND_MAX + a; }
inline float sqr(float x) { return x * x; }

#endif