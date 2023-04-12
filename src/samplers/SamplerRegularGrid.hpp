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
#ifndef _UTK_SAMPLER_REGULAR_GRID_
#define _UTK_SAMPLER_REGULAR_GRID_

#include "../utility/samplingUtils.hpp"
#include "../pointsets/Pointset.hpp"
#include "Sampler.h"

#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>

namespace utk {

    class SamplerRegularGrid : public Sampler {
    private:
        std::vector<glm::vec4> _points;

    public:

        SamplerRegularGrid() { ; }

        bool shouldTransform() override { return false; };

        void setRandomSeed(long unsigned int arg_seed) override {}

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

        template<unsigned int D, typename T, typename P>
        bool generateSamples(Pointset<D, T, P> &arg_pts, unsigned long long int arg_points) {
            if (!arg_pts.empty()) {
                std::cout << "SamplerRegularGrid::generateSamples the pointset to fill is not empty, clearing it ..."
                          << std::endl;
                arg_pts.clear();
            }

            if (!isFloatingType<T>()) {
                std::cerr
                        << "SamplerRegularGrid::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double"
                        << std::endl;
                return false;
            }

            double nthroot = pow(arg_points + 0.0001, 1.0 / double(D));
            int near_nthroot = int(nthroot);
            bool is_whole_number = pow(near_nthroot, D) == arg_points;

            //
//			/// Number of points to generate needs to be a power of D
//			double nthroot = pow(arg_points+0.0001, 1.0 / double(D));
//			std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1)<< pow(nthroot, 3.0) << " " << nthroot << " " << floor(nthroot) << " " << int(nthroot) << std::endl;
//			bool is_whole_number = floor(nthroot) == nthroot;
//			int near_nthroot = int(nthroot);



            if (!is_whole_number) {
                std::stringstream ss_dim;
                ss_dim << D;

                std::stringstream ss_given;
                ss_given << arg_points;

                std::stringstream ss_used;
                ss_used << pow(near_nthroot, D);

                std::cout << "SamplerRegularGrid::generateSamples needs " + ss_given.str() +
                             " (the number of points to generate) to be a power of " + ss_dim.str() + ". Using " +
                             ss_used.str() + " points instead." << std::endl;

                arg_points = pow(near_nthroot, D);
            }

            //load domain & toricity
            for (uint d = 0; d < D; d++) {
                arg_pts.domain.pMin.pos()[d] = 0;
                arg_pts.domain.pMax.pos()[d] = 1;
            }
            arg_pts.toricity = 1;

            arg_pts.resize(arg_points);

            std::vector<int> counts;
            counts.resize(D + 1, 0); /// D+1 to avoid overflow
            for (uint i = 0; i < arg_points; i++) {
                for (uint d = 0; d < D; d++) {
                    arg_pts[i].pos()[d] = counts[d] * (1.0 / near_nthroot);
                }

                counts[0] += 1;
                for (uint j = 0; j < D; j++) {
                    if (counts[j] >= near_nthroot) {
                        counts[j] = 0;
                        counts[j + 1] += 1;
                    }
                }
            }

            return true;
        };

        std::vector<glm::vec4> getSamples(unsigned index, bool transform) const final {
            return _points;
        }


        unsigned maxTransformIndex() const final {
            return 1;
        }
    };


} //end namespace utk


#endif
