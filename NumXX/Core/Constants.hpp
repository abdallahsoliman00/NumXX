/* Constants.hpp */
#pragma once

#include <limits>
#include <cmath>

namespace numxx {
    
    constexpr double pi = 3.14159265358979323846;
    constexpr double e = 2.71828182845904523536;

    template <typename T = float>
    constexpr T inf_t = std::numeric_limits<T>::infinity();

    constexpr float inf = inf_t<float>;

}   // namespace numxx
