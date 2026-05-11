// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/inline_container/testing/external_vector.hpp"
#include "arene/base/inline_container/tests/testing_types.hpp"
#include "arene/base/testing/gtest.hpp"

// The following is our internal instantiation of these tests; users can instantiate them in the same way.

using arene::base::testing::not_default_constructible;
using arene::base::testing::wrapped_char;

template <>
constexpr arene::base::array<std::int32_t, 10> arene::base::testing::test_value_array<std::int32_t> =
    {1, -2, 6, -24, 120, -720, 5'040, -40'320, 362'880, -3'628'800};

template <>
constexpr arene::base::array<double, 6> arene::base::testing::test_value_array<double> =
    {0.0, -1.0, 3.14, 2.72, 1.41, 0.577};

template <>
constexpr arene::base::array<wrapped_char, 4> arene::base::testing::test_value_array<wrapped_char> =
    {wrapped_char{'x'}, wrapped_char{'a'}, wrapped_char{'b'}, wrapped_char{'y'}};

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects) This test-only customization point must be (const) static.
template <>
arene::base::array<std::string, 4> const arene::base::testing::test_value_array<std::string> =
    {"red", "blue", "green", "yellow"};

template <>
constexpr arene::base::array<not_default_constructible, 5>
    arene::base::testing::test_value_array<not_default_constructible> = {
        not_default_constructible{-11},
        not_default_constructible{2222},
        not_default_constructible{0},
        not_default_constructible{-333333},
        not_default_constructible{44444444}
};

using TestTypes = ::testing::Types<std::int32_t, double, wrapped_char, not_default_constructible>;
ARENE_INSTANTIATE_TESTS(Internal, ExternalVector, TestTypes);
