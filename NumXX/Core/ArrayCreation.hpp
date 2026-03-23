/* ArrayCreation.hpp */
#pragma once

#include "../Containers/NArray.hpp"

namespace numxx {
    // TODO: Open https://numpy.org/doc/stable/reference/routines.array-creation.html and implement functions.

    // Creates an array filled with zeros
    template <typename T = double>
    NArray<T> zeros(const size_t size) {
        return NArray<T>(size, T());
    }

    template <typename T = double>
    NArray<T> zeros(Shape shape) {
        return NArray<T>(std::move(shape), T());
    }

    // Creates an array of zeros with the same shape as another array
    template <typename T = double, typename U>
    NArray<T> zeros_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape(), T());
    }

    template <typename T = double, typename U>
    NArray<T> zeros_like(const std::vector<U>& other) {
        return NArray<T>(other.size(), T());
    }


    // Creates an array filled with ones
    template <typename T = double, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    NArray<T> ones(const size_t size) {
        return NArray<T>(size, static_cast<T>(1));
    }

    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones(Shape shape) {
        return NArray<T>(std::move(shape), static_cast<T>(1));
    }


    // Creates an array of ones with the same shape as another array
    template <typename T = double, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape(), static_cast<T>(1));
    }


    // Creates an array of evenly spaced values within a given interval
    inline NArray<double> linspace(const double start, const double stop, const uint32_t count, const bool endpoint = true) {
        const double step = (stop - start)/(count - static_cast<double>(endpoint));
        NArray<double> out(Shape{count});

        for(size_t i = 0; i < count; i++) {
            out(i) = start + (step * i);
        }
        return out;
    }


    // Creates an array with values from start to stop with a given step
    inline NArray<double> arange(const double start, const double stop, const double step = 1.0) {
        if ((start > stop) && (step > 0)) return NArray<double>();
        if (step == 0.0) return NArray<double>();

        const auto count = static_cast<size_t>(std::ceil((stop - start) / step));
        NArray<double> out(Shape{count});

        for(int i = 0; i < count; ++i) {
            out(i) = start + step * i;
        }
        return out;
    }
    
    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    inline NArray<T> arange(const T start, const T stop, const T step = T(1)) {
        if ((start > stop) && (step > 0)) return NArray<T>();
        if (step == T()) return NArray<T>();

        const auto count = static_cast<size_t>(std::ceil((stop - start) / step));
        NArray<T> out(Shape{count});

        for(int i = 0; i < count; ++i) {
            out(i) = start + step * i;
        }
        return out;
    }


    // Creates a 2D identity matrix with ones on the diagonal
    template <typename T = double, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    NArray<T> eye(const size_t n, size_t m = 0) {
        if (m == 0) m = n;

        NArray<T> out_data(Shape{n, m}, T());

        for (size_t i = 0; i < n && i < m; ++i) {
            out_data(i * m + i) = static_cast<T>(1);
        }
        return out_data;
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


    inline NArray<double> logspace(
        const float start, const float stop, const uint32_t num = 50,
        const bool endpoint = true, const float base = 10.0f
    ) {
        if (num == 0) return {};
        if (num == 1 && endpoint) {
            return NArray<double>({stop});
        }

        const float step = (stop - start)/(static_cast<float>(num) - static_cast<float>(endpoint));
        NArray<double> out(Shape{num});

        for(int i = 0; i < num; i++) {
            out(i) = std::powf(base, start + (step * i));
        }
        return out;
    }


    inline NArray<double> geomspace(
        const float start, const float stop, const uint32_t num = 50, const bool endpoint = true
    ) {
        if (num == 0) return {};
        if (num == 1 && endpoint) {
            return NArray<double>({stop});
        }

        const auto ratio = std::powf(stop / start, (1.0f/(static_cast<float>(num) - static_cast<float>(endpoint))));

        NArray<double> out(Shape{num});
        out(0) = start;

        for (int i = 1; i < num; ++i) {
            out(i) = out(i - 1) * ratio;
        }

        if (endpoint) {
            out(num - 1) = stop;
        }

        return out;
    }

} // namespace numxx
