/* Matrix.hpp */
#pragma once

#include "NArray.hpp"


namespace numxx {

template <typename dtype = double>
class Matrix final : public NArray<dtype> {

protected:
    using NArray<dtype>::_data_ptr;
    using NArray<dtype>::_shape;


public:
    /* ====== Constructors ====== */

    // Default constructor
    Matrix() : NArray<dtype>() {}


    // Copy constructor
    Matrix(const Matrix& mat) : NArray<dtype>(mat) {}


    // Move constructor
    Matrix(Matrix&& mat) noexcept : NArray<dtype>(std::move(mat)) {}


    // Constructor from NArray (copy)
    explicit Matrix(const NArray<dtype>& arr) : NArray<dtype>(arr) {
        check_and_adjust_shape(this->_shape);
    }


    // Constructor from NArray (move)
    explicit Matrix(NArray<dtype>&& mat) noexcept : NArray<dtype>(std::move(mat)) {
        check_and_adjust_shape(this->_shape);
    }


    // Scalar constructor
    explicit Matrix(const dtype& num) : NArray<dtype>(num) {
        this->_shape = Shape({1,1});
    }


    // Vector constructor
    explicit Matrix(const std::vector<dtype>& data) : NArray<dtype>(data) {
        this->_shape = this->_shape.transpose();
    }

    explicit Matrix(std::vector<dtype>&& data) noexcept : NArray<dtype>(data) {
        this->_shape = this->_shape.transpose();
    }


    // List constructor
    Matrix(std::initializer_list<dtype> list) : NArray<dtype>(list) {
        this->_shape = this->_shape.transpose();
    }


    // Data vector + shape constructor (copy)
    Matrix(const std::vector<dtype>& vec, const Shape& shape) : NArray<dtype>(vec, shape) {
        check_and_adjust_shape(this->_shape);
    }

    
    // Data vector + shape constructor (move)
    Matrix(std::vector<dtype>&& vec, const Shape& shape) : NArray<dtype>(vec, shape) {
        check_and_adjust_shape(this->_shape);
    }


    // Data + shape constructor from pointer
    Matrix(dtype* ptr, const Shape& shape) : NArray<dtype>(ptr, shape) {
        check_and_adjust_shape(this->_shape);
    }

    Matrix(dtype* ptr, Shape&& shape) : NArray<dtype>(ptr, shape) {
        check_and_adjust_shape(this->_shape);
    }


    // Data + shape constructor from shared pointer
    Matrix(std::shared_ptr<dtype> sp, const Shape& shape) : NArray<dtype>(sp, shape) {
        check_and_adjust_shape(this->_shape);
    }

    Matrix(std::shared_ptr<dtype> sp, Shape&& shape) : NArray<dtype>(sp, shape) {
        check_and_adjust_shape(this->_shape);
    }


    // Shape + initializer value constructor
    Matrix(const Shape& shape, dtype val) : NArray<dtype>(shape, val) {
        check_and_adjust_shape(this->_shape);
    }

    Matrix(Shape&& shape, dtype val) : NArray<dtype>(shape, val) {
        check_and_adjust_shape(this->_shape);
    }


    // Empty shape constructor
    explicit Matrix(const Shape& shape) : NArray<dtype>(shape) {
        check_and_adjust_shape(this->_shape);
    }

    explicit Matrix(Shape&& shape) : NArray<dtype>(shape) {
        check_and_adjust_shape(this->_shape);
    }


    // Constructor from other NArrays
    template <typename... Arrays>
    Matrix(const NArray<dtype>& first, const NArray<dtype>& next, const Arrays&... rest) {
        if(first.get_shape().get_Ndim() > 1) {
            throw error::ShapeError("Cannot construct a Matrix with more than 2-Dimensions.");
        }

        this->_shape = first.get_shape();
        // Get size requirements for new NArray
        size_t total_size = 0;
        int new_dims = 0;
        this->get_size_requirements(total_size, new_dims, first, next, rest...);

        // Initialise _shape and _data_ptr
        this->_shape.insert_dimension(new_dims, 0);
        this->_data_ptr = std::shared_ptr<dtype>(new dtype[total_size], std::default_delete<dtype[]>());

        // Add data
        this->add_data_to_data_ptr(this->_data_ptr, 0, first, next, rest...);
    }


