/* Complex.hpp
 *
 * The only reason a custom complex type was made is that std::complex
 * does not offer the flexibility of performing operations with complex numbers of different types.
 * For example, std::complex<int>(1,1) + std::complex<float>(1,1) is not allowed,
 * whereas the same with numxx::complex is allowed.
 *
 * This was a requirement for NumXX since the flexibility of operations is a must.
 */
#pragma once

#include <type_traits>
#include <complex>
#include <mutex>

#include "Constants.hpp"


namespace numxx {

template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class complex {
public:
    /* ====== Constructors ====== */

    // Default constructor
    complex() : _real(0), _imaginary(0) {}

    // Normal constructor
    constexpr complex(const T& real, const T& imag = 0) : _real(real), _imaginary(imag) {}

    // Constructor form std::complex
    complex(const std::complex<T> num) : _real(num.real()), _imaginary(num.imag()) {}

    // Copy constructor
    complex(const complex& num) : _real(num.real()), _imaginary(num.imag()) {}

    // Move constructor
    complex(complex&& num) noexcept : _real(std::move(num._real)), _imaginary(std::move(num._imaginary)) {}


    /* ====== Helpers ====== */

    [[nodiscard]] T real() const { return _real; }

    [[nodiscard]] T imag() const { return _imaginary; }


    /* ====== Functionality ====== */

    [[nodiscard]] double abs() const {
        return std::hypot(_real, _imaginary);
    }

    [[nodiscard]] double arg() const {
        return std::atan2(_imaginary, _real);
    }

    [[nodiscard]] complex conj() const {
        return complex(_real, -_imaginary);
    }


    /* ====== Overloads ====== */
    // Print overload
    friend std::ostream& operator<< (std::ostream& os, const complex& number) {
        os << number.real() << (number.imag() >= 0 ? " + " : " - ")
        << std::abs(number.imag()) << "j";
        return os;
    }

    // Assignment operator
    complex& operator=(const complex&) = default;
    complex& operator=(complex&&) noexcept = default;

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator=(const complex<U>& other) {
        _real = static_cast<T>(other.real());
        _imaginary = static_cast<T>(other.imag());
        return *this;
    }

    /* ====== Comparison Overloads ====== */

    // operator==
    template <typename U>
    bool operator==(const complex<U>& other) const {
        return (_real == other.real()) && (_imaginary == other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator==(const U other) const {
        return (_real == other) && (_imaginary == 0);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator==(const U scalar, const complex& other) {
        return (scalar == other.real()) && (0 == other.imag());
    }


    // operator!=
    template <typename U>
    bool operator!=(const complex<U>& other) const {
        return !(*this == other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator!=(const U other) const {
        return !(*this == other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator!=(const U scalar, const complex& other) {
        return !(scalar == other);
    }


    // operator< (compares magnitudes)
    template <typename U>
    bool operator<(const complex<U>& other) const {
        return this->abs() < other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator<(const U other) const {
        return this->abs() < std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator<(const U scalar, const complex& other) {
        return std::abs(scalar) < other.abs();
    }


    // operator<= (compares magnitudes)
    template <typename U>
    bool operator<=(const complex<U>& other) const {
        return this->abs() <= other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator<=(const U other) const {
        return this->abs() <= std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator<=(const U scalar, const complex& other) {
        return std::abs(scalar) <= other.abs();
    }


    // operator> (compares magnitudes)
    template <typename U>
    bool operator>(const complex<U>& other) const {
        return this->abs() > other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator>(const U other) const {
        return this->abs() > std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator>(const U scalar, const complex& other) {
        return std::abs(scalar) > other.abs();
    }


    // operator>= (compares magnitudes)
    template <typename U>
    bool operator>=(const complex<U>& other) const {
        return this->abs() >= other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator>=(const U other) const {
        return this->abs() >= std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator>=(const U scalar, const complex& other) {
        return std::abs(scalar) >= other.abs();
    }



    /* ====== Arithmetic Overloads ====== */

    // operator+
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator+(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return complex<R>(_real+other.real(), _imaginary+other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator+(const U other) const
        -> complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return complex<R>(_real+other, _imaginary);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator+(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return complex<R>(scalar+other.real(), other.imag());
    }

    template <typename U>
    auto operator+(const std::complex<U>& other) const
        -> complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return complex<R>(_real+other.real(), _imaginary+other.imag());
    }

    template <typename U>
    friend auto operator+(const std::complex<U>& lhs, const complex& rhs)
        -> complex<decltype(std::declval<U>()+std::declval<T>())>
    {
        using R = decltype(std::declval<U>()+std::declval<T>());
        return complex<R>(lhs.real()+rhs.real(), lhs.imag()+rhs.imag());
    }


    // operator-
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator-(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return complex<R>(_real-other.real(), _imaginary-other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator-(const U other) const
        -> complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return complex<R>(_real-other, _imaginary);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator-(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return complex<R>(scalar-other.real(), other.imag());
    }

    template <typename U>
    auto operator-(const std::complex<U>& other) const
        -> complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return complex<R>(_real-other.real(), _imaginary-other.imag());
    }

    template <typename U>
    friend auto operator-(const std::complex<U>& lhs, const complex& rhs)
        -> complex<decltype(std::declval<U>()-std::declval<T>())>
    {
        using R = decltype(std::declval<U>()-std::declval<T>());
        return complex<R>(lhs.real()-rhs.real(), lhs.imag()-rhs.imag());
    }


    // operator*
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator*(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return complex<R>(
            _real*other.real() - _imaginary*other.imag(),
            _real*other.imag() + _imaginary*other.real());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator*(const U other) const
        -> complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return complex<R>(_real*other, _imaginary*other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator*(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return complex<R>(scalar*other.real(), scalar*other.imag());
    }

    template <typename U>
    auto operator*(const std::complex<U>& other) const
        -> complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return complex<R>(_real*other.real() - _imaginary*other.imag(),
                          _real*other.imag() + _imaginary*other.real());
    }

    template <typename U>
    friend auto operator*(const std::complex<U>& lhs, const complex& rhs)
        -> complex<decltype(std::declval<U>()*std::declval<T>())>
    {
        using R = decltype(std::declval<U>()*std::declval<T>());
        return complex<R>(lhs.real()*rhs.real() - lhs.imag()*rhs.imag(),
                          lhs.real()*rhs.imag() + lhs.imag()*rhs.real());
    }



    // operator/
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator/(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());

        const complex temp = this->operator*(other.conj());
        U denominator = (other.real()*other.real()) + (other.imag()*other.imag());
        return complex<R>(temp.real()/denominator, temp.imag()/denominator);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator/(const U other) const
        -> complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());
        return complex<R>(_real/other, _imaginary/other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator/(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());

        const complex temp = other.conj();
        T denominator = (other.real()*other.real()) + (other.imag()*other.imag());

        return complex<R>((temp.real()*scalar)/denominator, (temp.imag()*scalar)/denominator);
    }

    template <typename U>
    auto operator/(const std::complex<U>& other) const
        -> complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());
        auto denominator = other.real()*other.real() + other.imag()*other.imag();
        return complex<R>((_real*other.real() + _imaginary*other.imag())/denominator,
                          (_imaginary*other.real() - _real*other.imag())/denominator);
    }

    template <typename U>
    friend auto operator/(const std::complex<U>& lhs, const complex& rhs)
        -> complex<decltype(std::declval<U>()/std::declval<T>())>
    {
        using R = decltype(std::declval<U>()/std::declval<T>());
        auto denominator = rhs.real()*rhs.real() + rhs.imag()*rhs.imag();
        return complex<R>((lhs.real()*rhs.real() + lhs.imag()*rhs.imag())/denominator,
                          (lhs.imag()*rhs.real() - lhs.real()*rhs.imag())/denominator);
    }


    // operator +=
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator+=(const U& other) {
        _real += other;
        return *this;
    }

    template <typename U>
    complex& operator+=(const complex<U>& other) {
        _real += other.real();
        _imaginary += other.imag();
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator+=(const std::complex<U>& other) {
        _real += other.real();
        _imaginary += other.imag();
        return *this;
    }


    // operator -=
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator-=(const U& other) {
        _real -= other;
        return *this;
    }

    template <typename U>
    complex& operator-=(const complex<U>& other) {
        _real -= other.real();
        _imaginary -= other.imag();
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator-=(const std::complex<U>& other) {
        _real -= other.real();
        _imaginary -= other.imag();
        return *this;
    }


    // operator *=
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator*=(const U& other) {
        _real *= other;
        _imaginary *= other;
        return *this;
    }

    template <typename U>
    complex& operator*=(const complex<U>& other) {
        *this = this->operator*(other);
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator*=(const std::complex<U>& other) {
        *this = this->operator*(other);
        return *this;
    }


    // operator /=
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator/=(const U& other) {
        _real /= other;
        _imaginary /= other;
        return *this;
    }

    template <typename U>
    complex& operator/=(const complex<U>& other) {
        *this = this->operator/(other);
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    complex& operator/=(const std::complex<U>& other) {
        *this = this->operator/(other);
        return *this;
    }



private:
    T _real;
    T _imaginary;
};


/* ====== Function Wrappers ====== */

template <typename T>
T real(const complex<T>& num) { return num.real(); }

template <typename T>
T imag(const complex<T>& num) { return num.imag(); }

template <typename T>
double abs(const complex<T>& num) {
    return num.abs();
}

template <typename T>
double arg(const complex<T>& num) {
    return num.arg();
}

template <typename T>
double angle(const complex<T>& num) {
    return num.arg();
}

template <typename T>
complex<T> conj(const complex<T>& num) {
    return num.conj();
}

template <typename T>
complex<T> polar(const T& rho, const T& theta) {
    if (!rho) return complex<T>(0,0);
    return complex<T>(rho*std::cos(theta), rho*std::sin(theta));
}


template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T real(const T& num)
    { return num; }

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T real(const std::complex<T>& num)
    { return num.real(); }

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T imag(const T&)
    { return T(); }

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T imag(const std::complex<T>& num)
    { return num.imag(); }

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
double abs(const T& num)
    { return num >= 0 ? num : -num ; }

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
double arg(const T& num)
    { return num >= 0 ? 0 : pi ; }

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
double angle(const T& num)
    { return num >= 0 ? 0 : pi ; }

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T conj(const T& num)
    { return num; }


/* ====== Constants ====== */

// const complex<double> j(0,1);
// const auto i = j;


/* ====== Complex Number Type Checking ====== */

template<typename T>
struct is_complex : std::false_type {};

template<typename T>
struct is_complex<complex<T>> : std::true_type {};

template<typename T>
struct is_complex<std::complex<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_complex_v = is_complex<T>::value;


template<typename T>
struct is_complex_floating_point : std::false_type {};

template<typename T>
struct is_complex_floating_point<complex<T>> : std::bool_constant<std::is_floating_point_v<T>> {};

template<typename T>
inline constexpr bool is_complex_floating_point_v = is_complex_floating_point<T>::value;


template<typename T>
struct is_complex_or_arithmetic : std::bool_constant<std::is_arithmetic_v<T> || is_complex_v<T>> {};

template<typename T>
inline constexpr bool is_complex_or_arithmetic_v = is_complex_or_arithmetic<T>::value;


template <typename T>
struct underlying_type {
    using type = T;
};

template <typename T>
struct underlying_type<complex<T>> {
    using type = T;
};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

} // namespace numxx


constexpr numxx::complex<double> operator""_j(const long double value) {
    return {0,static_cast<double>(value)};
}

constexpr numxx::complex<long> operator""_j(const unsigned long long value) {
    return {0,static_cast<long>(value)};
}