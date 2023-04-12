#include "acquisitionZone.h"

AcquisitionZone::AcquisitionZone(glm::vec3 pos, glm::quat rotation, float width, float height, float depth) :
        _translation{glm::vec4(pos, 0)}, _rotation{rotation}, _height(height), _width(width), _depth(depth) {

    setRotation(rotation);
    setupSamplePoints();
}

bool AcquisitionZone::isInside(glm::vec3 point) const {
    glm::vec3 projectedPoint;
    projectPoint(point, projectedPoint);

    return projectedPoint.x >= _min().x && projectedPoint.y >= _min().y && projectedPoint.z >= _min().z
           && projectedPoint.x <= _max().x && projectedPoint.y <= _max().y && projectedPoint.z <= _max().z;
}

bool AcquisitionZone::cellIsInside(const Cell &cell) const {
    auto points = cell.getCorners();
    return std::any_of(points.begin(), points.end(), [this](glm::vec3 point) { return this->isInside(point); });
}

void AcquisitionZone::getPointsInsideZone(const Grid &grid, std::vector<glm::vec4> &resPoints) const {

    tbb::concurrent_vector<glm::vec4> concPoints;
    std::for_each(std::execution::par_unseq, std::cbegin(grid), std::cend(grid),
                  [&points = concPoints, &grid, this](const Cell &cell) {
                      std::vector<glm::vec4> cellPoints;
                      if (cellIsInside(cell)) {
                          cell.getPoints(grid.weightingFunction(), grid.sampler(), cellPoints);
                          for (const auto &point: cellPoints) {
                              if (this->isInside(point)) {
                                  points.emplace_back(point);
                              }
                          }
                      }
                  });
    resPoints.insert(resPoints.begin(), concPoints.begin(), concPoints.end());
}

void AcquisitionZone::projectPoint(const glm::vec4 &point, glm::vec4 &resPoint) const {
    float x = (point.x - _translation.x);
    float y = (point.y - _translation.y);
    float z = (point.z - _translation.z);

    resPoint.x = x * (qw2 + qx2 - qy2 - qz2) + y * (dqxqy + dqwqz) + z * (dqxqz - dqwqy);
    resPoint.y = x * (dqxqy - dqwqz) + y * (qw2 - qx2 + qy2 - qz2) + z * (dqyqz + dqwqx);
    resPoint.z = x * (dqxqz + dqwqy) + y * (dqyqz - dqwqx) + z * (qw2 - qx2 - qy2 + qz2);

    resPoint.w = point.w;
}

void AcquisitionZone::projectPoint(const glm::vec3 &point, glm::vec3 &resPoint) const {
    glm::vec4 res = glm::vec4(resPoint, 1.f);
    projectPoint(glm::vec4(point, 1), res);
    resPoint = res;
}

void AcquisitionZone::acquirePoints(const Grid &grid, std::vector<glm::vec4> &resPoints) const {
    resPoints.clear();
    if (grid.cellDim() >= _height || grid.cellDim() >= _width) {
        std::cerr << "Acquisition Zone thickness should be greater than cell dimension, considering all cells..."
                  << std::endl;
        std::vector<glm::vec4> cellPoints;
        for (const auto &cell: grid) {
            cell.getPoints(grid.weightingFunction(),
                           grid.sampler(),
                           cellPoints);
            for (const auto &point: cellPoints) {
                if (isInside(point)) {
                    resPoints.emplace_back(point);
                }
            }
        }
        return;
    }

    getPointsInsideZone_HashGrid(grid, resPoints);
}

void AcquisitionZone::acquireProjectedPoints(const Grid &grid, std::vector<glm::vec4> &resPoints) const {
    resPoints.clear();
    std::vector<glm::vec4> tmpPoints;

    if (grid.cellDim() >= _height || grid.cellDim() >= _width) {
        std::cerr << "Acquisition Zone thickness should be greater than cell dimension, considering all cells..."
                  << std::endl;
        std::vector<glm::vec4> cellPoints;
        glm::vec4 projectedPoint;
        for (const auto &cell: grid) {
            cell.getPoints(grid.weightingFunction(),
                           grid.sampler(),
                           cellPoints);
            for (const auto &point: cellPoints) {
                if (isInside(point)) {
                    projectPoint(point, projectedPoint);
                    resPoints.emplace_back(projectedPoint);
                }
            }
        }
        return;
    }
    getPointsInsideZone_HashGrid(grid, tmpPoints);
    projectPoints(tmpPoints, resPoints);
}

