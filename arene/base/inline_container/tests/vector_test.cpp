// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/testing/vector.hpp"  // IWYU pragma: keep

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/inline_container/testing/customization.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/tests/testing_types.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/testing/gtest.hpp"
#include "testlibs/utilities/configurable_value.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <iostream>
#endif

// This is a false positive leaking through from the parameterized tests through ARENE_INSTANTIATE_TESTS.
// Properly it should be prevented at the source via iwyu.imp but we couldn't immediately figure out how.
// IWYU pragma: no_include "arene/base/stdlib_choice/move_iterator.hpp"

/////////////////////////////////////////////////
// Instantiations of parameterized test suites //
/////////////////////////////////////////////////

// These types are customized by providing an array of values, which is the easiest way when it's possible for the type.

using arene::base::testing::not_default_constructible;
using arene::base::testing::wrapped_char;

using test_string = arene::base::inline_string<50>;

template <>
constexpr auto arene::base::testing::test_value_array<wrapped_char> =
    arene::base::to_array<wrapped_char>({{'i'}, {'a'}, {'b'}, {'y'}, {'c'}, {'d'}});

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects) This test-only customization point must be (const) static.
template <>
auto const arene::base::testing::test_value_array<test_string> = arene::base::to_array<test_string>(
    {test_string{"red"}, test_string{"blue"}, test_string{"green"}, test_string{"yellow"}}
);

template <>
constexpr auto arene::base::testing::test_value_array<not_default_constructible> = arene::base::to_array(
    {not_default_constructible{-11},
     not_default_constructible{2222},
     not_default_constructible{0},
     not_default_constructible{-333333},
     not_default_constructible{44444444}}
);

// The following is our internal instantiation of these tests; users can instantiate them in the same way.

using TestTypes = ::testing::Types<
    arene::base::inline_vector<std::int32_t, 25>,
    arene::base::inline_vector<double, 15>,
    arene::base::inline_vector<wrapped_char, 10>,
    arene::base::inline_vector<test_string, 5>,
    arene::base::inline_vector<not_default_constructible, 10>,
    arene::base::inline_vector<std::int32_t, 2>,
    arene::base::inline_vector<double, 1>>;
ARENE_INSTANTIATE_TESTS(Internal, InlineVector, TestTypes);

// This is to exercise a former bug where std::pairs of types with no operator< (like ::testing::configurable_value)
// broke inline_vector.
template <typename T>
using conf_pair = std::pair<::testing::configurable_value<T>, ::testing::configurable_value<T>>;
template <typename T>
constexpr auto arene::base::testing::test_value_array<conf_pair<T>> =
    arene::base::to_array<conf_pair<T>>({{1, -2}, {6, -24}, {120, -720}, {5'040, -40'320}, {362'880, -3'628'800}});

using throwing_default_ctor = ::testing::configurable_value<std::int32_t, ::testing::throws_on::default_construct>;
using throwing_copy_ctor = ::testing::configurable_value<std::int32_t, ::testing::throws_on::copy_construct>;
using throwing_move_ctor = ::testing::configurable_value<std::int32_t, ::testing::throws_on::move_construct>;
using throwing_copy_assign = ::testing::configurable_value<std::int32_t, ::testing::throws_on::copy_assign>;
using throwing_move_assign = ::testing::configurable_value<std::int32_t, ::testing::throws_on::move_assign>;
using not_copy_assignable =
    ::testing::configurable_value<std::int32_t, ::testing::throws_on::nothing, ::testing::disable::copy_assign>;
using not_copy_constructible =
    ::testing::configurable_value<std::int32_t, ::testing::throws_on::nothing, ::testing::disable::copy_construct>;

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)

namespace testing {

// GCC8 sees this function as used (and required) even though Clang and later GCC do not
//
// This function is defined in the testing namespace, the same namespace as
// first_type and second_type of conf_pair.
auto operator>>(std::istream& istr, conf_pair<std::int32_t>& pair) noexcept -> std::istream& {
  return istr >> pair.first >> pair.second;
}

}  // namespace testing

#endif

using pair_equal_only = conf_pair<std::int32_t>;

// This second instantiation is separate to test that the ARENE_INSTANTIATE_TESTS macro can be used multiple times.

using TestTypesConfigurable = ::testing::Types<
    arene::base::inline_vector<throwing_default_ctor, 5>,
    arene::base::inline_vector<throwing_copy_ctor, 7>,
    arene::base::inline_vector<throwing_move_ctor, 11>,
    arene::base::inline_vector<throwing_copy_assign, 13>,
    arene::base::inline_vector<throwing_move_assign, 17>,
    arene::base::inline_vector<not_copy_assignable, 19>,
    arene::base::inline_vector<not_copy_constructible, 23>,
    arene::base::inline_vector<pair_equal_only, 29>>;
ARENE_INSTANTIATE_TESTS(InternalConfigurable, InlineVector, TestTypesConfigurable);
