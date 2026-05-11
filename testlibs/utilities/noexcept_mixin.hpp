// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_NOEXCEPT_MIXIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_NOEXCEPT_MIXIN_HPP_

#include "stdlib/include/cstdint"
#include "testlibs/utilities/throws_on.hpp"  // IWYU pragma: export

namespace testing {

/// test utility class used to configure the noexcept qualifier for special member functions
/// @tparam ThrowsOn special member functions that can throw  (i.e. 'noexcept(false)')
///
/// Class where the special member functions are 'noexcept(false)' if specified
/// bit is contained within 'ThrowsOn'.
///
/// Useful in testing 'noexcept' qualifiers of generic code.
///
template <throws_on ThrowsOn>
struct noexcept_mixin {
  // NOLINTBEGIN(hicpp-use-equals-default,hicpp-noexcept-move)
  ~noexcept_mixin() = default;
  constexpr noexcept_mixin() noexcept(!contains<ThrowsOn>(throws_on::default_construct)) {}
  constexpr noexcept_mixin(noexcept_mixin const&) noexcept(!contains<ThrowsOn>(throws_on::copy_construct)) {}
  constexpr noexcept_mixin(noexcept_mixin&&) noexcept(!contains<ThrowsOn>(throws_on::move_construct)) {}
  constexpr auto operator=(noexcept_mixin const&) noexcept(!contains<ThrowsOn>(throws_on::copy_assign))
      -> noexcept_mixin& {
    return *this;
  }
  constexpr auto operator=(noexcept_mixin&&) noexcept(!contains<ThrowsOn>(throws_on::move_assign)) -> noexcept_mixin& {
    return *this;
  }
  // NOLINTEND(hicpp-use-equals-default,hicpp-noexcept-move)
};

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_NOEXCEPT_MIXIN_HPP_
