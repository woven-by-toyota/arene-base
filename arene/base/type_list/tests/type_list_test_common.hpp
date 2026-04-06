// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TESTS_TYPE_LIST_TEST_COMMON_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TESTS_TYPE_LIST_TEST_COMMON_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace arene {
namespace base {
namespace tests {
namespace type_list_tests {

template <class... Tn>
using type_list = arene::base::type_list<Tn...>;

struct user_defined_type {};

using tl1 = type_list<std::int32_t, std::int8_t, user_defined_type>;
using tl2 = type_list<>;
using tl3 = type_list<user_defined_type, user_defined_type>;
using tl4 = type_list<double, double, user_defined_type, user_defined_type, float, float, std::uint32_t>;
using tl5 = type_list<tl1, tl2, tl3>;

template <typename... Ts>
using duplicate_list = type_list<Ts..., Ts...>;

template <typename T>
using doubleT = type_list<T, T>;

template <typename... Ts>
using size_of = std::integral_constant<std::size_t, sizeof...(Ts)>;

}  // namespace type_list_tests
}  // namespace tests
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TESTS_TYPE_LIST_TEST_COMMON_HPP_
