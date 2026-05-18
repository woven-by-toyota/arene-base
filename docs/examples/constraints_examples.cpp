// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

#include "arene/base/constraints.hpp"

namespace {

//! [substitution_succeeds_usage_example]
// The helper which uses the foo member function to force a SFINAE context.
template <typename T>
using use_foo = decltype(std::declval<T>().foo());
//
// The "concept" of having a foo member function, which apples substitution_succeeds to the foo usage.
template <typename T>
constexpr bool has_foo = arene::base::substitution_succeeds<use_foo, T>;
//
// Later in the code, this is used as a constraint:
template <typename T, arene::base::constraints<std::enable_if_t<has_foo<T>>> = nullptr>
void needs_foo(T const& thing);
//! [substitution_succeeds_usage_example]

//! [contrained_function_example]
/// @brief A function that is callable only with unsigned integers the same size or smaller than @c unsigned
/// @tparam T The type of the integer
/// @param arg The unsigned integral argument value
template <
    class T,
    arene::base::constraints<
        std::enable_if_t<std::is_unsigned<T>::value>,
        std::enable_if_t<sizeof(T) <= sizeof(unsigned)>> = nullptr>
void foo(T arg);
//! [contrained_function_example]
}  // namespace
