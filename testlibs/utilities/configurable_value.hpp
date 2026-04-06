// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_CONFIGURABLE_VALUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_CONFIGURABLE_VALUE_HPP_

#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_fundamental.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "testlibs/utilities/disable_mixin.hpp"   // IWYU pragma: export
#include "testlibs/utilities/noexcept_mixin.hpp"  // IWYU pragma: export

namespace testing {

// NOLINTBEGIN(hicpp-special-member-functions,fuchsia-multiple-inheritance)

/// specifies if 'constexpr' is enabled or disabled for 'configurable value'
enum class is_constexpr : bool {
  no = false,
  yes = true,
};

/// function to disable 'constexpr' for 'configurable_value'
inline auto allow_constexpr_if(std::false_type) -> void {}

/// function to enable (by not explicitly disabling) 'constexpr' for 'configurable_value'
constexpr auto allow_constexpr_if(std::true_type) -> void {}

/// A test class with configurable special member functions
/// @tparam T The type of the value to contain; implicitly convertible from this type
/// @tparam ThrowsOn A specification for which special member functions are marked as @c noexcept(false)
/// @tparam Disable A specification for which (special) member function and ADL
///   functions are deleted or removed by SFINAE
/// @tparam Constexp A specification to enable to disable constexpr
template <
    class T,
    throws_on ThrowsOn = throws_on::nothing,
    disable Disable = disable::nothing,
    is_constexpr Constexpr = is_constexpr::yes>
class configurable_value : private noexcept_mixin<ThrowsOn> {
  // This needs to be data member due to the following bug which affects GCC 9
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=103879
  disable_mixin<Disable> disable_spec_;

 public:
  T value{};

  template <class Base = disable_mixin<Disable>, class = std::enable_if_t<std::is_default_constructible<Base>::value>>
  constexpr configurable_value() noexcept(std::is_nothrow_default_constructible<noexcept_mixin<ThrowsOn>>::value)
      : disable_spec_{} {
    allow_constexpr_if(std::integral_constant<bool, static_cast<bool>(Constexpr)>{});
  }

  template <class U, class = std::enable_if_t<std::is_constructible<T, U>::value>>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr configurable_value(U val) noexcept(!contains<ThrowsOn>(throws_on::value_construct))
      : disable_spec_{nullptr},
        value{static_cast<T>(val)} {
    allow_constexpr_if(std::integral_constant<bool, static_cast<bool>(Constexpr)>{});
  }

  /// relational operators
  ///
  /// @{

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::equal)>>
  friend constexpr auto operator==(configurable_value const& lhs, configurable_value const& rhs) noexcept -> bool {
    return lhs.value == rhs.value;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::not_equal)>>
  friend constexpr auto operator!=(configurable_value const& lhs, configurable_value const& rhs) noexcept -> bool {
    return lhs.value != rhs.value;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::less)>>
  friend constexpr auto operator<(configurable_value const& lhs, configurable_value const& rhs) noexcept(
      !contains<ThrowsOn>(throws_on::less)
  ) -> bool {
    return lhs.value < rhs.value;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::greater)>>
  friend constexpr auto operator>(configurable_value const& lhs, configurable_value const& rhs) noexcept -> bool {
    return lhs.value > rhs.value;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::less_equal)>>
  friend constexpr auto operator<=(configurable_value const& lhs, configurable_value const& rhs) noexcept -> bool {
    return lhs.value <= rhs.value;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::greater_equal)>>
  friend constexpr auto operator>=(configurable_value const& lhs, configurable_value const& rhs) noexcept -> bool {
    return lhs.value >= rhs.value;
  }

  /// @}

  /// swap
  ///
  /// @{

  // overload for fundamental types (e.g., int)
  static constexpr auto do_swap(std::true_type, T& lhs, T& rhs) noexcept -> void {
    auto tmp = std::move(lhs);
    lhs = std::move(rhs);
    rhs = std::move(tmp);
  }

