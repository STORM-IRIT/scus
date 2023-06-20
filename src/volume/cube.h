#ifndef SCUS_CUBE_H
#define SCUS_CUBE_H

class Cube : public WeightingFunction {
public :
    inline Cube(glm::vec3 center, float side) {
        _min = center - side / 2.f;
        _max = center + side / 2.f;
    }

    inline float getWeight(float x, float y, float z) final {

        return (x > _min.x && x < _max.x &&
                y > _min.y && y < _max.y &&
                z > _min.z && z < _max.z) ?
               expectedMax() : 1.f;
    }

    inline float expectedMax() final { return 10.f; }

private :
    glm::vec3 _min;
    glm::vec3 _max;
};

#endif //SCUS_CUBE_H
