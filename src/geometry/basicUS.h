#ifndef SCUS_BASICUS_H
#define SCUS_BASICUS_H

#include <memory>
#include <cmath>
#include <array>

#include "scatMap.h"
#include "realScatMap.h"
#include "../grid/grid.h"
#include "PSF.h"

class BasicUS {
public:
    explicit BasicUS(const Grid &grid);

    std::vector<unsigned char> getCorrespondingUS(const AcquisitionZone &az, unsigned resX = 192, unsigned resY = 512);

    std::vector<unsigned char>
    getCorrespondingRealUS(const AcquisitionZone &az, const Grid &grid, unsigned resX = 192, unsigned resY = 512);

    std::vector<unsigned char>
    getCorrespondingRealUS_fixedPSF(const AcquisitionZone &az, const Grid &grid, unsigned resX = 192,
                                    unsigned resY = 512);

    void setSigAxial(float ax);

    void setSigLateral(float lat);

    void setRsmSig(float val);

    void setDynamicRange(float range);

    float getDynamicRange() const;

    void setPSF(const std::string &filename = "");

    void
    getCurrentApproximatePSF(std::vector<unsigned char> &psf, unsigned resX, unsigned resY, float width, float depth);

    void getCurrentImportedPSF(std::vector<unsigned char> &psf, unsigned resX, unsigned resY, float width, float depth);

private:
    std::unique_ptr<ScatMap> _sm;
    std::unique_ptr<RealScatMap> _rsm;

    double _expectedMax;

    unsigned _kernX;
    unsigned _kernY;

    float _sigAxial = 0.20;
    float _sigLateral = 1.15;
    float _range = 20.f;

    double _lowClamp;
    double _highClamp;

    PSF _psf;

    std::vector<double> _kernHor;
    std::vector<double> _kernVert;

    void generateKernel(unsigned kernX, unsigned kernY, double sigX, double sigY);

    static inline double H_sep(int x, double sigma) {
        //std::cout << exp(-M_PI*(x*x)/(1.15*sigma*sigma)) << std::endl;
        return exp(-M_PI * (x * x) / (1.15 * sigma * sigma));
    };


    static inline double H(int x, int y, double sigmaX, double sigmaY) {
        double sigmaX2 = sigmaX * sigmaX;
        double sigmaY2 = sigmaY * sigmaY;
        return (exp((-M_PI * (x * x) / (2 * sigmaX2)) + (-M_PI * (y * y) / (2 * sigmaY2))));
    }

    void convolve(const std::vector<double> &scats, unsigned resX, unsigned resY, std::vector<double> &res);

};


#endif //SCUS_BASICUS_H
