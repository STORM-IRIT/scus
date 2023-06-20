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
#ifndef SOBOL_RANDOM_H
#define SOBOL_RANDOM_H

#include <cstdint>

template<unsigned int rng_a, unsigned int rng_c, unsigned int rng_m>
struct TRNG {
    TRNG() :
        mstate(1u), mseed(1u) {
    }

    TRNG(const unsigned int _seed) :
        mstate(_seed), mseed(_seed) {
    }

    void seed(const unsigned int _seed) {
        //~ mseed= _seed;
        mseed = hash(_seed);
        mstate = mseed;
    }

    void index(const int _index) {
        // advance
        // cf http://www.pcg-random.org, c++ implementation
        unsigned int cur_mul = rng_a;
        unsigned int cur_add = rng_c;
        unsigned int acc_mul = 1u;
        unsigned int acc_add = 0u;
        unsigned int delta = _index;
        while (delta > 0u) {
            if (delta & 1u) {
                acc_mul = acc_mul * cur_mul;
                acc_add = acc_add * cur_mul + cur_add;
            }
            
            //~ cur_add= (cur_mul+1u) * cur_add;
            cur_add = cur_mul * cur_add + cur_add;
            cur_mul = cur_mul * cur_mul;
            delta = delta >> 1u;
        }
        
        // advance current state
        // mstate= acc_mul * mstate + acc_add;
        
        // advance to sample index
        mstate = acc_mul * mseed + acc_add;
    }

    unsigned int operator()() {
        return sample();
    }

    unsigned int sample() {
        mstate = (mstate * rng_a + rng_c) % rng_m;
        return mstate;
    }

    double sample_double() {
        return sample() / double(rng_m);
    }

    unsigned int sample_range(const unsigned int range) {
        // Efficiently Generating a Number in a Range
        // cf http://www.pcg-random.org/posts/bounded-rands.html
        unsigned int divisor = rng_m / range;
        if (divisor == 0)
            return 0;

        while (true) {
            unsigned int x = sample() / divisor;
            if (x < range)
                return x;
        }
    }

    static
    uint32_t hash(uint32_t a) {
        // cf http://www.burtleburtle.net/bob/hash/integer.html
        a = (a + 0x7ed55d16) + (a << 12);
        a = (a ^ 0xc761c23c) ^ (a >> 19);
        a = (a + 0x165667b1) + (a << 5);
        a = (a + 0xd3a2646c) ^ (a << 9);
        a = (a + 0xfd7046c5) + (a << 3);
        a = (a ^ 0xb55a4f09) ^ (a >> 16);
        return a;
    }

    static
    uint32_t hash3( uint32_t x ) {
        // finalizer from murmurhash3
        x ^= x >> 16;
        x *= 0x85ebca6bu;
        x ^= x >> 13;
        x *= 0xc2b2ae35u;
        x ^= x >> 16;
        return x;
    }
    
    static
    uint32_t hash_combine( uint32_t seed, uint32_t v )
    {
      return seed ^ (v + (seed << 6) + (seed >> 2));
    }

    
protected:
    unsigned int mstate;
    unsigned int mseed;

};

typedef TRNG<1103515245u, 12345u, 1u << 31> RNG;

#endif //SOBOL_RANDOM_H
