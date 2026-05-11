// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_AS_TYPE_LIST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_AS_TYPE_LIST_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/tuple/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {
namespace tuple_detail {
namespace as_type_list_detail {

/// @brief view a tuple-like @c T as a @c type_list of its element types
/// @tparam T a tuple-like type (has @c std::tuple_size and @c std::tuple_element)
/// @tparam Seq index sequence <c>0 .. tuple_size<T>::value - 1</c>
template <class T, class Seq = std::make_index_sequence<std::tuple_size<T>::value>>
class as_type_list_impl;

/// @brief specialization that expands the index sequence into one @c std::tuple_element_t per position
/// @tparam T the tuple-like type
/// @tparam Is the indices <c>0 .. tuple_size<T>::value - 1</c>
template <class T, std::size_t... Is>
class as_type_list_impl<T, std::index_sequence<Is...>> {
 public:
  /// @brief The resulting type list
  using type = arene::base::type_list<std::tuple_element_t<Is, T>...>;
};

}  // namespace as_type_list_detail

/// @brief view a tuple-like @c T as a @c type_list of its element types
/// @tparam T a tuple-like type
template <class T, class = constraints<>>
class as_type_list {
 public:
  /// @brief The resulting type list
  using type = typename as_type_list_detail::as_type_list_impl<T>::type;
};

/// @brief Specialzation for std::tuple as it is already a type_list
template <class T>
class as_type_list<T, constraints<std::enable_if_t<is_instantiation_of_v<T, std::tuple>>>> {
 public:
  /// @brief The resulting type list
  using type = T;
};

/// @brief view a tuple-like @c T as a @c type_list of its element types
/// @tparam T a tuple-like type
///
/// This utility is intended to view tuple-like types which are not already type_lists as a type_list of its elements.
/// As @c std::tuple is already a type-list, the resulting type is the same as the input type.
///
/// ~~~{.cpp}
/// static_assert(std::is_same_v<
///   as_type_list_t<arene::base::array<int, 3>>,
///   arene::base::type_list<int, int, int>
/// >);
///
/// static_assert(std::is_same_v<
///   as_type_list_t<std::pair<int, double>>,
///   arene::base::type_list<int, double>
/// >);
///
/// static_assert(std::is_same_v<
///   as_type_list_t<std::tuple<int, double>>,
///   std::tuple<int, double>
/// >);
/// ~~~
template <class T>
using as_type_list_t = typename as_type_list<T>::type;

}  // namespace tuple_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_AS_TYPE_LIST_HPP_
