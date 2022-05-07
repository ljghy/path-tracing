#ifndef BOX_H
#define BOX_H

#include "path_tracer/Object.h"
#include "path_tracer/objects/Rectangle.h"

class Box : public Object
{
protected:
    Rectangle m_sides[6];
    /*
    xy bottom
    xy top
    xz left
    xz right
    yz front
    yz back
    */

public:
    Box(const glm::vec3 &minCorner, const glm::vec3 &maxCorner, glm::vec3 xAxis, glm::vec3 yAxis,
        Material *_mat = nullptr, bool _invNorm = false)
        : Object(_mat, _invNorm)
    {
        xAxis = glm::normalize(xAxis);
        yAxis = glm::normalize(yAxis - glm::dot(xAxis, yAxis) * xAxis);
        glm::vec3 zAxis = glm::cross(xAxis, yAxis);
        glm::vec3 l = maxCorner - minCorner;
        glm::vec3 h = 0.5f * l;
        glm::vec3 c = 0.5f * (maxCorner + minCorner);
        m_sides[0] = Rectangle(c - h.z * zAxis, -zAxis, xAxis, l.x, l.y, _mat, _invNorm);
        m_sides[1] = Rectangle(c + h.z * zAxis, zAxis, xAxis, l.x, l.y, _mat, _invNorm);
        m_sides[2] = Rectangle(c - h.y * yAxis, -yAxis, xAxis, l.x, l.z, _mat, _invNorm);
        m_sides[3] = Rectangle(c + h.y * yAxis, yAxis, xAxis, l.x, l.z, _mat, _invNorm);
        m_sides[4] = Rectangle(c - h.x * xAxis, -xAxis, yAxis, l.y, l.z, _mat, _invNorm);
        m_sides[5] = Rectangle(c + h.x * xAxis, xAxis, yAxis, l.y, l.z, _mat, _invNorm);
    }
    void rayIntersection(const Ray &r, IntersectionInfo &info) const
    {
        info.t = INFINITY;
        info.happen = false;
        IntersectionInfo sideInfo;
        for (const auto &side : m_sides)
        {
            side.rayIntersection(r, sideInfo);
            if (sideInfo.happen && sideInfo.t < info.t)
                info = sideInfo;
        }
    }
};

#endif