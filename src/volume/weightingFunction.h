#ifndef SCUS_WEIGHTINGFUNCTION_H
#define SCUS_WEIGHTINGFUNCTION_H

#include <glm/glm.hpp>
#include <memory>

#include "../utility/json.hpp"

class WeightingFunction {
public:
    virtual float getWeight(float x, float y, float z) = 0;

    virtual float expectedMax() = 0;
};


#endif //SCUS_WEIGHTINGFUNCTION_H
