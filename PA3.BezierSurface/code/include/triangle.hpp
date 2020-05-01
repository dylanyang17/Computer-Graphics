#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include "utils.hpp"
#include "plane.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;

// TODO (PA2, done): Copy from PA1
class Triangle: public Object3D
{

public:
	Triangle() = delete;
        ///@param a b c are three vertex positions of the triangle

	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		this->plane = Plane(Vector3f::cross(b-a, c-a).normalized(), a, m);
		this->a = a;
		this->b = b;
		this->c = c;
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
        // T 点为射线与三角形所在平面交点
		Hit tmph = hit;
		bool flag = this->plane.intersect(ray, tmph, tmin);
		if (flag == false)
			return false;
		Vector3f pointT = ray.pointAtParameter(tmph.getT());
		// 判断两次，判断 AT 是否在 AB 与 AC 之间，判断 BT 是否在 BA 与 BC 之间
		Vector3f vecAB = this->b - this->a, vecAC = this->c - this->a, vecBA = -vecAB;
		Vector3f vecBC = this->c - this->b, vecAT = pointT - this->a, vecBT = pointT - this->b;
		float tmp = Vector3f::dot(Vector3f::cross(vecAB, vecAT), Vector3f::cross(vecAT, vecAC));
		if (utils::sgn(tmp) < 0)
			return false;
		// utils::sgn(tmp)>=0. AT 在 AB 与 AC 之间（tmp==0 时表示 T 在 AB 或 AC 上）
		tmp = Vector3f::dot(Vector3f::cross(vecBA, vecBT), Vector3f::cross(vecBT, vecBC));
		if (utils::sgn(tmp) < 0)
			return false;
		hit = tmph;
		return true;
	}
	Vector3f normal;
	Vector3f vertices[3];

    void drawGL() override {
        Object3D::drawGL();
        glBegin(GL_TRIANGLES);
        glNormal3fv(normal);
        glVertex3fv(vertices[0]); glVertex3fv(vertices[1]); glVertex3fv(vertices[2]);
        glEnd();
    }

protected:
    Plane plane;
	Vector3f a, b, c;
};

#endif //TRIANGLE_H
