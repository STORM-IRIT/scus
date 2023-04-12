#ifndef SCUS_REALSCATMAP_H
#define SCUS_REALSCATMAP_H

#include <execution>

#include "../grid/grid.h"
#include "acquisitionZone.h"

class RealScatMap {
public:
    explicit RealScatMap();

    std::vector<double>
    getCorrespondingScats(const AcquisitionZone &az, const Grid &grid, unsigned resX = 192, unsigned resY = 512);

    std::vector<unsigned char>
    getCorrespondingAbsScats(const AcquisitionZone &az, const Grid &grid, unsigned resX = 192, unsigned resY = 512);

    void setSigZ(float val);

private:
    float _sig = 1;

};


#endif
