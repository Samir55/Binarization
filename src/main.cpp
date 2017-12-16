#include "Preprocessor/Preprocessor.h"


int main() {
    Preprocessor pp("img9.jpg");
    pp.pre_process();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}