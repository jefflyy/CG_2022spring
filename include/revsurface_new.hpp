#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include <tuple>
#include <math.h>
#include <assert.h>

const float nt_eps = 1e-6;

float sqr(float x){
    return x * x;
}

float sgn(float x){
    return x > 0 ? 1 : -1;
}

class RevSurface : public Object3D {

    Curve *pCurve;

public:
    RevSurface(Curve *pCurve, Material* material) : Object3D(material), pCurve(pCurve) {
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
    }

    ~RevSurface() override {
        delete pCurve;
    }

    bool getT(const Ray &r, Hit &h, float tmin, const Vector2f &f, const Vector2f &df){
        const Vector3f &o = r.getOrigin(), &d = r.getDirection();
        float t;
        if(fabs(d.z()) > 1e-2){
            t = (f.y() - o.z()) / d.z();
            // assert(fabs(r.pointAtParameter(t).z() - f.y()) < eps);
            // assert(fabs(r.pointAtParameter(t).xy().absSquared() - sqr(f.x())) < 1e-5);
        }else{
            float l2 = o.xy().absSquared(), r2 = sqr(f.x());
            float tp = -Vector2f::dot(o.xy(), d.xy().normalized());
            if(l2 >= r2 && tp <= 0)
                return false;
            float d2 = l2 - sqr(tp);
            if(d2 >= r2)
                return false;
            float t1 = sqrt(r2 - d2);
            if(l2 > r2)
                t = tp - t1;
            else
                t = tp + t1;
            t /= d.length();
        }
        if(t <= 0 || t < tmin || t >= h.getT())
            return false;
        Vector2f hd = r.pointAtParameter(t).xy().normalized();
        Vector3f dc(hd * sgn(f.x()) * df.x(), df.y());
        Vector3f norm = Vector3f::cross(Vector3f(hd.normal(), 0.0), dc).normalized();
        h.set(t, material, Vector3f::dot(d, norm) > 0 ? -norm : norm);
        return true;
    }

    bool Newton(const Ray &r, Hit &h, float tmin, float t){
        const int iterNum = 100;
        const Vector3f &o = r.getOrigin(), &d = r.getDirection();
        CurvePoint p;
        for(int i = 0; i < iterNum; i++){
            if(t < pCurve->mint || t > pCurve->maxt)
                return false;
            double a, b, c, f, df;
            p = pCurve->calc(t);
            a = o.x() * d.z() + d.x() * (p.V.y() - o.z());
            b = o.y() * d.z() + d.y() * (p.V.y() - o.z());
            c = d.z() * p.V.x();
            f = sqr(a) + sqr(b) - sqr(c);
            if(fabs(f) < nt_eps)
                return getT(r, h, tmin, p.V.xy(), p.T.xy());
            df = 2 * a * d.x() * p.T.y() + 2 * b * d.y() * p.T.y() - 2 * c * d.z() * p.T.x();
            if(df == 0)
                return false;
            t = t - t * f / df;
        }
        return false;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool res = false;
        const int N = 40;
        std::vector<CurvePoint> p;
        std::vector<float> t = pCurve->discretize(30, p);
        assert(p.size() == t.size());
        int label = -1;
        for(int i = 1; i < t.size(); i++){
            Vector3f c1 = p[i - 1].V, c2 = p[i].V;
            for(int j = 0; j < N; j++){
                float a1 = 2 * j * M_PI / N, a2 = 2 * (j + 1) * M_PI / N;
                Hit th;
                #define rev(p, a) Vector3f(fabs(p[0]) * cos(a), fabs(p[0]) * sin(a), p[1])
                #define gao(_p1, _p2, _p3) { \
                    Vector3f p1 = _p1, p2 = _p2, p3 = _p3; \
                    Triangle t(p1, p2, p3, material); \
                    t.normal = Vector3f::cross(p2 - p1, p3 - p2).normalized(); \
                    if(t.intersect(r, th, tmin)){ \
                        label = i; \
                        break; \
                    } \
                }
                gao(rev(c1, a1), rev(c1, a2), rev(c2, a1))
                gao(rev(c2, a1), rev(c2, a2), rev(c1, a2))
                #undef gao
                #undef rev
            } 
        }
        if(label == -1)
            return false;
        bool res = false;
        res |= Newton(r, h, tmin, t[label - 1]);
        res |= Newton(r, h, tmin, t[label]);
        return res;
    }

};

#endif //REVSURFACE_HPP
