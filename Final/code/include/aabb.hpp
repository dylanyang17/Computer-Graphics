#ifndef AABB_H
#define AABB_H

#include "object3d.hpp"
#include "utils.hpp"
#include <vecmath.h>
#include <cmath>

#define MAXDOUBLE 1e123

// TODO: Implement functions and add more fields as necessary

class AABB : public Object3D {
public:
    // AABB() = delete;

    AABB() : Object3D(NULL) {
        ;
    }

    AABB(Material *material) : Object3D(material) {
        ;
    }

    AABB(const Vector3f &minp, const Vector3f &maxp, Material *material) : Object3D(material) {
        this->minp = minp;
        this->maxp = maxp;
    }

    bool isEmpty() {
        for (int i = 0; i < 3; ++i) 
            if (sgn(maxp[i]-minp[i]) <= 0) return true;
        return false;
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        // 保证射线起点在包围盒之外
        double minT = tmin, maxT = MAXDOUBLE;  // t 需要在 [minT, maxT] 之间才能使得射线与其相交
        Vector3f d = r.getDirection(), o = r.getOrigin();
        Vector3f norm = Vector3f::ZERO;
        for (int i = 0; i < 3; ++i) {
            if (sgn(d[i]) == 0) {
                if (sgn(o[i] - minp[i]) > 0 && sgn(o[i] - maxp[i]) < 0) continue;
                else return false;
            } else {
                double tmp1 = (minp[i]-o[i])/d[i], tmp2 = (maxp[i]-o[i])/d[i];
                if (tmp1 > tmp2) swap(tmp1, tmp2);
                if (tmp1 > minT) {
                    minT = tmp1;
                    norm = Vector3f::ZERO;
                    norm[i] = ((d[i]>0) ? -1 : 1);
                }
                maxT = min(maxT, tmp2);
            }
        }
        if (sgn(maxT - minT) <= 0 || minT > h.getT()) return false;
        h.set(minT, material, norm);
        return true;
    }

    AABB operator| (const AABB &b) const {
        AABB ret = AABB(material);
        for (int i = 0; i < 3; ++i) {
            ret.minp[i] = std::min(b.minp[i], this->minp[i]);
            ret.maxp[i] = std::max(b.maxp[i], this->maxp[i]);
        }
        return ret;
    }

    Vector3f maxp, minp;
};


#endif
