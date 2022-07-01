#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>


class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0, float ref = -1, float trans = -1, float frac = 1) :
            diffuseColor(d_color), specularColor(s_color), shininess(s), ref_ratio(ref), trans_ratio(trans), index_frac(frac) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        Vector3f L = dirToLight.normalized(), N = hit.getNormal();
        float LN = Vector3f::dot(L, N);
        Vector3f R = 2 * LN * N - L;
        Vector3f V = (-ray.getDirection()).normalized();
        float VR = Vector3f::dot(V, R);
        shaded += diffuseColor * std::max(LN, 0.0f);
        shaded += specularColor * pow(std::max(VR, 0.0f), shininess);
        return lightColor * shaded;
    }

    bool getReflect(const Ray &r, const Hit &h, Vector3f &R, float &ratio){
        if(ref_ratio <= 0)
            return false;
        Vector3f N = h.getNormal(), L = -r.getDirection();
        float LN = Vector3f::dot(L, N);
        R = 2 * LN * N - L;
        ratio = ref_ratio;
        return true;
    }

    bool getRefract(const Ray &r, const Hit &h, bool inside, Vector3f &T, float &ratio){
        if(trans_ratio <= 0)
            return false;
        Vector3f N = h.getNormal(), L = -r.getDirection();
        float LN = Vector3f::dot(L, N);
        Vector3f TP = (LN * N - L) * (inside ? index_frac : (1 / index_frac));
        float n2 = L.squaredLength() - TP.squaredLength();
        if(n2 < 0)
            return false;
        T = TP - N * sqrt(n2);
        ratio = trans_ratio;
        return true;
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
    float ref_ratio, trans_ratio, index_frac;
};


#endif // MATERIAL_H
