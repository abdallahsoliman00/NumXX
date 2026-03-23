/* NArray.hpp */
#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory>

#include "../Containers/Shape.hpp"
#include "../Core/Complex.hpp"
#include "../Utils/VecOps.hpp"
#include "../Utils/MathFuncUtils.hpp"
#include "../Utils/Errors.hpp"


namespace numxx {

template <typename dtype = double>
class NArray {
protected:
    /* ====== Member Variables ====== */

    std::shared_ptr<dtype> _data_ptr;
    Shape _shape;


public:
    /* ====== Helper functions ====== */

    // Fetches the NArray shape
    [[nodiscard]] const Shape& get_shape() const { return this->_shape; }
    [[nodiscard]] const Shape& shape() const { return this->_shape; }


    // Returns a pointer `dtype*` to the data
    [[nodiscard]] dtype* get_data() const { return this->_data_ptr.get(); }


    // Returns the data as an std::vector
    [[nodiscard]] std::vector<dtype> get_data_as_vector() const {
        return std::vector<dtype>(_data_ptr.get(), _data_ptr.get() + _shape.get_total_size());
    }


    // Returns a shared pointer to newly allocated heap memory with the data
    [[nodiscard]] std::shared_ptr<dtype> get_data_copy_as_shared_ptr() const {
        std::shared_ptr<dtype> out(new dtype[_shape.get_total_size()], std::default_delete<dtype[]>());
        std::copy(_data_ptr.get(), _data_ptr.get() + _shape.get_total_size(), out.get());
        return out;
    }


    // Checks if two shapes are the same
    template <typename T, typename U>
    static bool same_shape(const NArray<T>& a, const NArray<U>& b) {
        return a.get_shape().same_shape(b.get_shape());
    }


    [[nodiscard]] size_t get_total_size() const {
        return _shape.get_total_size();
    }



protected:
    /* ====== Helper Functions ====== */

    // Gets the total size required to store multiple NArrays
    static void get_size_requirements(
        size_t& size, int& depth, const NArray& arr
    ) {
        size += arr.get_total_size();
        depth++;
    }

    template <typename... Arrays>
    static void get_size_requirements(
        size_t& size, int& depth,
        const NArray& arr, const NArray& next, 
        const Arrays&... rest
    ) {
        if(same_shape(arr, next)) {
            size += arr.get_total_size();
            depth++;
            get_size_requirements(size, depth, next, rest...);
        } else {
            throw error::ValueError("Could not initialise array because subarrays do not have the same shape.");
        }
    }


    // Adds the data to the array given its pointer
    static void add_data_to_data_ptr(
        const std::shared_ptr<dtype>& data_ptr, size_t starting_pos,
        const NArray& arr
    ) {
        std::copy(
            arr._data_ptr.get(),
            arr._data_ptr.get() + arr.get_total_size(),
            data_ptr.get() + starting_pos
        );
    }

    template <typename... Arrays>
    static void add_data_to_data_ptr(
        const std::shared_ptr<dtype>& data_ptr, size_t starting_pos,
        const NArray& first, const Arrays&... rest
    ) {
        // Copy first array's elements to data_ptr
        std::copy(
            first._data_ptr.get(),
            first._data_ptr.get() + first.get_total_size(),
            data_ptr.get() + starting_pos
        );
        // Pass the rest recursively
        add_data_to_data_ptr(data_ptr, starting_pos + first.get_total_size(), rest...);
    }


    // Enables Python-like indexing with negative indexes wrapping around (for viewing/slicing)
    [[nodiscard]] size_t get_index(const long long index) const {
        const auto size = static_cast<long long>(_shape[0]);

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        if((index < 0) && (index >= -size))
            return static_cast<size_t>(index + size);

        throw std::range_error("Array index out of range.");
    }

    // Enables Python-like indexing with negative indexes wrapping around (for accessing elements)
    [[nodiscard]] size_t get_element_index(const long long int index) const {
        auto const size = get_total_size();

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        if((index < 0) && (index >= -size))
            return (index + size);

        throw std::range_error("Element index out of range.");
    }


