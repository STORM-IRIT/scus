#ifndef SCUS_SCATMAP_H
#define SCUS_SCATMAP_H

#include <chrono>
#include <random>
#include <execution>

#include "../grid/grid.h"
#include "../volume/weightingFunction.h"
#include "acquisitionZone.h"

class ScatMap {
public :
    explicit ScatMap(const Grid &grid);

    std::vector<unsigned char>
    getCorrespondingScatMap(const AcquisitionZone &az, unsigned resX = 192, unsigned resY = 512);

    std::vector<double> getCorrespondingScats(const AcquisitionZone &az, unsigned resX = 192, unsigned resY = 512);

    std::vector<unsigned char>
    getCorrespondingAbsScats(const AcquisitionZone &az, unsigned resX = 192, unsigned resY = 512);

private :
    std::shared_ptr<WeightingFunction> _wf;
    long unsigned _seed;


};


#endif //SCUS_SCATMAP_H
