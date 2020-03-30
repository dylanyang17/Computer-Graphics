#ifndef UTILS_H
#define UTILS_H
#include <cmath>
#include <vecmath.h>
#include "ray.hpp"

int sgn(float f);

float calcT(const Ray &r, const Vector3f &p);

float calcReLU(const float &f);

#endif