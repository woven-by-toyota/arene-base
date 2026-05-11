// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TAKE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TAKE_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/type_list/at.hpp"

namespace arene {
namespace base {
namespace type_lists {

namespace take_detail {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
/// @brief helper keeping the first @c Count types
/// @tparam Count number of types to remove
/// @tparam Is indices
/// @tparam List type list type
/// @tparam Ts types
/// @return @c List after keeping the first @c Count types from @c Ts...
template <std::size_t... Is, template <class...> class List, class... Ts>
auto take_impl(std::index_sequence<Is...>, List<Ts...>) -> List<at_t<List<Ts...>, Is>...>;
// parasoft-end-suppress CERT_C-EXP37-a

/// @brief implementation helper for take
/// @tparam List type list
/// @tparam Count number of types to take
///
/// Undefined primary template.
template <class List, std::size_t Count, class = constraints<>>
class take;

/// @brief implementation helper for take
/// @tparam List type list type
/// @tparam Ts types
/// @tparam Count number of types to take
///
/// Specialization for @c Count less than the size of @c List<Ts...>
template <template <class...> class List, class... Ts, std::size_t Count>
class take<List<Ts...>, Count, constraints<std::enable_if_t<(Count < sizeof...(Ts))>>> {
 public:
  /// @brief resulting type list
  using type = decltype(take_impl(std::make_index_sequence<Count>{}, std::declval<List<Ts...>>()));
};

/// @brief implementation helper for take
/// @tparam List type list type
/// @tparam Ts types
/// @tparam Count number of types to take
///
/// Specialization for @c Count greater than or equal to the size of @c List<Ts...>
template <template <class...> class List, class... Ts, std::size_t Count>
class take<List<Ts...>, Count, constraints<std::enable_if_t<(Count >= sizeof...(Ts))>>> {
 public:
  /// @brief resulting type list
  using type = List<Ts...>;
};

}  // namespace take_detail

/// @brief takes types from the front of a type list
/// @tparam List type-list-like
/// @tparam Count number of types to take
///
/// For any type-list-like @c List, provides the member typedef @c type which is
/// the same as @c List but with the first @c Count types omitted. If @c Count
/// exceeds the number of types in @c List, @c type specifies @c List.
///
/// Usage:
/// @snippet docs/examples/type_list_examples.cpp take_example
///
///
template <class List, std::size_t Count>
using take = take_detail::take<List, Count>;

/// @brief takes types from the front of a type list
/// @tparam List type-list-like
/// @tparam Count number of types to take
/// @copydoc take
template <class List, std::size_t Count>
using take_t = typename take<List, Count>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TAKE_HPP_
