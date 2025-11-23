/* Misc.hpp */
#pragma once

#include "Core/NArray.hpp"

namespace numxx {

    // Used to cast an array to a new type `T`
    template <typename T, typename U>
    NArray<T> array_cast(const NArray<U>& arr) {
        NArray<T> out(arr.get_shape());

        for (size_t i = 0; i < arr.get_total_size(); i++) {
            out(i) = static_cast<T>(arr(i));
        }
        return out;
    }


    // Flattens the array and returns the sum
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    T sum(const NArray<T>& arr) {
        T sum = T();
        for (const auto& elem : arr) sum += elem;
        return sum;
    }

    // Returns the sum of an array given an axis
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    NArray<T> sum(const NArray<T>& arr, int axis)
    {
        const Shape& shape = arr.get_shape();
        const size_t ndim = shape.get_Ndim();

        if (axis < 0) axis += ndim;

        size_t strides[ndim];
        Shape::compute_strides(shape, strides, ndim);

        const size_t axis_size = shape[axis];
        int outer = 1;
        for (int i = 0; i < axis; ++i)
            outer *= shape[i];

        const int inner = strides[axis];

        const size_t out_size = outer * inner;
        NArray<T> out(Shape{out_size}, T());

        for (int m = 0; m < outer; ++m) {
            for (int n = 0; n < axis_size; ++n) {
                for (int k = 0; k < inner; ++k) {
                    out(m * inner + k) +=
                        arr(m * axis_size * inner + n * inner + k);
                }
            }
        }
        auto out_shape = shape.dimensions;
        out_shape.erase(out_shape.begin() + axis);
        return out.reshape(Shape{std::move(out_shape)});
    }


    // Flattens the array and returns the mean
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    T mean(const NArray<T>& arr) {
        T sum = T(), size = arr.get_total_size();
        for (const T& elem : arr) {
            sum += elem;
        }
        return sum/size;
    }

    // Returns the mean of an array along an axis
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    NArray<double> mean(const NArray<T>& arr, int axis) {
        return sum(arr, axis)/static_cast<double>(arr.get_shape()[axis]);
    }


    // Finds the differences between elements of an array
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    NArray<T> diff(const NArray<T>& arr, const uint16_t n = 1, int axis = -1) {
        if (n == 0) return arr;

        const Shape& shape = arr.get_shape();
        const size_t ndim = shape.get_Ndim();

        // Adjust axis
        if (axis < 0) axis += ndim;

        // If the diff is to be computed a number of times > the number of elements on said axis, return an empty array
        if (shape[axis] <= n) {
            std::cerr << "Warning: Returned an empty array. n >= Shape[axis]." << std::endl;
            return NArray<T>();
        }

        Shape out_shape = shape;
        out_shape(axis) -= n;

        size_t in_strides[ndim];
        size_t out_strides[ndim];
        Shape::compute_strides(shape, in_strides, ndim);
        Shape::compute_strides(out_shape, out_strides, ndim);

        NArray<T> out(out_shape);
        const size_t axis_stride = in_strides[axis];

        // For each output element
        for (size_t out_idx = 0; out_idx < out_shape.get_total_size(); out_idx++) {
            // Convert flat output index to multi-dimensional coords
            size_t coords[ndim];
            size_t temp = out_idx;
            for (size_t d = 0; d < ndim; d++) {
                coords[d] = temp / out_strides[d];
                temp %= out_strides[d];
            }

            // Convert coordinates to input flat index
            size_t in_idx = 0;
            for (size_t d = 0; d < ndim; d++) {
                in_idx += coords[d] * in_strides[d];
            }

            // Compute n-th order difference using binomial coefficients
            T result = 0;
            int sign = 1;
            for (uint16_t k = 0; k <= n; k++) {
                // Binomial coefficient: C(n, k)
                int coeff = 1;
                for (uint16_t j = 0; j < k; j++) {
                    coeff = coeff * (n - j) / (j + 1);
                }
                result += sign * coeff * arr(in_idx + (n - k) * axis_stride);
                sign = -sign;
            }
            out(out_idx) = result;
        }
        return out;
    }

