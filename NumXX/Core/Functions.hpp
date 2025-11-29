/* Functions.hpp */
#pragma once

#include <cmath>

#include "../Utils/FuncUtils.hpp"
#include "../Utils/MathFuncUtils.hpp"


namespace numxx {

/* ====== Trigonometric Functions ====== */

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto sin(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::sin(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto cos(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::cos(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto tan(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::tan(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto asin(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::asin(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto acos(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::acos(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto atan(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::atan(x); } );
}


template <typename T>
inline auto sin(T num) -> decltype(util::sin(num)) {
    return util::sin(num);
}

template <typename T>
inline auto cos(T num) -> decltype(util::cos(num)) {
    return util::cos(num);
}

template <typename T>
inline auto tan(T num) -> decltype(util::tan(num)) {
    return util::tan(num);
}

template <typename T>
inline auto asin(T num) -> decltype(util::asin(num)) {
    return util::asin(num);
}

template <typename T>
inline auto acos(T num) -> decltype(util::acos(num)) {
    return util::acos(num);
}

template <typename T>
inline auto atan(T num) -> decltype(util::atan(num)) {
    return util::atan(num);
}



/* ====== Hyperbolic Functions ====== */

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto sinh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::sinh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto cosh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::cosh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto tanh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::tanh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto asinh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::asinh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto acosh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::acosh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto atanh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::atanh(x); } );
}


template <typename T>
inline auto sinh(T num) -> decltype(util::sinh(num)) {
    return util::sinh(num);
}

template <typename T>
inline auto cosh(T num) -> decltype(util::cosh(num)) {
    return util::cosh(num);
}

template <typename T>
inline auto tanh(T num) -> decltype(util::tanh(num)) {
    return util::tanh(num);
}

template <typename T>
inline auto asinh(T num) -> decltype(util::asinh(num)) {
    return util::asinh(num);
}

template <typename T>
inline auto acosh(T num) -> decltype(util::acosh(num)) {
    return util::acosh(num);
}

template <typename T>
inline auto atanh(T num) -> decltype(util::atanh(num)) {
    return util::atanh(num);
}


/* ====== Exponentials and Logarithms ====== */

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto exp(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::exp(x); } );
}

template <typename T>
inline auto exp(T num) -> decltype(util::exp(num)) {
    return util::exp(num);
}


template <typename dtype, typename T,
    typename = std::enable_if_t<is_complex_or_arithmetic_v<dtype> && std::is_arithmetic_v<T>>>
auto pow(const NArray<dtype>& arr, const T& exponent)
    -> NArray<decltype(util::pow(std::declval<dtype>(), std::declval<T>()))>
{
    using R = decltype(util::pow(std::declval<dtype>(), std::declval<T>()));
    NArray<R> out(arr.get_shape());

    for(int i = 0; i < arr.get_total_size(); i++) {
        out.get_data()[i] = util::pow(arr.get_data()[i], exponent);
    }
    return out;
}

template <typename T, typename U>
inline auto pow(T base, U exponent) -> decltype(util::pow(base, exponent)) {
    return util::pow(base, exponent);
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto log(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::log(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto log10(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return util::log10(x); } );
}


template <typename T>
inline auto log(T num) -> decltype(util::log(num)) {
    return util::log(num);
}

template <typename T>
inline auto log10(T num) -> decltype(util::log10(num)) {
    return util::log10(num);
}


} // namespace numxx
