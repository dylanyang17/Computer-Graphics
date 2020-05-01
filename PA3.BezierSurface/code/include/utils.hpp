#ifndef UTILS_H
#define UTILS_H
#include <cmath>
#include <vecmath.h>

namespace utils {

    extern int sgn(float f);

    extern float calcT(const Ray &r, const Vector3f &p);

    extern float calcReLU(const float &f);

    extern int find_absmax_dim(const Vector3f &f);

    extern void printVector3f(const Vector3f &v);

}

#endif