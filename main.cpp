#include <iostream>

#include <vector>
#include <chrono>
#include "EigenImage.h"
#include "SeamlessCloning.h"

void usage(char *app_name) {
    std::cerr << "usage : " << app_name << " [-m] image_directory result_name" << std::endl;
    std::cerr << "\t-m :              specify to use mixing gradients" << std::endl;
    std::cerr << "\timage_directory : path to the directory which contains images. image_directory must contain"
            " 3 image files : source.png, target.png and mask.pgm." << std::endl;
    std::cerr << "\t                  source.png is the image to be cloned in target.png, mask.pgm is the portion of source.png that is cloned in target.png" << std::endl;
    std::cerr << "\tresult_name :     the filename of the resulting image to write in image_directory" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        usage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    bool mixingGradients;
    std::string dataPath, outputName;
    if (argc == 4) {
        if (strcmp(argv[1], "-m")) { usage(argv[0]); return 1; };
        mixingGradients = true;
        dataPath = argv[2];
        outputName = argv[3];
    } else {
        if (!strcmp(argv[1], "-m")) { usage(argv[0]); return 1; };
        mixingGradients = false;
        dataPath = argv[1];
        outputName = argv[2];
    }

    // Load images
    EigenImage mask, source, target;
    if (!mask.load(dataPath + "mask.pgm")) {
        std::cerr << "Can't load " + dataPath + "mask.pgm" << std::endl;
        return 1;
    }
    if (!source.load(dataPath + "source.png")) {
        std::cerr << "Can't load " + dataPath + "source.png" << std::endl;
        return 1;
    }
    if (!target.load(dataPath + "target.png")) {
        std::cerr << "Can't load " + dataPath + "target.png" << std::endl;
        return 1;
    }


    auto start = std::chrono::high_resolution_clock::now();

    // Compute seamless cloning
    EigenImage result = clone(mask, source, target, mixingGradients);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Done in " << elapsed_seconds.count() << "s" << std::endl;

    // Write result
    if (!result.write(dataPath + outputName + ".png")) {
        std::cerr << "Can't write " + dataPath + outputName + ".png" << std::endl;
        return 1;
    }

    return 0;
}

