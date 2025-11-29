/* Linalg.hpp */
#pragma once

#include "Containers/NArray.hpp"
#include "Utils/LinalgUtils.hpp"


namespace numxx::linalg {

    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    T det(const NArray<T>& mat) {
        if (!mat.get_shape().is_square()) {
            throw error::ShapeError(util::toString(mat.get_shape()) +
                " is not square. Cannot take the determinant of non-square matrix.");
        }

        // Extra protection in case a scalar is used
        if (mat.get_total_size() == 1) return mat(0);

        // Base case is a 2x2 matrix
        if (mat.get_total_size() == 4) {
            return (mat(0) * mat(3)) - (mat(1) * mat(2));
        }

        auto r0 = mat[0];
        T res = 0;
        for (size_t i = 0; i < r0.get_total_size(); i++) {
            if (i%2 == 0) res += r0(i) * det(util::get_minor_matrix(mat, 0, i));
            else res += -r0(i) * det(util::get_minor_matrix(mat, 0, i));
        }
        return res;
    }

} // namespace numxx::linalg