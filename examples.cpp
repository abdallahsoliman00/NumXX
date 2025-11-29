/* examples.cpp */
#include "NumXX/NumXX.hpp"

namespace nx = numxx;
using nx::Shape;


void initialisation_example() {
    /* For more, see NumXX/Core/ArrayCreation.hpp */
    // Array initialisation
    auto array1 = nx::NArray({1,2,3});
    auto array2 = nx::linspace(0,1,20, false);
    auto array3 = nx::arange(0,10,1);

    auto mat1 = nx::Matrix({
        {1,2},
        {3,4}
    });
    auto mat2 = nx::NArray({
        {2,4},
        {6,8}
    });

    auto tensor = nx::NArray({
        {{1,2},{3,4}},
        {{5,6},{7,8}}
    });

    std::cout << "array1:\n" << array1 << "\n\n";
    std::cout << "array2:\n" << array2 << "\n\n";
    std::cout << "array3:\n" << array3 << "\n\n";
    std::cout << "mat1:\n" << mat1 << "\n\n";
    std::cout << "mat2:\n" << mat2 << "\n\n";
    std::cout << "tensor:\n" << tensor << "\n\n";

    /* Output:
    
    >> array1:
    [1, 2, 3]

    >> array2:
    [0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95]

    >> array3:
    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

    >> mat1:
    [[1, 2],
     [3, 4]]

    >> mat2:
    [[2, 4],
     [6, 8]]

    >> tensor:
    [[[1, 2],
      [3, 4]],

    [[5, 6],
      [7, 8]]]
    
    */
}


void constructors_example() {
    /* There are 17 differeny ways to construct an NArray
       Here are the most useful ones: */

    // initializer_list constructor
    auto arr1 = nx::NArray({1,2,3,4});

    // n-dimensional array ... any number of dimensions
    auto arr2 = nx::NArray({
        {{{1,2},{3,4}},
        {{1,2},{3,4}}},

        {{{1,2},{3,4}},
        {{1,2},{3,4}}},

        {{{1,2},{3,4}},
        {{1,2},{3,4}}}
    });

    // repeat/shape constructor
    auto arr3 = nx::NArray(Shape{2,2}, 3.14f);
    auto arr4 = nx::NArray(5, 1.25f);

    // from std::vector
    std::vector<int> vec(4);
    auto arr5 = nx::NArray(vec);

    // from iterator
    vec[2] = 4;
    auto arr6 = nx::NArray<int>(vec.begin(), vec.end() - 1);

    // input flat data + shape
    auto arr7 = nx::NArray(vec, Shape{2,2});

    // share a shared_ptr
    std::shared_ptr<double> data_ptr(new double[7](), std::default_delete<double[]>());
    data_ptr.get()[3] = 13.13;
    auto arr8 = nx::NArray(data_ptr, Shape{7});

    std::cout << "arr1:\n" << arr1 << "\n\n";
    std::cout << "arr2:\n" << arr2 << "\n\n";
    std::cout << "arr3:\n" << arr3 << "\n\n";
    std::cout << "arr4:\n" << arr4 << "\n\n";
    std::cout << "arr5:\n" << arr5 << "\n\n";
    std::cout << "arr6:\n" << arr6 << "\n\n";
    std::cout << "arr7:\n" << arr7 << "\n\n";
    std::cout << "arr8:\n" << arr8 << "\n\n";

    /* Output: 
    
    >> arr1:
    [1, 2, 3, 4]

    >> arr2:
    [[[[1, 2],
       [3, 4]],
      [[1, 2],
       [3, 4]]],

    [[[1, 2],
       [3, 4]],
      [[1, 2],
       [3, 4]]],

    [[[1, 2],
      [3, 4]],
     [[1, 2],
      [3, 4]]]]

    >> arr3:
    [[3.14, 3.14],
     [3.14, 3.14]]

    >> arr4:
    [1.25, 1.25, 1.25, 1.25, 1.25]

    >> arr5:
    [0, 0, 0, 0]

    >> arr6:
    [0, 0, 4]

    >> arr7:
    [[0, 0],
    [4, 0]]

    >> arr8:
    [0, 0, 0, 13.13, 0, 0, 0]

    */

}


int main() {

    initialisation_example();
    constructors_example();

    return 0;
}

