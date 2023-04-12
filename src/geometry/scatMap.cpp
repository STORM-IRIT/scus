#include "scatMap.h"


ScatMap::ScatMap(const Grid &grid) : _wf{grid.weightingFunction()} {
    std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());

    _seed = generator();
}


std::vector<unsigned char>
ScatMap::getCorrespondingScatMap(const AcquisitionZone &az, unsigned int resX, unsigned int resY) {
    std::vector<unsigned char> res;
    res.reserve(resX * resY);

    for (unsigned y = 0; y < resY; ++y) {
        for (unsigned x = 0; x < resX; ++x) {
            float X = AcquisitionZone::lerp(az.width() / 2.f, -az.width() / 2.f, float(x) / float(resX));
            float Y = AcquisitionZone::lerp(0, az.depth(), float(y) / float(resY));

            glm::vec4 point;

            az.invProjectPoint({Y, 0.f, X, 1.f}, point);


            res.emplace_back(_wf->getWeight(point.x, point.y, point.z));
            //res.emplace_back(1.f);
        }
    }

    return res;
}

std::vector<double>
ScatMap::getCorrespondingScats(const AcquisitionZone &az, unsigned int resX, unsigned int resY) {
    glm::vec4 pos = az.position();
    glm::quat rot = az.rotation();
    auto localSeed = static_cast<long unsigned>(_seed * (pos.x * pos.y * pos.z +
                                                         pos.x * pos.y +
                                                         pos.y * pos.z +
                                                         pos.x * pos.z +
                                                         pos.x + pos.y + pos.z)
                                                * (rot.x * rot.y * rot.z * rot.w +
                                                   rot.x * rot.w +
                                                   rot.y * rot.w +
                                                   rot.x * rot.z +
                                                   rot.x + rot.y + rot.z + rot.w));
    auto generator = std::mt19937(localSeed);

    std::normal_distribution<double> distribution(0, 1);

    double absmax = 0;

    std::vector<double> res;
    res.reserve(resX * resY);

    for (unsigned y = 0; y < resY; ++y) {
        for (unsigned x = 0; x < resX; ++x) {
            float X = AcquisitionZone::lerp(az.width() / 2.f, -az.width() / 2.f, float(x) / float(resX));
            float Y = AcquisitionZone::lerp(0, az.depth(), float(y) / float(resY));

            glm::vec4 point;

            az.invProjectPoint({Y, 0.f, X, 1.f}, point);


            double w = distribution(generator) * _wf->getWeight(point.x, point.y, point.z);
            if (std::abs(w) > absmax) absmax = std::abs(w);

            res.emplace_back(w);
        }
    }

    //normalize output between -1 and 1
    //std::for_each(std::execution::par_unseq,std::begin(res),std::end(res),[absmax](double &x){x = x/absmax;});

    return res;
}

std::vector<unsigned char>
ScatMap::getCorrespondingAbsScats(const AcquisitionZone &az, unsigned int resX, unsigned int resY) {
    glm::vec4 pos = az.position();
    glm::quat rot = az.rotation();
    auto localSeed = static_cast<long unsigned>(_seed * (pos.x * pos.y * pos.z +
                                                         pos.x * pos.y +
                                                         pos.y * pos.z +
                                                         pos.x * pos.z +
                                                         pos.x + pos.y + pos.z)
                                                * (rot.x * rot.y * rot.z * rot.w +
                                                   rot.x * rot.w +
                                                   rot.y * rot.w +
                                                   rot.x * rot.z +
                                                   rot.x + rot.y + rot.z + rot.w));
    auto generator = std::mt19937(localSeed);

    std::normal_distribution<double> distribution(0, 1);

    double absmax = 0;

    std::vector<double> res;
    res.reserve(resX * resY);

    for (unsigned y = 0; y < resY; ++y) {
        for (unsigned x = 0; x < resX; ++x) {
            float X = AcquisitionZone::lerp(az.width() / 2.f, -az.width() / 2.f, float(x) / float(resX));
            float Y = AcquisitionZone::lerp(0, az.depth(), float(y) / float(resY));

            glm::vec4 point;

            az.invProjectPoint({Y, 0.f, X, 1.f}, point);


            double w = distribution(generator) * _wf->getWeight(point.x, point.y, point.z);
            if (std::abs(w) > absmax) absmax = std::abs(w);

            res.emplace_back(w);
        }
    }

    //abs to be displayed
    std::for_each(std::execution::par_unseq, std::begin(res), std::end(res), [](double &x) {
        x = std::min(std::abs(x), 254.);
    });

    std::vector<unsigned char> resC(res.begin(), res.end());

    return resC;
}
