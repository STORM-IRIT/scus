#ifndef SCUS_POINTWRITER_H
#define SCUS_POINTWRITER_H

#include <ostream>
#include <vector>

#include "../geometry/acquisitionZone.h"
#include "../grid/grid.h"
#include "../volume/weightingFunction.h"
#include "../volume/weightingFactory.h"
#include "json.hpp"


enum VisualisationMode {
    SPACE,
    PROJECTED,
    RAW
};

class PointWriter {
public:
    PointWriter() = delete;

    static void WritePoints(Grid &grid, AcquisitionZone &acquisitionZone, VisualisationMode visualisationMode,
                            const std::string &fileName = "toto.dat");

    static void WritePointsFromJson(const std::string &jsonName);

    static void
    WritePointsRaw(Grid &grid, AcquisitionZone &acquisitionZone, const std::string &fileName = "points.dat");

    static void WritePointsRawFromJson(const std::string &jsonName);

    static void
    WritePointsCreanuis(Grid &grid, AcquisitionZone &acquisitionZone, const std::string &fileName = "pointscrea.bin");

    static void WritePointsCreanuisFromJson(const std::string &jsonName);
};

#endif //SCUS_POINTWRITER_H
