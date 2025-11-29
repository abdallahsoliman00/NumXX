/* ArrayCreation.hpp */
#pragma once

#include "../Containers/NArray.hpp"

namespace numxx {
    // TODO: Open https://numpy.org/doc/2.3/reference/routines.array-creation.html and implement functions.

    // Creates an array filled with zeros
    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros(const size_t& size) {
        return NArray<T>(size, static_cast<T>(0));
    }

    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros(const Shape& shape) {
        return NArray<T>(shape, static_cast<T>(0));
    }

    // Creates an array of zeros with the same shape as another array
    template <typename T = double, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape(), static_cast<T>(0));
    }

    template <typename T = double, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros_like(const std::vector<U>& other) {
        return NArray<T>(other.size(), static_cast<T>(0));
    }


    // Creates an array filled with ones
    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones(const size_t& size) {
        return NArray<T>(size, static_cast<T>(1));
    }

    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones(const Shape& shape) {
        return NArray<T>(shape, static_cast<T>(1));
    }


    // Creates an array of ones with the same shape as another array
    template <typename T = double, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape(), static_cast<T>(1));
    }


    // Creates an array of evenly spaced values within a given interval
    inline NArray<double> linspace(const float& start, const float& stop, const size_t& count, const bool& endpoint = true) {
        const float step = (stop - start)/(count - static_cast<float>(endpoint));
        std::vector<double> out(count);

        for(size_t i = 1; i < count; i++) {
            out[i] = start + (step * i);
        }
        return NArray(out);
    }


    // Creates an array with values from start to stop with a given step
    inline NArray<double> arange(const float& start, const float& stop, const float& step = 1) {
        const auto count = static_cast<size_t>(1 + (stop - start)/step);
        std::vector<double> out(count);

        for(size_t i = 1; i < count; i++) {
            out[i] = start + step * i;
        }
        return NArray(std::move(out));
    }


    // Creates a 2D identity matrix with ones on the diagonal
    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> eye(size_t n, size_t m = 0) {
        if (m == 0) m = n;

        std::vector<T> out_data(n * m, static_cast<T>(0));

        for (size_t i = 0; i < n && i < m; ++i) {
            out_data[i * m + i] = static_cast<T>(1);
        }

        return NArray<T>(std::move(out_data), Shape({n, m}));
    }


    // Creates a square identity matrix
    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> identity(const size_t n) {
        return eye<T>(n);
    }


    // Creates an uninitialized array with the given shape
    template <typename T = double>
    NArray<T> empty(const Shape& shape) {
        return NArray<T>(shape);
    }

    template <typename T = double>
    NArray<T> empty(Shape&& shape) {
        return NArray<T>(std::move(shape));
    }

    template <typename T = double>
    NArray<T> empty(const size_t size) {
        return NArray<T>(Shape(size));
    }


    // Creates an uninitialized array with the same shape as another array
    template <typename T = double, typename U>
    NArray<T> empty_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape());
    }


    // Returns a new array of given shape and type, filled with `fill_value`
    template <typename T = double>
    NArray<T> full(Shape shape, T fill_value) {
        return NArray<T>(std::move(shape), fill_value);
    }

    template <typename T = double>
    NArray<T> full(Shape&& shape, T fill_value) {
        return NArray<T>(std::move(shape), fill_value);
    }

    template <typename T = double>
    NArray<T> full(const size_t shape, T fill_value) {
        return NArray<T>(Shape{shape}, fill_value);
    }


    // Returns a full array with the same shape as a given array
    template <typename T = double, typename U>
    NArray<T> full_like(const NArray<U>& other, T fill_value) {
        return NArray<T>(other.get_shape(), fill_value);
    }


    // Returns a shallow copy of the input array
    template <typename T>
    NArray<T> copy(const NArray<T>& arr) {
        return arr.copy();
    }

    // Returns a deep copy of the input array (could remove this and just keep the one below)
    template <typename T>
    NArray<T> deepcopy(const NArray<T>& arr) {
        return arr.deepcopy();
    }

    // Returns a deep copy of whatever element was given
    template <typename T>
    T deepcopy(const T& elem) {
        return T(elem);
    }

} // namespace numxx
