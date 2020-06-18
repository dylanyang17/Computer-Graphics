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
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction).normalized();
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
            const Vector3f &up, int imgW, int imgH, double angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        // 注意透视相机模型中，有 tan(angle/2) = (imgW/2.0/fx) / 1
        // 另外这里 cx 和 cy 分别为 imgW/2.0 和 imgH/2.0
        this->angleW = angle;
        this->angleH = angle * imgH / imgW;
        fx = imgW / 2.0 / tan(angleW/2);
        fy = imgH / 2.0 / tan(angleH/2);
    }

    Ray generateRay(const Vector2f &point) override {
        // 
        Vector3f vec = Vector3f((point[0]-this->width/2.0)/fx, (point[1]-this->height/2.0)/fy, 1).normalized();
        Matrix3f R = Matrix3f(this->horizontal, this->up, this->direction);
        vec = R * vec;
        //if (point[0] >= 98 && point[0] <= 102)
        //    printf("(%.1f,%.1f): (%.3f,%.3f,%.3f) \n",point[0],point[1],vec[0],vec[1],vec[2]);
        return Ray(this->center, vec.normalized());
    }

private:
    // fx 和 fy 为图像空间到真实世界空间的尺度参数
    // angleW 和 angleH 为宽和高方向的视角
    double fx, fy, angleW, angleH;
};

class LensCamera : public Camera {
public:
    LensCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, int scale, double focalDist, double imageDist, double aperture) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        // 注意透视相机模型中，有 tan(angle/2) = (imgW/2.0/fx) / 1
        // 另外这里 cx 和 cy 分别为 imgW/2.0 和 imgH/2.0
        this->scale = scale;
        this->focalDist = focalDist;
        this->imageDist = imageDist;
        this->aperture = aperture;
        Xi[0] = time(NULL), Xi[1] = clock(), Xi[2] = imgW * imgH;
    }

    Ray generateRay(const Vector2f &point) override {
        // 注意这里避免了成倒像
        Vector2f revPoint = Vector2f(this->width-point[0], this->height-point[1]);
        double objectDist = 1 / (1/focalDist - 1/imageDist);
        // u, v 为透镜所在圆上两单位向量, A 点为像平面上对应点，B 为随机出的圆上的点，C 为对焦物平面上的对应点
        Vector3f lensCenter = center + direction * imageDist, u = this->up, v = this->horizontal;
        Vector3f pointA = center + (revPoint[0]-this->width/2.0)/scale*v + (revPoint[1]-this->height/2.0)/scale*u;
        // 该随机方法是否有问题？
        double tmp = erand48(Xi), r = tmp * tmp * aperture, theta = 2 * M_PI * erand48(Xi);
        Vector3f pointB = lensCenter + r*sin(theta)*u + r*cos(theta)*v;
        Vector3f pointC = lensCenter + (lensCenter - pointA) * (objectDist / imageDist);
        return Ray(pointB, (pointC-pointB).normalized());
    }

private:
    // scale 为实际像平面大小到图像大小的缩放尺度，值越大则视角越小
    int scale;
    // focalDist 为焦距，imageDist 为成像平面到透镜的距离，aperture 为光圈大小，即透镜实际可透光的半径
    // center 为像平面中心
    double focalDist, imageDist, aperture;
    unsigned short Xi[3];
};

#endif //CAMERA_H
