// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_DISABLE_MIXIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_DISABLE_MIXIN_HPP_

#include "stdlib/include/cstddef"
#include "stdlib/include/cstdint"
#include "stdlib/include/type_traits"

namespace testing {

/// bitmask enum specifying special member functions to disable
///
enum class disable : std::uint32_t {
  // clang-format off
  nothing                 = 0U,
  default_construct       = 1U << 0U,
  value_construct         = 1U << 1U,
  copy_construct          = 1U << 2U,
  move_construct          = 1U << 3U,
  copy_assign             = 1U << 4U,
  move_assign             = 1U << 5U,
  swap                    = 1U << 6U,
  equal                   = 1U << 7U,
  not_equal               = 1U << 8U,
  less                    = 1U << 9U,
  greater                 = 1U << 10U,
  less_equal              = 1U << 11U,
  greater_equal           = 1U << 12U,
  plus                    = 1U << 13U,
  minus                   = 1U << 14U,
  multiplies              = 1U << 15U,
  divides                 = 1U << 16U,
  modulus                 = 1U << 17U,
  plus_assign             = 1U << 18U,
  minus_assign            = 1U << 19U,
  multiply_assign         = 1U << 20U,
  divide_assign           = 1U << 21U,
  modulo_assign           = 1U << 22U,
  pre_increment           = 1U << 23U,
  post_increment          = 1U << 24U,
  pre_decrement           = 1U << 25U,
  post_decrement          = 1U << 26U,
  // clang-format off
};

/// decay to underlying type
constexpr auto operator+(disable arg) noexcept
{
  return static_cast<std::uint32_t>(arg);
}

/// bitwise OR
constexpr auto operator|(disable lhs, disable rhs) noexcept -> disable
{
  return static_cast<disable>(+lhs | +rhs);
}

/// bitwise AND
constexpr auto operator&(disable lhs, disable rhs) noexcept -> disable
{
  return static_cast<disable>(+lhs & +rhs);
}

/// checks if a specification is a subset of value
/// @tparam Spec specification of disabled special member functions
/// @param value value of disabled special member functions
/// @return 'true' if 'Spec' is a subset of 'value'
///
/// Checks if 'Spec' is contained within 'value'.
///
template <disable Spec>
constexpr auto contains(disable value) noexcept -> bool {
  return 0U != +(value & Spec);
}

// NOLINTBEGIN(hicpp-special-member-functions,fuchsia-multiple-inheritance)

/// helper for 'disable_mixin' that provides an alternative constructor
/// if the default constructor is disabled
///
/// @{
template <bool>  // false
struct disable_default_construct {
  constexpr disable_default_construct() = default;
  constexpr explicit disable_default_construct(std::nullptr_t) noexcept {}
};
/// @}

/// helper for 'disable_mixin' that conditionally defines or deletes move
/// construction
///
/// @{
template <bool>  // false
struct disable_move_construct {};

// this type must disable both move and copy constructors, otherwise the type
// will still be move constructible
template <>
struct disable_move_construct<true> {
  disable_move_construct() = default;
  disable_move_construct(disable_move_construct const&) = delete;
  disable_move_construct(disable_move_construct&&) = delete;
  auto operator=(disable_move_construct const&) -> disable_move_construct& = default;
  auto operator=(disable_move_construct&&) -> disable_move_construct& = default;
};
/// @}

/// helper for 'disable_mixin' that conditionally defines or deletes copy
/// construction
///
/// @{
template <bool>  // false
struct disable_copy_construct {};

template <>
struct disable_copy_construct<true> {
  disable_copy_construct() = default;
  disable_copy_construct(disable_copy_construct const&) = delete;
  disable_copy_construct(disable_copy_construct&&) = default;
  auto operator=(disable_copy_construct const&) -> disable_copy_construct& = default;
  auto operator=(disable_copy_construct&&) -> disable_copy_construct& = default;
};
/// @}

/// helper for 'disable_mixin' that conditionally defines or deletes move
/// assignment
///
/// @{
template <bool>  // false
struct disable_move_assign {};

// this type must disable both move and copy assignment operators, otherwise
// the type will still be move assignable
template <>
struct disable_move_assign<true> {
  disable_move_assign() = default;
  disable_move_assign(disable_move_assign const&) = default;
  disable_move_assign(disable_move_assign&&) = default;
  auto operator=(disable_move_assign const&) -> disable_move_assign& = delete;
  auto operator=(disable_move_assign&&) -> disable_move_assign& = delete;
};
///  @}

/// helper for 'disable_mixin' that conditionally defines or deletes copy
/// assignment
///
/// @{
template <bool>  // false
struct disable_copy_assign {};

template <>
struct disable_copy_assign<true> {
  disable_copy_assign() = default;
  disable_copy_assign(disable_copy_assign const&) = default;
  disable_copy_assign(disable_copy_assign&&) = default;
  auto operator=(disable_copy_assign const&) -> disable_copy_assign& = delete;
  auto operator=(disable_copy_assign&&) -> disable_copy_assign& = default;
};
///  @}

/// test utility class used to disable special member functions
/// @tparam SpecialMembers enum value of special member functions to delete
///
/// Class where the special member functions are deleted according to
/// 'SpecialMembers'.
///
/// Useful in testing generic code that may operate on types that are not
/// semiregular.
///
template <disable SpecialMembers>
struct disable_mixin
    : disable_default_construct<contains<SpecialMembers>(disable::default_construct)>
    , disable_move_construct<contains<SpecialMembers>(disable::move_construct)>
    , disable_copy_construct<contains<SpecialMembers>(disable::copy_construct)>
    , disable_move_assign<contains<SpecialMembers>(disable::move_assign)>
    , disable_copy_assign<contains<SpecialMembers>(disable::copy_assign)> {
  static constexpr auto is_default_constructible = !contains<SpecialMembers>(disable::default_construct);

  template <bool B = is_default_constructible, class = std::enable_if_t<B>>
  constexpr disable_mixin() noexcept {}

  constexpr explicit disable_mixin(std::nullptr_t) noexcept
      : disable_default_construct<contains<SpecialMembers>(disable::default_construct)>{nullptr} {}
};

// NOLINTEND(hicpp-special-member-functions,fuchsia-multiple-inheritance)

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_DISABLE_MIXIN_HPP_
