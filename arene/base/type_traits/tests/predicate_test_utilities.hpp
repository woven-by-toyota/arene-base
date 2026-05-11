// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_TESTS_PREDICATE_TEST_UTILITIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_TESTS_PREDICATE_TEST_UTILITIES_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/logical_and.hpp"
#include "arene/base/stdlib_choice/logical_or.hpp"
#include "testlibs/utilities/configurable_value.hpp"  // IWYU pragma: export
#include "testlibs/utilities/tuplet.hpp"

namespace type_traits_test {

/// @brief returns a tuple of function objects that are invocable with args
template <class... Args>
auto lambda_predicate_candidates() {
  return testing::make_tuplet(
      [](Args...) -> bool { return {}; },
      [](Args...) -> int { return {}; },
      [](Args...) -> std::integral_constant<int, 0> { return {}; },
      [](Args...) -> int* { return {}; },
      [](Args...) -> testing::configurable_value<bool> { return {}; }
  );
}

/// @brief perform a transform and reduce across elements of a tuple instead of a range
template <  //
    std::size_t... Is,
    template <class...>
    class Tuple,
    class... Ts,
    class T,
    class F1,
    class F2>
auto tuple_transform_reduce(  //
    std::index_sequence<Is...>,
    Tuple<Ts...> const& tuple,
    T init,
    F1 reduce,
    F2 transform
) -> T {
  using arene::base::get;
  auto transformed = arene::base::to_array({transform(get<Is>(tuple))...});

  return [](auto first, auto last, T acc, F1 bop) {
    while (first != last) {
      acc = bop(acc, *first++);
    }
    return acc;
  }(transformed.begin(), transformed.end(), init, reduce);
}
template <template <class...> class Tuple, class... Ts, class T, class F1, class F2>
auto tuple_transform_reduce(Tuple<Ts...> const& tuple, T init, F1 reduce, F2 transform) -> T {
  return tuple_transform_reduce(std::index_sequence_for<Ts...>{}, tuple, init, reduce, transform);
}

/// @brief check that a predicate is @c true for *all* elements of a tuple
template <template <class...> class Tuple, class... Ts, class F>
auto tuple_all_of(Tuple<Ts...> const& tuple, F pred) -> bool {
  return tuple_transform_reduce(tuple, true, std::logical_and<>{}, pred);
}

/// @brief check that a predicate is @c false for *all* elements of a tuple
template <template <class...> class Tuple, class... Ts, class F>
auto tuple_none_of(Tuple<Ts...> const& tuple, F pred) -> bool {
  return !tuple_transform_reduce(tuple, false, std::logical_or<>{}, pred);
}

}  // namespace type_traits_test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_TESTS_PREDICATE_TEST_UTILITIES_HPP_
