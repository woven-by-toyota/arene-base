// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REVERSE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REVERSE_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/type_list/at.hpp"

namespace arene {
namespace base {
namespace type_lists {

namespace reverse_detail {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
/// @brief helper reversing the types in a type list
/// @tparam Is indices
/// @tparam List type list type
/// @tparam Ts types
/// @return @c List with the types in reverse order
template <std::size_t... Is, template <class...> class List, class... Ts>
auto reverse_impl(std::index_sequence<Is...>, List<Ts...>) -> List<at_t<List<Ts...>, sizeof...(Ts) - Is - 1>...>;
// parasoft-end-suppress CERT_C-EXP37-a

}  // namespace reverse_detail

/// @brief reverses the types in a type list
/// @tparam List type-list-like
///
/// For any type-list-like @c List, provides the member typedef @c type which is
/// the same as @c List but with the types in reverse order.
///
/// Usage:
/// @snippet docs/examples/type_list_examples.cpp reverse_example
///
///
template <class List>
class reverse;

/// @brief specialization for type-list-like types
/// @tparam List type list type
/// @tparam Ts types
template <template <class...> class List, class... Ts>
class reverse<List<Ts...>> {
 public:
  /// @brief resulting type list
  using type = decltype(reverse_detail::reverse_impl(std::index_sequence_for<Ts...>{}, std::declval<List<Ts...>>()));
};

/// @brief reverses the types in a type list
/// @tparam List type-list-like
/// @copydoc reverse
template <class List>
using reverse_t = typename reverse<List>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REVERSE_HPP_
