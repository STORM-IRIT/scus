cmake_minimum_required(VERSION 3.8)
project(Scus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# search for DCMTK library and header files
find_package(DCMTK REQUIRED)

add_compile_options(-Wunknown-pragmas -fopenmp)

# specify DCMTK header include directories
include_directories(${DCMTK_INCLUDE_DIRS})

set(Files src/main.cpp
        src/utility/samplingUtils.hpp

        src/samplers/SamplerDartThrowing.hpp

        src/samplers/SamplerRegularGrid.hpp

        src/samplers/SamplerCascadedSobol.h
        src/samplers/cascadedSobol/OwenScrambling.h
        src/samplers/cascadedSobol/Random.h
        src/samplers/cascadedSobol/SobolGenerator1D.h

        src/pointsets/Domain.hpp
        src/pointsets/Histogram.hpp
        src/pointsets/Point.hpp
        src/pointsets/Pointset.hpp
        src/pointsets/Vector.hpp

        src/grid/grid.cpp src/grid/grid.h
        src/grid/cell.cpp src/grid/cell.h

        src/geometry/acquisitionZone.cpp src/geometry/acquisitionZone.h

        src/volume/weightingFunction.h
        src/volume/sphere.h
        src/volume/plane.h
        src/volume/dicom.h src/volume/dicom.cpp

        src/utility/json.hpp
        src/utility/stb_image.h
        src/utility/stb_image_write.h
        src/utility/L2Discrepancy.hpp
        src/utility/GeneralizedL2Discrepancy.hpp
        src/utility/Diaphony.hpp

        src/utility/pointWriter.cpp src/utility/pointWriter.h
        src/utility/TestUtils.cpp src/utility/TestUtils.h
        src/volume/weightingFactory.h src/volume/cube.h
        src/samplers/SamplerUniformRandom.h src/samplers/Sampler.h
        src/samplers/SamplerFactory.h src/volume/dicom.cpp src/volume/dicom.h
        src/geometry/scatMap.cpp src/geometry/scatMap.h src/geometry/basicUS.cpp
        src/geometry/basicUS.h src/Scus.h
        src/geometry/PSF.h src/geometry/PSF.cpp

        src/utility/utils.h
        src/geometry/realScatMap.cpp src/geometry/realScatMap.h
        src/utility/TestUtils.cpp src/utility/TestUtils.h

        )

add_library (ScusLib ${Files} )

add_executable(${PROJECT_NAME} ${Files})


# link DCMTK library files
target_link_libraries(${PROJECT_NAME} ${DCMTK_LIBRARIES} tbb)
target_link_libraries(ScusLib ${DCMTK_LIBRARIES} tbb)
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OPENMP=1)
endif()
