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

inline double clamp(double x) {
    return x < 0 ? 0 : x > 1 ? 1 : x;
}

inline Vector3f clamp(Vector3f vec) {
    return Vector3f(clamp(vec.x()), clamp(vec.y()), clamp(vec.z()));
}

Vector3f gammaCorrection(const Vector3f &color) {
    Vector3f ret;
    for (int i = 0; i < 3; ++i) {
        ret[i] = pow(color[i], 1/2.2);
    }
    return ret;
}

Vector3f revGammaCorrection(const Vector3f &color) {
    Vector3f ret;
    for (int i = 0; i < 3; ++i) {
        ret[i] = pow(color[i], 2.2);
    }
    return ret;
}

Vector3f radiance(int ttx, int tty, int tts, const Ray &r, int depth, unsigned short *Xi, Group *group, const Vector3f background) {
    // TODO: 检查反射时是否会与反射处物体相交
    Hit hit;
    bool isIntersect = group->intersect(r, hit, 1e-2);
    if (!isIntersect)
        return background;
    Material *m = hit.getMaterial();
    // 令单位入射向量为 in，单位法向为 norm
    Vector3f f = m->color;
    Vector3f ret = m->emission;
    Vector3f hitPoint = r.pointAtParameter(hit.getT());
    Vector3f norm = hit.getNormal();
    double refRatio = m->refRatio;
    double p = f.x()>f.y() && f.x()>f.z() ? f.x() : f.y()>f.z() ? f.y() : f.z();
    if (f.length() < 1e-8) return ret;  // 剪枝
    if (depth > 5) if (erand48(Xi) < min((double)p, 0.9)) f = f/p; else return ret;  // 轮盘赌
    int only = 0;  // 为 0 时表示两个部分都需要计算，为 1 时表示仅计算镜面反射部分，为 2 时表示仅计算漫反射部分
    if (depth > 2) only = (erand48(Xi) < refRatio) ? 1 : 2;
    Vector3f sum = Vector3f::ZERO;  // 存储从该交点出发之后的颜色效果总和
    if ((only == 0 || only == 1) && refRatio > 1e-5) {
        // 镜面反射部分
        Vector3f in = r.getDirection().normalized();
        Vector3f newDir = (in - Vector3f::dot(in, norm)*2*norm).normalized();
        Vector3f tmp = radiance(ttx, tty, tts, Ray(hitPoint, newDir), depth+1, Xi, group, background);
        if (only == 0) tmp = tmp * refRatio;
        sum = sum + tmp;
    }
    if ((only == 0 || only == 2) && refRatio < 1-1e-5) {
        // 漫反射部分
        // u, v, w 为一组正交基，其中 w 即交点处法向 norm
        // r2s 即单位反射向量在 uv 平面上的投影长度
        double r1 = 2*M_PI*erand48(Xi), r2=erand48(Xi), r2s=sqrt(r2);
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

    if ((argc != 3 && argc != 4) || (argc == 4 && strcmp(argv[3], "-c")!=0)) {
        cout << "Usage: ./bin/Final <filename> <spp> [-c]" << endl;
        return 1;
    }

#ifndef _OPENMP
    fprintf(stderr, "Warning: OpenMP not supported.\n");
#endif

    string filename = argv[1];
    int spp = atoi(argv[2]) / 4 * 4;

    SceneParser parser((filename + ".txt").c_str());
    Camera *camera = parser.getCamera();
    Image image(camera->getWidth(), camera->getHeight());
    Image *lastImage;
    int lastSpp = 0;
    Group *group = parser.getGroup();

    FILE *logFile = fopen((filename+".log").c_str(), "r");
    if (logFile != NULL && ~fscanf(logFile, "%d", &lastSpp)) {
        if (argc == 4) {
            lastImage = Image::LoadPPM((filename+".ppm").c_str());
        } else {
            printf("检测到log文件，是否继续上次运行？(Y/N)\n");
            char c;
            while ((c = getchar()) != EOF && c != 'Y' && c != 'N');
            if (c=='Y') {
                lastImage = Image::LoadPPM((filename+".ppm").c_str());
            } else {
                lastSpp = 0;
            }
        }
        fclose(logFile);
    } else lastSpp = 0;

   #pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < camera->getWidth(); ++x) {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", spp, 100.*x/(camera->getWidth()-1));
        for (int y = 0; y < camera->getHeight(); ++y) {
            unsigned short Xi[3] = {time(NULL), clock(), x * x * x};
            tmpColor[x][y] = Vector3f::ZERO;
            Vector3f r = Vector3f::ZERO;
            for (int sx = 0; sx < 2; ++sx) {
                for (int sy = 0; sy < 2; ++sy) {
                    for (int s = 0; s < spp/4; ++s) {
                        double r1 = 2 * erand48(Xi), r2 = 2 * erand48(Xi);
                        double dx = r1 < 1 ? sqrt(r1)-1 : 1-sqrt(2-r1);
                        double dy = r2 < 1 ? sqrt(r2)-1 : 1-sqrt(2-r2);
                        Ray ray = camera->generateRay(Vector2f((sx+0.5+dx)/2 + x, (sy+0.5+dy)/2 + y));
                        r = r + radiance(x, y, s, ray, 0, Xi, group, parser.getBackgroundColor()) / (spp/4);
                    }
                    tmpColor[x][y] = tmpColor[x][y] + clamp(r)*0.25;
                }
            }
            tmpColor[x][y] = clamp(tmpColor[x][y]);
        }
    }

    int sumSpp = lastSpp + spp;
    double p = (double)spp / sumSpp;
    for (int x = 0; x < camera->getWidth(); ++x) {
        for (int y = 0; y < camera->getHeight(); ++y) {
            if (lastSpp != 0) {
                image.SetPixel(x, y, gammaCorrection(revGammaCorrection(lastImage->GetPixel(x, y)) * (1-p) + tmpColor[x][y] * p));
            } else image.SetPixel(x, y, gammaCorrection(tmpColor[x][y]));
        }
    }
    logFile = fopen((filename+".log").c_str(), "w");
    fprintf(logFile, "%d\n", sumSpp);
    image.SavePPM((filename + ".ppm").c_str());
    return 0;
}