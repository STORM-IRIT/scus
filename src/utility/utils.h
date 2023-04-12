#ifndef SCUS_UTILS_H
#define SCUS_UTILS_H

#include <glm/glm.hpp>
#include "dcmtk/dcmdata/dctk.h"

class Utils {
public:
    static glm::vec3 getDICOMDimensions(const std::vector<std::string> &names) {
        if (names.empty()) {
            std::cerr << "Name list empty" << std::endl;
            exit(-1);
        }

        DcmFileFormat file_format;
        OFCondition status = file_format.loadFile(names.front().c_str());

        if (status.bad()) {
            std::cerr << "Problem openning file:" << names.front().c_str() << std::endl;
            exit(-1);
        }
        DcmDataset *dataset = file_format.getDataset();

        OFString property;
        OFCondition condition;

        condition = dataset->findAndGetOFStringArray(DCM_PixelSpacing, property);
        if (condition.good()) {
            std::cout << "Pixel spacing: " << property << std::endl;
        } else {
            std::cerr << "Could not get Pixel spacing" << std::endl;
            exit(-1);
        }

        float pixwidth = std::stof(property.substr(0, property.find("\\")).c_str());
        float pixheight = std::stof(property.substr(property.find("\\") + 1, property.size()).c_str());

        condition = dataset->findAndGetOFStringArray(DCM_SliceThickness, property);
        if (condition.good()) {
            std::cout << "Slice Thickness: " << property << std::endl;
        } else {
            std::cerr << "Could not get Slice Thickness" << std::endl;
            exit(-1);
        }

        float slicethickness = std::stof(property.c_str());

        condition = dataset->findAndGetOFStringArray(DCM_Columns, property);
        if (condition.good()) {
            std::cout << "Columns: " << property << std::endl;
        } else {
            std::cerr << "Could not get Columns" << std::endl;
            exit(-1);
        }

        float imageWidth = std::stoi(property.c_str());

        condition = dataset->findAndGetOFStringArray(DCM_Rows, property);
        if (condition.good()) {
            std::cout << "Rows : " << property << std::endl;
        } else {
            std::cerr << "Could not get Rows" << std::endl;
            exit(-1);
        }

        float imageHeight = std::stoi(property.c_str());

        float width = imageWidth * pixwidth;
        float height = imageHeight * pixheight;
        float thickness = slicethickness * names.size();

        return glm::vec3(height, thickness, width);
    }
};

#endif //SCUS_UTILS_H
