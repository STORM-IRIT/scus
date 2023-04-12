#include "cell.h"

Cell::Cell(glm::vec3 min, glm::vec3 max, float dim, bool useRot, unsigned seed, unsigned transformationIndex) : _min{
        min},
                                                                                                                _dim{dim},
                                                                                                                _seed{seed},
                                                                                                                _useRot{useRot},
                                                                                                                _transformationIndex{
                                                                                                                        transformationIndex} {
    //normalDistribution = std::normal_distribution<double>(0.,1.);
}


Cell::Cell(const Cell &cell) : _min{cell._min}, _dim{cell._dim}, _seed{cell._seed}, _useRot{cell._useRot},
                               _transformationIndex{cell._transformationIndex} {
    std::cout << "Copy" << std::endl;
    //normalDistribution = std::normal_distribution<double>(0.,1.);
    //_included.store(cell._included.load());
}


std::ostream &operator<<(std::ostream &os, const Cell &cell) {
    os << "Point min : " << glm::to_string(cell._min) << " | Point max : " << glm::to_string(cell._min + cell._dim)
       << " | Seed : " << cell._seed << std::endl;
    return os;
}

void Cell::getPoints(const std::shared_ptr<WeightingFunction> &function,
                     const std::shared_ptr<Sampler> &sampler,
                     std::vector<glm::vec4> &resPoints) const {
    resPoints.clear();

    _generator.seed(_seed);
    std::normal_distribution<double> normalDistribution(0., 1.);
    glm::vec4 base(_min, 0);

    resPoints = sampler->getSamples(_transformationIndex, _useRot);

    for (auto &point: resPoints) {
        point *= _dim;
        point += base;
        point.w = static_cast<float>(normalDistribution(_generator)) * function->getWeight(point.x, point.y, point.z);
    }
}

