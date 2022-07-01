#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;

// TODO (PA2): Copy from PA1
class Triangle: public Object3D
{

public:
	Triangle() = delete;
        ///@param a b c are three vertex positions of the triangle

	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m)
		: Object3D(m) {
			vertices[0] = a;
			vertices[1] = b;
			vertices[2] = c;
		}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Vector3f Rd = ray.getDirection();
        Vector3f E1 = vertices[0] - vertices[1], E2 = vertices[0] - vertices[2], S = vertices[0] - ray.getOrigin();
		float d = Matrix3f(Rd, E1, E2).determinant();
		if(d == 0){
			return false;
		}
		Vector3f res(Matrix3f(S, E1, E2).determinant(), Matrix3f(Rd, S, E2).determinant(), Matrix3f(Rd, E1, S).determinant());
		res = res / d;
		float t = res.x(), alp = res.y(), bet = res.z();
		if(t <= 0 || t < tmin || t >= hit.getT() || alp < 0 || bet < 0 || alp + bet > 1)
			return false;
		// assert((alp * vertices[1] + bet * vertices[2] + (1 - alp - bet) * vertices[0] - ray.pointAtParameter(t)).squaredLength() < 1e-10);
		hit.set(t, material, Vector3f::dot(Rd, normal) > 0 ? -normal : normal);
        return true;
	}
	
	Vector3f normal;
	Vector3f vertices[3];

protected:
};

#endif //TRIANGLE_H
