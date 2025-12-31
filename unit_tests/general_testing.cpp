#include <iostream>
#include "NumXX.hpp"

namespace nx = numxx;


template <typename T = std::string>
void print(const T& obj = " ", const int float_precision = 0) {
    if(float_precision == 0)
        std::cout << obj << std::endl;
    else
        std::cout << std::fixed << std::setprecision(float_precision) << obj << std::endl;
}


namespace numxx {
    // Place test code here


}


int main() {
    // or here
    auto arr = nx::zeros<nx::complex<float>>({2,4});

    auto n1 = arr[1][2];
    // n1 += (10+2_j);
    n1 += 10.1_j;

    print(arr);
}

