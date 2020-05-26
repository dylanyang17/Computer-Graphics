#pragma once

#include <image.hpp>
#include <algorithm>
#include <queue>
#include <cstdio>

class Element {
public:
    virtual void draw(Image &img) = 0;
    virtual ~Element() = default;
};

class Line : public Element {

public:
    int xA, yA;
    int xB, yB;
    Vector3f color;
    void draw(Image &img) override {
        // TODO: Implement Bresenham Algorithm
        printf("Draw a line from (%d, %d) to (%d, %d) using color (%f, %f, %f)\n", xA, yA, xB, yB,
                color.x(), color.y(), color.z());
        if (xA == xB) {
            if (yA > yB) {
                std::swap(yB, yB);
            }
            for (int y = yA; y <= yB; ++y) {
                img.SetPixel(xA, y, color);
            }
        } else {
            // 记录是否 x, y 互换
            bool rev = (fabs((double)(yA - yB) / (xA - xB)) > 1);
            if (rev) {
                std::swap(xA, yA);
                std::swap(xB, yB);
            } 
            if (xA > xB) {
                std::swap(xA, xB);
                std::swap(yA, yB);
            }
            // 此时一定有 |k| <= 1 且 xA <= xB
            int dx = xB - xA, dy = yB - yA, y = yA, e = ((dy >= 0) ? -dx : dx);
            for (int x = xA; x <= xB; ++x) {
                img.SetPixel((rev ? y : x), (rev ? x : y), color);
                e += 2 * dy;
                if (dy >= 0) {
                    if (e >= 0) {
                        e -= 2 * dx;
                        ++y;
                    }
                } else {
                    if (e <= 0) {
                        e += 2 * dx;
                        --y;
                    }
                }
            }
        }
    }
};

class Circle : public Element {

public:
    int cx, cy;
    int radius;
    Vector3f color;
    void draw(Image &img) override {
        // TODO: Implement Algorithm to draw a Circle
        printf("Draw a circle with center (%d, %d) and radius %d using color (%f, %f, %f)\n", cx, cy, radius,
               color.x(), color.y(), color.z());
    }
};

class Fill : public Element {

public:
    int cx, cy;
    Vector3f color;
    void draw(Image &img) override {
        // TODO: Flood fill
        printf("Flood fill source point = (%d, %d) using color (%f, %f, %f)\n", cx, cy,
                color.x(), color.y(), color.z());
    }
};