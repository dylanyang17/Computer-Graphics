#ifndef UTILS_H
#define UTILS_H
#include <cmath>
#include <vecmath.h>
#include "ray.hpp"

int sgn(double f);

double calcT(const Ray &r, const Vector3f &p);

double calcReLU(const double &f);

void printVector3f(const Vector3f &f);

#endif