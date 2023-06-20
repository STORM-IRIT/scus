#ifndef SCUS_SAMPLER_H
#define SCUS_SAMPLER_H

#include <glm/glm.hpp>
#include <vector>

class Sampler {
public :
    virtual void setRandomSeed(long unsigned int arg_seed) = 0;

    virtual void initSamples(unsigned nbScatterers) = 0;

    virtual bool shouldTransform() { return true; };

    virtual std::vector<glm::vec4> getSamples(unsigned index, bool transform) const = 0;

    //SAMPLE TRANSFORMATION
    virtual unsigned maxTransformIndex() const = 0;

protected:
    inline const static std::vector<glm::uvec3> _projections{
            glm::uvec3(
                    0, 1, 2
            ),
            glm::uvec3(
                    1, 2, 3
            ),
            glm::uvec3(
                    2, 3, 4
            ),
            glm::uvec3(
                    4, 5, 6
            ),
            glm::uvec3(
                    5, 6, 7
            ),
            glm::uvec3(
                    6, 7, 8
            ),
            glm::uvec3(
                    7, 8, 9
            )
    };

    inline const static std::vector<glm::mat4> _rotations{
            glm::mat4(
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, 1, 0,
                    0, 1, 0, 0,
                    -1, 0, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    -1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, -1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    1, 0, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, -1, 0, 0,
                    1, 0, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, 1, 0,
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 1, 0, 0,
                    1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, -1, 0,
                    1, 0, 0, 0,
                    0, -1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 1, 0, 0,
                    -1, 0, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, 1, 0,
                    -1, 0, 0, 0,
                    0, -1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, -1, 0, 0,
                    -1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, -1, 0,
                    -1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 1, 0, 0,
                    0, 0, -1, 0,
                    -1, 0, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    -1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, -1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, -1, 0, 0,
                    0, 0, -1, 0,
                    1, 0, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    1, 0, 0, 0,
                    0, -1, 0, 0,
                    0, 0, -1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, -1, 0,
                    0, -1, 0, 0,
                    -1, 0, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    -1, 0, 0, 0,
                    0, -1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 0, 1, 0,
                    0, -1, 0, 0,
                    1, 0, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    1, 0, 0, 0,
                    0, 0, 1, 0,
                    0, -1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, -1, 0, 0,
                    0, 0, 1, 0,
                    -1, 0, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    -1, 0, 0, 0,
                    0, 0, 1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1
            ),
            glm::mat4(
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    1, 0, 0, 0,
                    0, 0, 0, 1
            )
    };
};

#endif //SCUS_SAMPLER_H