    // Returns the largest element in the array. For complex numbers, the number with the largest argument is returned
    // By default, the input is flattened
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    T max(const NArray<T>& arr) {
        T max = arr(0);
        if constexpr (is_complex_v<T>) {
            for (int i = 0; i < arr.get_total_size(); ++i) {
                if (arr(i).arg() > max.arg()) max = arr(i);
            }
        } else {
            for (int i = 0; i < arr.get_total_size(); ++i) {
                if (arr(i) > max) max = arr(i);
            }
        }
        return max;
    }

    // Finds the max element along an axis
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    NArray<T> max(const NArray<T>& arr, int axis)
    {
        const Shape& shape = arr.get_shape();
        const size_t ndim = shape.get_Ndim();

        if (axis < 0) axis += ndim;

        size_t strides[ndim];
        Shape::compute_strides(shape, strides, ndim);

        const size_t axis_size = shape[axis];
        int outer = 1;
        for (int i = 0; i < axis; ++i)
            outer *= shape[i];

        const int inner = strides[axis];

        const size_t out_size = outer * inner;
        NArray<T> out(Shape{out_size});

        // Initialise with first elements along axis
        for (int m = 0; m < outer; ++m) {
            for (int k = 0; k < inner; ++k) {
                out(m * inner + k) = arr(m * axis_size * inner + k);
            }
        }

        // Find max along axis
        for (int m = 0; m < outer; ++m) {
            for (int n = 1; n < axis_size; ++n) {
                for (int k = 0; k < inner; ++k) {
                    size_t out_idx = m * inner + k;
                    T current = arr(m * axis_size * inner + n * inner + k);

                    if constexpr (is_complex_v<T>) {
                        if (current.arg() > out(out_idx).arg()) {
                            out(out_idx) = current;
                        }
                    } else {
                        if (current > out(out_idx)) {
                            out(out_idx) = current;
                        }
                    }
                }
            }
        }

        auto out_shape = shape.dimensions;
        out_shape.erase(out_shape.begin() + axis);
        return out.reshape(Shape{std::move(out_shape)});
    }

    // Returns the smallest element in the array. For complex numbers, the number with the smallest argument is returned
    // By default, the input is flattened
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    T min(const NArray<T>& arr) {
        T min = arr(0);
        if constexpr (is_complex_v<T>) {
            for (int i = 0; i < arr.get_total_size(); ++i) {
                if (arr(i).arg() < min.arg()) min = arr(i);
            }
        } else {
            for (int i = 0; i < arr.get_total_size(); ++i) {
                if (arr(i) < min) min = arr(i);
            }
        }
        return min;
    }


    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    NArray<T> min(const NArray<T>& arr, int axis)
    {
        const Shape& shape = arr.get_shape();
        const size_t ndim = shape.get_Ndim();

        if (axis < 0) axis += ndim;

        size_t strides[ndim];
        Shape::compute_strides(shape, strides, ndim);

        const size_t axis_size = shape[axis];
        int outer = 1;
        for (int i = 0; i < axis; ++i)
            outer *= shape[i];

        const int inner = strides[axis];

        const size_t out_size = outer * inner;
        NArray<T> out(Shape{out_size});

        // Initialize with first elements along axis
        for (int m = 0; m < outer; ++m) {
            for (int k = 0; k < inner; ++k) {
                out(m * inner + k) = arr(m * axis_size * inner + k);
            }
        }

        // Find max along axis
        for (int m = 0; m < outer; ++m) {
            for (int n = 1; n < axis_size; ++n) {
                for (int k = 0; k < inner; ++k) {
                    size_t out_idx = m * inner + k;
                    T current = arr(m * axis_size * inner + n * inner + k);

                    if constexpr (is_complex_v<T>) {
                        if (current.arg() < out(out_idx).arg()) {
                            out(out_idx) = current;
                        }
                    } else {
                        if (current < out(out_idx)) {
                            out(out_idx) = current;
                        }
                    }
                }
            }
        }

        auto out_shape = shape.dimensions;
        out_shape.erase(out_shape.begin() + axis);
        return out.reshape(Shape{std::move(out_shape)});
    }


} // namespace numxx