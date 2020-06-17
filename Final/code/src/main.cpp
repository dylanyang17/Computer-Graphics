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

Vector3f radiance(const Ray &r, int depth, unsigned short *Xi, Group *group) {
    // TODO: 检查反射时是否会与反射处物体相交
    Hit hit;
    bool isIntersect = group->intersect(r, hit, 1e-4);
    if (!isIntersect)
        return Vector3f(0, 0, 0);
    Material *m = hit.getMaterial();
    // 令单位入射向量为 in，单位法向为 norm
    int only = 0;  // 为 0 时表示两个部分都需要计算，为 1 时表示仅计算镜面反射部分，为 2 时表示仅计算漫反射部分
    if (depth > 2) {
        only = (erand48(Xi) < m->refRatio) ? 1 : 2;
    }
    Vector3f ret = m->emission;
    if ((only == 0 || only == 1) && m->refRatio > 1e-5) {
        // 镜面反射部分
        Vector3f in = r.getDirection().normalized();
        Vector3f norm = hit.getNormal();
        Vector3f newDir = (in + Vector3f::dot(in, norm)*2*norm).normalized();

    }
    if (m->refRatio < 1-1e-5) {
        // 漫反射部分
    }
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 4) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file> <spp>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    int spp = atoi(argv[3]);

    SceneParser parser(inputFile.c_str());
    Camera *camera = parser.getCamera();
    Image image(camera->getWidth(), camera->getHeight());
    Vector3f r(0, 0, 0);
    Vector3f tmpColor[2048][2048];
    Group *group = parser.getGroup();
    for (int x = 0; x < camera->getWidth(); ++x) {
        for (int y = 0; y < camera->getHeight(); ++y) {
            unsigned short Xi[3] = {0, 0, x * x * x};
            for (int sx = 0; sx < 2; ++sx) {
                for (int sy = 0; sy < 2; ++sy) {
                    for (int s = 0; s < spp; ++s) {
                        float r1 = 2 * erand48(Xi), r2 = 2 * erand48(Xi);
                        float dx = r1 < 1 ? sqrt(r1)-1 : 1-sqrt(2-r1);
                        float dy = r2 < 1 ? sqrt(r2)-1 : 1-sqrt(2-r2);
                        Ray ray = camera->generateRay(Vector2f((sx+0.5+dx)/2 + x, (sy+0.5+dy)/2 + y));
                        r = r + radiance(ray, 0, Xi, group) / spp;
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