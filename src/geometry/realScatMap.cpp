#include "realScatMap.h"

RealScatMap::RealScatMap() {

}

std::vector<double>
RealScatMap::getCorrespondingScats(const AcquisitionZone &az, const Grid &grid, unsigned int resX, unsigned int resY) {
    std::vector<double> res(resX * resY, 0);
    std::vector<glm::vec4> scats;
    az.acquireProjectedPoints(grid, scats);
    std::vector<double> final;

//    std::for_each(std::execution::par_unseq,std::begin(scats),std::end(scats),[resX=resX,resY=resY,sig=_sig,&az=az,&res=res](glm::vec4& scat){
//        scat.w = scat.w * std::exp(-(scat.y*scat.y)/(2*sig));
//        scat.y = std::sqrt(scat.y*scat.y + scat.x*scat.x);
//    });
//    std::for_each(std::execution::unseq,std::begin(scats),std::end(scats),[resX=resX,resY=resY,sig=_sig,&az=az,&res=res](glm::vec4& scat) {
//        unsigned x = std::round( std::max(0.f,resX*((-scat.z/az.width()) + 0.5f)) );
//        unsigned y = std::round( std::max(0.f,resY*(scat.x/az.depth())-1.f) );
//
//        res[y*resX+x] += scat.w;
//    });
//
//#pragma omp parallel for
    for (int i = 0; i < scats.size(); ++i) {
        auto scat = scats[i];
        scat.w = scat.w * std::exp(-(scat.y * scat.y) / (2 * _sig));
        scat.y = std::sqrt(scat.y * scat.y + scat.x * scat.x);
        unsigned x = std::round(std::max(0.f, resX * ((-scat.z / az.width()) + 0.5f)));
        unsigned y = std::round(std::max(0.f, resY * (scat.x / az.depth()) - 1.f));
        res[y * resX + x] += scat.w;
    }

    return res;
}

void RealScatMap::setSigZ(float val) {
    _sig = val;
}

std::vector<unsigned char>
RealScatMap::getCorrespondingAbsScats(const AcquisitionZone &az, const Grid &grid, unsigned int resX,
                                      unsigned int resY) {
    std::vector<double> res = getCorrespondingScats(az, grid, resX, resY);

    std::for_each(std::execution::par_unseq, std::begin(res), std::end(res), [](double &x) {
        x = std::min(std::abs(x), 254.);
    });

    std::vector<unsigned char> resC(res.begin(), res.end());

    return resC;
}
