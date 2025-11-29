/* Random.hpp */
#pragma once

#include <random>
#include "Core/ArrayCreation.hpp"


namespace numxx::random {

inline int MAX_RAND = 2147483647;
inline std::random_device rd;
inline std::mt19937 gen(rd());
inline std::uniform_int_distribution<> uniform_int_dist(0, MAX_RAND);
inline std::normal_distribution<> normal_dist(0, 1);


// Seeds the random number generator with a given value
inline void seed(const unsigned num) {
    gen = std::mt19937(num);
}

// Returns a random float between 0 and 1 from a uniform distribution
inline double rand() {
    return uniform_int_dist(gen)/static_cast<double>(MAX_RAND);
}

inline NArray<double> rand(const Shape& shape) {
    auto out = empty<double>(shape);
    for (int i = 0; i < out.get_total_size(); i++) {
        out.get_data()[i] = uniform_int_dist(gen)/static_cast<double>(MAX_RAND);
    }
    return out;
}


// Returns a random value from a standard normal distribution (mean=0, std=1)
inline double randn() {
    return normal_dist(gen);
}

inline NArray<double> randn(const Shape& shape) {
    auto out = empty<double>(shape);
    for (int i = 0; i < out.get_total_size(); i++) {
        out.get_data()[i] = normal_dist(gen);
    }
    return out;
}


// Returns a random integer in the range [low, high] (inclusive)
inline int randint(int low, int high) {
    if (low > high) std::swap(low,high);
    return low + (uniform_int_dist(gen) % (high-low+1));
}

inline int randint(const int low) {
    if (low <= 0)
        throw error::ValueError("Please include a 'high' value or pick a number > 0.");
    return uniform_int_dist(gen) % (low+1);
}

inline NArray<int> randint(int low, int high, const Shape& shape) {
    if (low > high) std::swap(low,high);

    auto out = empty<int>(shape);
    for (int i = 0; i < out.get_total_size(); i++) {
        out.get_data()[i] = uniform_int_dist(gen) % (high-low+1);
    }
    return out;
}

template <typename T>
NArray<T> shuffle(const NArray<T>& arr) {
    const size_t len = arr.get_total_size();

    NArray<T> out(arr);

    for (size_t i = len - 1; i > 0; --i) {
        auto dist = std::uniform_int_distribution<size_t>(0, i);
        size_t j = dist(gen);

        std::swap(out.get_data()[i], out.get_data()[j]);
    }

    return out;
}

} // namespace numxx