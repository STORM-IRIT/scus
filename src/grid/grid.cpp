#include "grid.h"

#include <utility>

Grid::Grid(unsigned int dimX, unsigned int dimY, unsigned int dimZ, unsigned int nbSamplePerCell,
           std::shared_ptr<WeightingFunction> weightingFunction, std::shared_ptr<Sampler> sampler,
           bool useRot, float cellDim, long fixedSeed)
        : _weighting(std::move(weightingFunction)), _sampler(std::move(sampler)), _dimX(dimX), _dimY(dimY), _dimZ(dimZ),
          _nbSamplesPerCell(nbSamplePerCell), _cellDim(cellDim),
          _nbCellsX{static_cast<unsigned>(dimX / cellDim)},
          _nbCellsY{static_cast<unsigned>(dimY / cellDim)},
          _nbCellsZ{static_cast<unsigned>(dimZ / cellDim)} {

    if (fmod(dimX, cellDim) != 0 || fmod(dimY, cellDim) != 0 || fmod(dimZ, cellDim) != 0) {
        std::cerr << "Grid dimensions should be multiples of cell dimension" << std::endl;
    }

    std::mt19937 generator;


    if (fixedSeed != -1) {
        _sampler->setRandomSeed(fixedSeed);
        generator.seed(fixedSeed);
    } else {
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }

    _sampler->initSamples(nbSamplePerCell);

    _cells.reserve(_nbCellsX * _nbCellsY * _nbCellsZ);
    long randomNumber;
    for (int i = 0; i < _nbCellsX; ++i) {
        const float Imin(i * _cellDim);
        const float Imax(Imin + _cellDim);
        for (int j = 0; j < _nbCellsY; ++j) {
            const float Jmin(j * _cellDim);
            const float Jmax(Jmin + _cellDim);
            for (int k = 0; k < _nbCellsZ; ++k) {
                randomNumber = generator();
                _cells.emplace_back(glm::vec3(Imin, Jmin, k * _cellDim),
                                    glm::vec3(Imax, Jmax, k * _cellDim + _cellDim),
                                    _cellDim,
                                    useRot,
                                    randomNumber,
                                    randomNumber % _sampler->maxTransformIndex()
                );
            }
        }
    }
}

std::vector<Cell>::iterator Grid::begin() {
    return _cells.begin();
}

std::vector<Cell>::iterator Grid::end() {
    return _cells.end();
}

std::vector<Cell>::const_iterator Grid::begin() const {
    return _cells.cbegin();
}

std::vector<Cell>::const_iterator Grid::end() const {
    return _cells.cend();
}

const std::shared_ptr<WeightingFunction> &Grid::weightingFunction() const {
    return _weighting;
}

const unsigned Grid::dimX() const {
    return _dimX;
}

const unsigned Grid::dimY() const {
    return _dimY;
}

const unsigned Grid::dimZ() const {
    return _dimZ;
}

void Grid::reset() const {
    std::for_each(std::execution::par_unseq, _cells.begin(), _cells.end(), [](const Cell &cell) {
        cell.clearIncluded();
    });
}

const unsigned Grid::nbSamples() const {
    return _nbSamplesPerCell;
}

const float Grid::cellDim() const {
    return _cellDim;
}

Grid Grid::fromJson(nlohmann::json jsonFile) {
    unsigned gridX = jsonFile.at("Dimension").at("x");
    unsigned gridY = jsonFile.at("Dimension").at("y");
    unsigned gridZ = jsonFile.at("Dimension").at("z");


    float cellDim = jsonFile.at("CellDim");
    unsigned samplesPerCell = jsonFile.at("SamplesPerCell");
    long seed = jsonFile.at("FixedSeed");

    std::shared_ptr<WeightingFunction> wf = WeightingFactory::createVolumeFromJson(jsonFile.at("WeightingObject"));
    std::shared_ptr<Sampler> sampler = SamplerFactory::createSampler(std::string(jsonFile.at("Sampler")));

    bool useRot = jsonFile.at("UseRot") && sampler->shouldTransform();

    return Grid(gridX, gridY, gridZ, samplesPerCell, wf, sampler, useRot, cellDim, seed);
}

const float Grid::expectedMax() const {
    return _weighting->expectedMax();
}

const std::shared_ptr<Sampler> &Grid::sampler() const {
    return _sampler;
}

void Grid::exportGridScatterersInUnitCube(const std::string &filename) const {
    if (_dimX != _dimY || _dimY != _dimZ) {
        std::cerr << "Grid is not a cube" << std::endl;
        return;
    }

    double d = _dimX;

    std::ofstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Issues (export grid) trying to create or open " << filename << std::endl;
    }

    std::vector<glm::vec4> points;
    for (const auto &cell: _cells) {
        cell.getPoints(_weighting, _sampler, points);
        for (const auto &point: points) {
            f << double(point.x) / d << " " << double(point.y) / d << " " << double(point.z) / d << "\n";
        }
    }

    f.flush();
    f.close();
}

utk::Pointset<3, double, utk::Point<3, double>> Grid::getPointsInUnitCube() {
    if (_dimX != _dimY || _dimY != _dimZ) {
        std::cerr << "Grid is not a cube" << std::endl;
        return {};
    }

    utk::Pointset<3, double, utk::Point<3, double>> pointset;
    pointset.clear();
    double d = _dimX;

    std::vector<glm::vec4> points;
    for (const auto &cell: _cells) {
        cell.getPoints(_weighting, _sampler, points);
        for (const auto &point: points) {
            pointset.emplace_back(double(point.x) / d, double(point.y) / d, double(point.z) / d);
        }
    }


    return pointset;
}

void Grid::getCellPointsAt(const glm::vec3 &pos, std::vector<glm::vec4> &resPoints,
                           std::vector<glm::vec3> &clearPoints) const {
    unsigned long index = unsigned(pos.z / _cellDim) + unsigned(pos.y / _cellDim) * _nbCellsZ +
                          unsigned(pos.x / _cellDim) * _nbCellsZ * _nbCellsY;
    if (index >= 0 && index < _cells.size()) {
        if (!_cells[index].TestAndSetIncluded()) {
            _cells[index].getPoints(_weighting, _sampler, resPoints);
            clearPoints.emplace_back(pos);
        }
    } else {
        return;
    }
}

void Grid::clearCellFlagAt(const glm::vec3 &pos) const {
    unsigned long index = unsigned(pos.z / _cellDim) + unsigned(pos.y / _cellDim) * _nbCellsZ +
                          unsigned(pos.x / _cellDim) * _nbCellsZ * _nbCellsY;
    if (index >= 0 && index < _cells.size()) {
        _cells[index].clearIncluded();
    }
}
