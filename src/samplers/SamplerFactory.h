#ifndef SCUS_SAMPLERFACTORY_H
#define SCUS_SAMPLERFACTORY_H

#include <memory>
#include <iostream>

#include "Sampler.h"
#include "SamplerDartThrowing.hpp"
#include "SamplerRegularGrid.hpp"
#include "SamplerUniformRandom.h"
#include "SamplerCascadedSobol.h"

class SamplerFactory {
public:
    static std::shared_ptr<Sampler> createSampler(const std::string &name) {
        std::shared_ptr<Sampler> sampler;
        if (name == "Dart" || name == "DART" || name == "dart") {
            sampler.reset(new utk::SamplerDartThrowing());
        } else if (name == "Regular" || name == "REGULAR" || name == "regular") {
            sampler.reset(new utk::SamplerRegularGrid());
        } else if (name == "Uniform" || name == "UNIFORM" || name == "uniform") {
            sampler.reset(new SamplerUniformRandom());
        } else if (name == "Cascaded" || name == "CASCADED" || name == "cascaded") {
            sampler.reset(new SamplerCascadedSobol());
        } else {
            std::cerr << "Unknown sampler type, defaulting to regular grid" << std::endl;
            sampler.reset(new utk::SamplerRegularGrid());
        }

        return sampler;
    }

};


#endif //SCUS_SAMPLERFACTORY_H
