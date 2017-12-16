#include "Preprocessor/Preprocessor.h"


int main() {
    Preprocessor pp("img10.png");
    pp.pre_process();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}