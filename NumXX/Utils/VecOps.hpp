/* VecOps.hpp */
#pragma once

#include <vector>

#include "../Containers/Shape.hpp"

namespace numxx::util {

// Returns a vector containing the flattened matrix product
template <typename dtype, typename T>
auto matmul(
    const dtype* larr, const Shape& lshape,
    const T* rarr, const Shape& rshape
) -> std::vector<decltype(std::declval<dtype>() * std::declval<T>())>
{
    using U = decltype(std::declval<dtype>() * std::declval<T>());
    const auto out_shape = Shape::get_product_shape(lshape, rshape);
    std::vector<U> out(out_shape.get_total_size());
    
    size_t m,k,n;

    switch(Shape::get_matmul_type(lshape, rshape)) {
        case MatmulType::Dot:
            m = 1; k = lshape[0]; n = 1;
            break;
        case MatmulType::RowMat:
            m = 1; k = lshape[0]; n = rshape[1];
            break;
        case MatmulType::MatCol:
            m = lshape[0]; k = lshape[1]; n = 1;
            break;
        case MatmulType::MatMat:
            m = lshape[0]; k = lshape[1]; n = rshape[1];
            break;
        default:
            throw std::runtime_error("Unhandled MatmulType");
    }

    for (int i = 0; i < m; i++) {
        for (int t = 0; t < k; t++) {
            for (int j = 0; j < n; j++) {
                out[i * n + j] += larr[i * k + t] * rarr[t * n + j];
            }
        }
    }

    return out;
}


// Takes in an array, transposes it in place
template <typename dtype>
void transpose(dtype* arr, const Shape& shape) {
    if(shape.get_Ndim() == 1) return;
    
    std::vector<dtype> temp(arr, arr + shape.get_total_size());
    
    const size_t rows = shape[0];
    const size_t cols = shape[1];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[j * rows + i] = temp[i * cols + j];
        }
    }
}

// Takes in the array where the transpose result will be stored, the data, and transposes the array using the data 
template <typename dtype>
void transpose(dtype* arr, dtype* data_in, const Shape& shape) {
    if (shape.get_Ndim() == 1) {
        const size_t n = shape[0];
        for (int i = 0; i < n; i++)
            arr[i] = data_in[i];
        return;
    }

    const size_t rows = shape[0];
    const size_t cols = shape[1];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[j * rows + i] = data_in[i * cols + j];
        }
    }
}


// Splits a vector vin into n_groups
// Warning: DO NOT use if vector.size() % n_groups != 0
template <typename T>
std::vector<std::vector<T>> split(std::vector<T>&& vin, const size_t& n_groups) {
    const size_t grp_size = vin.size()/n_groups;

    std::vector<std::vector<T>> vout;
    vout.reserve(n_groups);

    for (size_t g = 0; g < n_groups; ++g) {
        vout.emplace_back();
        vout.back().reserve(grp_size);
    }

    for(size_t i = 0; i < vin.size(); i++) {
        vout[i/grp_size].push_back(std::move(vin[i]));
    }
    return vout;
}

} // namespace numxx::util