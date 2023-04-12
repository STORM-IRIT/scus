#ifndef SCUS_ACQUISITIONZONE_H
#define SCUS_ACQUISITIONZONE_H


#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>

#include "../grid/grid.h"
#include "../grid/cell.h"
#include "../utility/json.hpp"


#include "tbb/concurrent_vector.h"
#include <execution>

class AcquisitionZone {
public:
    AcquisitionZone(glm::vec3 pos, glm::quat rotation, float width, float height, float depth);

    bool isInside(glm::vec3 point) const;

    bool cellIsInside(const Cell &cell) const;

    void getPointsInsideZone(const Grid &grid, std::vector<glm::vec4> &resPoints) const;

    void getPointsInsideZone_HashGrid(const Grid &grid, std::vector<glm::vec4> &resPoints) const;

    void projectPoint(const glm::vec4 &point, glm::vec4 &resPoint) const;

    void projectPoint(const glm::vec3 &point, glm::vec3 &resPoint) const;

    void invProjectPoint(const glm::vec4 &point, glm::vec4 &resPoint) const;

    void invProjectPoint(const glm::vec3 &point, glm::vec3 &resPoint) const;

    void projectPoints(const std::vector<glm::vec4> &points, std::vector<glm::vec4> &resPoints) const;

    void invProjectPoints(const std::vector<glm::vec4> &points, std::vector<glm::vec4> &resPoints) const;

    void acquirePoints(const Grid &grid, std::vector<glm::vec4> &resPoints) const;

    void acquireProjectedPoints(const Grid &grid, std::vector<glm::vec4> &resPoints) const;

    [[nodiscard]] std::vector<glm::vec4> getProbePoints(float depth = 15.f, int elemumber = 10) const;

    [[nodiscard]] const glm::vec4 &position() const;

    [[nodiscard]] const glm::quat &rotation() const;

    [[nodiscard]] inline const float height() const { return _height; };

    [[nodiscard]] inline const float width() const { return _width; };

    [[nodiscard]] inline const float depth() const { return _depth; };

    void setPosition(glm::vec3 pos);

    void setPosition(glm::vec4 pos);

    void setRotation(glm::quat rot);

    void setHeight(float height);

    void setWidth(float width);

    void setDepth(float depth);

    static inline float lerp(float a, float b, float t) { return a + t * (b - a); };

    static AcquisitionZone fromJson(nlohmann::json jsonFile);

    static AcquisitionZone fromText(const std::string &text, float width, float height, float depth);

private:

    float _height;
    float _width;
    float _depth;

    glm::vec4 _translation;
    glm::quat _rotation;

    //quat rot simplified
    float qw2;
    float qx2;
    float qy2;
    float qz2;

    float dqxqy;
    float dqwqz;
    float dqxqz;
    float dqwqy;
    float dqyqz;
    float dqwqx;

    std::vector<glm::vec3> _samplePoints;

    //in box coordinate
    [[nodiscard]] inline const glm::vec3 _min() const { return glm::vec3(0, -_height / 2.f, -_width / 2.f); };

    [[nodiscard]] inline const glm::vec3 _max() const { return glm::vec3(_depth, _height / 2.f, _width / 2.f); };

    void setupSamplePoints();

};


#endif

