// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/to_array.hpp"

#include <gtest/gtest.h>

#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

// 'using' is buggy with IWYU
// https://github.com/include-what-you-use/include-what-you-use/issues/1539
//
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"

namespace {

template <class T>
class MdspanDetailToArrayTest : public testing::Test {};

// NOLINTBEGIN(google-runtime-int)
using IndexTypes = ::testing::Types<  //
    unsigned char,
    unsigned short,
    unsigned int,
    unsigned long,
    unsigned long long,
    signed char,
    short,
    int,
    long,
    long long>;
// NOLINTEND(google-runtime-int)

TYPED_TEST_SUITE(MdspanDetailToArrayTest, IndexTypes, );

/// @test the array values match the extent values
CONSTEXPR_TYPED_TEST(MdspanDetailToArrayTest, ValuesAreEqual) {
  using extents_type = arene::base::dextents<TypeParam, 5>;

  auto const ext = extents_type{
      TypeParam{0},
      TypeParam{1},
      TypeParam{2},
      TypeParam{3},
      TypeParam{4},
  };

  auto const arr = arene::base::mdspan_detail::to_array(ext);

  CONSTEXPR_ASSERT(arr.size() == ext.rank);
  CONSTEXPR_ASSERT(  //
      std::is_same<
          arene::base::remove_cvref_t<decltype(arr[0])>,
          arene::base::remove_cvref_t<decltype(ext.extent(0))>  //
          >::value
  );

  for (auto dim : arene::base::sequential_values<std::size_t, extents_type::rank>) {
    CONSTEXPR_ASSERT(arr[dim] == ext.extent(dim));
  }
}

/// @test array is empty if extents is rank 0
CONSTEXPR_TYPED_TEST(MdspanDetailToArrayTest, EmptyArrayIfRankZero) {
  using extents_type = arene::base::extents<TypeParam>;

  auto const ext = extents_type{};

  auto const arr = arene::base::mdspan_detail::to_array(ext);

  CONSTEXPR_ASSERT(arr.size() == ext.rank);
  for (auto dim : arene::base::sequential_values<std::size_t, extents_type::rank>) {
    CONSTEXPR_ASSERT(arr[dim] == ext.extent(dim));
  }
}

}  // namespace
