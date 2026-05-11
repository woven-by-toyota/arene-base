// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_THROWS_ON_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_THROWS_ON_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"  // IWYU pragma: export

namespace testing {

/// bitmask enum specifying special member functions that can throw
///
/// Used by test utility classes that allow configuration of `noexcept`, e.g.
/// '::testing::noexcept_mixin' and
/// '::testing::noexcept_configurable_forward_iterator'.
///
enum class throws_on : std::uint64_t {
  // clang-format off
  nothing                 = std::uint64_t{},
  default_construct       = std::uint64_t{1} << 0U,
  value_construct         = std::uint64_t{1} << 1U,
  copy_construct          = std::uint64_t{1} << 2U,
  move_construct          = std::uint64_t{1} << 3U,
  copy_assign             = std::uint64_t{1} << 4U,
  move_assign             = std::uint64_t{1} << 5U,
  swap                    = std::uint64_t{1} << 6U,
  dereference             = std::uint64_t{1} << 7U,
  arrow                   = std::uint64_t{1} << 8U,
  pre_increment           = std::uint64_t{1} << 9U,
  post_increment          = std::uint64_t{1} << 10U,
  pre_decrement           = std::uint64_t{1} << 11U,
  post_decrement          = std::uint64_t{1} << 12U,
  equal                   = std::uint64_t{1} << 13U,
  not_equal               = std::uint64_t{1} << 14U,
  less                    = std::uint64_t{1} << 15U,
  greater                 = std::uint64_t{1} << 16U,
  less_equal              = std::uint64_t{1} << 17U,
  greater_equal           = std::uint64_t{1} << 18U,
  plus_equals_difference  = std::uint64_t{1} << 19U,
  plus_difference         = std::uint64_t{1} << 20U,
  difference_plus         = std::uint64_t{1} << 21U,
  minus_equals_difference = std::uint64_t{1} << 22U,
  minus_difference        = std::uint64_t{1} << 23U,
  minus_self              = std::uint64_t{1} << 24U,
  index                   = std::uint64_t{1} << 25U,
  plus                    = std::uint64_t{1} << 26U,
  minus                   = std::uint64_t{1} << 27U,
  multiply                = std::uint64_t{1} << 28U,
  divide                  = std::uint64_t{1} << 29U,
  modulo                  = std::uint64_t{1} << 30U,
  multiply_assign         = std::uint64_t{1} << 31U,
  divide_assign           = std::uint64_t{1} << 32U,
  modulo_assign           = std::uint64_t{1} << 33U,
  everything              = ~std::uint64_t{},
  // clang-format on
};

/// integral constants of values of the 'throws_on' enum
///
/// Useful in situations where information needs to be propagated at
/// compile-time but a template parameter cannot be used:
/// ~~~{.cpp}
/// auto const is_noexcept_if_iterator = [](auto specifier_constant, auto... eq) {
///   using iterator = testing::noexcept_configurable_forward_iterator<specifier_constant>;
///   return noexcept(std::unique(iterator{}, iterator{}, eq...));
/// };
///
/// using throws_on = testing::throws_on_specifiers;
///
/// ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing, testing::always_true));
/// ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct, testing::always_true));
/// ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference, testing::always_true));
/// ~~~
///
struct throws_on_specifiers {
  static constexpr auto nothing = std::integral_constant<throws_on, throws_on::nothing>{};
  static constexpr auto default_construct = std::integral_constant<throws_on, throws_on::default_construct>{};
  static constexpr auto value_construct = std::integral_constant<throws_on, throws_on::value_construct>{};
  static constexpr auto copy_construct = std::integral_constant<throws_on, throws_on::copy_construct>{};
  static constexpr auto move_construct = std::integral_constant<throws_on, throws_on::move_construct>{};
  static constexpr auto copy_assign = std::integral_constant<throws_on, throws_on::copy_assign>{};
  static constexpr auto move_assign = std::integral_constant<throws_on, throws_on::move_assign>{};
  static constexpr auto swap = std::integral_constant<throws_on, throws_on::swap>{};
  static constexpr auto dereference = std::integral_constant<throws_on, throws_on::dereference>{};
  static constexpr auto arrow = std::integral_constant<throws_on, throws_on::arrow>{};
  static constexpr auto pre_increment = std::integral_constant<throws_on, throws_on::pre_increment>{};
  static constexpr auto post_increment = std::integral_constant<throws_on, throws_on::post_increment>{};
  static constexpr auto pre_decrement = std::integral_constant<throws_on, throws_on::pre_decrement>{};
  static constexpr auto post_decrement = std::integral_constant<throws_on, throws_on::post_decrement>{};
  static constexpr auto equal = std::integral_constant<throws_on, throws_on::equal>{};
  static constexpr auto not_equal = std::integral_constant<throws_on, throws_on::not_equal>{};
  static constexpr auto less = std::integral_constant<throws_on, throws_on::less>{};
  static constexpr auto greater = std::integral_constant<throws_on, throws_on::greater>{};
  static constexpr auto less_equal = std::integral_constant<throws_on, throws_on::less_equal>{};
  static constexpr auto greater_equal = std::integral_constant<throws_on, throws_on::greater_equal>{};
  static constexpr auto plus_difference = std::integral_constant<throws_on, throws_on::plus_difference>{};
  static constexpr auto minus_self = std::integral_constant<throws_on, throws_on::minus_self>{};
  static constexpr auto plus_equals_difference = std::integral_constant<throws_on, throws_on::plus_equals_difference>{};
};

/// decay to underlying type
constexpr auto operator+(throws_on arg) noexcept { return static_cast<std::uint64_t>(arg); }

/// bitwise OR
constexpr auto operator|(throws_on lhs, throws_on rhs) noexcept -> throws_on {
  return static_cast<throws_on>(+lhs | +rhs);
}

/// bitwise AND
constexpr auto operator&(throws_on lhs, throws_on rhs) noexcept -> throws_on {
  return static_cast<throws_on>(+lhs & +rhs);
}

/// checks if a specification is a subset of value
/// @tparam Spec specification of the 'noexcept' special member functions
/// @param value value containing all the 'noexcept 'special member functions
/// @return 'true' if 'Spec' is a subset of 'value'
///
/// Checks if 'Spec' is contained within 'value'.
///
template <throws_on Spec>
constexpr auto contains(throws_on value) noexcept -> bool {
  return 0U != +(value & Spec);
}

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_THROWS_ON_HPP_
