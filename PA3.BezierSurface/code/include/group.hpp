#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO (PA2, done): Implement Group - copy from PA1
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

    void drawGL() override {
        for (int i = 0; i < objects.size(); ++i) {
            objects[i]->drawGL();
        }
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
	
