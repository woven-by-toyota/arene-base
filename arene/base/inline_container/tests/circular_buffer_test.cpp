// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/circular_buffer.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/inline_container/testing/deque.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

namespace {

using counting_wrapper = ::testing::counting_wrapper<int>;

////////////////////////////
// counting_wrapper tests //
////////////////////////////

/// @test When a `circular_buffer` is already full and a new element is pushed in, an existing element is destroyed
TEST(CircularBufferCountingWrapper, Overwrites) {
  counting_wrapper const source;
  counting_wrapper::count = {};
  {
    arene::base::circular_buffer<counting_wrapper, 3U> cbuf;
    cbuf.push_back(source);
    cbuf.push_back(source);
    cbuf.push_back(source);
    EXPECT_EQ(cbuf.size(), 3);
    EXPECT_EQ(counting_wrapper::count.copy_ctor, 3);
    EXPECT_EQ(counting_wrapper::count.dtor, 0);

    cbuf.push_back(source);
    EXPECT_EQ(cbuf.size(), 3);
    EXPECT_EQ(counting_wrapper::count.copy_ctor, 4);
    EXPECT_EQ(counting_wrapper::count.dtor, 1);
  }
  EXPECT_EQ(counting_wrapper::count.dtor, 4);
}

}  // namespace

using arene::base::literals::operator""_asv;

using not_default_constructible =
    ::testing::configurable_value<int, ::testing::throws_on::nothing, ::testing::disable::default_construct>;

using non_copyable = ::testing::configurable_value<
    int,
    ::testing::throws_on::nothing,
    ::testing::disable::copy_construct | ::testing::disable::copy_assign>;

using noexcept_false = ::testing::configurable_value<std::size_t, ::testing::throws_on::everything>;

template <>
constexpr auto arene::base::testing::test_value_array<arene::base::string_view> =
    arene::base::to_array({"red"_asv, "blue"_asv, "green"_asv, "yellow"_asv});

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects) This test-only customization point must be (const) static.
template <>
auto const arene::base::testing::test_value_array<arene::base::inline_vector<arene::base::string_view, 3>> =
    arene::base::to_array<arene::base::inline_vector<arene::base::string_view, 3>>(
        {{"1"_asv}, {"1"_asv, "2"_asv}, {"1"_asv, "2"_asv, "3"_asv}}
    );

using TestTypes = ::testing::Types<
    arene::base::circular_buffer<int, 10>,
    arene::base::circular_buffer<double, 8>,
    arene::base::circular_buffer<arene::base::string_view, 5>,
    arene::base::circular_buffer<non_copyable, 10>,
    arene::base::circular_buffer<not_default_constructible, 10>,
    arene::base::circular_buffer<noexcept_false, 15>,
    arene::base::circular_buffer<int, 2>,
    arene::base::circular_buffer<double, 1>,
    arene::base::circular_buffer<arene::base::inline_vector<arene::base::string_view, 3>, 3>>;

ARENE_INSTANTIATE_TESTS(Internal, InlineDeque, TestTypes);
