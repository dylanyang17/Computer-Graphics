#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        objects.reserve(num_objects);
    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool ret = false;
        for (int i = 0; i < objects.size(); ++i) {
            ret |= objects[i]->intersect(r, h, tmin);
        }
        return ret;
    }

    void addObject(int index, Object3D *obj) {
        // 下标从 0 开始
        objects.insert(objects.begin()+index, obj);
    }

    int getGroupSize() {
        return objects.size();
    }

private:
    std::vector<Object3D*> objects;
};

#endif
	
