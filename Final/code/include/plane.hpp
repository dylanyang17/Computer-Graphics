#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include "utils.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions
// 使用点+单位法向量的方式表示

class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &normal, const Vector3f &p, Material *m) : Object3D(m) {
        this->normal = normal.normalized();
        this->p = p;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        // 令射线起点为 A， 射线单位向量为 AB， A 到平面的垂足为 C, 平面上某点为 P, 交点为 T
        Vector3f pointA = r.getOrigin(), vecAP = this->p - pointA, vecAB = r.getDirection().normalized();
        Vector3f vecAC = this->normal * Vector3f::dot(vecAP, this->normal);
        double cs = Vector3f::dot(vecAC, vecAB) / vecAC.length() / vecAB.length();
        if (sgn(vecAC.length()) == 0) {
            // 射线起点在面上
            return false;
        }
        if (sgn(cs) == 0) {
            // AB 所在直线与平面平行
            return false;
        }
        Vector3f pointT = pointA + vecAB * (vecAC.length() / cs);
        double t = calcT(r, pointT);
        if (t < tmin || t > h.getT())
            return false;
        Vector3f n = this->normal;
        if (Vector3f::dot(this->normal, vecAP) > 0)
            n = -n;
        h.set(t, this->material, n);
        return true;
    }

protected:
    Vector3f normal, p;

};

#endif //PLANE_H
		

