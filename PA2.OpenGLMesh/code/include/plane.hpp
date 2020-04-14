#ifndef PLANE_H
#define PLANE_H

#include "utils.hpp"
#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO (PA2, done): Copy from PA1
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions
// 使用点+单位法向量的方式表示

class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &norm, float d, Material *m) : Object3D(m) {
        this->norm = norm.normalized();
        this->d = d;
        int dim = utils::find_absmax_dim(norm);  // 这一维一定不为 0
        this->d = d * this->norm[dim] / norm[dim];
        // 为了兼容 PA1，所以计算出一个在平面上的点 p
        this->p = Vector3f(0, 0, 0);
        this->p[dim] = this->d / this->norm[dim];
    }

    Plane(const Vector3f &norm, Vector3f p, Material *m) : Object3D(m) {
        this->norm = norm.normalized();
        this->p = p;
        // 为了兼容 PA2，计算 d
        this->d = Vector3f::dot(this->norm, this->p);
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // 令射线起点为 A， 射线单位向量为 AB， A 到平面的垂足为 C, 平面上某点为 P, 交点为 T
        Vector3f pointA = r.getOrigin(), vecAP = this->p - pointA, vecAB = r.getDirection().normalized();
        Vector3f vecAC = this->norm * Vector3f::dot(vecAP, this->norm);
        float cs = Vector3f::dot(vecAC, vecAB) / vecAC.length() / vecAB.length();
        if (utils::sgn(cs) == 0) {
            // AB 所在直线与平面平行
            return false;
        }
        Vector3f pointT = pointA + vecAB * (vecAC.length() / cs);
        float t = utils::calcT(r, pointT);
        if (t < tmin || t > h.getT())
            return false;
        Vector3f n = this->norm;
        if (Vector3f::dot(this->norm, vecAP) > 0)
            n = -n;
        h.set(t, this->material, n);
        return true;
    }

    void drawGL() override {
        Object3D::drawGL();
        Vector3f xAxis = Vector3f::RIGHT;
        Vector3f yAxis = Vector3f::cross(norm, xAxis);
        xAxis = Vector3f::cross(yAxis, norm);
        const float planeSize = 10.0;
        glBegin(GL_TRIANGLES);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glVertex3fv(d * norm + planeSize * xAxis - planeSize * yAxis);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glEnd();
    }

protected:
    Vector3f norm, p;
    float d;

};

#endif //PLANE_H
		

