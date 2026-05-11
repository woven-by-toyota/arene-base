// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_FUNCTIONAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_FUNCTIONAL_HPP_

namespace testing {

/// @brief function object that always returns a NTTP value
///
template <class T, T Value>
struct always_nttp_value {
  template <class... Ts>
  constexpr auto operator()(Ts const&...) const noexcept -> T {
    return Value;
  }
};

/// @brief a function object constant that always returns @c true
///
constexpr auto always_true = always_nttp_value<bool, true>{};

/// @brief a function object constant that always returns @c false
///
constexpr auto always_false = always_nttp_value<bool, false>{};

/// @brief function object that always returns a default constructed value
///
template <class Constant>
struct always_constant {
  template <class... Ts>
  constexpr auto operator()(Ts const&...) const noexcept -> Constant {
    return {};
  }
};

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_FUNCTIONAL_HPP_
