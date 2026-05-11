// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/is_valid_submdspan_slice_type_for.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace {
using ::arene::base::extent_slice;
using ::arene::base::full_extent_t;
using ::arene::base::mdspan_detail::is_valid_submdspan_slice_type_for_v;

// NOLINTBEGIN(readability-identifier-length)
template <std::size_t N>
using zc = std::integral_constant<std::size_t, N>;
// NOLINTEND(readability-identifier-length)

/// @test @c is_valid_submdspan_slice_type_for_v returns @c false for non-canonical slice types
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, NonCanonicalSliceTypes) {
  struct not_a_slice {};

  EXPECT_FALSE((is_valid_submdspan_slice_type_for_v<std::nullptr_t, 3>));
  EXPECT_FALSE((is_valid_submdspan_slice_type_for_v<not_a_slice, 3>));
  EXPECT_FALSE((is_valid_submdspan_slice_type_for_v<void*, 3>));
}

/// @test @c is_valid_submdspan_slice_type_for_v returns @c true for @c full_extent_t
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, FullExtent) {
  EXPECT_TRUE((is_valid_submdspan_slice_type_for_v<full_extent_t, 0>));
  EXPECT_TRUE((is_valid_submdspan_slice_type_for_v<full_extent_t, 3>));
  EXPECT_TRUE((is_valid_submdspan_slice_type_for_v<full_extent_t, arene::base::dynamic_extent>));
}

/// @test @c is_valid_submdspan_slice_type_for_v returns @c true for plain integral types
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, PlainIntegralTypes) {
  EXPECT_TRUE((is_valid_submdspan_slice_type_for_v<int, 3>));
  EXPECT_TRUE((is_valid_submdspan_slice_type_for_v<std::size_t, 3>));
  EXPECT_TRUE((is_valid_submdspan_slice_type_for_v<unsigned char, 3>));
  EXPECT_TRUE((is_valid_submdspan_slice_type_for_v<int, arene::base::dynamic_extent>));
}

/// @test an @c extent_slice with an integral-constant-like offset where the
/// _submdspan-slice-range_ *could* be [offset, offset) is valid
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, OffsetAtUpperBound) {
  EXPECT_TRUE((                                           //
      is_valid_submdspan_slice_type_for_v<                //
          extent_slice<zc<3>, std::size_t, std::size_t>,  //
          3                                               //
          >                                               //
  ));

  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<3>, zc<0>, std::size_t>,  //
          3                                         //
          >                                         //
  ));

  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<3>, std::size_t, zc<0>>,  //
          3                                         //
          >                                         //
  ));

  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<3>, std::size_t, zc<1>>,  //
          3                                         //
          >                                         //
  ));

  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<3>, std::size_t, zc<2>>,  //
          3                                         //
          >                                         //
  ));
}

/// @test an @c extent_slice with a runtime offset and a zero valued
/// integral-constant-like extent where the _submdspan-slice_range_ is [offset,
/// offset) is valid
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, RuntimeOffsetZeroExtent) {
  EXPECT_TRUE((                                           //
      is_valid_submdspan_slice_type_for_v<                //
          extent_slice<std::size_t, zc<0>, std::size_t>,  //
          3                                               //
          >                                               //
  ));

  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<0>, zc<1>>,  //
          3                                         //
          >                                         //
  ));
}

