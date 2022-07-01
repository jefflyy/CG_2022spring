#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        g.resize(num_objects);
    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool ok = false;
        for(Object3D *x:g)
            ok |= x->intersect(r, h, tmin);
        return ok;
    }

    void addObject(int index, Object3D *obj) {
        g[index] = obj;
    }

    int getGroupSize() {
        return g.size();
    }

private:
    std::vector<Object3D*> g;

};

#endif
    
