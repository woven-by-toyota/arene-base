// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_TUPLET_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_TUPLET_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace testing {

// NOLINTBEGIN(readability-identifier-length)

/// @brief a simplified tuple that does not have the template depth limitations
/// of the C++14 recursive tuple implementation for older GCCs (<= 9)
///
/// @{
template <std::size_t I, class T>
struct tuplet_leaf {
  T value;

  template <std::size_t J, class = std::enable_if_t<J == I>>
  friend constexpr auto get(tuplet_leaf const& self) noexcept -> T const& {
    return self.value;
  }
};

template <class Indices, class Types>
struct tuplet_impl;

template <std::size_t... Is, class... Ts>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct tuplet_impl<std::index_sequence<Is...>, arene::base::type_list<Ts...>> : tuplet_leaf<Is, Ts>... {
  constexpr explicit tuplet_impl(Ts... args)
      : tuplet_leaf<Is, Ts>{args}... {}
};

template <class... Ts>
using tuplet_impl_t = tuplet_impl<std::index_sequence_for<Ts...>, arene::base::type_list<Ts...>>;

template <class... Ts>
struct tuplet : tuplet_impl_t<Ts...> {
  using impl_type = tuplet_impl_t<Ts...>;
  using impl_type::impl_type;
};

template <class... Ts>
constexpr auto make_tuplet(Ts... args) {
  return tuplet<Ts...>{args...};
}
/// @}

// NOLINTEND(readability-identifier-length)

}  // namespace testing

template <class... Ts>
class std::tuple_size<::testing::tuplet<Ts...>> : public std::integral_constant<std::size_t, sizeof...(Ts)> {};
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_TUPLET_HPP_
