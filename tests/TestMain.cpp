#include "TestMacros.h"
#include <iostream>
#include "../include/Numerics.h"

int main() {
    initFPU();
    auto& testCases = getTestCases();
    std::cout << "Running " << testCases.size() << " tests..." << std::endl;
    for (auto& testCase : testCases) {
        std::cout << "Running " << testCase.name << "..." << std::endl;
        testCase.func();
        std::cout << testCase.name << " passed." << std::endl;
    }
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