    // Elementwise operation of two NArrays
    template <typename T, typename Func>
    auto elementWiseOp(
        const NArray<T> &other,
        Func func
    ) const -> NArray<decltype(func(std::declval<dtype>(), std::declval<T>()))>
    {
        using U = decltype(func(std::declval<dtype>(), std::declval<T>()));
        NArray<U> out(_shape);

        for(size_t i = 0; i < _shape.get_total_size(); i++) {
            out.get_data()[i] = func(this->get_data()[i], other.get_data()[i]);
        }
        return out;
    }


    // Performs the scalar operation to the right of each element in the array
    template <typename T, typename Func>
    auto fullVecOpR(const T scalar, Func func)
        const -> NArray<decltype(func(std::declval<dtype>(), std::declval<T>()))>
    {
        using U = decltype(func(std::declval<dtype>(), std::declval<T>()));

        NArray<U> newArr(_shape);
        for(size_t i = 0; i < _shape.get_total_size(); i++) {
            newArr(i) = func(get_data()[i], scalar);
        }
        return newArr;
    }


    // Performs the scalar operation to the left of each element in the array
    template <typename T, typename Func>
    auto fullVecOpL(const T scalar, Func func)
        const -> NArray<decltype(func(std::declval<T>(), std::declval<dtype>()))>
    {
        using U = decltype(func(std::declval<T>(), std::declval<dtype>()));

        NArray<U> newArr(_shape);
        for(size_t i = 0; i < _shape.get_total_size(); i++) {
            newArr(i) = func(scalar, get_data()[i]);
        }
        return newArr;
    }


    // Returns a mask of which NArray elements are the same
    template <typename T, typename Func>
    NArray<bool> elementwiseCompare(const NArray<T>& other, Func comparison_func) const {
        if(!same_shape(*this, other)) {
            throw error::ShapeError(this->_shape, other.get_shape(), "compare");
        }
        NArray<bool> out(this->_shape);
        for(size_t i = 0; i < _shape.get_total_size(); i++) {
            out.get_data()[i] = comparison_func(get_data()[i], other.get_data()[i]);
        }
        return out;
    }


    // Print a 1D array
    static void OneDPrint(
        std::ostream& os, const NArray& arr, const util::PrintAttributes& attributes
    ) {
        os << '[';
        for(size_t i = 0; i < arr._shape[0]; i++) {
            if constexpr (std::is_same_v<dtype, bool>) {
                os << (arr.get_data()[i] ? " true " : " false");
            } else if constexpr (is_complex_or_arithmetic_v<dtype>) {
                os << util::num_to_str_from_attributes(arr.get_data()[i], attributes);
            } else {
                os << util::toString(arr.get_data()[i]);
            }
            if(i < arr._shape[0] - 1) os << ' ';
        }
        os << ']';
    }


    // Recursively prints the N-Dimensional arrays (> 2D arrays)
    static void recursivePrint(
    std::ostream& os, std::vector<dtype>&& data, const Shape& shape,
        const util::PrintAttributes& attributes, const int depth = 0
    ) {
        // Base case
        if(shape.get_Ndim() == 1) {
            OneDPrint(os, NArray(std::move(data)), attributes);
            return;
        }

        // Take the first element and split
        size_t n_grps = shape[0];
        auto groups = util::split(std::move(data), n_grps);

        const Shape subshape(shape.dimensions.begin() + 1, shape.dimensions.end());

        os << '[';
        for (size_t i = 0; i < n_grps; i++) {
            if (i > 0) {
                os << " ";
                if (depth == 0)
                    os << "\n\n";
                else
                    os << "\n" << std::string(depth + 1, ' ');
            }
            recursivePrint(os, std::move(groups[i]), subshape, attributes, depth + 1);
        }
        os << ']';
    }



public:
    /* ====== 1D constructors ====== */

    // Default constructor
    NArray() : _data_ptr(nullptr), _shape() {}