  // not implemented, implement if needed
  static constexpr auto do_swap(std::false_type, T&, T&) noexcept -> void;

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::swap)>>
  friend constexpr auto swap(configurable_value& lhs, configurable_value& rhs) noexcept(
      !contains<ThrowsOn>(throws_on::swap)
  ) -> void {
    configurable_value::do_swap(std::is_fundamental<T>{}, lhs.value, rhs.value);
  }

  /// @}

  /// arithmetic operators
  ///
  /// @{

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::plus)>>
  friend constexpr auto operator+(configurable_value const& lhs, configurable_value const& rhs) noexcept(
      !contains<ThrowsOn>(throws_on::plus)
  ) -> configurable_value {
    return configurable_value{lhs.value + rhs.value};
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::minus)>>
  friend constexpr auto operator-(configurable_value const& lhs, configurable_value const& rhs) noexcept(
      !contains<ThrowsOn>(throws_on::minus)
  ) -> configurable_value {
    return configurable_value{lhs.value - rhs.value};
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::multiplies)>>
  friend constexpr auto operator*(configurable_value const& lhs, configurable_value const& rhs) noexcept(
      !contains<ThrowsOn>(throws_on::multiply)
  ) -> configurable_value {
    return configurable_value{lhs.value * rhs.value};
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::divides)>>
  friend constexpr auto operator/(configurable_value const& lhs, configurable_value const& rhs) noexcept(
      !contains<ThrowsOn>(throws_on::divide)
  ) -> configurable_value {
    return configurable_value{lhs.value / rhs.value};
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::modulus)>>
  friend constexpr auto operator%(configurable_value const& lhs, configurable_value const& rhs) noexcept(
      !contains<ThrowsOn>(throws_on::modulo)
  ) -> configurable_value {
    return configurable_value{lhs.value % rhs.value};
  }

  /// @}

  /// compound assignment operators
  ///
  /// @{

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::plus_assign)>>
  constexpr auto operator+=(configurable_value const& rhs
  ) noexcept(!contains<ThrowsOn>(throws_on::plus_equals_difference)) -> configurable_value& {
    value += rhs.value;
    return *this;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::minus_assign)>>
  constexpr auto operator-=(configurable_value const& rhs
  ) noexcept(!contains<ThrowsOn>(throws_on::minus_equals_difference)) -> configurable_value& {
    value -= rhs.value;
    return *this;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::multiply_assign)>>
  constexpr auto operator*=(configurable_value const& rhs) noexcept(!contains<ThrowsOn>(throws_on::multiply_assign))
      -> configurable_value& {
    value *= rhs.value;
    return *this;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::divide_assign)>>
  constexpr auto operator/=(configurable_value const& rhs) noexcept(!contains<ThrowsOn>(throws_on::divide_assign))
      -> configurable_value& {
    value /= rhs.value;
    return *this;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::modulo_assign)>>
  constexpr auto operator%=(configurable_value const& rhs) noexcept(!contains<ThrowsOn>(throws_on::modulo_assign))
      -> configurable_value& {
    value %= rhs.value;
    return *this;
  }

  /// @}

  /// increment and decrement operators
  ///
  /// @{

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::pre_increment)>>
  constexpr auto operator++() noexcept(!contains<ThrowsOn>(throws_on::pre_increment)) -> configurable_value& {
    ++value;
    return *this;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::post_increment)>>
  constexpr auto operator++(int) noexcept(!contains<ThrowsOn>(throws_on::post_increment)) -> configurable_value {
    return configurable_value{value++};
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::pre_decrement)>>
  constexpr auto operator--() noexcept(!contains<ThrowsOn>(throws_on::pre_decrement)) -> configurable_value& {
    --value;
    return *this;
  }

  template <disable D = Disable, class = std::enable_if_t<!contains<D>(disable::post_decrement)>>
  constexpr auto operator--(int) noexcept(!contains<ThrowsOn>(throws_on::post_decrement)) -> configurable_value {
    return configurable_value{value--};
  }

  /// @}

  // GCC8 sees this function as used (and required) even though Clang and later GCC do not
  template <class Istream>
  friend auto operator>>(Istream& istr, configurable_value& wrapper) noexcept -> Istream& {
    return istr >> wrapper.value;
  }

  // This is intended to be implicitly convertible to the wrapped type.
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T() const { return value; }
};

// NOLINTEND(hicpp-special-member-functions,fuchsia-multiple-inheritance)

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_CONFIGURABLE_VALUE_HPP_
