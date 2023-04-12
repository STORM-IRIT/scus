#ifndef SCUS_GRID_H
#define SCUS_GRID_H

#include <vector>
#include <memory>
#include <execution>

#include "../samplers/Sampler.h"
#include "cell.h"
#include "../volume/weightingFunction.h"
#include "../volume/weightingFactory.h"
#include "../samplers/SamplerFactory.h"
#include "../utility/json.hpp"

using Point = utk::Point<3, double>;

class Grid {
public:
    Grid(unsigned dimX, unsigned dimY, unsigned dimZ, unsigned nbSamplePerCell,
         std::shared_ptr<WeightingFunction> weightingFunction, std::shared_ptr<Sampler> sampler,
         bool useRot, float cellDim = 1.f, long fixedSeed = -1);

    inline const Cell &operator[](unsigned index) const { return _cells[index]; };

    std::vector<Cell>::iterator begin();

    std::vector<Cell>::iterator end();

    [[nodiscard]] std::vector<Cell>::const_iterator begin() const;

    [[nodiscard]] std::vector<Cell>::const_iterator end() const;

    [[nodiscard]] const std::shared_ptr<WeightingFunction> &weightingFunction() const;

    [[nodiscard]] const std::shared_ptr<Sampler> &sampler() const;

    [[nodiscard]] const unsigned dimX() const;

    [[nodiscard]] const unsigned dimY() const;

    [[nodiscard]] const unsigned dimZ() const;

    [[nodiscard]] const unsigned nbSamples() const;

    [[nodiscard]] const float cellDim() const;

    [[nodiscard]] const float expectedMax() const;

    void reset() const;

    static Grid fromJson(nlohmann::json jsonFile);

    void exportGridScatterersInUnitCube(const std::string &filename) const;

    utk::Pointset<3, double, utk::Point<3, double>> getPointsInUnitCube();

    void
    getCellPointsAt(const glm::vec3 &pos, std::vector<glm::vec4> &resPoints, std::vector<glm::vec3> &clearPoints) const;

    void clearCellFlagAt(const glm::vec3 &pos) const;

private:
    std::vector<Cell> _cells;
    const std::shared_ptr<WeightingFunction> _weighting;
    const std::shared_ptr<Sampler> _sampler;

    const unsigned _nbSamplesPerCell;

    const float _cellDim;

    const unsigned _dimX;
    const unsigned _dimY;
    const unsigned _dimZ;

    const unsigned _nbCellsX;
    const unsigned _nbCellsY;
    const unsigned _nbCellsZ;
};


#endif //SCUS_GRID_H
