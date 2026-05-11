// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/tuple/tests/tuple_cat_fwd.hpp"
#include "stdlib/include/tuple"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace tuple_cat_test {
namespace config {

/// @brief constexpr-preserving function object wrapping @c std::tuple_cat
///
/// @c FUNCTION_LIFT produces a C++14 lambda whose @c operator() is not @c constexpr, so
/// it cannot be called in a constant expression. This wrapper exposes
/// @c std::tuple_cat (itself @c constexpr since C++14) through a @c constexpr
/// @c operator(), which allows shared @c CONSTEXPR_TEST bodies in
/// @c arene/base/tuple/tests/tuple_cat.hpp to exercise @c std::tuple_cat at compile
/// time.
struct std_tuple_cat_fn {
  template <class... Ts>
  constexpr auto operator()(Ts&&... args) const noexcept(noexcept(std::tuple_cat(std::forward<Ts>(args)...)))
      -> decltype(std::tuple_cat(std::forward<Ts>(args)...)) {
    return std::tuple_cat(std::forward<Ts>(args)...);
  }
};

}  // namespace config
}  // namespace tuple_cat_test

template <>
constexpr auto tuple_cat_test::config::version<tuple_cat_test::config::override> =
    tuple_cat_test::config::flavor::std_tuple_cat;

template <>
constexpr auto tuple_cat_test::config::function_under_test<tuple_cat_test::config::override> =
    tuple_cat_test::config::std_tuple_cat_fn{};

#include "arene/base/tuple/tests/tuple_cat.hpp"  // IWYU pragma: keep
