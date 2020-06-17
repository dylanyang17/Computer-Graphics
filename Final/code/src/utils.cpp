#include "utils.hpp"

int sgn(double f) {
    if (fabs(f) < 1e-13)
        return 0;
    else if (f > 0)
        return 1;
    else
        return -1;
}

double calcT(const Ray &r, const Vector3f &p) {
    // 保证 p 在 r 所在直线上，计算 t 值
    // DEBUG: 注意不能随便选择一维，例如当 direction 为 (0,0,1) 而 tmpv2 为 (0,0,5) 时选择第一维便会出错
    Vector3f tmpv1 = r.getDirection(), tmpv2 = p - r.getOrigin();
    assert(sgn(Vector3f::dot(tmpv1, tmpv2) - tmpv1.length()*tmpv2.length()) == 0);
    assert(sgn(tmpv1.length()) != 0);
    int w = ((fabs(tmpv1[0]) > fabs(tmpv1[1])) ? 0 : 1);
    w = ((fabs(tmpv1[w]) > fabs(tmpv1[2])) ? w : 2);
    return tmpv2[w] / tmpv1[w];
}

double calcReLU(const double &f) {
    return ((f > 0) ? f : 0); 
}

void printVector3f(const Vector3f &v) {
    printf("(%.2f,%.2f,%.2f)", v[0], v[1], v[2]);
}