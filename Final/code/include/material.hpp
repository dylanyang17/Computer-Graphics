#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "utils.hpp"
#include "image.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const double diffuseRatio, bool isGlass, Vector3f emission, Vector3f color, Image *texture) :
            diffuseRatio(diffuseRatio), isGlass(isGlass), emission(emission), color(color), texture(texture) {
        ;
    }

    virtual ~Material() = default;

    double diffuseRatio;
    bool isGlass;
    Image *texture;

    Vector3f getEmission(Vector3f p) {
        return emission;
    }

    Vector3f getColor(Vector3f p) {
        if (!texture) return color;
        else {
            int w = texture->Width(), h = texture->Height();
            int x = (int)(p[0]*40-250)%w, y = (int)(p[1]*40)%h;
            if (x < 0) x += w;
            if (y < 0) y += h;
            return texture->GetPixel(x, y);
        }
    }

private:
    Vector3f emission, color;
};


#endif // MATERIAL_H