void AcquisitionZone::projectPoints(const std::vector<glm::vec4> &points, std::vector<glm::vec4> &resPoints) const {
    resPoints.clear();
    resPoints.reserve(points.size());
    glm::vec4 projectedPoint;
    for (auto &point: points) {
        projectPoint(point, projectedPoint);
        resPoints.emplace_back(projectedPoint);
    }
}

std::vector<glm::vec4> AcquisitionZone::getProbePoints(float depth, int elemnumber) const {
    std::vector<glm::vec4> probePoints;
    probePoints.reserve(elemnumber * 8);

    for (int i = 0; i < elemnumber * 2; i += 2) {
        float kerf = _width * 0.5f / float(elemnumber);
        float dec = _width * 0.125f / float(elemnumber);

        float start = lerp(-_width / 2.f + dec, _width / 2.f + kerf, (float(i) - 0.125f) / (2.f * float(elemnumber)));
        float end = lerp(-_width / 2.f + dec, _width / 2.f + kerf, (float(i) + 1.125f) / (2.f * float(elemnumber)));


        probePoints.emplace_back(_rotation * glm::vec4(-0.75, -_height / 2.f, start, 1) + _translation);
        probePoints.emplace_back(_rotation * glm::vec4(-depth - 0.75, -_height / 2.f, start, 1) + _translation);
        probePoints.emplace_back(_rotation * glm::vec4(-depth - 0.75, _height / 2.f, start, 1) + _translation);
        probePoints.emplace_back(_rotation * glm::vec4(-0.75, _height / 2.f, start, 1) + _translation);

        probePoints.emplace_back(_rotation * glm::vec4(-0.75, -_height / 2.f, end, 1) + _translation);
        probePoints.emplace_back(_rotation * glm::vec4(-depth - 0.75, -_height / 2.f, end, 1) + _translation);
        probePoints.emplace_back(_rotation * glm::vec4(-depth - 0.75, _height / 2.f, end, 1) + _translation);
        probePoints.emplace_back(_rotation * glm::vec4(-0.75, _height / 2.f, end, 1) + _translation);
    }


    return probePoints;
}

AcquisitionZone AcquisitionZone::fromJson(nlohmann::json jsonFile) {
    glm::vec3 position(jsonFile.at("Position").at("x"),
                       jsonFile.at("Position").at("y"),
                       jsonFile.at("Position").at("z"));

    float angle = jsonFile.at("Rotation").at("Angle");
    glm::quat rotation = glm::angleAxis(glm::radians(angle),
                                        glm::normalize(
                                                glm::vec3(
                                                        jsonFile.at("Rotation").at("Axis").at("x"),
                                                        jsonFile.at("Rotation").at("Axis").at("y"),
                                                        jsonFile.at("Rotation").at("Axis").at("z"))));
    float width = jsonFile.at("Dimension").at("Width");
    float height = jsonFile.at("Dimension").at("Height");
    float depth = jsonFile.at("Dimension").at("Depth");

    return AcquisitionZone(position, rotation, width, height, depth);
}

void AcquisitionZone::invProjectPoint(const glm::vec4 &point, glm::vec4 &resPoint) const {
    resPoint.x =
            point.x * (qw2 + qx2 - qy2 - qz2) + point.y * (dqxqy - dqwqz) + point.z * (dqxqz + dqwqy) + _translation.x;
    resPoint.y =
            point.x * (dqxqy + dqwqz) + point.y * (qw2 - qx2 + qy2 - qz2) + point.z * (dqyqz - dqwqx) + _translation.y;
    resPoint.z =
            point.x * (dqxqz - dqwqy) + point.y * (dqyqz + dqwqx) + point.z * (qw2 - qx2 - qy2 + qz2) + _translation.z;

    resPoint.w = point.w;
}

void AcquisitionZone::invProjectPoint(const glm::vec3 &point, glm::vec3 &resPoint) const {
    glm::vec4 res = glm::vec4(resPoint, 1.f);
    invProjectPoint(glm::vec4(point, 1.f), res);
    resPoint = res;
}

