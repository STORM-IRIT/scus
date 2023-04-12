#ifndef SCUS_SPHERE_H
#define SCUS_SPHERE_H

#include <glm/gtx/norm.hpp>

#include "weightingFunction.h"


class Sphere : public WeightingFunction {
public :
    inline Sphere(glm::vec3 center, float radius) : _center(center), _sqradius(radius * radius) {}

    inline float getWeight(float x, float y, float z) final {
        return glm::distance2(_center, glm::vec3(x, y, z)) < _sqradius ? expectedMax() : 1.f;
    }

    inline float expectedMax() final { return 10.f; }

private :
    glm::vec3 _center;
    float _sqradius;
};


#endif //SCUS_SPHERE_H
