#include "mesh.hpp"
#include "aabb.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

const int MAXNODE = 5000005;

// KD-Tree 结点
struct Node {
    int child[2];
    int leaf;  // 等于分配给该结点的面片序号
    AABB aabb;

    Node() {
        child[0] = child[1] = 0;
    }
} node[MAXNODE];

int ndnum = 0;

bool Mesh::intersectKD(int s, const Ray &r, Hit &h, double tmin) {
    if (node[s].aabb.intersect(r, h, tmin) == false)
        return false;
    triangles[node[s].leaf]
}

bool Mesh::intersect(const Ray &r, Hit &h, double tmin) {

    // Optional: Change this brute force method into a faster one.
    // 遍历所有存储的三角形依次求交
    bool result = false;
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        result |= triangles[triId].intersect(r, h, tmin);
    }
    return result;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        } else if (tok == fTok) {
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                t.push_back(trig);
            } else {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                t.push_back(trig);
            }
        } else if (tok == texTok) {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        }
    }

    // 计算法向
    computeNormal();

    // 存储 Triangle
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Triangle triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);
        triangle.normal = n[triId];
        triangles.push_back(triangle);
    }

    // 初始化 seq 并计算面片中点
    for (int i = 0; i < t.size(); ++i) {
        seq.push_back(i);
        Vector3f mm = Vector3f::ZERO;
        for (int j = 0; j < 3; ++j) {
            mm = mm + v[t[i][j]]/3;
        }
        m.push_back(mm);
    }
    buildTree(0, t.size()-1, 0);

    f.close();
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}

AABB Mesh::getAABB(int id) {
    // id 为面片序号
    AABB aabb;
    for (int j = 0; j < 3; ++j) {
        aabb.minp[j] = MAXDOUBLE;
        aabb.maxp[j] = -MAXDOUBLE;
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            aabb.minp[j] = std::min(aabb.minp[j], v[t[id][i]][j]);
            aabb.maxp[j] = std::max(aabb.maxp[j], v[t[id][i]][j]);
        }
    }
    return aabb;
}

void Mesh::updateNode(int s) {
    node[s].aabb = getAABB(node[s].leaf);
    for (int i = 0; i < 2; ++i) {
        int c = node[s].child[i];
        if (c) {
            node[s].aabb = node[s].aabb | node[c].aabb;
        }
    }
}

// 对面片的比较
bool Mesh::cmp (int a, int b) {
    return m[a][cmpDim] < m[b][cmpDim];
}

int Mesh::buildTree(int ll, int rr, int dim) {
    if (ll > rr)
        return 0;
    int s = ++ndnum;
    if (ll == rr) {
        node[s].aabb = getAABB(seq[ll]);
        node[s].leaf = seq[ll];
        return s;
    }
    int mid = (ll+rr) / 2;
    // TODO: nth_element ... calc aabb ...
    cmpDim = dim;
    nth_element(seq.begin()+ll, seq.begin()+mid, seq.begin()+rr+1, cmp);
    node[s].leaf = seq[mid];
    node[s].child[0] = buildTree(ll, mid-1, (dim+1)%3);
    node[s].child[1] = buildTree(mid+1, rr, (dim+1)%3);
    updateNode(s);
}