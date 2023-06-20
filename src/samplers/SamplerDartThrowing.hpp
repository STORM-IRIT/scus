/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Halton project.
 */
#ifndef _UTK_SAMPLER_POISSONDT_
#define _UTK_SAMPLER_POISSONDT_

#include "../utility/samplingUtils.hpp"
#include "../pointsets/Pointset.hpp"
#include "Sampler.h"

#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace utk {

    class SamplerDartThrowing : public Sampler {
    private:
        std::vector<glm::vec4> _points;

    public:

        SamplerDartThrowing() {
            setRandomSeedTime();
            m_relaxed = true;
        }

        void initSamples(unsigned nbScatterers) override {
            utk::Pointset<3, double, utk::Point<3, double>> pointset;
            generateSamples(pointset, nbScatterers);

            _points.clear();
            _points.reserve(pointset.size());

            std::transform(pointset.begin(), pointset.end(), std::back_inserter(_points),
                           [](utk::Point<3, double> point) -> glm::vec4 {
                               return glm::vec4(point.pos()[0], point.pos()[1], point.pos()[2], 1);
                           });


        }


        void setRandomSeed(long unsigned int arg_seed) override { m_mersenneTwister.seed(arg_seed); }

        void setRandomSeedTime() {
            m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count());
        }

        void setRelaxed(bool arg_relaxed) { m_relaxed = arg_relaxed; }

        template<unsigned int D, typename T, typename P>
        bool generateSamples(Pointset<D, T, P> &arg_pts, unsigned int arg_points) {

            if (!arg_pts.empty()) {
                std::cout << "SamplerDartThrowing::generateSamples the pointset to fill is not empty, clearing it ..."
                          << std::endl;
                arg_pts.clear();
            }

            if (!isFloatingType<T>()) {
                std::cerr
                        << "SamplerDartThrowing::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double"
                        << std::endl;
                return false;
            }

            if (D < 2 || D > 8) {
                std::cerr
                        << "SamplerDartThrowing::generateSamples the maximum packing density has only been defined between 1 and 8 dimensions (http://mathworld.wolfram.com/HyperspherePacking.html)"
                        << std::endl;
                return false;
            }

            //load domain & toricity
            for (uint d = 0; d < D; d++) {
                arg_pts.domain.pMin.pos()[d] = 0;
                arg_pts.domain.pMax.pos()[d] = 1;
            }
            arg_pts.toricity = 1;

            arg_pts.reserve(arg_points);

            double gamman_max = 0;
            switch (D) {
                case 1:
                    gamman_max = 1;
                    break;
                case 2:
                    gamman_max = (1.0 / 6.0) * M_PI * sqrt(3);
                    break;
                case 3:
                    gamman_max = (1.0 / 6.0) * M_PI * sqrt(2);
                    break;
                case 4:
                    gamman_max = (1.0 / 16.0) * M_PI * M_PI;
                    break;
                case 5:
                    gamman_max = (1.0 / 30.0) * M_PI * M_PI * sqrt(2);
                    break;
                case 6:
                    gamman_max = (1.0 / 144.0) * M_PI * M_PI * M_PI * sqrt(3);
                    break;
                case 7:
                    gamman_max = (1.0 / 105.0) * M_PI * M_PI * M_PI;
                    break;
                case 8:
                    gamman_max = (1.0 / 384.0) * M_PI * M_PI * M_PI * M_PI;
                    break;
            }
            double distDT = pow((gamman_max / (double) arg_points) * tgamma(D / 2 + 1) / (pow(M_PI, D / 2)),
                                1.0 / (double) D);

            P p;
            for (uint i = 0; i < arg_points; ++i) {
                int iter = 1;
                bool accept = false;
                double current_dist = distDT;
                while (!accept) {
                    if (m_relaxed) {
                        if (iter % 1000 == 0) {
                            current_dist *= 0.9;
                        }
                    }

                    for (uint d = 0; d < D; d++)
                        p.pos()[d] = getRandom01();

                    accept = true;
                    for (uint j = 0; (j < arg_pts.size()) & (accept); ++j) {
                        double dist = 0;
                        for (uint d = 0; d < D; d++) {
                            double diff = std::abs((p.pos()[d] - arg_pts[j].pos()[d]));
                            double toric_diff = std::min(diff, 1 - diff);
                            dist += toric_diff * toric_diff;
                        }
                        dist = sqrt(dist);

                        if (dist < current_dist)
                            accept = false;
                    }
                    iter++;
                }
                arg_pts.emplace_back(p);
            }

            return true;
        };

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

    protected:
        std::mt19937 m_mersenneTwister;

        double getRandom01() {
            return (double) m_mersenneTwister() / (double) m_mersenneTwister.max();
        }

        bool m_relaxed;

    };

} //end namespace utk


#endif