void AcquisitionZone::invProjectPoints(const std::vector<glm::vec4> &points, std::vector<glm::vec4> &resPoints) const {
    resPoints.clear();
    resPoints.reserve(points.size());
    glm::vec4 resPoint;
    for (auto &point: points) {
        invProjectPoint(point, resPoint);
        resPoints.emplace_back(resPoint);
    }
}

const glm::vec4 &AcquisitionZone::position() const {
    return _translation;
}

const glm::quat &AcquisitionZone::rotation() const {
    return _rotation;
}

AcquisitionZone AcquisitionZone::fromText(const std::string &text, float width, float height, float depth) {

    size_t prev = 0;
    size_t next = 0;

    float tab[7];
    for (float &i: tab) {
        next = text.find(' ', prev);
        std::string tmp = text.substr(prev, next - prev);
        i = std::stof(tmp);
        prev = next + 1;
    }

    glm::vec3 position(tab[0], tab[1], tab[2]);
    glm::quat rot(tab[3], tab[4], tab[5], tab[6]);

    return {position, rot, width, height, depth};
}

void AcquisitionZone::setPosition(glm::vec3 pos) {
    _translation = glm::vec4(pos, 1);
}

void AcquisitionZone::setPosition(glm::vec4 pos) {
    _translation = pos;
}

void AcquisitionZone::setRotation(glm::quat rot) {
    _rotation = rot;

    qw2 = _rotation.w * _rotation.w;
    qx2 = _rotation.x * _rotation.x;
    qy2 = _rotation.y * _rotation.y;
    qz2 = _rotation.z * _rotation.z;

    dqxqy = 2 * _rotation.x * _rotation.y;
    dqwqz = 2 * _rotation.w * _rotation.z;
    dqxqz = 2 * _rotation.x * _rotation.z;
    dqwqy = 2 * _rotation.w * _rotation.y;
    dqyqz = 2 * _rotation.y * _rotation.z;
    dqwqx = 2 * _rotation.w * _rotation.x;
}

void AcquisitionZone::setHeight(float height) {
    _height = height;
    setupSamplePoints();
}

void AcquisitionZone::setWidth(float width) {
    _width = width;
    setupSamplePoints();
}

void AcquisitionZone::setDepth(float depth) {
    _depth = depth;
    setupSamplePoints();
}

void AcquisitionZone::getPointsInsideZone_HashGrid(const Grid &grid, std::vector<glm::vec4> &resPoints) const {

    glm::vec3 projPoint;
    resPoints.reserve(_width * _depth * _height * grid.nbSamples());
    std::vector<glm::vec3> clearPoints(_width * _depth * _height / grid.cellDim());

    for (const auto &samplePoint: _samplePoints) {
        invProjectPoint(samplePoint, projPoint);
        std::vector<glm::vec4> points(grid.nbSamples());
        grid.getCellPointsAt(projPoint, points, clearPoints);
        for (const auto &point: points) {
            if (isInside(point)) {
                resPoints.emplace_back(point);
            }
        }
    }
    for (const auto &point: clearPoints) {
        grid.clearCellFlagAt(point);
    }
}

void AcquisitionZone::setupSamplePoints() {
    _samplePoints.clear();

    float step = 0.7f;

    unsigned numPointW = unsigned(_width / step) + 2;
    float widthOffset = ((numPointW - 1) * step - _width) / 2.f;

    unsigned numPointH = unsigned(_height / step) + 2;
    float heightOffset = ((numPointH - 1) * step - _height) / 2.f;

    unsigned numPointD = unsigned(_depth / step) + 2;
    float depthOffset = ((numPointD - 1) * step - _depth) / 2.f;

    _samplePoints.reserve(numPointD * numPointH * numPointW);

    for (unsigned i = 0; i < numPointD; ++i) {
        for (unsigned j = 0; j < numPointH; ++j) {
            for (unsigned k = 0; k < numPointW; ++k) {
                _samplePoints.emplace_back(i * step - depthOffset, j * step - heightOffset - (_height / 2.f),
                                           k * step - widthOffset - (_width / 2.f));
            }
        }
    }
}


