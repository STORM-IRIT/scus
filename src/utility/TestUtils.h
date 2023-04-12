#ifndef SCUS_TESTUTILS_H
#define SCUS_TESTUTILS_H

#include "../Scus.h"
#include "L2Discrepancy.hpp"

class TestUtils {
public:
    TestUtils() = delete;

    static void testCubeExtract(long seed, const std::string &filename = "res.dat", unsigned nbSamples = 16);

    static void testCubeRealUS(long seed, const std::string &filename = "res.jpg", unsigned nbSamples = 16);

    static void testCubeBasicUS(long seed, const std::string &filename = "res.jpg", unsigned nbSamples = 16);

    static void meanTimeCubeRealUs(long seed, unsigned nbRuns, unsigned nbSamples = 16);

    static void meanTimeExtraction(long seed, unsigned nbRuns, unsigned dim, unsigned nbSamples = 16);

    static void createDataset(const std::string &datasetPath);

    static void CalcDiscr(unsigned nbRun, unsigned gridDim);
};


#endif //SCUS_TESTUTILS_H
