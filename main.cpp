#include <iostream>

#include <vector>
#include <chrono>
#include "EigenImage.h"
#include "imdisplay.h"
#include "SeamlessCloning.h"

int main(int argc, char* argv[]) {

    std::string dataPath = "../Images/gradf/";

    EigenImage mask(dataPath + "mask.pgm");
    EigenImage source(dataPath + "source.png");
    EigenImage target(dataPath + "target.png");

    mask.printInfos();
    source.printInfos();
    target.printInfos();

    SeamlessCloning sc;
    auto start = std::chrono::high_resolution_clock::now();
    EigenImage result = sc.compute(mask, source, target);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << elapsed_seconds.count() << "s" << std::endl;

    result.write(dataPath + "result.png");

    result.printInfos();

    imShow(mask, "Mask");
    imShow(source, "Source");
    imShow(target, "Target");
    imShow(result, "Result");

//    SeamlessCloning sc;
//    sc.test();

    std::cout << "Wait for windows to be closed !" << std::endl;
    waitImClosed();
    std::cout << "Closed !" << std::endl;

    return 0;
}

