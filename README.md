<p align="center">
  <img src="Resources/logo.svg" alt="Description" width="40%" />
</p>
<!-- # NumXX -->

A C++ header-only library inspired by NumPy.

(Compatible with C++17 and above)

## Overview
**NumXX** is an implementation of NumPy in C++. It hopes to bring about the easy number and array manipulation found in NumPy ... but in C++.


## How To Use
This library was made completely in vanilla C++, so using it should be simple. There are two ways:
### Method 1:
- Simply clone this repository:
```shell
git clone https://github.com/abdallahsoliman00/NumXX.git
```
- Add the folder [NumXX](NumXX) to your include path.
- To use, add the following line to the top of your C++ file:
```c++
#include "NumXX.hpp"
```

### Method 2:
- Add the following to your CMakeLists.txt project file:
```cmake
# Fetch NumXX from GitHub
include(FetchContent)

FetchContent_Declare(
    NumXX
    GIT_REPOSITORY https://github.com/abdallahsoliman00/NumXX.git
    GIT_TAG main
)

FetchContent_MakeAvailable(NumXX)

# Link library
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    NumXX
)

```
- You can now use the library by adding the following:
```c++
#include <NumXX.hpp>
```

## Project Status
This project is still underway, but current features include:
* Easy array creation for any number of dimensions
* Element-wise operations
* Array slicing
* Basic matrix and vector operations (see [here](NumXX/VecOps.hpp))
* Reshaping arrays
* More features coming soon!

For a fully working implementation, see [https://github.com/dpilger26/NumCpp](https://github.com/dpilger26/NumCpp).


## Examples
This library aims to mimic NumPy functionality, so for example, if you want to run this in NumPy:
```python
import numpy as np

x = np.array([1,2,3,4])
np.sum(x)
```

Try this in NumXX:
```c++
#include "NumXX.hpp"
namespace nx = numxx;

auto x = nx::NArray({1,2,3,4});
nx::sum(x);
```
If it doesn't exist, it hasn't been implemented yet, and if you're feeling generous perhaps you can implement it yourself and open a pull request.

To see a small taster snippet of code, see this [examples](examples.cpp) file.


## Contribution
This repository is open to contributions. If anyone wishes to contribute, please feel free to open a pull request.


## Disclaimer
This library was made solely to help learn C++. The reliability of this library is therefore not guaranteed. Please use with caution.

---

Inspired by [NumPy](https://numpy.org/).
