#include "TestUtils.h"

#include "stb_image_write.h"

void TestUtils::testCubeExtract(long seed, const std::string &filename, unsigned int nbSamples) {
    std::shared_ptr<WeightingFunction> wf = std::make_shared<Cube>(glm::vec3(50), 10.f);
    std::shared_ptr<Sampler> sampler = std::make_shared<utk::SamplerDartThrowing>();

    Grid grid(100, 100, 100, nbSamples, wf, sampler, true, 1.f, seed);
    AcquisitionZone acq(glm::vec3(20, 40, 50), glm::angleAxis(glm::radians(20.f), glm::vec3(0, 0, 1)), 50.f, 1.1f,
                        60.f);

    PointWriter::WritePoints(grid, acq, VisualisationMode::SPACE, filename);
    PointWriter::WritePointsRaw(grid, acq, "raw_" + filename);
}

void TestUtils::testCubeRealUS(long seed, const std::string &filename, unsigned int nbSamples) {
    std::shared_ptr<WeightingFunction> wf = std::make_shared<Cube>(glm::vec3(50), 10.f);
    std::shared_ptr<Sampler> sampler = std::make_shared<utk::SamplerDartThrowing>();

    Grid grid(100, 100, 100, nbSamples, wf, sampler, true, 1.f, seed);
    AcquisitionZone acq(glm::vec3(20, 40, 50), glm::angleAxis(glm::radians(20.f), glm::vec3(0, 0, 1)), 50.f, 1.1f,
                        60.f);

    BasicUS bu(grid);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<unsigned char> res = bu.getCorrespondingRealUS(acq, grid, 500, 600);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Generated Real Us Image " << filename << " in " << duration.count() << " milliseconds" << std::endl;
    res.shrink_to_fit();
    stbi_write_jpg(filename.c_str(), 500, 600, 1, res.data(), 90);
}

void TestUtils::testCubeBasicUS(long seed, const std::string &filename, unsigned int nbSamples) {
    std::shared_ptr<WeightingFunction> wf = std::make_shared<Cube>(glm::vec3(50), 10.f);
    std::shared_ptr<Sampler> sampler = std::make_shared<utk::SamplerDartThrowing>();

    Grid grid(100, 100, 100, nbSamples, wf, sampler, true, 1.f, seed);
    AcquisitionZone acq(glm::vec3(20, 50, 50), glm::angleAxis(0.f, glm::vec3(1, 0, 0)), 50.f, 1.1f, 60.f);

    BasicUS bu(grid);

    std::vector<unsigned char> res = bu.getCorrespondingUS(acq, 500, 600);
    res.shrink_to_fit();
    stbi_write_jpg(filename.c_str(), 500, 600, 1, res.data(), 90);
}

void TestUtils::meanTimeCubeRealUs(long seed, unsigned nbRuns, unsigned int nbSamples) {
    std::shared_ptr<WeightingFunction> wf = std::make_shared<Cube>(glm::vec3(50), 10.f);
    std::shared_ptr<Sampler> sampler = std::make_shared<utk::SamplerDartThrowing>();

    Grid grid(100, 100, 100, nbSamples, wf, sampler, true, 1.f, seed);
    AcquisitionZone acq(glm::vec3(20, 50, 50), glm::angleAxis(0.f, glm::vec3(1, 0, 0)), 50.f, 1.1f, 60.f);

    BasicUS bu(grid);

    long long unsigned totalDur = 0;

    std::cout << "Launching real US test for " << nbRuns << " runs" << std::endl;
    for (unsigned i = 0; i < nbRuns; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<unsigned char> res = bu.getCorrespondingRealUS(acq, grid, 500, 600);
        auto stop = std::chrono::high_resolution_clock::now();
        totalDur += std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        if (!(i % (nbRuns / 20)))
            std::cout << i << " " << std::flush;
    }

    std::cout << std::endl << "Generated Real Us Image in mean time (" << nbRuns << " runs) "
              << double(totalDur) / double(nbRuns) << " milliseconds" << std::endl;
}

void TestUtils::meanTimeExtraction(long seed, unsigned int nbRuns, unsigned int dim, unsigned int nbSamples) {
    std::shared_ptr<WeightingFunction> wf = std::make_shared<Cube>(glm::vec3(50), 10.f);
    std::shared_ptr<Sampler> sampler = std::make_shared<utk::SamplerDartThrowing>();

    Grid grid(dim, dim, dim, nbSamples, wf, sampler, true, 1.f, seed);
    AcquisitionZone acq(glm::vec3(20, 50, 50), glm::angleAxis(0.f, glm::vec3(1, 0, 0)), 50.f, 1.1f, 60.f);
    std::vector<glm::vec4> res;

    long long unsigned totalDur = 0;

    std::cout << "Launching extraction test on grid " << dim << " for " << nbRuns << " runs" << std::endl;
    for (unsigned i = 0; i < nbRuns; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        acq.acquirePoints(grid, res);
        auto stop = std::chrono::high_resolution_clock::now();
        totalDur += std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        if (!(i % (nbRuns / 20)))
            std::cout << i << " " << std::flush;
    }

    std::cout << std::endl << "Extracted scatterers in mean time (" << nbRuns << " runs) "
              << double(totalDur) / double(nbRuns) << " milliseconds" << std::endl;
}

