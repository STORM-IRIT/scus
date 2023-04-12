#ifndef SCUS_PLANE_H
#define SCUS_PLANE_H

#include "weightingFunction.h"


class Plane : public WeightingFunction {
public :
    inline explicit Plane(float height) : _height(height) {}

    inline float getWeight(float x, float y, float z) final {
        return y < _height ? expectedMax() : 1.f;;
    }

    inline float expectedMax() final { return 10.f; }

private :
    float _height;
};


#endif //SCUS_PLANE_H
