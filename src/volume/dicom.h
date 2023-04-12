#ifndef SCUS_DICOM_H
#define SCUS_DICOM_H

#include "weightingFunction.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

class Dicom : public WeightingFunction {
public:
    Dicom(const std::vector<std::string> &names);

    ~Dicom() = default;

    float getWeight(float x, float y, float z) final;

    inline void setInterp(bool interp) { _interp = interp; };

    inline bool interp() const { return _interp; };

    inline float expectedMax() final { return 255.f; }


private:
    float _width;
    float _height;
    float _thickness;

    unsigned _pixWidth;
    unsigned _pixHeight;

    std::vector<std::vector<unsigned char>> _slices;

    bool _interp = true;

    static inline float lerp(float a, float b, float t) { return a + t * (b - a); };

    void getDicomPixels(const std::string &filename, std::vector<unsigned char> &res);

    float getValueAt(unsigned x, unsigned y, unsigned z);
};


#endif //SCUS_DICOM_H
