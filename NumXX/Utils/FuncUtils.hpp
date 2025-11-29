/* FuncUtils.hpp */
#pragma once

#include "../Containers/NArray.hpp"


namespace numxx::util {

template <typename T, typename Func>
auto elementwiseFunc(const NArray<T>& arr, Func func)
    -> NArray<decltype(func(std::declval<T>()))>
{
    using U = decltype(func(std::declval<T>()));
    NArray<U> out(arr.get_shape());

    for (int i = 0; i < arr.get_shape().get_total_size(); i++) {
        out.get_data()[i] = func(arr.get_data()[i]);
    }
    return out;
}

} // namespace numxx