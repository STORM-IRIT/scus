#ifndef SCUS_SAMPLERUNIFORMRANDOM_H
#define SCUS_SAMPLERUNIFORMRANDOM_H

#include "Sampler.h"

class SamplerUniformRandom : public Sampler {
public:
    SamplerUniformRandom() { setRandomSeedTime(); }

    void setRandomSeed(unsigned long arg_seed) override {
        m_mersenneTwister.seed(arg_seed);
    }

    void initSamples(unsigned nbScatterers) override {

        _points.clear();
        for (unsigned i = 0; i < nbScatterers; ++i) {
            _points.emplace_back(getRandom01(), getRandom01(), getRandom01(), 1);
        }
    }

    void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

    std::vector<glm::vec4> getSamples(unsigned index, bool transform) const final {
        std::vector<glm::vec4> res(_points.begin(), _points.end());

        if (transform) {
            glm::vec4 offset(0.5f, 0.5f, 0.5f, 0);
            for (auto &point: res) {
                point = (_rotations[index] * (point - offset) + offset);

            }
        }

        return res;
    }

    unsigned maxTransformIndex() const final {
        return 24;
    }

private:
    std::vector<glm::vec4> _points;

    std::mt19937 m_mersenneTwister{};

    double getRandom01() {
        return (double) m_mersenneTwister() / (double) m_mersenneTwister.max();
    }

};


#endif //SCUS_SAMPLERUNIFORMRANDOM_H