    // Nested initializer_list constructor
    Matrix(std::initializer_list<std::initializer_list<dtype>> list) : NArray<dtype>() {

        // Shape validation
        if (list.size() == 0 || list.begin()->size() == 0) {
            throw error::ValueError("Empty initializer list");
        }

        const size_t rows = list.size();
        size_t cols = list.begin()->size();

        for (const auto& row : list) {
            if (row.size() != cols) {
                throw error::ValueError("Jagged initializer lists are not supported.");
            }
        }

        // Matrix initialisation
        this->_shape = Shape{rows, cols};
        this->_data_ptr = std::shared_ptr<dtype>(new dtype[this->_shape.get_total_size()], std::default_delete<dtype[]>());

        size_t count = 0;
        for(const auto& row : list) {
            for(const auto& element : row) {
                this->_data_ptr.get()[count++] = element;
            }
        }
    }



    /* ====== Operator Overloads ====== */

    // Multiplication Overloads
    template <typename T>
    auto operator*(const Matrix<T>& other)
        const -> Matrix<decltype(std::declval<dtype>() * std::declval<T>())>
    {
        using U = decltype(std::declval<dtype>() * std::declval<T>());

        if (!are_multipliable(*this, other)) {
            throw error::ShapeError(this->_shape, other.get_shape(), "multiply");
        }
        auto out_data = util::matmul<dtype, T>(
            this->_data_ptr.get(), this->_shape,
            other.get_data(), other.get_shape()
        );
        auto out_shape = Shape::get_product_shape(this->_shape, other.get_shape());
        return Matrix<U>(std::move(out_data), out_shape);
    }

    template <typename T>
    auto operator*(const NArray<T>& other)
        const -> NArray<decltype(std::declval<dtype>() * std::declval<T>())>
    {
        using U = decltype(std::declval<dtype>() * std::declval<T>());

        // Try downcasting to a Matrix<T> and using the previous function's logic
        if(const auto* matrix_other = dynamic_cast<const Matrix<T>*>(&other)) {
            return this->operator*(*matrix_other);
        }
        
        // Check if it's a valid 2D NArray that can be treated as a matrix
        if(are_multipliable(*this, other)) {
            // Treat the NArray as a matrix for multiplication
            auto out_data = util::matmul<dtype, T>(
                this->get_data(), this->get_shape(),
                other.get_data(), other.get_shape()
            );
            auto out_shape = Shape::get_product_shape(this->_shape, other.get_shape());
            return Matrix<U>(std::move(out_data), out_shape);
        }
        
        throw error::ShapeError(this->_shape, other.get_shape(), "multiply");
    }



    /* ====== Helper Functions ====== */

    // Checks if two matrices can be multiplied
    template <typename T, typename U>
    static bool are_multipliable(const NArray<T>& lmat, const NArray<U>& rmat) {
        return static_cast<bool>(Shape::get_matmul_type(lmat.get_shape(), rmat.get_shape()));
    }


    // Does what the name says
    Matrix transpose() {
        auto out_shape = this->_shape.transpose();
        auto out_data_ptr = this->get_data_copy_as_shared_ptr();
        util::transpose(out_data_ptr.get(), this->_shape);
        return Matrix(out_data_ptr, out_shape);
    }


private:
    static void check_and_adjust_shape(Shape& shape) {
        if(shape.get_Ndim() == 1)
            shape = Shape{1,shape[0]};
        else if(shape.get_Ndim() > 2 || shape.get_Ndim() < 1)
            throw error::ShapeError("Could not construct a Matrix with shape " + util::toString(shape) + ".");
    }

};

} // namespace numxx
