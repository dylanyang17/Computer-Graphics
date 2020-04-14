#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>
#include "utils.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <glut.h>

// TODO (PA2, done): Copy from PA1.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        // 令 T 为交点。 注意 dirToLight 为交点到光源方向的向量
        Vector3f pointT = ray.pointAtParameter(hit.getT());
        Vector3f vecN = hit.getNormal().normalized(), vecLx = dirToLight.normalized();
        Vector3f vecV = -ray.getDirection().normalized();
        Vector3f vecRx = 2 * Vector3f::dot(vecLx, vecN) * vecN - vecLx;
        return (this->diffuseColor * utils::calcReLU(Vector3f::dot(vecLx, vecN)) +
               this->specularColor * pow(utils::calcReLU(Vector3f::dot(vecV, vecRx)), this->shininess)) * lightColor;
    }

    // For OpenGL, this is fully implemented
    void Use() {
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Vector4f(diffuseColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Vector4f(specularColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, Vector2f(shininess * 4.0, 1.0f));
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
};


#endif // MATERIAL_H
