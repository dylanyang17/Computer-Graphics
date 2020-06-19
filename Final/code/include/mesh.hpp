#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"

class AABB;

class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<int> seq;          // 用于 kd-tree 中的排序
    std::vector<Vector3f> v;       // 结点集
    std::vector<TriangleIndex> t;  // 面片对应的三个结点编号
    std::vector<Vector3f> n;       // 面片法向
    std::vector<Vector3f> m;       // 面片中点

    std::vector<Triangle> triangles;  // 面片集合
    bool intersect(const Ray &r, Hit &h, double tmin) override;

private:

    // Normal can be used for light estimation
    void computeNormal();

    bool intersectKD(int s, const Ray &r, Hit &h, double tmin);
    int buildTree(int ll, int rr, int dim);
    void updateNode(int s);
    bool cmp (int a, int b);
    int cmpDim;

    AABB getAABB(int id);
};

#endif
