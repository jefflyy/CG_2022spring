#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>


class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = Vector3f::ZERO;
        radius = 1;
    }

    Sphere(const Vector3f &center, float radius, Material *material)
        : Object3D(material), center(center), radius(radius) {

    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f l = center - r.getOrigin();
        float l2 = l.squaredLength(), r2 = radius * radius;
        Vector3f r0 = r.getDirection();
        float r0l = r0.length();
        float tp = Vector3f::dot(l, r0) / r0l;
        if(l2 >= r2 && tp <= 0)
            return false;
        float d2 = l2 - tp * tp;
        if(d2 >= r2)
            return false;
        float dt = sqrt(r2 - d2);
        float t1 = (tp - dt) / r0l, t2 = (tp + dt) / r0l;
        
        if(t1 > tmin && t1 < h.getT()){
            Vector3f norm = (r.pointAtParameter(t1) - center).normalized();
            h.set(t1, material, norm);
            return true;
        }
        if(t2 > tmin && t2 < h.getT()){
            Vector3f norm = (r.pointAtParameter(t2) - center).normalized();
            h.set(t2, material, -norm);
            return true;
        }

        return false;
    }

protected:
    Vector3f center;
    float radius;

};


#endif
