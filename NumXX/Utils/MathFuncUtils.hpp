/* MathFuncUtils.hpp */
/* Everything below is mostly copied from STL's <complex> header file */
#pragma once

#include "../Core/Complex.hpp"


namespace numxx::util {

/* Exponentials and Logarithms */

template <typename T>
auto log(const T& num) -> decltype(std::log(num)) {
    return std::log(num);
}

template <typename T>
complex<T> log(const complex<T>& num) {
    return complex<T>(std::log(abs(num)), arg(num));
}

template <typename T>
auto log10(const T& num) -> decltype(std::log(num)) {
    return std::log10(num);
}

template <typename T>
complex<T> log10(const complex<T>& num) {
    return util::log(num)/std::log(T(10.0));
}


template <typename T>
auto exp(const T& num) -> decltype(std::log(num)) {
    return std::exp(num);
}

template <typename T>
complex<T> exp(const complex<T>& num) {
    return polar(std::exp(num.real()), num.imag());
}


/* Exponentiation */

// Complex number exponentiation
template <typename T, typename U,
    typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
auto pow(const complex<T>& num, const U& n)
{
    using R = decltype(std::pow(std::declval<T>(), std::declval<U>()));
    // if num is zero
    if (num.real() == T() && num.imag() == T()) return complex<R>(0,0);

    // if num is purely real
    if (num.imag() == T() && num.real() > T()) {
        return complex<R>(std::pow(num.real(), n));
    }
    // if number is properly complex
    auto temp = util::log(num);
    return polar<R>(std::exp(n * temp.real()), n * temp.imag());
}

// Complex - complex exponentiation
template <typename T, typename U,
    typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
auto pow(const complex<T>& num, const complex<U>& n) {
    using R = decltype(std::pow(std::declval<T>(), std::declval<U>()));

    if (n == 0) return complex<R>(1,0);

    return (num == T()) ?
            complex<R>(0,0) :
            util::exp(n * util::log(num));
}

// Regular exponentiation
template <typename T, typename U>
auto pow(const T& base, const U& exponent) -> decltype(std::pow(base, exponent)) {
    return std::pow(base, exponent);
}


// Complex square root
template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
auto sqrt(const complex<T>& z) {
    const T x = z.real();
    const T y = z.imag();

    if (x == T()) {
        T t = std::sqrt(std::abs(y) / 2);
        return complex<T>(t, y < T() ? -t : t);
    }

    T t = std::sqrt(2 * (z.abs() + std::abs(x)));
    T u = t / 2;

    if (x > T()) return complex<T>(u, y / t);

    return complex<T>(std::abs(y) / t, y < T() ? -u : u);
}

// Normal square root
template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>>
auto sqrt(const T& num) {
    return std::sqrt(num);
}


/* Regular and Complex Trig function overloads */

// sin
template <typename T>
auto sin(const T& num) -> decltype(std::sin(num)) {
    return std::sin(num);
}

template <typename T>
complex<T> sin(const complex<T>& num) {
    const T x = num.real();
    const T y = num.imag();
    return complex<T>(std::sin(x) * std::cosh(y), std::cos(x) * std::sinh(y));
}

// cos
template <typename T>
auto cos(const T& num) -> decltype(std::cos(num)) {
    return std::cos(num);
}

template <typename T>
complex<T> cos(const complex<T>& num) {
    const T x = num.real();
    const T y = num.imag();
    return complex<T>(std::cos(x) * std::cosh(y), -std::sin(x) * std::sinh(y));
}

// tan
template <typename T>
auto tan(const T& num) -> decltype(std::tan(num)) {
    return std::tan(num);
}

template <typename T>
complex<T> tan(const complex<T>& num) {
    return util::sin(num)/util::cos(num);
}


/* Regular and Complex Hyperbolic function overloads */

// sinh
template <typename T>
auto sinh(const T& num) -> decltype(std::sinh(num)) {
    return std::sinh(num);
}

template <typename T>
complex<T> sinh(const complex<T>& num) {
    const T x = num.real();
    const T y = num.imag();
    return complex<T>(std::sinh(x) * std::cos(y), std::cosh(x) * std::sin(y));
}

// cosh
template <typename T>
auto cosh(const T& num) -> decltype(std::cosh(num)) {
    return std::cosh(num);
}

template <typename T>
complex<T> cosh(const complex<T>& num) {
    const T x = num.real();
    const T y = num.imag();
    return complex<T>(std::cosh(x) * std::cos(y), std::sinh(x) * std::sin(y));
}

// tanh
template <typename T>
auto tanh(const T& num) -> decltype(std::tanh(num)) {
    return std::tanh(num);
}

template <typename T>
complex<T> tanh(const complex<T>& num) {
    return util::sinh(num)/util::cosh(num);
}

// asinh
template <typename T>
auto asinh(const T& num) -> decltype(std::asin(num)) {
    return std::asinh(num);
}

template <typename T>
complex<T> asinh(const complex<T>& num) {
    complex<T> temp(
        (num.real() - num.imag()) * (num.real()+num.imag()) + T(1.0),
        T(2.0) * num.real() * num.imag()
    );
    temp = util::sqrt(temp);
    return util::log(temp + num);
}

// acosh
template <typename T>
auto acosh(const T& num) -> decltype(std::acosh(num)) {
    return std::acosh(num);
}

template <typename T>
complex<T> acosh(const complex<T>& num) {
    return T(2.0) * util::log(util::sqrt(T(0.5) * (num + T(1.0)))
                 + std::sqrt(T(0.5) * (num - T(1.0))));
}

// atanh
template <typename T>
auto atanh(const T& num) -> decltype(std::atanh(num)) {
    return std::atanh(num);
}

template <typename T>
complex<T> atanh(const complex<T>& n) {
    const T i2 = n.imag() * n.imag();
    const T x = T(1.0) - i2 - n.real() * n.real();

    T _num = T(1.0) + n.real();
    T _den = T(1.0) - n.real();

    _num = i2 + _num * _num;
    _den = i2 + _den * _den;

    return complex<T>(T(0.25) * (std::log(_num) - std::log(_den)),
                 T(0.5) * std::atan2(T(2.0) * n.imag(), x));
}


// asin
template <typename T>
auto asin(const T& num) -> decltype(std::asin(num)) {
    return std::asin(num);
}

template <typename T>
complex<T> asin(const complex<T>& num) {
    complex<T> temp(-num.imag(), num.real());
    temp = util::asinh(temp);
    return complex<T>(temp.imag(), -temp.real());
}

// asin
template <typename T>
auto acos(const T& num) -> decltype(std::acos(num)) {
    return std::acos(num);
}

template <typename T>
complex<T> acos(const complex<T>& num) {
    const complex<T> temp = std::asin(num);
    const T pi_2 = 1.5707963267948966192313216916397514L;
    return complex<T>(pi_2 - temp.real(), -temp.imag());
}

// atan
template <typename T>
auto atan(const T& num) -> decltype(std::atan(num)) {
    return std::atan(num);
}

template <typename T>
complex<T> atan(const complex<T>& n) {
    const T r2 = n.real() * n.real();
    const T x = T(1.0) - r2 - n.imag() * n.imag();

    T _num = n.imag() + T(1.0);
    T _den = n.imag() - T(1.0);

    _num = r2 + _num * _num;
    _den = r2 + _den * _den;

    return complex<T>(T(0.5) * std::atan2(T(2.0) * n.real(), x),
                 T(0.25) * std::log(_num / _den));
}


} // namespace numxx::util