void TestUtils::CalcDiscr(unsigned int nbRun, unsigned int gridDim) {
    std::shared_ptr<WeightingFunction> wf = std::make_shared<Cube>(glm::vec3(50), 10.f);


    utk::L2Discrepancy l2d;

    std::ofstream outfile;
    long double res;

    for (int i = 0; i < nbRun; ++i) {
        std::shared_ptr<Sampler> samplerDT = std::make_shared<utk::SamplerDartThrowing>();
        std::shared_ptr<Sampler> samplerDTF = std::make_shared<utk::SamplerDartThrowing>();
        std::shared_ptr<Sampler> samplerUniform = std::make_shared<SamplerUniformRandom>();
        std::shared_ptr<Sampler> samplerRegular = std::make_shared<utk::SamplerRegularGrid>();
        std::shared_ptr<Sampler> samplerCascaded = std::make_shared<SamplerCascadedSobol>();
        std::cout << std::endl << "Run number " << i << std::endl;
        for (const auto nb: {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 27, 64, 125, 343}) {
            std::cout << nb << " " << std::flush;
            //Regular
            if (nb == 1 || nb == 8 || nb == 27 || nb == 64 || nb == 125 || nb == 343) {
                auto grid = Grid(gridDim, gridDim, gridDim, nb, wf, samplerRegular, false);

                auto points = grid.getPointsInUnitCube();
                l2d.compute(points, res);

                outfile.open("disc/Regular/" + std::to_string(nb) + ".dat", std::ios_base::app);
                outfile << res << std::endl;
                outfile.close();
            }

            //Dart
            auto gridDart = Grid(gridDim, gridDim, gridDim, nb, wf, samplerDT, true);

            auto points = gridDart.getPointsInUnitCube();
            l2d.compute(points, res);

            outfile.open("disc/Dart/" + std::to_string(nb) + ".dat", std::ios_base::app);
            outfile << res << std::endl;
            outfile.close();

            //Dart false
            auto gridDartf = Grid(gridDim, gridDim, gridDim, nb, wf, samplerDTF, false);

            points = gridDartf.getPointsInUnitCube();
            l2d.compute(points, res);

            outfile.open("disc/Dartf/" + std::to_string(nb) + ".dat", std::ios_base::app);
            outfile << res << std::endl;
            outfile.close();

            //Uniform
            auto gridUnif = Grid(gridDim, gridDim, gridDim, nb * gridDim * gridDim * gridDim, wf, samplerUniform, false,
                                 gridDim);

            points = gridUnif.getPointsInUnitCube();
            l2d.compute(points, res);

            outfile.open("disc/Uniform/" + std::to_string(nb) + ".dat", std::ios_base::app);
            outfile << res << std::endl;
            outfile.close();

            //Cascaded
            auto gridCascaded = Grid(gridDim, gridDim, gridDim, nb, wf, samplerCascaded, true);

            points = gridCascaded.getPointsInUnitCube();
            l2d.compute(points, res);

            outfile.open("disc/Cascaded/" + std::to_string(nb) + ".dat", std::ios_base::app);
            outfile << res << std::endl;
            outfile.close();
        }
    }


}

void TestUtils::createDataset(const std::string &datasetPath) {
    long seed = 19011998;
    auto samplerDart = std::make_shared<utk::SamplerDartThrowing>();
    auto wf = std::shared_ptr<WeightingFunction>(new Cube(glm::vec3(50), 10.f));
    auto grid = Grid(100, 100, 100, 32, wf, samplerDart, true, 1.f, seed);

    BasicUS bu = BasicUS(grid);
    bu.setDynamicRange(20.f);
    bu.setSigAxial(0.20f);
    bu.setSigLateral(1.25f);

    ScatMap sm = ScatMap(grid);

    std::mt19937 gen(seed);
    std::normal_distribution<> nd{0, 0.1f};

    int count = 0;


    std::ofstream f(datasetPath + "/infos.dat");
    if (!f.is_open()) {
        std::cerr << "Error: " << std::strerror(errno) << std::endl;
    }
    for (int x = 145; x < 255; ++x) {
        AcquisitionZone az = AcquisitionZone({25.f + nd(gen), (x / 4.) + nd(gen), 50.f + nd(gen)},
                                             glm::angleAxis(glm::radians(float(nd(gen))), glm::normalize(
                                                     glm::vec3(nd(gen) / 7.5, nd(gen) / 7.5, 1.f + nd(gen) / 7.5))),
                                             50.f, 5.f, 60.f);
        auto scatmap = sm.getCorrespondingScatMap(az, 500, 600);
        auto us = bu.getCorrespondingRealUS(az, grid, 500, 600);


        std::string smname(datasetPath + "/img_" + std::to_string(count) + ".jpg");
        stbi_write_jpg(smname.c_str(), 500, 600, 1, (void *) scatmap.data(),
                       90);

        std::string usname(datasetPath + "/us/img_" + std::to_string(count++) + ".jpg");
        stbi_write_jpg(usname.c_str(), 500, 600, 1, (void *) us.data(),
                       90);


        f << az.position().x << " " << az.position().y << " " << az.position().z << " ";
        f << az.rotation().w << " " << az.rotation().x << " " << az.rotation().y << " " << az.rotation().z << " ";
        f << az.width() << " " << az.depth() << std::endl;
    }

}

