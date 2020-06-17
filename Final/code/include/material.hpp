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

    explicit Material(const float refRatio, Vector3f emission, Vector3f color) :
            refRatio(refRatio), emission(emission), color(color) {
        ;
    }

    virtual ~Material() = default;

    float refRatio;
    Vector3f emission, color;
};


#endif // MATERIAL_H
