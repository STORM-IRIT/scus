#include <cmath>
#include <algorithm>
#include "PSF.h"

PSF::PSF() {
    std::cout << "Init base PSF" << std::endl;
    initBasePSF();
}

PSF::PSF(const std::string &filename) {
    std::ifstream file;
    file.open(filename);

    if (!file.is_open()) {
        std::cerr << "Issue trying to open PSF file: " << filename << ", base PSF initialized" << std::endl;
        initBasePSF();
    } else {
        std::string line;
        int currLine = 0;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            if (currLine == 0) {
                if (!(ss >> _psfResX >> _psfResY)) {
                    std::cerr << "Issue reading PSF resolution in file: " << filename << std::endl;
                    std::cerr << "Initializing with base values: 50,50" << std::endl;
                    _psfResX = 50;
                    _psfResY = 50;
                }
                _psf.clear();
                _psf.reserve(_psfResX * _psfResY);
            } else if (currLine == 1) {
                if (!(ss >> _psfSizeX >> _psfSizeY)) {
                    std::cerr << "Issue reading PSF realsize in file: " << filename << std::endl;
                    std::cerr << "Initializing with base values: 5,5" << std::endl;
                    _psfSizeX = 5;
                    _psfSizeY = 5;
                }
            } else {
                double psfValue;
                if (!(ss >> psfValue)) {
                    std::cerr << "Issue trying to read PSF value number " << currLine - 1 << " using 0" << std::endl;
                    _psf.push_back(0);
                } else {
                    _psf.push_back(psfValue);
                }
            }
            currLine++;
        }
        file.close();
    }

    _pixelSizePSFX = _psfSizeX / float(_psfResX);
    _pixelSizePSFY = _psfSizeY / float(_psfResY);

    std::cout << "Initialized a PSF with values :" << std::endl;
    std::cout << "psfmmX : " << _psfSizeX << " Y : " << _psfSizeY << std::endl;
    std::cout << "psfresX : " << _psfResX << " Y : " << _psfResY << std::endl;
}

void PSF::initBasePSF() {
    _psfResX = 168;
    _psfResY = 40;
    _psfSizeX = 2;
    _psfSizeY = 1;

    _pixelSizePSFX = _psfSizeX / float(_psfResX);
    _pixelSizePSFY = _psfSizeY / float(_psfResY);

    _psf = PSF::_basePsf;
}

void PSF::convolve(const std::vector<double> &scats, unsigned int resX, unsigned int resY, float millimetersX,
                   float millimetersY, std::vector<double> &res) const {
    res.clear();
    res.resize(resX * resY);

    float pixelSizeImgX = millimetersX / float(resX);
    float pixelSizeImgY = millimetersY / float(resY);

    float ratioX = pixelSizeImgX / _pixelSizePSFX;
    float ratioY = pixelSizeImgY / _pixelSizePSFY;

    int psfSpanX = int(std::floor(_psfSizeX / pixelSizeImgX));
    int psfSpanY = int(std::floor(_psfSizeY / pixelSizeImgY));

//    std::cout << "psfmmX : " << _psfSizeX << " Y : " << _psfSizeY << std::endl;
//    std::cout << "psfresX : " << _psfResX << " Y : " << _psfResY << std::endl;
//    std::cout << std::endl;
//    std::cout << "mmX : " << millimetersX << " Y : " << millimetersY << std::endl;
//    std::cout << "resX : " << resX << " Y : " << resY << std::endl;
//    std::cout << std::endl;
//    std::cout << "pixSizIMGX : " << pixelSizeImgX << " Y:" << pixelSizeImgY << std::endl;
//    std::cout << "pixSizPSFX : " << pixelSizePSFX << " Y:" << pixelSizePSFY << std::endl;
//    std::cout << "ratioX : " << ratioX << " Y:" << ratioY << std::endl;
//    std::cout << "psfSpanX : " << psfSpanX << " Y:" << psfSpanY << std::endl;
//    std::cout << "______________________________________" << std::endl;


#pragma omp parallel for collapse(2)
    for (int j = 0; j < resY; ++j) {
        for (int i = 0; i < resX; ++i) {
            double val = 0;
            for (int ii = -psfSpanX / 2; ii < psfSpanX / 2; ++ii) {
                for (int jj = -psfSpanY / 2; jj < psfSpanY / 2; ++jj) {
                    if ((i - ii) >= 0 && (j - jj) >= 0 && i - ii < resX && j - jj < resY) {
                        val += _psf[int(std::round(float(jj + psfSpanY / 2) * ratioY)) * _psfResX +
                                    int(std::round(float(ii + psfSpanX / 2) * ratioX))] *
                               scats[(j - jj) * resX + (i - ii)];


                    }
                }
            }
            res[j * resX + i] = val;
        }
    }


}

void PSF::getDisplayablePSF(std::vector<unsigned char> &psf, unsigned int resX, unsigned int resY, float width,
                            float depth) const {
    psf.clear();
    psf.reserve(resX * resY);

    std::vector<double> rawPSF;
    rawPSF.reserve(resX * resY);

    float psfOffsetX = (width - _psfSizeX) / 2;
    float psfOffsetY = (depth - _psfSizeY) / 2;

    float pixelSizeImgX = width / float(resX);
    float pixelSizeImgY = depth / float(resY);

    double min = 255.;
    double max = 0.;

    for (int y = 0; y < resY; ++y) {
        for (int x = 0; x < resX; ++x) {
            float posX = x * pixelSizeImgX;
            float posY = y * pixelSizeImgY;

            if (posX > psfOffsetX && posX < psfOffsetX + _psfSizeX && posY > psfOffsetY &&
                posY < psfOffsetY + _psfSizeY) {
                double val = _psf[int(std::floor(((posY - psfOffsetY) / _psfSizeY) * _psfResY)) * _psfResX +
                                  int(std::floor(((posX - psfOffsetX) / _psfSizeX) * _psfResX))];

                if (val > max) {
                    max = val;
                }
                if (val < min) {
                    min = val;
                }
                rawPSF.push_back(val);
            } else {
                rawPSF.push_back(0);
            }
        }
    }

    for (const auto &val: rawPSF) {
        psf.emplace_back(static_cast<unsigned char>(255 * ((val - min) / (max - min))));
    }
//    std::transform(rawPSF.begin(),rawPSF.end(),psf.begin(),[max,min](double val) {
//        return static_cast<unsigned char>(255*((val-min)/(max-min)));
//    });

}
