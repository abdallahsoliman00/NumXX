/* Shape.hpp */
#pragma once

#include <vector>
#include <algorithm>

#include "../Utils/Errors.hpp"

namespace numxx {

/* An enum class to help label the different types of vector and matrix multiplications */
enum class MatmulType {
    Invalid = 0,
    Dot,
    RowMat,
    MatCol,
    MatMat
}; // enum class MatmulType


/* A helper class to hold the shape of the NArray */
struct Shape {

    std::vector<size_t> dimensions;

    /* ====== Constructors ====== */

    // Default constructor
    Shape() : dimensions({}) {}

    // Scalar constructor
    explicit Shape(unsigned int num) : dimensions({num}) {}

    // Initializer constructor
    Shape(const std::initializer_list<size_t> dims) :  dimensions(dims) {}

    // Copy constructor
    Shape(const Shape& newShape) = default;

    // Move constructor
    Shape(Shape&& other) noexcept : dimensions(std::move(other.dimensions)) {}

    // Iterator constructor
    template <typename Iter>
    Shape(Iter first, Iter last) : dimensions(first, last) {}

    // Vector constructor
    explicit Shape(const std::vector<size_t>& dims) : dimensions(dims) {}

    // Move constructor
    explicit Shape(std::vector<size_t>&& dims) noexcept : dimensions(std::move(dims)) {}



    /* ====== Helper Functions ====== */

    [[nodiscard]] const size_t *begin() const { return &dimensions[0]; }
    size_t *begin() { return &dimensions[0]; }

    [[nodiscard]] const size_t *end() const { return &dimensions[0] + dimensions.size(); }
    size_t *end() { return &dimensions[0] + dimensions.size(); }

    // Inserts a dimension at a specified position
    void insert_dimension(const size_t dimension, const int position) {
        dimensions.insert(dimensions.begin() + get_index(position), dimension);
    }

    
    // Returns the total number of dimensions is a shape
    [[nodiscard]] size_t get_Ndim() const { return dimensions.size(); }

    
    // Checks if two shapes are identical
    [[nodiscard]] bool same_shape(const Shape& other) const {
        return this->dimensions == other.dimensions;
    }


    // Returns the total size required to store the array
    [[nodiscard]] size_t get_total_size() const {
        size_t result = 1;
        for(const auto& d : dimensions) result *= d;
        return result;
    }


    // Checks if a shape is a square
    [[nodiscard]] bool is_square() const {
        if (get_Ndim() > 2) return false;
        if (get_Ndim() == 2 && dimensions[0] == dimensions[1]) return true;
        if (get_total_size() == 1) return true;
        return false;
    }


    // Computes the strides for an array
    [[nodiscard]] std::vector<size_t> compute_strides() const {
        const auto ndim = get_Ndim();
        int stride = 1;
        std::vector<size_t> strides(ndim);
        for (int i = ndim - 1; i >= 0; --i) {
            strides[i] = stride;
            stride *= dimensions[i];
        }
        return strides;
    }

    static void compute_strides(const Shape& shape, size_t *arr, const size_t ndim) {
        int stride = 1;
        for (int i = ndim - 1; i >= 0; --i) {
            arr[i] = stride;
            stride *= shape[i];
        }
    }


    /* ====== Main Functions ====== */

    // Changes the shape
    void reshape(const std::initializer_list<size_t>& dims) { dimensions = dims; }


    // Returns a new flat shape
    [[nodiscard]] Shape flatten() const { return Shape({get_total_size()}); }


    // Transpose a vector or matrix;
    [[nodiscard]] Shape transpose() const {
        switch(get_Ndim()) {
            case 1:
                return Shape({1,dimensions[0]});
            case 2:
                return Shape({dimensions[1],dimensions[0]});
            default:
                throw std::runtime_error("Cannot transpose arrays with more than 2 dimensions.");
        }
    }



    /* ====== Static Functions ====== */

    // Returns the shape of the product of two matrices
    static Shape get_product_shape(const Shape& lshape, const Shape& rshape) {
        switch (get_matmul_type(lshape, rshape)) {
        case MatmulType::Invalid:
            throw error::ShapeError("Invalid shapes for matrix multiplication.");
        case MatmulType::Dot:
            return Shape(1);
        case MatmulType::RowMat:
            return Shape(rshape[1]);
        case MatmulType::MatCol:
            return Shape(lshape[0]);
        case MatmulType::MatMat:
            return Shape({lshape[0], rshape[1]});
        default:
            throw error::ShapeError("Invalid shapes for matrix multiplication.");
        }
    }


    // Returns an enum that holds the type of matrix multiplication taking place
    static MatmulType get_matmul_type(const Shape& a, const Shape& b) {
        const size_t ndim1 = a.get_Ndim();
        const size_t ndim2 = b.get_Ndim();

        if (ndim1 > 2 || ndim2 > 2)
            return MatmulType::Invalid;

        if (ndim1 == 1 && ndim2 == 1)
            return (a.dimensions[0] == b.dimensions[0])
                ? MatmulType::Dot : MatmulType::Invalid;

        if (ndim1 == 1 && ndim2 == 2)
            return (a.dimensions[0] == b.dimensions[0])
                ? MatmulType::RowMat : MatmulType::Invalid;

        if (ndim1 == 2 && ndim2 == 1)
            return (a.dimensions[1] == b.dimensions[0])
                ? MatmulType::MatCol : MatmulType::Invalid;

        if (ndim1 == 2 && ndim2 == 2)
            return (a.dimensions[1] == b.dimensions[0])
                ? MatmulType::MatMat : MatmulType::Invalid;

        return MatmulType::Invalid;
    }



    /* ====== Operator Overloads ====== */

    // Assignment operators
    Shape& operator=(const Shape&) = default; // allow copy assignment
    Shape& operator=(Shape&&) = default;      // allow move assignment

    // Equality operator
    bool operator==(const Shape& other) const {
        return same_shape(other);
    }

    // Inequality operator
    bool operator!=(const Shape& other) const {
        return !same_shape(other);
    }

    // Index operator (immutable reference)
    const size_t& operator[](const long long index) const {
        return dimensions[get_index(index)];
    }

    // Index operator (mutable reference)
    size_t& operator()(const long long index) {
        return dimensions[get_index(index)];
    }

    friend std::ostream& operator<<(std::ostream& os, const Shape& shape) {
        os << '(';
        for(size_t i = 0; i < shape.dimensions.size(); i++) {
            os << shape.dimensions[i];
            if((i != shape.dimensions.size() - 1) || (0 == i)) os << ',';
        }
        os << ')';
        return os;
    }


private:

    // Enables Python-like indexing with negative indexes wrapping around
    [[nodiscard]] size_t get_index(const long long index) const {
        const size_t size = dimensions.size();

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        if((index < 0) && (index >= -size))
            return (size + index);

        throw std::runtime_error("Shape index out of range.\n");
    }
};  // struct Shape


} // namespace numxx