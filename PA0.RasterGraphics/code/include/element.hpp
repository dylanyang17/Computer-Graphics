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

    void circlepoints(Image &img, int x, int y) {
        // 对称地画 8 个点，注意 x, y 是以 (0, 0) 作为圆心时的点坐标
        for (int i = -1; i <= 1; i += 2) {
            for (int j = -1; j <= 1; j += 2) {
                img.SetPixel(cx + x * i, cy + y * j, color);
                img.SetPixel(cx + y * i, cy + x * j, color);
            }
        }
    }

    void draw(Image &img) override {
        // TODO: Implement Algorithm to draw a Circle
        printf("Draw a circle with center (%d, %d) and radius %d using color (%f, %f, %f)\n", cx, cy, radius,
               color.x(), color.y(), color.z());
        int x = 0, y = radius;
        // d 乘上 4 变为整数
        int d = 5 - 4 * radius;
        circlepoints(img, x, y);
        while (x <= y) {
            if (d < 0) {
                d += 4 * (2*x+3);
            } else {
                d += 4 * (2*(x-y)+5);
                y--;
            }
            x++;
            circlepoints(img, x, y);
        }
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