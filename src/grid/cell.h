#ifndef SCUS_CELL_H
#define SCUS_CELL_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <chrono>
#include <random>
#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <atomic>

#include "../pointsets/Pointset.hpp"
#include "../volume/weightingFunction.h"
#include "../samplers/Sampler.h"

class Cell {
public :
    Cell(glm::vec3 min, glm::vec3 max, float dim, bool useRot, unsigned seed, unsigned index);

    Cell(const Cell &cell);

    friend std::ostream &operator<<(std::ostream &os, const Cell &cell);

    void getPoints(const std::shared_ptr<WeightingFunction> &function,
                   const std::shared_ptr<Sampler> &sampler,
                   std::vector<glm::vec4> &resPoints) const;

    [[nodiscard]] inline const glm::vec3 &min() const { return _min; };

    [[nodiscard]] inline std::array<glm::vec3, 8> getCorners() const {
        //
        //     a----b   y^
        //    /|   /|    |
        //   e----f |    |
        //   | d--|-c    0--->x
        //   |/   |/    /
        //   h----g    z
        //
        //       d->min f->max
        glm::vec3 max(_min + _dim);

        return {
                glm::vec3(min().x, min().y + _dim, min().z),
                glm::vec3(max.x, max.y, max.z - _dim),
                glm::vec3(min().x + _dim, min().y, min().z),
                glm::vec3(min()),
                glm::vec3(max.x - _dim, max.y, max.z),
                glm::vec3(max),
                glm::vec3(max.x, max.y - _dim, max.z),
                glm::vec3(min().x, min().y, min().z + _dim)
        };
    }

    inline void clearIncluded() const { _included = false; };

    [[nodiscard]] inline bool TestAndSetIncluded() const {
        auto r = _included;
        _included = true;
        return r;
    };


private :

    glm::vec3 _min;

    unsigned _transformationIndex;
    float _dim;
    unsigned _seed;
    bool _useRot;

    //mutable std::atomic_flag _included;
    mutable bool _included;
    mutable std::default_random_engine _generator;
    //mutable std::normal_distribution<double> normalDistribution;
};


#endif //SCUS_CELL_H
