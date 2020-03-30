#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        // 注意透视相机模型中，有 tan(angle/2) = (imgW/2.0/fx) / 1
        // 另外这里 cx 和 cy 分别为 imgW/2.0 和 imgH/2.0
        fx = imgW / 2.0 / tan(angle/2);
        fy = imgH / 2.0 / tan(angle/2);
        this->angle = angle;
    }

    Ray generateRay(const Vector2f &point) override {
        // 
        Vector3f vec = Vector3f((point[0]-this->width/2.0)/fx, (point[1]-this->height/2.0)/fy, 1).normalized();
        Matrix3f R = Matrix3f(this->horizontal, -this->up, this->direction);
        vec = R * vec;
        return Ray(this->center, vec.normalized());
    }

private:
    // fx 和 fy 为图像空间到真实世界空间的尺度参数
    float fx, fy, angle;
};

#endif //CAMERA_H
