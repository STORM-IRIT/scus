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
#ifndef MYSOBOL_SOBOLGENERATOR1D_H
#define MYSOBOL_SOBOLGENERATOR1D_H

#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <bitset>
#include <climits>
#include <limits>
#include <sstream>
#include <fstream>

#include "OwenScrambling.h"

#ifdef _MSC_VER 
typedef unsigned int uint;
#endif

inline uint32_t ReverseBits(uint32_t n) {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

class SobolGenerator1D{

public:
    //dimension of Sobol polynomial
    uint32_t s;
    uint32_t d;

    //binary Sobol polynomial
    uint32_t a;

    //Sobol matrice
    std::array<uint32_t, CHAR_BIT*sizeof(uint32_t)> m;

    inline SobolGenerator1D(){}

    inline SobolGenerator1D(uint32_t _d, uint32_t _s, uint32_t _a, const std::vector<uint32_t>& _m){
        init1D(_d, _s, _a, _m);
    }

    //Generate matrix from sobol polynomial rule
    inline void generateMatrix() {
        //fill each matrix rank
    	if (s == 0) {	// van der Corput
    		for (uint32_t k = s; k < m.size(); ++k)
    			m[k] = 1;
    	} else {
            for (uint32_t k = s; k < m.size(); ++k) {
                m[k] = 0;
                for (int i = 1; i < s; ++i) {
                    // `akj` stores aj_k, note that the polynomial rep is reversed
                    // `pw2` stores 2^k
                    const uint32_t ai = (a >> (s - 1 - i)) & 1;
                    const uint32_t pw2 = (1 << i);
                    m[k] ^= ai * pw2 * m[k - i];
                }
                m[k] ^= (1 << s) * m[k - s];
                m[k] ^= m[k - s];
            }
    	}
    }

    inline void init1D(uint32_t _d, uint32_t _s, uint32_t _a, const std::vector<uint32_t>& _m) {
        d = _d;
        s = _s;
        a = _a;
        for (int i = 0; i < _m.size(); ++i) {
            m[i] = _m[i];
        }
        generateMatrix();
    }

    inline uint32_t getSobolInt(uint32_t n) const{
        uint32_t res = 0;
        for (int i = 0; i < CHAR_BIT * sizeof(uint32_t); ++i) {
            res <<= 1;
            res ^= (n & 1) * m[i];
            n /= 2;
        }
        return res;
    }

    inline double getSobolDouble(uint32_t n) const{

        uint32_t res = getSobolInt(n);
        return double(res) / std::numeric_limits<uint32_t>::max();
    }

    inline friend std::ostream& operator<<(std::ostream& out, const SobolGenerator1D& sobol){

        out << sobol.d << "\t" << sobol.s << "\t" << sobol.a  << "\t";
        for (uint i = 0; i < sobol.s; ++i) {
            out << sobol.m[i] << " ";
        }

        return out;
    }

};


inline bool readOneInitVector(std::istream& in, uint32_t& d, uint32_t& s, uint32_t& a, std::vector<uint32_t>& m){

    std::string line;
    m.clear();
    //Read a line and skip if it's a header
    bool test;
    do {
        test = bool(getline(in, line));
    } while (test && line[0] == 'd');

    if(test){
        std::istringstream lineStream(line);

        lineStream >> d  >> s >> a;

        uint32_t mi;
        while(lineStream >> mi){
            m.push_back(mi);
        }
        return true;
    } else {
        return false;
    }
}

inline void load_init_table(std::istream& in,
                            std::vector<uint32_t>& d,
                            std::vector<uint32_t>& s,
                            std::vector<uint32_t>& a,
                            std::vector<std::vector<uint32_t>>& m,
                            uint32_t nbMax = -1,
                            uint32_t offset = 0){
    uint32_t ai;
    uint32_t di;
    uint32_t si;
    std::vector<uint32_t> mi;
    std::vector<uint32_t> mi_vdc = {1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1};
    while( offset-- && readOneInitVector(in, di, si, ai, mi) );
    int i = 0;
    // first, push a pseudo-entry for dim 0 == van der corput
    d.push_back(0);
    s.push_back(0);
    a.push_back(0);
    m.push_back(std::move(mi_vdc));
    // now, read enties from the input file
    while( i++ != nbMax && readOneInitVector(in, di, si, ai, mi) ){
        d.push_back(di);
        s.push_back(si);
        a.push_back(ai);
        if(si == 0)
        	m.push_back(std::move(mi_vdc));
        else
        	m.push_back(std::move(mi));
   }
}

inline void init_sobols(std::vector<SobolGenerator1D> &sobols,
                        const std::vector<uint32_t>& d,
                        const std::vector<uint32_t>& s,
                        const std::vector<uint32_t>& a,
                        const std::vector<std::vector<uint32_t>>& m
    ) {
    sobols.resize(m.size());
    for(int i = 0; i < m.size(); ++i) {
        sobols[i].init1D(d[i], s[i], a[i], m[i]);
    }
}

#endif //MYSOBOL_SOBOLGENERATOR1D_H
