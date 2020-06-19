#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "utils.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const double diffuseRatio, bool isGlass, Vector3f emission, Vector3f color) :
            diffuseRatio(diffuseRatio), isGlass(isGlass), emission(emission), color(color) {
        ;
    }

    virtual ~Material() = default;

    double diffuseRatio;
    bool isGlass;
    Vector3f emission, color;
};


#endif // MATERIAL_H
