/* StringOps.hpp */
#pragma once

#include <sstream>
#include <cmath>
#include <iomanip>

#include "../Core/MathOps.hpp"
#include "../Core/Complex.hpp"

namespace numxx::util {

/* A struct containing any attributes that
will be used to format NArray printing */
typedef struct PrintAttributes {
    int largest_exponent = 0;
    int left_padding = 0;
    int right_padding = 0;
    bool is_scientific = false;
    bool is_complex = false;
    bool negative = false;
} PrintAttributes;


// Pads a number with zeros to the left
inline std::string fill_with_zeros(const int number, const int length = 2) {
    std::string num = std::to_string(number);
    if (static_cast<int>(num.size()) < length)
        return std::string(length - num.size(), '0') + num;
    return num;
}


// Converts anything to a string, given this thing has an << overload.
template <typename T>
std::string toString(const T& obj, const int float_precision = 0) {
    std::ostringstream oss;
    if(float_precision == 0)
        oss << obj;
    else
        oss << std::fixed << std::setprecision(float_precision) << obj;
    return oss.str();
}

// Removes all zeros at the end of a string
inline void remove_trailing_zeros(std::string& num_str) {
    while (!num_str.empty() && num_str.back() == '0')
        num_str.pop_back();
}


/* Print attribute helpers */

template <typename T>
bool is_scientific(T num) {
    T abs_num = std::abs(num);
    return ((abs_num >= 1e6) || (abs_num <= 1e-5 && abs_num != 0)) && (!isinf(num));
}

template <typename T>
bool is_scientific(const complex<T>& num) {
    return (is_scientific(num.real()) || is_scientific(num.imag()));
}

template <typename T>
bool is_scientific(const std::complex<T>& num) {
    return (is_scientific(num.real()) || is_scientific(num.imag()));
}


// TODO: cache result for later use?
// Counts the number of digits to the left of the decimal point
template <typename T>
int get_left_padding(T num) {
    if (num == 0) return 1;
    
    if constexpr (std::is_floating_point_v<T>) {
        T integer_part = std::floor(std::abs(num));
        if (integer_part == 0)
            return 1;
        return static_cast<int>(std::log10(integer_part) + 1);
    } else {
        return static_cast<int>(std::log10(std::abs(num)) + 1);
    }
}

template <typename T>
int get_left_padding(const complex<T>& num) {
    return get_left_padding(std::max(num.real(), num.imag()));
}

template <typename T>
int get_left_padding(const std::complex<T>& num) {
    return get_left_padding(std::max(num.real(), num.imag()));
}

// TODO: cache result for later use?
// Counts the number of digits to the right of the decimal point
template <typename T>
int get_right_padding(T num) {
    if constexpr (std::is_integral_v<T>)
        return 0;

    std::string s = toString(std::abs(num));

    // Find the decimal point
    const size_t dot = s.find('.');
    if (dot == std::string::npos)
        return 0;

    // If the last char is '.', remove it too
    if (!s.empty() && s.back() == '.')
        return 0;

    return static_cast<int>(s.size() - dot - 1);
}

template <typename T>
int get_right_padding(const complex<T>& num) {
    return std::max(get_right_padding(num.real()), get_right_padding(num.imag()));
}

template <typename T>
int get_right_padding(const std::complex<T>& num) {
    return std::max(get_right_padding(num.real()), get_right_padding(num.imag()));
}

// TODO: cache result for later use?
template <typename T>
int get_exponent(T num) {
    if (num == 0) return 0;
    const double lognum = std::log10(std::abs(static_cast<double>(num)));
    const int exponent = static_cast<int>(std::floor(lognum));
    return exponent;
}

template <typename T>
int get_exponent(const complex<T>& num) {
    return get_exponent(std::max(num.real(), num.imag()));
}

template <typename T>
int get_exponent(const std::complex<T>& num) {
    return get_exponent(std::max(num.real(), num.imag()));
}


template <typename T>
bool is_negative(T num) { return num < 0; }

template <typename T>
bool is_negative(complex<T> num) { return (num.real() < 0) || (num.imag() < 0); }

template <typename T>
bool is_negative(std::complex<T> num) { return (num.real() < 0) || (num.imag() < 0); }


template <typename dtype>
PrintAttributes GetPrintAttributes(dtype* data_ptr, const size_t arrsize) {
    PrintAttributes attributes;

    for(int i = 0; i < arrsize; i++) {

        // Check if should be printed as a complex number
        if constexpr (is_complex_v<dtype>) {
            if (data_ptr[i].imag()) {
                attributes.is_complex = true;
            }
        }

        // Check if number is to be printed in scientific notation
        if constexpr (std::is_floating_point_v<dtype> || is_complex_floating_point_v<dtype>) {
            if (is_scientific(data_ptr[i]))
                attributes.is_scientific = true;
        }

        // Get the largest exponent in case scientific notation is used
        int _exp = get_exponent(data_ptr[i]);
        if (std::abs(_exp) > attributes.largest_exponent)
            attributes.largest_exponent = std::abs(_exp);

        // Check if negative
        if (is_negative(data_ptr[i]))
            attributes.negative = true;

        // Check if largest
        int lpad = get_left_padding(data_ptr[i]);
        if(lpad > attributes.left_padding)
            attributes.left_padding = lpad;

        // Check highest precision
        if constexpr (std::is_floating_point_v<dtype> || is_complex_floating_point_v<dtype>) {
            int rpad = get_right_padding(data_ptr[i]);
            if (rpad > attributes.right_padding)
                attributes.right_padding = rpad;
        }
    }
    return attributes;
}

template <typename T>
std::string num_to_scientific(T num, const int exponent_length) {
    
    if(!isinf(num)) {
        std::string exp_sign;
        const int exponent = get_exponent(num);
        const double base = num * std::pow(10,-exponent);

        if(exponent >= 0) exp_sign = "e+";
        else exp_sign = "e-";   
        return std::to_string(base) + exp_sign + fill_with_zeros(std::abs(exponent), exponent_length);
    } else {
        return std::string(9, ' ') + std::to_string(num);
    }
}

inline void pad_left(std::string& str, const int pad_depth) {
    if(pad_depth)
        str.insert(0, pad_depth, ' ');
}

inline void pad_right(std::string& str, const int pad_depth) {
    if(pad_depth)
        str.append(pad_depth, ' ');
}


// Function used to print the numbers
template <typename T>
std::string num_to_str_from_attributes(T num, const PrintAttributes& attributes) {
    // Add space in place of -ve sign if num >= 0
    std::string result;
    if(attributes.negative && (num >= 0)) result += " ";

    // `is_scientific` overrides all other print attributes
    if(attributes.is_scientific)
        result += num_to_scientific(num, get_left_padding(attributes.largest_exponent));
    else {
        if(isinf(num)) {
            pad_left(result, attributes.left_padding + attributes.right_padding - 2);
            result += std::to_string(num);
        } else {
            pad_left(result, attributes.left_padding - get_left_padding(num));

            std::string num_str = toString(num, attributes.right_padding);
            if(attributes.right_padding) remove_trailing_zeros(num_str);
            result += num_str;
            if(attributes.right_padding) {
                num >= 0 ?
                    pad_right(result, attributes.right_padding + get_left_padding(num) + 1 - num_str.size())
                :   pad_right(result, attributes.right_padding + get_left_padding(num) + 2 - num_str.size());
            } else {
                num >= 0 ?
                    pad_right(result, get_left_padding(num) - num_str.size())
                :   pad_right(result, get_left_padding(num) + 1 - num_str.size());
            }
        }
    }
    return result;
}

template <typename T>
std::string num_to_str_from_attributes(const complex<T>& num, const PrintAttributes& attributes) {
    if (attributes.is_complex) {
        return "(" + num_to_str_from_attributes(num.real(), attributes) +
        (num.imag() >= 0 ? " + " : " - ") +
        num_to_str_from_attributes(std::abs(num.imag()), attributes) + "j)";
    }
    return num_to_str_from_attributes(num.real(), attributes);
}

template <typename T>
std::string num_to_str_from_attributes(const std::complex<T>& num, const PrintAttributes& attributes) {
    if (attributes.is_complex) {
        return "(" + num_to_str_from_attributes(num.real(), attributes) +
        (num.imag() >= 0 ? " + " : " - ") +
        num_to_str_from_attributes(std::abs(num.imag()), attributes) + "j)";
    }
    return num_to_str_from_attributes(num.real(), attributes);
}


// TODO: The biggest issue here is that the numbers used to calculate PrintAttributes are
// different to the ones being printed. The main cause is usually floating point number precision.

} // namespace numxx::util
