#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// TODO (PA3): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
    int n, k;  // 控制点数为 n + 1，度数为 k
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

    void drawGL() override {
        Object3D::drawGL();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);
        glBegin(GL_LINE_STRIP);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        glPointSize(4);
        glBegin(GL_POINTS);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        std::vector<CurvePoint> sampledPoints;
        discretize(30, sampledPoints);
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (auto & cp : sampledPoints) { glVertex3fv(cp.V); }
        glEnd();
        glPopAttrib();
    }
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        // 为 n, k 赋值
        n = points.size() - 1, k = n;
        // new 一个 [x][y] 的二维数组
        int x = n + 1, y = n + 1;
        _B = new double[x * y];
        B = new double*[x];
        for (int i = 0; i < x; ++i) {
            B[i] = _B + i*y;
        }        
    }

    ~BezierCurve() {
        delete[] B;
        delete[] _B;
    }

    // 给定 t，计算 B_{i,j}(t)，其中 i 为 0~n, j 为 0~n，且 i<=j
    // i 为控制结点的标号，j 为度数
    void calcBasis(double t) {
        for (int j = 0; j <= n; ++j) {
            for (int i = 0; i <= j; ++i) {
                if (i == 0) {
                    B[i][j] = pow(1-t, j);
                } else if (i == j) {
                    B[i][j] = pow(t, i);
                } else {
                    B[i][j] = (1-t)*B[i][j-1] + t*B[i-1][j-1];
                }
            }
        }
    }

    // 给定 t，返回对应的 CurvePoint
    CurvePoint getCurvePoint(double t) {
        calcBasis(t);
        CurvePoint ret;
        ret.V = Vector3f(0, 0, 0), ret.T = Vector3f(0, 0, 0);
        for (int i = 0; i <= n; ++i) {
            ret.V += B[i][n] * controls[i];
            ret.T += n * controls[i] * (((i>0)?B[i-1][n-1]:0) - ((i<n)?B[i][n-1]:0));
        }
        ret.T.normalize();
        return ret;
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < resolution; ++j) {
                double t = (i+(double)j/resolution) / n;
                data.push_back(getCurvePoint(t));
            }
        }
        data.push_back(getCurvePoint(1));
    }

protected:
    double **B;  // B[i][j] 对应第 i 个控制结点在度数为 k 时的系数 (参数为某个 t)
    double *_B;  // 用于辅助开辟二维数组
};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        // 为 n, k 赋值，这里固定 k 为 3
        n = points.size() - 1, k = 3;
        // new 一个 [x][y] 的二维数组
        int x = n + k + 2, y = k + 1;
        _B = new double[x * y];
        B = new double*[x];
        for (int i = 0; i < x; ++i) {
            B[i] = _B + i*y;
        }
        // 定义节点序列
        T = new double(n + k + 2);
        for (int i = 0; i <= n + k + 1; ++i) {
            T[i] = (double)i / (n + k + 1);
        }
    }

    ~BsplineCurve() {
        delete[] B;
        delete[] _B;
        delete[] T;
    }

    // 给定 t，计算 B_{i,j}(t)，其中 i 为 0~n+k, j 为 0~k
    // i 为控制结点的标号，j 为度数
    void calcBasis(double t) {
        for (int i = 0; i <= n + k; ++i) {
            B[i][0] = ((t >= T[i] && t < T[i+1]) ? 1 : 0);
        }
        for (int j = 1; j <= k; ++j) {
            for (int i = 0; i <= n + k - j; ++i) {
                B[i][j] = (t-T[i])/(T[i+j]-T[i])*B[i][j-1] +
                          (T[i+j+1]-t)/(T[i+j+1]-T[i+1])*B[i+1][j-1];
            } 
        }   
    }

    // 给定 t，返回对应的 CurvePoint
    CurvePoint getCurvePoint(double t) {
        calcBasis(t);
        CurvePoint ret;
        ret.V = Vector3f(0, 0, 0), ret.T = Vector3f(0, 0, 0);
        for (int i = 0; i <= n; ++i) {
            ret.V += B[i][k] * controls[i];
            ret.T += controls[i] * k * (B[i][k-1]/(T[i+k]-T[i]) - B[i+1][k-1]/(T[i+k+1]-T[i+1]));
        }
        ret.T.normalize();
        return ret;
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        for (int i = 0; i < n + k + 1; ++i) {
            for (int j = 0; j < resolution; ++j) {
                double t = (i+(double)j/resolution) / (n + k + 1);
                data.push_back(getCurvePoint(t));
            }
        }
        data.push_back(getCurvePoint(1));
    }

protected:
    double **B;  // B[i][j] 对应第 i 个控制结点在度数为 k 时的系数 (参数为某个 t)
    double *_B;  // 用于辅助开辟二维数组
    double *T;   // 节点序列 t_i
};

#endif // CURVE_HPP
