/* VecOps.hpp */
#pragma once

#include "../Containers/NArray.hpp"
#include "../Containers/Matrix.hpp"

namespace numxx {

// Matrix-multiplication of two matrices
template <typename dtype, typename T>
auto matmul(const NArray<dtype>& lmat, const NArray<T>& rmat)
    -> NArray<decltype(std::declval<dtype>() * std::declval<T>())>
{
    using U = decltype(std::declval<dtype>() * std::declval<T>());


    if(!static_cast<bool>(Shape::get_matmul_type(lmat.get_shape(), rmat.get_shape()))) {
        throw error::ShapeError(lmat.get_shape(), rmat.get_shape(), "multiply");
    }
    std::vector<U> out_data = util::matmul<dtype, T>(
        lmat.get_data(), lmat.get_shape(),
        rmat.get_data(), rmat.get_shape()
    );
    auto out_shape = Shape::get_product_shape(lmat.get_shape(), rmat.get_shape());

    return NArray<U>(std::move(out_data), out_shape);
}


// Dot product of two vectors
template <typename A, typename B>
auto dot(const NArray<A>& a, const NArray<B>& b)
    -> NArray<decltype(std::declval<A>() * std::declval<B>())>
{
    using U = decltype(std::declval<A>() * std::declval<B>());

    MatmulType type = Shape::get_matmul_type(a.get_shape(), b.get_shape());

    switch(type) {
        case MatmulType::Invalid:
            throw error::ShapeError(a.get_shape(), b.get_shape(), "dot");

        case MatmulType::Dot: {
            U sum = 0;
            for(size_t i = 0; i < a.get_total_size(); i++)
                sum += (a.get_data()[i] * b.get_data()[i]);
            return NArray<U>(sum);
        }

        case MatmulType::MatCol:
            return matmul(a,b);

        case MatmulType::RowMat:
            return matmul(a,b);

        case MatmulType::MatMat:
            return matmul(a,b);

        default:
            throw error::ShapeError(a.get_shape(), b.get_shape(), "dot");
    }
}

template <typename A, typename B>
auto dot(const Matrix<A>& a, const Matrix<B>& b)
    -> Matrix<decltype(std::declval<A>() * std::declval<B>())>
{
    return a * b;
}

template <typename A, typename B>
auto dot(A a, B b) { return a * b; }


// Flattens the input NArrays and returns the dot-product
template <typename A, typename B>
auto vdot(const NArray<A>& a, const NArray<B>& b)
    -> NArray<decltype(std::declval<A>() * std::declval<B>())>
{
    using U = decltype(std::declval<A>() * std::declval<B>());

    if(!NArray<>::same_shape(a,b))
        throw error::ShapeError(a.get_shape(), b.get_shape(), "vdot");

    U sum = 0;
    for(size_t i = 0; i < a.get_total_size(); i++)
        sum += (conj(a.get_data()[i]) * b.get_data()[i]);

    return NArray<U>(sum);
}


// Returns the elementwise multiplication of two arrays of any dimension
template <typename T, typename U>
auto hadamard(const NArray<T>& larr, const NArray<U>& rarr)
    -> NArray<decltype(std::declval<T>() * std::declval<U>())>
{
    if (larr.get_shape() != rarr.get_shape())
        throw error::ShapeError(larr.get_shape(), rarr.get_shape(), "multiply");

    using R = decltype(std::declval<T>() * std::declval<U>());
    NArray<R> out(larr.get_shape());

    for (size_t i = 0; i < larr.get_total_size(); i++)
        out.get_data()[i] = static_cast<R>(larr[i]) * static_cast<R>(rarr[i]);

    return out;
}


template <typename T, typename U, typename = std::enable_if_t<is_complex_or_arithmetic_v<T> && is_complex_or_arithmetic_v<U>>>
auto cross(const NArray<T>& a, const NArray<U>& b)
    -> NArray<decltype(std::declval<T>() * std::declval<U>())>
{
    using A = decltype(std::declval<T>() * std::declval<U>());

    const Shape &a_shape = a.get_shape(), &b_shape = b.get_shape();
    if ((a_shape.get_Ndim() != 1 || (a_shape[0] != 2 && a_shape[0] != 3)) ||
        (b_shape.get_Ndim() != 1 || (b_shape[0] != 2 && b_shape[0] != 3)))
    {
        throw error::ShapeError("Both input arrays must have shapes (2,) or (3,).");
    }

    const T x = a(0), y = a(1); T z;
    if (a.get_shape()[0] == 2) z = 0;
    else z = a(2);

    const U p = b(0), q = b(1); U r;
    if (b.get_shape()[0] == 2) r = 0;
    else r = b(2);

    NArray<A> out(Shape{3});
    out(0) = y*r - z*q;    out(1) = z*p - x*r;    out(2) = x*q - y*p;
    return out;
}


} // namespace numxx
