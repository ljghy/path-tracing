#ifndef LIGHT_H
#define LIGHT_H
#include "path_tracer/Object.h"

class Light : public Object
{
public:
	Light() {}
	float area;

	virtual float getArea() = 0;
	virtual glm::vec3 sampleLight() = 0;
};
#endif