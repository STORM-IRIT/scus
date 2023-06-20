#include "dicom.h"

Dicom::Dicom(const std::vector<std::string> &names) {

    if (names.empty()) {
        std::cerr << "Name list empty" << std::endl;
        exit(-1);
    }

    std::cout << "Opening Dicom with " << names.size() << " slices" << std::endl;

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

    _pixWidth = std::stoi(property.c_str());

    condition = dataset->findAndGetOFStringArray(DCM_Rows, property);
    if (condition.good()) {
        std::cout << "Rows : " << property << std::endl;
    } else {
        std::cerr << "Could not get Rows" << std::endl;
        exit(-1);
    }

    _pixHeight = std::stoi(property.c_str());

    _width = float(_pixWidth) * pixwidth;
    _height = float(_pixHeight) * pixheight;
    _thickness = slicethickness * names.size();

    std::vector<unsigned char> temp;
    temp.reserve(_pixWidth * _pixHeight);
    for (const auto &name: names) {
        getDicomPixels(name, temp);
        _slices.emplace_back(temp);
    }

    std::cout << "Covered dimensions are: {x: " << _height << ", y: " << _thickness << ", z: " << _width << "}"
              << std::endl;

}

float Dicom::getWeight(float x, float y, float z) {


    if (x < 0 || y < 0 || z < 0) {
        return 0.f;
    }


    if (x > _height || y > _thickness || z > _width) {
        return 0.f;
    }


    float val = 0;
    double posX = (x / _height) * double(_pixHeight);
    double sliceIdx = (y / _thickness) * double(_slices.size());
    double posY = (z / _width) * double(_pixWidth);

    if (_interp) {
        //
        //     a--i1--b   y^
        //    /|     /|    |
        //   e--i2--f |    |
        //   | d--i4|-c    0--->x
        //   |/     |/    /
        //   h--i3--g    z

        unsigned fx = std::floor(posX);
        unsigned fy = std::floor(sliceIdx);
        unsigned fz = std::floor(posY);
        unsigned cx = std::ceil(posX);
        unsigned cy = std::ceil(sliceIdx);
        unsigned cz = std::ceil(posY);

        float a = getValueAt(fx, fz, cy);
        float b = getValueAt(cx, fz, cy);
        float c = getValueAt(cx, fz, fy);
        float d = getValueAt(fx, fz, fy);
        float e = getValueAt(fx, cz, cy);
        float f = getValueAt(cx, cz, cy);
        float g = getValueAt(cx, cz, fy);
        float h = getValueAt(fx, cz, fy);

        float dx = posX - fx;
        float dy = sliceIdx - fy;
        float dz = posY - fz;

        float i1 = lerp(a, b, dx);
        float i2 = lerp(e, f, dx);
        float i3 = lerp(h, g, dx);
        float i4 = lerp(d, c, dx);

        float j1 = lerp(i1, i2, dz);
        float j2 = lerp(i4, i3, dz);

        val = lerp(j2, j1, dy);

    } else {
        unsigned pixX = std::round(posX);
        unsigned pixY = std::round(posY);
        unsigned sliceIndex = std::round(sliceIdx);

        val = getValueAt(pixX, pixY, sliceIndex);
    }


    return val;
}

void Dicom::getDicomPixels(const std::string &filename, std::vector<unsigned char> &res) {
    DicomImage image(filename.c_str());

    if (image.getStatus() == EIS_Normal) {
        if (image.isMonochrome()) {

            image.setMinMaxWindow();

            auto *pixelData = (Uint8 *) (image.getOutputData(8 /* bits */));

            res.clear();
            res.insert(res.begin(), pixelData, pixelData + image.getHeight() * image.getWidth());
        }
    } else {
        std::cerr << "Error: cannot load DICOM image (" << DicomImage::getString(image.getStatus()) << ")" << std::endl;
        exit(-1);
    }
}

float Dicom::getValueAt(unsigned int x, unsigned int y, unsigned int z) {
    if (x < 0 || x > _pixHeight || y < 0 || y > _pixWidth || z < 0 || z >= _slices.size())
        return 0.f;

    return float(_slices[z][x * _pixWidth + y]);
}