    // Scalar constructor
    explicit NArray(const dtype& num) : _data_ptr(new dtype[1], std::default_delete<dtype[]>()), _shape({1})
        { _data_ptr.get()[0] = num; }

    
    // Iterator constructor
    template <typename Iter>
    NArray(Iter first, Iter last) :
        _data_ptr(new dtype[static_cast<size_t>(std::distance(first, last))], std::default_delete<dtype[]>()),
        _shape(static_cast<size_t>(std::distance(first, last)))
    {
        if (std::distance(first, last) < 0)
            throw std::invalid_argument("NArray: iterator range is invalid (last before first)");

        size_t i = 0;
        for (auto it = first; it != last; ++it, ++i) {
            _data_ptr.get()[i] = *it;
        }
    }


    // Vector constructor
    explicit NArray(const std::vector<dtype>& data) :
        _data_ptr(new dtype[data.size()], std::default_delete<dtype[]>()),
        _shape({static_cast<size_t>(data.size())})
    {
        std::copy(data.begin(), data.end(), _data_ptr.get());
    }

    // Removed made the constructor implicit to allow NArray functions to work with std::vector as well
    NArray(std::vector<dtype>&& data) :
        _data_ptr(new dtype[data.size()], std::default_delete<dtype[]>()),
        _shape({data.size()})
    {
        std::move(data.begin(), data.end(), _data_ptr.get());
    }


    // initializer_list constructor
    NArray(std::initializer_list<dtype> list) :
        _data_ptr(new dtype[list.size()], std::default_delete<dtype[]>()),
        _shape({static_cast<size_t>(list.size())})
    {
        size_t i = 0;
        for(auto item : list)
            _data_ptr.get()[i++] = item;
    }


    // Array constructor from heap array (ownership takeover)
    template <typename Deleter>
    NArray(dtype *array, const size_t size, Deleter deleter) :
        _data_ptr(array, deleter), _shape({size}) {}


    // Array constructor from heap array (copy)
    NArray(dtype *array, const size_t size) :
        _data_ptr(new dtype[size], std::default_delete<dtype[]>()),
        _shape({size})
    {
        std::copy(array, array + size, _data_ptr.get());
    }


    // Array constructor from a stack array
    template <size_t N>
    explicit NArray(dtype (&array)[N]) : _data_ptr(new dtype[N], std::default_delete<dtype[]>()), _shape({N}) {
        for(size_t i = 0; i < N; i++)
            _data_ptr.get()[i] = array[i];
    }


