// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTING_WRAPPER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTING_WRAPPER_HPP_

#include "stdlib/include/type_traits"
#include "testlibs/utilities/counter_mixin.hpp"
#include "testlibs/utilities/disable_mixin.hpp"
#include "testlibs/utilities/noexcept_mixin.hpp"

namespace testing {

/// A test class with configurable special member functions and a global counter for white-box testing
/// @tparam T The type of the value to contain; implicitly convertible from this type
/// @tparam ThrowsOn A specification for which special member functions are marked as @c noexcept(false)
/// @tparam SpecialMembers A specification for which special member functions are entirely deleted
template <class T, throws_on ThrowsOn = throws_on::nothing, disable SpecialMembers = disable::nothing>
struct counting_wrapper
    : counter_mixin<counting_wrapper<T, ThrowsOn, SpecialMembers>>
    , noexcept_mixin<ThrowsOn>
    , disable_mixin<SpecialMembers> {
  T value{};
  template <
      class Base = disable_mixin<SpecialMembers>,
      class = std::enable_if_t<std::is_default_constructible<Base>::value>>
  counting_wrapper() noexcept(std::is_nothrow_default_constructible<noexcept_mixin<ThrowsOn>>::value) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  counting_wrapper(T val) noexcept(contains<ThrowsOn>(throws_on::value_construct))
      : disable_mixin<SpecialMembers>{nullptr},
        value{val} {
    --counter_mixin<counting_wrapper>::count.default_ctor;
    ++counter_mixin<counting_wrapper>::count.value_ctor;
  }
  friend auto operator==(counting_wrapper const& lhs, counting_wrapper const& rhs) noexcept -> bool {
    return lhs.value == rhs.value;
  }
  friend auto operator!=(counting_wrapper const& lhs, counting_wrapper const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
};

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTING_WRAPPER_HPP_
