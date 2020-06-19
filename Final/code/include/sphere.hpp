#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include "utils.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        this->radius = 1.0f;
        this->center = Vector3f(0.0f);
    }

    Sphere(const Vector3f &center, double radius, Material *material) : Object3D(material) {
        // 
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        // 令 A 点为射线起点， AB 为射线单位向量， O 点为球心， C 点为 O 点到 AB 的垂足
        Vector3f pointA = r.getOrigin(), pointO = this->center;
        Vector3f vecAB = r.getDirection().normalized(), vecAO = pointO - pointA;
        Vector3f vecAC = vecAB * Vector3f::dot(vecAB, vecAO);
        Vector3f vecCO = pointO - (pointA + vecAC);
        double dis = vecCO.length();
        // // 射线起点在圆上或圆内
        // if (sgn(vecAO.length() - this->radius) <= 0)
        //     return false;
        // // 射线方向与到圆心的方向相反
        // if (sgn(Vector3f::dot(vecAB, vecAO)) <= 0)
        //     return false;
        // 射线所在直线与圆无交点
        if (sgn(dis - this->radius) > 0)
            return false;
        else if (sgn(dis - this->radius) == 0) {
            // 相切，此时 C 点即交点
            if (sgn(vecAC.length()-radius) == 0) {
                // 排除起点即 C 点的情况
                return false;
            }
            double t = calcT(r, pointA + vecAC);
            if (t < tmin || t > h.getT())
                return false;
            h.set(t, this->material, -vecCO.normalized());
            return true;
        } else {
            // 有两个交点，找满足 t>tmin 的最近的一个，令其为 D 点
            double tmp = sqrt(this->radius * this->radius - vecCO.squaredLength());
            double t = vecAC.length() * (Vector3f::dot(vecAC, vecAB)>0 ? 1 : -1) - tmp;  // 特别的，这是有向长度
            if (t < tmin) {
                // 第一个点不符合，找另一个点
                t += 2 * tmp;
                if (t < tmin || t > h.getT()) {
                    return false;  // 均不符合
                }
            } else if (t > h.getT()) {
                return false;
            }
            Vector3f pointD = r.pointAtParameter(t);
            Vector3f vecOD = pointD-pointO, vecAD = pointD-pointA;
            bool into = Vector3f::dot(vecOD, vecAD)>0 ? false : true;
            h.set(t, this->material, (vecOD * (into ? 1 : -1)).normalized(), into);
            return true;
        }
    }

protected:
    Vector3f center;
    double radius;
};


#endif
