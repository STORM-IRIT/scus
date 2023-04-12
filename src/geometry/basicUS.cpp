#include "basicUS.h"
#include "omp.h"

BasicUS::BasicUS(const Grid &grid) {
    _sm = std::make_unique<ScatMap>(grid);
    _rsm = std::make_unique<RealScatMap>();
    _expectedMax = double(grid.expectedMax());

    _lowClamp = 2.5 * _expectedMax * std::pow(10., (-double(_range) / 20.));
    _highClamp = 2.5 * _expectedMax;
}

std::vector<unsigned char>
BasicUS::getCorrespondingUS(const AcquisitionZone &az, unsigned int resX, unsigned int resY) {
    std::vector<double> base = _sm->getCorrespondingScats(az, resX, resY);
    std::vector<double> res;

    double scaleX = double(resX) / az.width();
    double scaleY = double(resY) / az.depth();

    generateKernel(std::ceil(_sigLateral * 3 * scaleX), std::ceil(_sigAxial * 3 * scaleY), _sigLateral * scaleX,
                   _sigAxial * scaleY);

    convolve(base, resX, resY, res);

    std::for_each(std::execution::par_unseq, std::begin(res), std::end(res), [this](double &x) {
        x = std::clamp(std::abs(x), _lowClamp, _highClamp) / _highClamp;
        x = 20.0 * log10(x) + _range;
        x = 255.0 * x / _range;
    });

    std::vector<unsigned char> US;
    US.reserve(res.size());
    for (auto &e: res) {
        US.emplace_back(round(e));
    }
    return US;
}

void BasicUS::generateKernel(unsigned kernX, unsigned kernY, double sigX, double sigY) {
    _kernHor.clear();
    _kernVert.clear();

    _kernX = kernX;
    _kernY = kernY;

    for (int y = -int(kernY / 2); y <= int(kernY / 2); ++y) {
        _kernVert.emplace_back(H_sep(y, sigY));
    }
    for (int x = -int(kernX / 2); x <= int(kernX / 2); ++x) {
        _kernHor.emplace_back(H_sep(x, sigX));
    }


}


void
BasicUS::convolve(const std::vector<double> &image, unsigned int resX, unsigned int resY, std::vector<double> &res) {
    res.clear();
    res.resize(resX * resY);


#pragma omp parallel
    {
        //Horizontal conv
#pragma omp for collapse(2)
        for (int j = 0; j < resY; ++j) {
            for (int i = 0; i < resX; ++i) {
                double val = 0;
                for (int ii = 0; ii <= _kernX; ++ii) {
                    unsigned pos = (i - ii + (_kernX / 2)) + j * resX;
                    if (pos < image.size()) {
                        val += _kernHor[ii] * image[pos];
                    }
                }
                res[j * resX + i] = val;
            }
        }

#pragma omp barrier
        //Vertical conv
#pragma omp for collapse(2)
        for (int j = 0; j < resY; ++j) {
            for (int i = 0; i < resX; ++i) {
                double val = 0;
                for (int ii = 0; ii <= _kernY; ++ii) {
                    unsigned pos = i + (j - ii + (_kernY / 2)) * resX;
                    if (pos < res.size()) {
                        val += _kernVert[ii] * res[pos];
                    }

                }

                res[j * resX + i] = val;
            }
        }
    }
}


void BasicUS::setSigAxial(float ax) {
    _sigAxial = ax;
}

void BasicUS::setSigLateral(float lat) {
    _sigLateral = lat;
}

