# Bug Report

## 1. Error in printing

### Steps to replicate
Run the following snippet:
```cpp
#include "NumXX.hpp"

int main() {
    auto arr = nx::geomspace(10000, 1, 3);
    std::cout << arr << std::endl;
}
```
This produces an edge case that makes printing behave incorrectly.


### Error
```
terminate called after throwing an instance of 'std::length_error'
  what():  basic_string::_M_replace_aux
```
add this to print each element inividually and that the elements are, indeed, correctly stored:
```cpp
    for (const auto& e : arr) {
        print(e, 10);
    }
    print(arr.shape());
```
Or, alternatively, run the geomspace function with other arguments to make sure that this is only an edge case.


### Diagnosis
The issue is probably due to a floating point rounding error.
