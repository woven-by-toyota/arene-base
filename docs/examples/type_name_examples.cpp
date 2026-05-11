// Copyright 2024, Toyota Motor Corporation

// Examples from the docs of \c arene::base::type_name_v

#include <iostream>
#include <type_traits>
#include <utility>

#include "arene/base/type_info.hpp"

//! [print_type_name_example]
template <typename T>
void print_with_type_name(T&& val) {
  std::cout << arene::base::string_view{arene::base::type_name_v<T>} << " : " << val << "\n";
}
//! [print_type_name_example]

//! [ordered_pair_example]
template <typename A, typename B>
using ordered_pair =
    std::conditional_t<(arene::base::type_name_v<A> < arene::base::type_name_v<B>), std::pair<A, B>, std::pair<B, A>>;

using pair1 = ordered_pair<int, double>;
using pair2 = ordered_pair<double, int>;
static_assert(std::is_same<pair1, pair2>::value, "Types must be the same");
//! [ordered_pair_example]
