/* LinalgUtils.hpp */
#pragma once

#include "../Containers/NArray.hpp"


namespace numxx::util {

    template <typename T>
    NArray<T> get_minor_matrix(const NArray<T>& mat, const size_t i, const size_t j) {
        auto& in_shape = mat.get_shape();
        auto &rows = in_shape[0], &cols = in_shape[1];

        if (in_shape.get_Ndim() != 2)
            throw error::ShapeError("An NArray with shape " + toString(in_shape) + " does not have a minor matrix");
        if (i >= rows || j >= cols) {
            throw error::ValueError("The indices i,j = " + toString(i) + "," + toString(j) +
                " are out of bounds for the shape " + toString(in_shape) + ". (Indexing begins at zero)");
        }

        Shape out_shape({rows-1, cols-1});
        NArray<T> out(out_shape);

        size_t count = 0;
        for (size_t r = 0; r < rows; r++) {
            for (size_t c = 0; c < cols; c++) {
                if ((r != i) && (c != j)) {
                    out(count++) = mat(r*cols + c);
                }
            }
        }
        return out;
    }

} // namespace numxx::util