std::vector<unsigned char>
BasicUS::getCorrespondingRealUS(const AcquisitionZone &az, const Grid &grid, unsigned int resX, unsigned int resY) {
    std::vector<double> base = _rsm->getCorrespondingScats(az, grid, resX, resY);
    std::vector<double> res;

    double scaleX = double(resX) / az.width();
    double scaleY = double(resY) / az.depth();

    generateKernel(std::ceil(_sigLateral * 3 * scaleX), std::ceil(_sigAxial * 3 * scaleY), _sigLateral * scaleX,
                   _sigAxial * scaleY);

    convolve(base, resX, resY, res);

//    std::for_each(std::execution::par_unseq,std::begin(res),std::end(res),[this](double &x){
//        x = std::clamp(std::abs(x),0.125*_expectedMax,2.5*_expectedMax)/(2.5*_expectedMax);
//        x = 20.0*log10(x)+30.0;
//        x = 255.0*x/30.0;
//    });

    std::for_each(std::execution::par_unseq, std::begin(res), std::end(res), [this](double &x) {
        x = std::clamp(std::abs(x), _lowClamp, _highClamp) / _highClamp;
        x = 20.0 * log10(x) + _range;
        x = 255.0 * x / _range;
    });

    std::vector<unsigned char> US;
    US.reserve(res.size());
    for (auto &e: res) {
        US.emplace_back(round(e));
    }
    return US;
}

void BasicUS::setRsmSig(float val) {
    _rsm->setSigZ(val);
}

void BasicUS::setDynamicRange(float range) {
    _range = range;
    _lowClamp = 2.5 * _expectedMax * std::pow(10., (-double(_range) / 20.));
}

float BasicUS::getDynamicRange() const {
    return _range;
}

std::vector<unsigned char>
BasicUS::getCorrespondingRealUS_fixedPSF(const AcquisitionZone &az, const Grid &grid, unsigned int resX,
                                         unsigned int resY) {
    std::vector<double> base = _rsm->getCorrespondingScats(az, grid, resX, resY);
    std::vector<double> res;


    _psf.convolve(base, resX, resY, az.width(), az.depth(), res);

    std::for_each(std::execution::par_unseq, std::begin(res), std::end(res), [this](double &x) {
        x = std::clamp(std::abs(x), _lowClamp, _highClamp) / _highClamp;
        x = 20.0 * log10(x) + _range;
        x = 255.0 * x / _range;
    });

    std::vector<unsigned char> US;
    US.reserve(res.size());
    for (auto &e: res) {
        US.emplace_back(round(e));
    }
    return US;
}

void BasicUS::setPSF(const std::string &filename) {
    if (filename.empty()) {
        _psf = PSF();
    } else {
        _psf = PSF(filename);
    }
}

void
BasicUS::getCurrentApproximatePSF(std::vector<unsigned char> &psf, unsigned int resX, unsigned int resY, float width,
                                  float depth) {
    psf.clear();
    psf.reserve(resX * resY);

    std::vector<double> rawPSF;
    rawPSF.reserve(resX * resY);

    double scaleX = double(resX) / width;
    double scaleY = double(resY) / depth;

    float sigX = std::ceil(_sigLateral * 3 * scaleX);
    float sigY = std::ceil(_sigAxial * 3 * scaleY);

    double min = 255.;
    double max = 0.;
    int count = 0;


    for (int y = 0; y < resY; ++y) {
        for (int x = 0; x < resX; ++x) {
            double val = H(x - resX / 2, y - resY / 2, sigX, sigY);
            if (val > max) {
                max = val;
            }
            if (val < min) {
                min = val;
            }
            count++;
            rawPSF.emplace_back(val);
        }
    }

    for (const auto &val: rawPSF) {
        psf.emplace_back(static_cast<unsigned char>(255 * ((val - min) / (max - min))));
    }

//    std::transform(rawPSF.begin(),rawPSF.end(),psf.begin(),[max,min](double val) {
//       return static_cast<unsigned char>(255*((val-min)/(max-min)));
//    });
}

void BasicUS::getCurrentImportedPSF(std::vector<unsigned char> &psf, unsigned int resX, unsigned int resY, float width,
                                    float depth) {
    _psf.getDisplayablePSF(psf, resX, resY, width, depth);
}
