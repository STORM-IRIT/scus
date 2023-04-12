#ifndef SCUS_SAMPLERCASCADEDSOBOL_H
#define SCUS_SAMPLERCASCADEDSOBOL_H

#include <string>
#include <random>

#include "Sampler.h"
#include "cascadedSobol/SobolGenerator1D.h"


class SamplerCascadedSobol : public Sampler {
private:

    std::vector<double> _points;

    uint32_t _seed;
    short _nDims = 10;

    inline void getCascadedSobol(std::vector<double> &values,
                                 const std::vector<SobolGenerator1D> &sobols,
                                 const uint32_t *seeds,
                                 const int nDims,
                                 const uint32_t n,
                                 const uint32_t nbits,
                                 const bool owen_permut_flag = true,
                                 const uint32_t owen_tree_depth = 32) {
        uint32_t index = n;        // dim 0: take n-th point as index -> into 32-bit integer
        for (int idim = 0; idim < nDims; idim++) {
            index = sobols[idim].getSobolInt(index);    // radix-inversion + sobol
            uint32_t result = index;
            if (owen_permut_flag)                        // we need to apply OwenScrambling only when this flag is set
                result = OwenScrambling(result, seeds[idim], owen_tree_depth);
            values.emplace_back(double(result) /
                                double(std::numeric_limits<uint32_t>::max()));    // final value (double) for this dimension
            index = index >> (32 - nbits);                // this will be used as new index for the next dimension
        }
    }


public:
    SamplerCascadedSobol() { setRandomSeedTime(); }

    void setRandomSeedTime() { _seed = std::chrono::system_clock::now().time_since_epoch().count(); }

    void setRandomSeed(long unsigned int arg_seed) override { _seed = arg_seed; }

    void initSamples(unsigned nbScatterers) override {
        std::vector<SobolGenerator1D> sobols;    // array of sobol data per dim
        std::vector<uint32_t> d;
        std::vector<uint32_t> s;
        std::vector<uint32_t> a;
        std::vector<std::vector<uint32_t>> m;
        bool owen_permut_flag = true;

        std::ifstream tableFile("../src/samplers/cascadedSobol/cascaded_sobol_init_tab.dat");
        if (!tableFile.is_open()) {
            std::cerr << "../src/samplers/cascadedSobol/cascaded_sobol_init_tab.dat cannot be read." << std::endl;
            exit(1);
        };
        load_init_table(tableFile, d, s, a, m, _nDims);
        init_sobols(sobols, d, s, a, m);

        std::uniform_int_distribution<uint32_t> unif32bits(
                0);  // uniform distribution of uint32_ts between 0 and 2^32-1
        std::mt19937_64 gen(_seed);

        auto nbits = (uint32_t) ((double) log((double) nbScatterers) / (double) log((double) 2));

        _points.clear();
        _points.reserve(nbScatterers * _nDims);

        std::vector<uint32_t> realSeeds(_nDims);
        for (int iDim = 0; iDim < _nDims; ++iDim) {
            realSeeds[iDim] = unif32bits(gen);
        }

        //get each point
        for (int ipt = 0; ipt < nbScatterers; ipt++) {
            getCascadedSobol(_points, sobols, realSeeds.data(), _nDims, ipt, nbits, owen_permut_flag);
        }

    }

    unsigned maxTransformIndex() const final {
        return 7;
    }

    std::vector<glm::vec4> getSamples(unsigned index, bool transform) const final {
        std::vector<glm::vec4> res;
        res.reserve(_points.size() / _nDims);

        if (!transform) {
            index = 1;
        }

        glm::uvec3 projection = _projections[index];

        for (int i = 0; i < _points.size(); i += _nDims) {
            res.emplace_back(_points[i + projection.x], _points[i + projection.y], _points[i + projection.z], 1);
        }


        return res;
    }

};


#endif //SCUS_SAMPLERCASCADEDSOBOL_H
