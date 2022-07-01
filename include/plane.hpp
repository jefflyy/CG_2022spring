#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>


class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &normal, float offset, Material *m)
        : Object3D(m), norm(normal.normalized()), d(offset) {

    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f rd = r.getDirection();
        float dd = Vector3f::dot(rd, norm);
        if(fabs(dd) < 1e-4)
            return false;
        Vector3f r0 = r.getOrigin();
        float d0 = Vector3f::dot(r0, norm);
        float t = (d - d0) / dd;
        if(t <= 0 || t < tmin || t >= h.getT())
            return false;
        h.set(t, material, dd > 0 ? -norm : norm);
        return true;
    }

protected:
    Vector3f norm;
    float d;

};

#endif //PLANE_H
		

