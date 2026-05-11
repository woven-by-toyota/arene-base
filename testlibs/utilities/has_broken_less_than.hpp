// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_HAS_BROKEN_LESS_THAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_HAS_BROKEN_LESS_THAN_HPP_

namespace testing {

struct has_broken_less_than {
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Implicit conversion makes it easier to use this in tests
  constexpr has_broken_less_than(int val)
      : value(val) {}
  int value;

  friend constexpr auto operator==(has_broken_less_than const& lhs, has_broken_less_than const& rhs) -> bool {
    return lhs.value == rhs.value;
  }
  template <decltype(nullptr) Dummy = nullptr>
  friend constexpr auto operator<(has_broken_less_than const&, has_broken_less_than const&) -> bool {
    static_assert((static_cast<void>(Dummy), false), "Function can not actually be instantiated");
    return false;
  }
};

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_HAS_BROKEN_LESS_THAN_HPP_
