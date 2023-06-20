#include "pointWriter.h"

void PointWriter::WritePoints(Grid &grid, AcquisitionZone &acquisitionZone, VisualisationMode visualisationMode,
                              const std::string &fileName) {
    std::vector<glm::vec4> points;
    std::ofstream f(fileName);
    if (!f.is_open()) {
        std::cerr << "Issues trying to create or open " << fileName << std::endl;
    }

    std::vector<glm::vec4> probePoints = acquisitionZone.getProbePoints(0.15f, 10);

    switch (visualisationMode) {
        case PROJECTED:
            acquisitionZone.acquireProjectedPoints(grid, points);
            f << "p" << std::endl;
            break;

        case RAW:
        case SPACE:
        default:
            acquisitionZone.acquirePoints(grid, points);
            f << "s" << std::endl;
            break;
    }

    f << grid.dimX() << " " << grid.dimY() << " " << grid.dimZ() << "\n";

    f << probePoints.size() / 8 << "\n";
    for (const auto &point: probePoints) {
        f << point.x << " " << point.y << " " << point.z << "\n";
    }

    for (const auto &point: points) {
        f << point.x << " " << point.y << " " << point.z << " " << point.w << "\n";
    }

    f << std::flush;
    std::cout << "Written data file : " << fileName << " | " << points.size() << " scatterers." << std::endl;
    f.close();
}


void PointWriter::WritePointsFromJson(const std::string &jsonName) {
    std::ifstream in(jsonName);
    if (!in.is_open() || in.fail()) {
        std::cerr << "Could not open settings file : " << jsonName << std::endl;
        return;
    }

    nlohmann::json jf = nlohmann::json::parse(in);

    Grid grid = Grid::fromJson(jf.at("Grid"));
    AcquisitionZone acquisitionZone = AcquisitionZone::fromJson(jf.at("AcquisitionZone"));
    std::string fileName(jf.at("OutFile").at("Name"));

    VisualisationMode mode = SPACE;
    if (jf.at("OutFile").at("Mode") == "SPACE") {
        mode = SPACE;
        PointWriter::WritePoints(grid, acquisitionZone, mode, fileName);
    } else if (jf.at("OutFile").at("Mode") == "PROJECTED") {
        mode = PROJECTED;
        PointWriter::WritePoints(grid, acquisitionZone, mode, fileName);
    } else if (jf.at("OutFile").at("Mode") == "RAW") {
        PointWriter::WritePointsRaw(grid, acquisitionZone, fileName);
    } else {
        std::cerr << "Unknown visualisation mode : " << jf.at("OutFile").at("Mode") << std::endl;
        return;
    }


}

void PointWriter::WritePointsRaw(Grid &grid, AcquisitionZone &acquisitionZone,
                                 const std::string &fileName) {
    std::vector<glm::vec4> points;
    std::ofstream f(fileName);
    if (!f.is_open()) {
        std::cerr << "Issues trying to create or open " << fileName << std::endl;
    }

    acquisitionZone.acquireProjectedPoints(grid, points);


    for (const auto &point: points) {
        f << -point.z << " " << point.y << " " << point.x << " " << point.w << "\n";
    }

    f << std::flush;
    std::cout << "Written raw data file : " << fileName << " | " << points.size() << " points." << std::endl;
}

void PointWriter::WritePointsRawFromJson(const std::string &jsonName) {
    std::ifstream in(jsonName);
    if (!in.is_open() || in.fail()) {
        std::cerr << "Could not open settings file : " << jsonName << std::endl;
        return;
    }

    nlohmann::json jf = nlohmann::json::parse(in);

    Grid grid = Grid::fromJson(jf.at("Grid"));
    AcquisitionZone acquisitionZone = AcquisitionZone::fromJson(jf.at("AcquisitionZone"));

    std::string fileName(jf.at("OutFile").at("Name"));

    PointWriter::WritePointsRaw(grid, acquisitionZone, fileName);
}

void PointWriter::WritePointsCreanuisFromJson(const std::string &jsonName) {
    std::ifstream in(jsonName);
    if (!in.is_open() || in.fail()) {
        std::cerr << "Could not open settings file : " << jsonName << std::endl;
        return;
    }

    nlohmann::json jf = nlohmann::json::parse(in);

    Grid grid = Grid::fromJson(jf.at("Grid"));
    AcquisitionZone acquisitionZone = AcquisitionZone::fromJson(jf.at("AcquisitionZone"));

    std::string fileName(jf.at("OutFile").at("Name"));
    fileName = fileName + ".bin";

    PointWriter::WritePointsCreanuis(grid, acquisitionZone, fileName);
}

void PointWriter::WritePointsCreanuis(Grid &grid, AcquisitionZone &acquisitionZone, const std::string &fileName) {
    std::vector<glm::vec4> points;
    std::ofstream f(fileName, std::ios::binary);
    if (!f.is_open()) {
        std::cerr << "Issues trying to create or open " << fileName << std::endl;
    }

    acquisitionZone.acquireProjectedPoints(grid, points);
    std::sort(points.begin(), points.end(), [](const glm::vec4 &a, const glm::vec4 &b) { return -a.z < -b.z; });

    auto val = std::uint32_t(points.size());
    f.write((char *) &val, sizeof(std::uint32_t));
    for (const auto &point: points) {
        auto val = float(-point.z / 1000);
        f.write((char *) &val, sizeof(float));
        std::cout << -point.z << std::endl;
    }
    for (const auto &point: points) {
        auto val = float(point.y / 1000);
        f.write((char *) &val, sizeof(float));
    }
    for (const auto &point: points) {
        auto val = float(point.x / 1000);
        f.write((char *) &val, sizeof(float));
    }
    for (const auto &point: points) {
        auto val = float(point.w);
        f.write((char *) &val, sizeof(float));
    }

    f << std::flush;
    std::cout << "Written creanuis data file : " << fileName << " | " << points.size() << " points." << std::endl;


}
