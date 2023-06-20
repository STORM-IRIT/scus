#ifndef SCUS_WEIGHTINGFACTORY_H
#define SCUS_WEIGHTINGFACTORY_H

#include <memory>
#include <iostream>

#include "weightingFunction.h"
#include "plane.h"
#include "sphere.h"
#include "cube.h"
#include "dicom.h"

#include "../utility/json.hpp"

class WeightingFactory {
public:
    static std::shared_ptr<WeightingFunction> createVolumeFromJson(nlohmann::json jsonFile) {
        std::string weighType(jsonFile.at("Type"));
        std::shared_ptr<WeightingFunction> wf;
        if (weighType == "Sphere") {
            wf.reset(new Sphere(glm::vec3(jsonFile.at("Position").at("x"),
                                          jsonFile.at("Position").at("y"),
                                          jsonFile.at("Position").at("z")),
                                jsonFile.at("Radius")));
        } else if (weighType == "Plane") {
            wf.reset(new Plane(jsonFile.at("Height")));
        } else if (weighType == "Cube") {
            wf.reset(new Cube(glm::vec3(jsonFile.at("Position").at("x"),
                                        jsonFile.at("Position").at("y"),
                                        jsonFile.at("Position").at("z")),
                              jsonFile.at("Side")));
        } else if (weighType == "Dicom") {
            std::vector<std::string> files = jsonFile.at("Files").get<std::vector<std::string>>();

            wf.reset(new Dicom(files));
        } else {
            std::cerr << "Unknown Weighting object in settings" << std::endl;
            wf.reset(new Plane(0.f));
        }

        return wf;
    };
};

#endif //SCUS_WEIGHTINGFACTORY_H
