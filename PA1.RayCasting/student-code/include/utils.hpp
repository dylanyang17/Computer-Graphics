#ifndef UTILS_H
#define UTILS_H
#include <cmath>
#include <vecmath.h>

int sgn(float f) {
    if (fabs(f) < 1e-10)
        return 0;
    else if (f > 0)
        return 1;
    else
        return -1;
}

float calcT(const Ray &r, const Vector3f &p) {
    // 保证 p 在 r 所在直线上，计算 t 值
    Vector3f tmpv1 = r.getDirection(), tmpv2 = p - r.getOrigin();
    assert(sgn(Vector3f::dot(tmpv1, tmpv2) - tmpv1.length()*tmpv2.length()) == 0);
    return tmpv2[0] / tmpv1[0];
}

float calcReLU(const float &f) {
    return ((f > 0) ? f : 0); 
}

#endif