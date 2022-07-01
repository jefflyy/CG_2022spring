#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// TODO (PA2): Implement Bernstein class to compute spline basis function.
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

public:

    float mint, maxt;
    
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual std::vector<float> discretize(int resolution, std::vector<CurvePoint>& data) = 0;

    virtual CurvePoint calc(float t) = 0;

};

class BezierCurve : public Curve {

    std::vector<float> B, Bd;

public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        int n = controls.size();
        B.resize(n);
        Bd.resize(n);
        mint = 0.0;
        maxt = 1.0;
    }

    std::vector<float> discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        std::vector<float> ts;
        for(int i = 0; i <= resolution; i++){
            float t = i / (float)resolution;
            ts.push_back(t);
            data.push_back(calc(t));
        }
        return ts;
    }

    CurvePoint calc(float t) override { // slow
        int n;
        n = controls.size() - 1;
        B[0] = 1;
        for(int i = 1; i < n; i++){
            B[i] = B[i - 1] * t;
            for(int j = i - 1; j > 0; j--)
                B[j] = B[j - 1] * t + B[j] * (1 - t);
            B[0] *= (1 - t);
        }
        Bd[n] = B[n - 1] * n;
        B[n] = B[n - 1] * t;
        for(int i = n - 1; i > 0; i--){
            Bd[i] = n * (B[i - 1] - B[i]);
            B[i] = B[i - 1] * t + B[i] * (1 - t);
        }
        Bd[0] = -B[0] * n;
        B[0] *= (1 - t);
        Vector3f V,T;
        V = T = Vector3f::ZERO;
        for(int i = 0; i <= n ;i++){
            V += controls[i] * B[i];
            T += controls[i] * Bd[i];
        }
        return (CurvePoint){V, T};
    }

protected:

};

class BsplineCurve : public Curve {

    std::vector<float> ti, B;

public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        int n, k;
        n = controls.size() - 1;
        k = 3;
        ti.resize(n + k + 2);
        B.resize(n + k + 1);
        for(int i = 0; i <= n + k + 1; i++)
            ti[i] = i / (float)(n + k + 1);
        mint = ti[k];
        maxt = ti[n + 1];
    }

    std::vector<float> discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        std::vector<float> ts;
        int n, k, m;
        float t;
        n = controls.size() - 1;
        k = 3;
        m = (n + k + 1) * resolution;
        for(int i = k; i <= n; i++){
            for(int j = 0; j < resolution; j++){
                t = (i * resolution + j) / (float)m;
                ts.push_back(t);
                data.push_back(calc(t));
            }
        }
        t = (n + 1) / (float)(n + k + 1);
        ts.push_back(t);
        data.push_back(calc(t));
        return ts;
    }

    CurvePoint calc(float t) override { // fast
        int n, k, L, R;
        n = controls.size() - 1;
        k = 3;
        R = t * (n + k + 1);
        while(R < n + k + 1 && t >= ti[R + 1])
            R++;
        while(R > 0 && t < ti[R])
            R--;
        B[R] = 1.0;
        B[R + 1] = 0.0;
        L = R - k;
        for(int i = L; i < R; i++)
            B[i] = 0.0;
        for(int p = 1; p < k; p++){
            for(int i = L; i <= R; i++){
                B[i] = (t - ti[i]) / (ti[i + p] - ti[i]) * B[i] + (ti[i + p + 1] - t) / (ti[i + p + 1] - ti[i + 1]) * B[i + 1];
            }
        }
        Vector3f V , T;
        V = T = Vector3f::ZERO;
        for(int i = L; i <= R; i++){
            float Bd = k * (B[i] / (ti[i + k] - ti[i]) - B[i + 1] / (ti[i + k + 1] - ti[i + 1]));
            B[i] = (t - ti[i]) / (ti[i + k] - ti[i]) * B[i] + (ti[i + k + 1] - t) / (ti[i + k + 1] - ti[i + 1]) * B[i + 1];
            V += controls[i] * B[i];
            T += controls[i] * Bd;
        }
        return (CurvePoint){V, T};
    }

protected:

};

#endif // CURVE_HPP