    // Copy constructor
    NArray(const NArray& oldarray) :
        _data_ptr(new dtype[oldarray._shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(oldarray._shape)
    {
        std::copy(
            oldarray._data_ptr.get(),
            oldarray._data_ptr.get() + oldarray._shape.get_total_size(),
            _data_ptr.get()
        );
    }


    // Move constructor
    NArray(NArray&& other) noexcept : _data_ptr(std::move(other._data_ptr)), _shape(std::move(other._shape)) {}


    // Repeat constructor
    NArray(size_t count, dtype val) :
        _data_ptr(new dtype[count], std::default_delete<dtype[]>()),
        _shape({count})
    {
        for(size_t i = 0; i < count; i++)
            _data_ptr.get()[i] = val;
    }


    /* ====== N-Dimensional constructors ====== */

    // Recursive constructor
    NArray(std::initializer_list<NArray> arr) {
        // Check if empty
        if(!arr.size()) return; // Base case

        // Read first dimension
        _shape.dimensions.push_back(arr.size());

        // Continue adding dimensions recursively until base case is reached
        const NArray& first = *arr.begin();
        _shape.dimensions.insert(_shape.dimensions.end(),
                                first._shape.dimensions.begin(),
                                first._shape.dimensions.end());

        // Consistency check
        for (const NArray& sub : arr) {
            if (sub._shape.dimensions != first._shape.dimensions) {
                throw error::ValueError("Jagged initializer lists are not supported.");
            }
        }

        // Reserve space for values
        _data_ptr = std::shared_ptr<dtype>(new dtype[_shape.get_total_size()], std::default_delete<dtype[]>());

        // Add values
        size_t offset = 0;
        for (const NArray& sub : arr) {
            size_t sz = sub._shape.get_total_size();
            std::copy(sub._data_ptr.get(), sub._data_ptr.get() + sz, _data_ptr.get() + offset);
            offset += sz;
        }
    }


    // Data + shape constructor (move)
    NArray(std::vector<dtype>&& vec, Shape shape) :
        _data_ptr(new dtype[vec.size()], std::default_delete<dtype[]>()),
        _shape(std::move(shape))
    {
        if (_shape.get_total_size() != vec.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
        std::move(vec.begin(), vec.end(), _data_ptr.get());
    }


    // Data + shape constructor (copy)
    NArray(const std::vector<dtype>& vec, Shape shape) :
        _data_ptr(new dtype[vec.size()], std::default_delete<dtype[]>()),
        _shape(std::move(shape))
    {
        if (this->_shape.get_total_size() != vec.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
        
        std::copy(vec.begin(), vec.end(), _data_ptr.get());
    }


    // Data + shape constructor (from initializer_list)
    NArray(std::initializer_list<dtype> list, Shape shape) :
        _data_ptr(new dtype[list.size()], std::default_delete<dtype[]>()),
        _shape(std::move(shape))
    {
        if (this->_shape.get_total_size() != list.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }

        size_t i = 0;
        for(auto item : list)
            _data_ptr.get()[i++] = item;
    }


    // Shape + initializer value constructor
    NArray(const Shape& shape, dtype val) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(shape)
    {
        for(size_t i = 0; i < _shape.get_total_size(); i++)
            _data_ptr.get()[i] = val;
    }

    NArray(Shape&& shape, dtype val) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(std::move(shape))
    {
        for(size_t i = 0; i < _shape.get_total_size(); i++)
            _data_ptr.get()[i] = val;
    }


    // Empty shape constructor
    explicit NArray(const Shape& shape) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(shape) {}
    
    explicit NArray(Shape&& shape) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(std::move(shape)) {}


    // Constructor from shared_ptr (used for slicing)
    NArray(std::shared_ptr<dtype> base, dtype* slice_start, Shape&& new_shape) :
        _data_ptr(base, slice_start), _shape(std::move(new_shape)) {}

    NArray(std::shared_ptr<dtype> base, dtype* slice_start, const Shape& new_shape) :
        _data_ptr(base, slice_start), _shape(new_shape) {}


    // Shared pointer + shape constructor
    NArray(const std::shared_ptr<dtype>& sp, const Shape& shape) : _data_ptr(sp), _shape(shape) {}
    NArray(const std::shared_ptr<dtype>& sp, Shape&& shape) : _data_ptr(sp), _shape(std::move(shape)) {}


    // Raw pointer + shape constructor (ownership takeover)
    template <typename Deleter>
    NArray(dtype* ptr, Shape shape, Deleter deleter) : _data_ptr(ptr, deleter), _shape(std::move(shape)) {}

    template <typename Deleter>
    NArray(dtype* ptr, Shape&& shape, Deleter deleter) : _data_ptr(ptr, deleter), _shape(std::move(shape)) {}


    // Raw pointer + shape constructor (copy)
    NArray(dtype* ptr, Shape shape) :
        _data_ptr(new dtype[shape.get_total_size()],
        std::default_delete<dtype[]>()), _shape(std::move(shape))
    {
        std::copy(ptr, ptr + _shape.get_total_size(), _data_ptr.get());
    }

    NArray(dtype* ptr, Shape&& shape) :
    _data_ptr(new dtype[shape.get_total_size()],
    std::default_delete<dtype[]>()), _shape(std::move(shape))
    {
        std::copy(ptr, ptr + _shape.get_total_size(), _data_ptr.get());
    }


    // Constructor from other NArrays
    template <typename... Arrays>
    NArray(const NArray& first, const NArray& next, const Arrays&... rest) : _shape(first._shape) {
        // Get size requirements for new NArray
        size_t total_size = 0;
        int new_dims = 0;
        get_size_requirements(total_size, new_dims, first, next, rest...);

        // Initialise _shape and _data_ptr
        _shape.insert_dimension(new_dims, 0);
        _data_ptr = std::shared_ptr<dtype>(new dtype[total_size], std::default_delete<dtype[]>());

        // Add data
        add_data_to_data_ptr(_data_ptr, 0, first, next, rest...);
    }


    /* ====== Destructor ====== */
    virtual ~NArray() = default;



    /* ====== Operator Overloading ====== */

    /* ELEMENTWISE Assignment and operation overloads */

    // Array addition
    template <typename T>
    NArray& operator+=(const T other) {
        for (int i = 0; i < get_total_size(); i++) {
            get_data()[i] += other;
        }
        return *this;
    }

    template <typename T>
    NArray& operator-=(const T other) {
        for (int i = 0; i < get_total_size(); i++) {
            get_data()[i] -= other;
        }
        return *this;
    }

    template <typename T>
    NArray& operator*=(const T other) {
        for (int i = 0; i < get_total_size(); i++) {
            get_data()[i] *= other;
        }
        return *this;
    }

    template <typename T>
    NArray& operator/=(const T other) {
        for (int i = 0; i < get_total_size(); i++) {
            get_data()[i] /= other;
        }
        return *this;
    }

    template <typename T>
    NArray& operator +=(const NArray<T>& other) {
        if (!same_shape(*this, other)) {
            throw error::ShapeError(
                "Cannot add together shapes " + util::toString(shape()) + " and " +
                util::toString(other.shape()) + "elementwise."
            );
        }
        for (size_t i = 0; i < get_total_size(); i++) {
            get_data()[i] += static_cast<dtype>(other.get_data()[i]);
        }
        return *this;
    }

    template <typename T>
    NArray& operator -=(const NArray<T>& other) {
        if (!same_shape(*this, other)) {
            throw error::ShapeError(
                "Cannot subtract shapes " + util::toString(shape()) + " and " +
                util::toString(other.shape()) + " elementwise."
            );
        }
        for (size_t i = 0; i < get_total_size(); i++) {
            get_data()[i] -= static_cast<dtype>(other.get_data()[i]);
        }
        return *this;
    }

    template <typename T>
    NArray& operator *=(const NArray<T>& other) {
        if (!same_shape(*this, other)) {
            throw error::ShapeError(
                "Cannot multiply together shapes " + util::toString(shape()) + " and " +
                util::toString(other.shape()) +
                " elementwise. For matrix multiplication try using numxx::matmul() or numxx::Matrix::operator*()."
            );
        }
        for (size_t i = 0; i < get_total_size(); i++) {
            get_data()[i] *= static_cast<dtype>(other.get_data()[i]);
        }
        return *this;
    }

    template <typename T>
    NArray& operator /=(const NArray<T>& other) {
        if (!same_shape(*this, other)) {
            throw error::ShapeError(
                "Cannot divide shapes " + util::toString(shape()) + " and " +
                util::toString(other.shape()) + " elementwise."
            );
        }
        for (size_t i = 0; i < get_total_size(); i++) {
            get_data()[i] /= static_cast<dtype>(other.get_data()[i]);
        }
        return *this;
    }

    friend dtype& operator+=(dtype& other, NArray& self) {
        if (self.get_total_size() == 1) {
            other += *self.get_data();
        } else {
            throw error::ShapeError("Cannot add NArray to a single element.");
        }
        return other;
    }

    friend dtype& operator-=(dtype& other, NArray& self) {
        if (self.get_total_size() == 1) {
            other -= *self.get_data();
        } else {
            throw error::ShapeError("Cannot subtract NArray from a single element.");
        }
        return other;
    }

    friend dtype& operator*=(dtype& other, NArray& self) {
        if (self.get_total_size() == 1) {
            other *= *self.get_data();
        } else {
            throw error::ShapeError("Cannot multiply single element by an NArray.");
        }
        return other;
    }

    friend dtype& operator/=(dtype& other, NArray& self) {
        if (self.get_total_size() == 1) {
            other /= *self.get_data();
        } else {
            throw error::ShapeError("Cannot divide single element by NArray.");
        }
        return other;
    }


    /* NArray operation overloads */

    // Array addition
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator+(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other.get_shape(), "add");

        return elementWiseOp(other, [] (const auto& a, const auto& b) { return a + b; } );
    }


    // Array subtraction
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator-(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other.get_shape(), "subtract");

        return elementWiseOp(other, [] (const auto& a, const auto& b) { return a - b; } );
    }


    // Array multiplication
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator*(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other.get_shape(), "multiply");

        return elementWiseOp(other, [] (const auto& a, const auto& b) { return a * b; } );
    }


    // Array division
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator/(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other.get_shape(), "divide");

        return elementWiseOp(other, [] (const auto& a, const auto& b) { return a / b; } );
    }