/// @test an @c extent_slice with a runtime offset, a positive
/// integral-constant-like extent, and a runtime stride is valid if the
/// _submdspan-slice-range_ is potentially valid
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, RuntimeOffsetPositiveExtentRuntimeStride) {
  // Since extent is positive, stride *must* be positive and the smallest
  // possible stride value is 1.

  // _submdspan-slice-range_: [offset, offset + 1 + (0) * stride)
  // [offset, offset + 1) can be bounded by [0, 3)
  EXPECT_TRUE((                                           //
      is_valid_submdspan_slice_type_for_v<                //
          extent_slice<std::size_t, zc<1>, std::size_t>,  //
          3                                               //
          >                                               //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + (2 - 1) * stride)
  // [offset, offset + 1 + stride) can be bounded by [0, 3)
  // e.g. [0, 0 + 1 + 1) -> [0, 2)
  EXPECT_TRUE((                                           //
      is_valid_submdspan_slice_type_for_v<                //
          extent_slice<std::size_t, zc<2>, std::size_t>,  //
          3                                               //
          >                                               //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + (3 - 1) * stride)
  // [offset, offset + 1 + 2 * stride) can be bounded by [0, 3)
  // e.g. [0, 0 + 1 + 2 * 1) -> [0, 3)
  EXPECT_TRUE((                                           //
      is_valid_submdspan_slice_type_for_v<                //
          extent_slice<std::size_t, zc<3>, std::size_t>,  //
          3                                               //
          >                                               //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + (4 - 1) * stride)
  // [offset, offset + 1 + 3 * stride) cannot be bounded by [0, 3)
  // e.g. [0, 0 + 1 + 3 * 1) -> [0, 4)
  EXPECT_FALSE((                                          //
      is_valid_submdspan_slice_type_for_v<                //
          extent_slice<std::size_t, zc<4>, std::size_t>,  //
          3                                               //
          >                                               //
  ));
}

/// @test an @c extent_slice with a non-negative offset, a positive
/// integral-constant-like extent, and a runtime stride is valid if the
/// _submdspan-slice-range_ is potentially valid
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, NonNegativeOffsetPositiveExtentRuntimeStride) {
  // _submdspan-slice-range_: [0, 1 + 0 * stride)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<0>, zc<1>, std::size_t>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [0, 1 + 1 * stride)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<0>, zc<2>, std::size_t>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [0, 1 + 2 * stride)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<0>, zc<3>, std::size_t>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [0, 1 + 3 * stride)
  // cannot be bounded by [0, 3)
  EXPECT_FALSE((                                    //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<0>, zc<4>, std::size_t>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [1, 1 + 1 + 2 * stride)
  // cannot be bounded by [0, 3)
  // e.g. stride = 1 -> [1, 4)
  EXPECT_FALSE((                                    //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<1>, zc<3>, std::size_t>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [1, 1 + 1 + 1 * stride)
  // can be bounded by [0, 3)
  // e.g. stride = 1 -> [1, 3)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<1>, zc<2>, std::size_t>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [2, 2 + 1 + 1 * stride)
  // cannot be bounded by [0, 3)
  // e.g. stride = 1 -> [2, 4)
  EXPECT_FALSE((                                    //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<zc<2>, zc<2>, std::size_t>,  //
          3                                         //
          >                                         //
  ));
}

/// @test an @c extent_slice with a runtime offset, a positive
/// integral-constant-like extent, and a positive stride is valid if the
/// _submdspan-slice-range_ is potentially valid
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, RuntimeOffsetPositiveExtentPositiveStride) {
  // _submdspan-slice-range_: [offset, offset + 1 + 0 * 1)
  // can be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 1)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<1>, zc<1>>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + 1 * 1)
  // can be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 2)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<2>, zc<1>>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + 2 * 1)
  // can be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 3)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<3>, zc<1>>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + 0 * 2)
  // can be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 1)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<1>, zc<2>>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + 1 * 2)
  // can be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 3)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<2>, zc<2>>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + 2 * 2)
  // cannot be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 5)
  EXPECT_FALSE((                                    //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<3>, zc<2>>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + 0 * 3)
  // cannot be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 1)
  EXPECT_TRUE((                                     //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<1>, zc<3>>,  //
          3                                         //
          >                                         //
  ));

  // _submdspan-slice-range_: [offset, offset + 1 + 1 * 3)
  // cannot be bounded by [0, 3)
  // e.g. offset = 0 -> [0, 4)
  EXPECT_FALSE((                                    //
      is_valid_submdspan_slice_type_for_v<          //
          extent_slice<std::size_t, zc<2>, zc<3>>,  //
          3                                         //
          >                                         //
  ));
}

/// @test an @c extent_slice with a non-negative offset, a positive
/// integral-constant-like extent, and a positive stride is valid if the
/// _submdspan-slice-range_ is potentially valid
TEST(MdspanDetailIsValidSubmdspanSliceTypeFor, NonNegativeOffsetPositiveExtentPositiveStride) {
  // _submdspan-slice-range_: [0, 0 + 1 + 0 * 1) -> [0, 1)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<1>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 0 * 2) -> [0, 1)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<1>, zc<2>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 0 * 3) -> [0, 1)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<1>, zc<3>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 0 * std::size_t(-1)) -> [0, 1)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                                                          //
      is_valid_submdspan_slice_type_for_v<                               //
          extent_slice<zc<0>, zc<1>, zc<static_cast<std::size_t>(-1)>>,  //
          3                                                              //
          >                                                              //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 1 * 1) -> [0, 2)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<2>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 2 * 1) -> [0, 3)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<3>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 3 * 1) -> [0, 4)
  // cannot be bounded by [0, 3)
  EXPECT_FALSE((                              //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<4>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 1 * 2) -> [0, 3)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<2>, zc<2>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 1 * 3) -> [0, 4)
  // cannot be bounded by [0, 3)
  EXPECT_FALSE((                              //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<2>, zc<3>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [0, 0 + 1 + 2 * 2) -> [0, 5)
  // cannot be bounded by [0, 3)
  EXPECT_FALSE((                              //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<0>, zc<3>, zc<2>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [1, 1 + 1 + 0 * 1) -> [1, 2)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<1>, zc<1>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [1, 1 + 1 + 0 * 2) -> [1, 2)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<1>, zc<1>, zc<2>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [1, 1 + 1 + 1 * 1) -> [1, 3)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<1>, zc<2>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [1, 1 + 1 + 1 * 2) -> [1, 4)
  // cannot be bounded by [0, 3)
  EXPECT_FALSE((                              //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<1>, zc<2>, zc<2>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [1, 1 + 1 + 2 * 1) -> [1, 4)
  // cannot be bounded by [0, 3)
  EXPECT_FALSE((                              //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<1>, zc<3>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [2, 2 + 1 + 0 * 1) -> [2, 3)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<2>, zc<1>, zc<1>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [2, 2 + 1 + 0 * 2) -> [2, 3)
  // can be bounded by [0, 3)
  EXPECT_TRUE((                               //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<2>, zc<1>, zc<2>>,  //
          3                                   //
          >                                   //
  ));

  // _submdspan-slice-range_: [2, 2 + 1 + 1 * 1) -> [2, 4)
  // cannot be bounded by [0, 3)
  EXPECT_FALSE((                              //
      is_valid_submdspan_slice_type_for_v<    //
          extent_slice<zc<2>, zc<2>, zc<1>>,  //
          3                                   //
          >                                   //
  ));
}

}  // namespace
