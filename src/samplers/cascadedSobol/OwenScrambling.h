/**
 MIT License
 
 Copyright (c) 2021 CNRS
 
 Loïs Paulin, David Coeurjolly, Jean-Claude Iehl, Nicolas Bonneel, Alexander Keller, Victor Ostromoukhov
 "Cascaded Sobol' Sampling", ACM Transactions on Graphics (Proceedings of SIGGRAPH Asia), 40(6), pp. 274:1–274:13, December 2021
 December 2021
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#ifndef SOBOL_OWENSCRAMBLING_H
#define SOBOL_OWENSCRAMBLING_H

#include <bitset>
#include <iostream>
#include "Random.h"

//seed has to stay the same for a given dimention
inline uint32_t OwenScrambling(uint32_t sobolPoint, uint32_t seed, const uint32_t owen_tree_depth) {

    RNG scramble_rng;

    constexpr uint32_t bits= 8*sizeof(uint32_t);
    constexpr uint32_t shift= 8*sizeof(uint32_t) -1;

    // flip root, node_index == 0, implicit rng.index(0)
    scramble_rng.seed(seed);
    uint32_t flip= scramble_rng.sample_range(2) << shift;
    uint32_t code= sobolPoint ^ flip;        // flip MSB

    for(uint32_t idigit= 1; idigit < owen_tree_depth; idigit++)
    {
        uint32_t level_base= (1u << idigit) - 1;
        uint32_t level_offset = sobolPoint >> (bits - idigit);	// level > 0 == 2^d nodes
        uint32_t node_index = level_base + level_offset;

        uint32_t node_seed= seed ^ node_index;
        scramble_rng.seed(node_seed);

        uint32_t flip= scramble_rng.sample_range(2) << (shift - idigit);
        code= code ^ flip;
    }

    return code;
}


#endif //SOBOL_OWENSCRAMBLING_H
