#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include "utils.hpp"
#include <cassert>
#include <vecmath.h>
#include <cmath>
#include <glut.h>

// TODO (PA2, done): Copy from PA1

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        this->radius = 1.0f;
        this->center = Vector3f(0.0f);
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        //
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // 令 A 点为射线起点， AB 为射线单位向量， O 点为球心， C 点为 O 点到 AB 的垂足
        Vector3f pointA = r.getOrigin(), pointO = this->center;
        Vector3f vecAB = r.getDirection().normalized(), vecAO = pointO - pointA;
        Vector3f vecAC = vecAB * Vector3f::dot(vecAB, vecAO);
        Vector3f vecCO = pointO - (pointA + vecAC);
        float dis = vecCO.length();
        if (utils::sgn(dis - this->radius) > 0)
            return false;
        else if (utils::sgn(dis - this->radius) == 0) {
            // 相切，此时 C 点即交点
            float t = utils::calcT(r, pointA + vecAC);
            if (t < tmin || t > h.getT())
                return false;
            h.set(t, this->material, -vecCO.normalized());
            return true;
        } else {
            // 有两个交点，找最近的一个，令其为 D 点
            // NOTE: 暂时默认射线起点在球外部
            float lenAD = vecAC.length() - sqrt(this->radius * this->radius - vecCO.squaredLength());
            assert(utils::sgn(lenAD) > 0);
            Vector3f pointD = pointA + lenAD * vecAB;
            float t = utils::calcT(r, pointD);
            if (t < tmin || t > h.getT())
                return false;
            h.set(t, this->material, (pointD - pointO).normalized());
            return true;
        }
    }

    void drawGL() override {
        Object3D::drawGL();
        glMatrixMode(GL_MODELVIEW); glPushMatrix();
        glTranslatef(center.x(), center.y(), center.z());
        glutSolidSphere(radius, 80, 80);
        glPopMatrix();
    }

protected:
    Vector3f center;
    float radius;

};


#endif
