#include <iostream>
#include <fstream>
#include <chrono>
#include "Scus.h"

#define STB_IMAGE_IMPLEMENTATION

#include "utility/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "utility/stb_image_write.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [Setting File]" << std::endl;
        return -1;
    }

    PointWriter::WritePointsFromJson(argv[1]);
    return 0;
}