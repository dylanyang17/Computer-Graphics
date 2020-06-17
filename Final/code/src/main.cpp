#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>
#include <cmath>

using namespace std;

inline float clamp(float x) {
    return x < 0 ? 0 : x > 1 ? 1 : x;
}

inline int toInt(float x) {
    return int(pow(clamp(x), 1/2.2)*255 + 0.5);
}

Vector3f gammaCorrection(const Vector3f &color) {
    // TODO
    // return Vector3f();
}

Vector3f radiance(int ttx, int tty, int tts, const Ray &r, int depth, unsigned short *Xi, Group *group, const Vector3f background) {
    // TODO: 检查反射时是否会与反射处物体相交
    Hit hit;
    bool isIntersect = group->intersect(r, hit, 1e-4);
    if (!isIntersect)
        return background;
    Material *m = hit.getMaterial();
    // 令单位入射向量为 in，单位法向为 norm
    Vector3f f = m->color;
    Vector3f ret = m->emission;
    Vector3f hitPoint = r.pointAtParameter(hit.getT());
    Vector3f norm = hit.getNormal();
    float refRatio = m->refRatio;
    float p = f.x()>f.y() && f.x()>f.z() ? f.x() : f.y()>f.z() ? f.y() : f.z();
    if (depth > 5) if (erand48(Xi) < min((double)p, 0.9)) f = f/p; else return ret;  // 轮盘赌
    int only = 0;  // 为 0 时表示两个部分都需要计算，为 1 时表示仅计算镜面反射部分，为 2 时表示仅计算漫反射部分
    if (depth > 2) only = (erand48(Xi) < refRatio) ? 1 : 2;
    Vector3f sum = Vector3f::ZERO;  // 存储从该交点出发之后的颜色效果总和
    if ((only == 0 || only == 1) && refRatio > 1e-5) {
        // 镜面反射部分
        Vector3f in = r.getDirection().normalized();
        Vector3f newDir = (in + Vector3f::dot(in, norm)*2*norm).normalized();
        Vector3f tmp = radiance(ttx, tty, tts, Ray(hitPoint, newDir), depth+1, Xi, group, background);
        if (only == 0) tmp = tmp * refRatio;
        sum = sum + tmp;
    }
    if ((only == 0 || only == 2) && refRatio < 1-1e-5) {
        // 漫反射部分
        // u, v, w 为一组正交基，其中 w 即交点处法向 norm
        // r2s 即单位反射向量在 uv 平面上的投影长度
        float r1 = 2*M_PI*erand48(Xi), r2=erand48(Xi), r2s=sqrt(r2);
        Vector3f w = norm, u = Vector3f::cross(fabs(w.x())>0.1 ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0), w).normalized(),
            v = Vector3f::cross(w, u);
        Vector3f newDir = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).normalized();
        Vector3f tmp = radiance(ttx, tty, tts, Ray(hitPoint, newDir), depth+1, Xi, group, background);
        if (only == 0) tmp = tmp * (1-refRatio);
        sum = sum + tmp;
    }
    ret = ret + f * sum;
    return ret;
}

Vector3f tmpColor[2048][2048];

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 4) {
        cout << "Usage: ./bin/Final <input scene file> <output bmp file> <spp>" << endl;
        return 1;
    }

#ifndef _OPENMP
    fprintf(stderr, "Warning: OpenMP not supported.\n");
#endif

    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    int spp = atoi(argv[3]) / 4;

    SceneParser parser(inputFile.c_str());
    Camera *camera = parser.getCamera();
    Image image(camera->getWidth(), camera->getHeight());
    Group *group = parser.getGroup();
  #pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < camera->getWidth(); ++x) {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", spp*4, 100.*x/(camera->getWidth()-1));
        for (int y = 0; y < camera->getHeight(); ++y) {
            unsigned short Xi[3] = {0, 0, x * x * x};
            tmpColor[x][y] = Vector3f::ZERO;
            Vector3f r = Vector3f::ZERO;
            for (int sx = 0; sx < 2; ++sx) {
                for (int sy = 0; sy < 2; ++sy) {
                    for (int s = 0; s < spp; ++s) {
                        float r1 = 2 * erand48(Xi), r2 = 2 * erand48(Xi);
                        float dx = r1 < 1 ? sqrt(r1)-1 : 1-sqrt(2-r1);
                        float dy = r2 < 1 ? sqrt(r2)-1 : 1-sqrt(2-r2);
                        Ray ray = camera->generateRay(Vector2f((sx+0.5+dx)/2 + x, (sy+0.5+dy)/2 + y));
                        r = r + radiance(x, y, s, ray, 0, Xi, group, parser.getBackgroundColor()) / spp;
                    }
                    tmpColor[x][y] = tmpColor[x][y] + Vector3f(clamp(r.x()), clamp(r.y()), clamp(r.z()))*0.25;
                }
            }
            image.SetPixel(x, y, tmpColor[x][y]);
        }
    }
    image.SaveBMP(outputFile.c_str());
    return 0;
}