    /* Scalar Overloads */

    // Right exponent overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator^(T num) const {
        return fullVecOpR(num, [] (const dtype b, const T e) { return util::pow(b,e); });
    }


    // Right addition overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator+(T num) const {
        return fullVecOpR(num, [] (const dtype a, const T b) { return a + b; });
    }


    // Right subtraction overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator-(T num) const {
        return fullVecOpR(num, [] (const dtype a, const T b) { return a + b; });
    }


    // Right multiplication overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator*(T num) const {
        return fullVecOpR(num, [] (const dtype a, const T b) { return a * b; });
    }


    // Right division overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    auto operator/(T num) const {
        return fullVecOpR(num, [] (const dtype a, const T b) { return a / b; });
    }



    // Left base overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    friend auto operator^(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (const T b, const dtype e) { return util::pow(b,e); });
    }


    // Left addition overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    friend auto operator+(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (const T a, const dtype b) { return a + b; });
    }


    // Left subtraction overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    friend auto operator-(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (const T a, const dtype b) { return a - b; });
    }


    // Left multiplication overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    friend auto operator*(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (const T a, const dtype b) { return a * b; });
    }


    // Left division overload
    template <typename T, typename = std::enable_if_t<is_complex_or_arithmetic_v<T>>>
    friend auto operator/(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (const T a, const dtype b) { return a / b; });
    }


    // Negation overload
    friend auto operator-(const NArray& arr) {
        return arr.fullVecOpL(dtype(-1), [] (const dtype a, const dtype b) { return a * b; });
    }

    // Do nothing, but just in case someone uses it overload
    friend NArray& operator+(NArray& arr) {
        return arr;
    }


    /* Comparison Overloads */

    template <typename T>
    NArray<bool> operator==(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (const dtype a, const T b) { return a == b; } );
    }

    template <typename T>
    NArray<bool> operator!=(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (const dtype a, const T b) { return a != b; } );
    }

    template <typename T>
    NArray<bool> operator<=(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (const dtype a, const T b) { return a <= b; });
    }

    template <typename T>
    NArray<bool> operator>=(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (const dtype a, const T b) { return a >= b; });
    }

    template <typename T>
    NArray<bool> operator<(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (const dtype a, const T b) { return a < b; });
    }

    template <typename T>
    NArray<bool> operator>(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (const dtype a, const T b) { return a > b; });
    }



    /* Index Overload */
    // For slicing / viewing
    NArray operator[](const long long int i) const {
        auto index = get_index(i);

        if (_shape.get_Ndim() == 1)
            return NArray(_data_ptr, _data_ptr.get() + index, Shape(1));

        auto slice_start = _data_ptr.get() + _shape[1] * index;
        auto new_shape = Shape(_shape.dimensions.begin() + 1, _shape.dimensions.end());
        return NArray(_data_ptr, slice_start, std::move(new_shape));
    }

    NArray operator[](const long long int i) {
        auto index = get_index(i);

        if (_shape.get_Ndim() == 1)
            return NArray(_data_ptr, _data_ptr.get() + index, Shape(1));

        auto slice_start = _data_ptr.get() + _shape[1] * index;
        auto new_shape = Shape(_shape.dimensions.begin() + 1, _shape.dimensions.end());
        return NArray(_data_ptr, slice_start, std::move(new_shape));
    }

    // For direct access to the elements
    dtype& operator()(const long long int i) const {
        return _data_ptr.get()[get_element_index(i)];
    }

    /* if left empty, returns the first element of the NArray
     * can be used with the [] overload as such:
     * auto arr = NArray({{1,2},{3,4}});
     * to access the element containing 2, do the following:
     * auto elem& = arr[0]();
     */
    dtype& operator()() {
        return *_data_ptr.get();
    }


    /* Assignment Overload */
    NArray& operator=(const NArray& other) {
        if(same_shape(*this, other)) {
            std::copy(
                other.get_data(),
                other.get_data() + _shape.get_total_size(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal in size.");
        }
        return *this;
    }
        
    NArray& operator=(NArray&& other) noexcept {
        if(same_shape(*this, other)) {
            std::move(
                other.get_data(),
                other.get_data() + _shape.get_total_size(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal in size.");
        }
        return *this;
    }

    NArray& operator=(std::initializer_list<dtype> list) {
        if(this->_shape.get_Ndim() == 1 && this->_shape[0] == list.size()) {
            int i = 0;
            for(dtype val : list)
                this->get_data()[i++] = val;
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal in size.");
        }
        return *this;
    }

    NArray& operator=(const std::vector<dtype>& other) {
        if(this->_shape.get_Ndim() == 1 && this->_shape[0] == other.size()) {
            std::copy(
                other.begin(),
                other.end(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal.");
        }
        return *this;
    }

    NArray& operator=(std::vector<dtype>&& other) {
        if(this->_shape.get_Ndim() == 1 && this->_shape[0] == other.size()) {
            std::move(
                other.begin(),
                other.end(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal.");
        }
        return *this;
    }

    NArray& operator=(dtype val) {
        if(this->_shape == Shape{1}) {
            this->get_data()[0] = val;
        }
        return *this;
    }


    /* ====== Print Overload ====== */
    friend std::ostream& operator<<(std::ostream& os, const NArray& arr) {
        // Check if the array is empty
        if (arr._shape.get_Ndim() == 0) {
            os << "[]"; return os;
        }
        // Fetch print attributes
        util::PrintAttributes attributes;
        if constexpr (is_complex_or_arithmetic_v<dtype>)
            attributes = util::GetPrintAttributes(arr.get_data(), arr.get_total_size());

        switch(arr._shape.get_Ndim()) {
        case 1: // 1D vector
            if(arr._shape[0] == 1) os << *(arr.get_data());
            else OneDPrint(os, arr, attributes);
            break;

        case 2: { // Martix
            os << '[';
            auto grps = util::split(arr.get_data_as_vector(), arr._shape[0]);
            for(size_t i = 0; i < grps.size(); i++) {
                OneDPrint(os, NArray(grps[i]), attributes);
                if(i != grps.size() - 1) os << "\n ";
            }
            os << ']';
            break;
        }

        default: // Anything else
            recursivePrint(os, arr.get_data_as_vector(), arr._shape, attributes);
            break;
        }
        return os;
    }



    /* ====== Conversion Operators ====== */

    explicit operator int() const {
        if (_shape.get_total_size() == 1)
            return static_cast<int>(get_data()[0]);

        throw error::ConversionError(_shape, "int");
    }

    explicit operator long long() const {
        if (_shape.get_total_size() == 1)
            return static_cast<long long>(get_data()[0]);

        throw error::ConversionError(_shape, "long long");
    }

    explicit operator double() const {
        if (_shape.get_total_size() == 1)
            return static_cast<double>(get_data()[0]);

        throw error::ConversionError(_shape, "double");
    }

    explicit operator bool() const {
        return (_shape.get_total_size() != 0);
    }



    /* ===== NArray functions ====== */

    // Applies the input function `func` to each element in the array
    template <typename Func>
    auto funcToNArray(Func func)
        const -> NArray<decltype(func(std::declval<dtype>()))>
    {
        using U = decltype(func(std::declval<dtype>()));

        NArray<U> out(_shape);
        for (size_t i = 0; i < get_total_size(); i++)
            out(i) = func(get_data()[i]);

        return out;
    }


    // Returns a NEW transposed matrix
    [[nodiscard]] NArray transpose() const {
        auto out_shape = _shape.transpose();
        std::shared_ptr<dtype> out_data_ptr(new dtype[_shape.get_total_size()], std::default_delete<dtype[]>());
        util::transpose(out_data_ptr.get(), _data_ptr.get(), _shape);
        return NArray(out_data_ptr, out_shape);
    }


    // Returns a NEW transposed matrix (alias for NArray::transpose())
    [[nodiscard]] NArray T() const { return transpose(); }


    // Returns a new flat vector
    [[nodiscard]] NArray flatten() const {
        if (_shape.get_Ndim() == 1) return NArray(*this);
        return NArray(get_data_copy_as_shared_ptr(), _shape.flatten());
    }


    // Returns a flat view of the array
    [[nodiscard]] NArray ravel() {
        if (_shape.get_Ndim() == 1) return *this;
        return NArray(_data_ptr, _shape.flatten());
    }


    // Returns a deep copy of the NArray
    [[nodiscard]] NArray deepcopy() const { return NArray(*this); }


    // Returns a shallow copy of the NArray
    [[nodiscard]] NArray copy() const { return NArray(_data_ptr, _shape); }


    NArray& reshape(const Shape& shape) {
        if (shape.get_total_size() != this->get_total_size())
            throw error::ShapeError("New shape does not fit existing data.");
        _shape = shape;
        return *this;
    }

    NArray& reshape(Shape&& shape) {
        if (shape.get_total_size() != this->get_total_size())
            throw error::ShapeError("New shape does not fit existing data.");
        _shape = std::move(shape);
        return *this;
    }


    // Returns the real components of a complex vector
    [[nodiscard]] auto real() const
        -> NArray<underlying_type_t<dtype>>
    {
        using R = underlying_type_t<dtype>;

        if constexpr (is_complex_v<dtype>) {
            NArray<R> out(_shape);
            for (size_t i = 0; i < get_total_size(); i++) {
                out.get_data()[i] = get_data()[i].real();
            }
            return out;
        } else {
            return NArray<R>(*this);
        }
    }

    // Returns the imaginary components of a complex vector
    [[nodiscard]] auto imag() const
        -> NArray<underlying_type_t<dtype>>
    {
        using R = underlying_type_t<dtype>;

        if constexpr (is_complex_v<dtype>) {
            NArray<R> out(_shape);
            for (size_t i = 0; i < get_total_size(); i++) {
                out.get_data()[i] = get_data()[i].imag();
            }
            return out;
        } else {
            return NArray<R>(_shape, 0);
        }
    }

    // Returns the length of the NArray (the last element of _shape)
    [[nodiscard]] size_t length() const { return *_shape.end(); }
    [[nodiscard]] size_t len() const { return *_shape.end(); }

    // begin iterator over the NArray's data
    dtype* begin() { return _data_ptr.get(); }
    [[nodiscard]] const dtype* begin() const {return _data_ptr.get(); }

    // end iterator over the NArray's data
    dtype* end() { return _data_ptr.get() + _shape.get_total_size(); }
    [[nodiscard]] const dtype* end() const { return _data_ptr.get() + _shape.get_total_size(); }

};


/* ====== Deduction guides ======= */

// Scalar constructor
template <typename T>
NArray(T) -> NArray<T>;

// Vector constructor
template <typename T>
NArray(std::vector<T>) -> NArray<T>;

// List constructor
template <typename T>
NArray(std::initializer_list<T>) -> NArray<T>;

// Array constructor
template <typename T>
NArray(T*, size_t) -> NArray<T>;

// Array constructor (copy)
template <typename T, typename Deleter>
NArray(T*, size_t, Deleter) -> NArray<T>;

// Repeat constructor
template <typename T>
NArray(size_t, T) -> NArray<T>;

// Data + Shape constructor
template <typename T>
NArray(std::vector<T>, Shape) -> NArray<T>;

// Shape + initializer value constructor
template <typename T>
NArray(Shape, T) -> NArray<T>;

// Constructor from shared_ptr (used for slicing)
template <typename T>
NArray(std::shared_ptr<T>, T*, Shape) -> NArray<T>;

// Shared pointer + shape constructor
template <typename T>
NArray(std::shared_ptr<T>, Shape) -> NArray<T>;

} // namespace numxx
