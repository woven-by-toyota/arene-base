// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SLICE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SLICE_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/type_traits/is_integral_constant_like.hpp"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this class is POD"

/// @brief represents a set of @c extent regularly spaced integer indices
/// @tparam OffsetType type used to specify the offset
/// @tparam ExtentType type used to specify the extent
/// @tparam StrideType type used to specify the stride
///
/// <c> extent_slice<int>{1, 4, 3} </c> indicates the indices 1, 4,
/// 7, and 10. The slice contains 4 indices, starting from 1, with a stride of 3.
template <  //
    class OffsetType,
    class ExtentType = OffsetType,
    class StrideType = std::integral_constant<std::size_t, 1>>
class extent_slice {
 public:
  static_assert(  //
      std::is_integral<OffsetType>::value || is_integral_constant_like_v<OffsetType>,
      "'OffsetType' must be an integer type or model integral-constant-like"
  );
  static_assert(  //
      std::is_integral<ExtentType>::value || is_integral_constant_like_v<ExtentType>,
      "'ExtentType' must be an integer type or model integral-constant-like"
  );
  static_assert(  //
      std::is_integral<StrideType>::value || is_integral_constant_like_v<StrideType>,
      "'StrideType' must be an integer type or model integral-constant-like"
  );

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: these identifiers do not hide identifiers in
  // 'extent_slice'"
  /// @brief offset type
  using offset_type = OffsetType;
  /// @brief extent type
  using extent_type = ExtentType;
  /// @brief stride type
  using stride_type = StrideType;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief start of indices
  /// First index in this slice, i.e. offset from zero.
  /// @attention This value must be non-negative to represent a valid slice.
  offset_type offset{};
  /// @brief extent of indices
  /// Number of indices in this slice.
  /// @attention This value must be non-negative to represent a valid slice.
  extent_type extent{};
  /// @brief stride of indices
  /// Stride (or step) between indices in this slice.
  /// @attention This value must be positive to represent a valid slice.
  stride_type stride{};
};

// parasoft-end-suppress AUTOSAR-M11_0_1-a-2

// parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this class is POD"

/// @brief represents a slice of integer indices given the first and last
/// @tparam FirstType type used to specify the first index
/// @tparam LastType type used to specify the last index
/// @tparam StrideType type used to specify the stride
///
/// <c> range_slice<int>{1, 11, 3} </c> indicates the indices 1, 4,
/// 7, and 10. Indices are selected from the half-open interval [1, 11) with a
/// stride of 3.
template <  //
    class FirstType,
    class LastType = FirstType,
    class StrideType = std::integral_constant<std::size_t, 1>>
class range_slice {
 public:
  static_assert(  //
      std::is_integral<FirstType>::value || is_integral_constant_like_v<FirstType>,
      "'FirstType' must be an integer type or model integral-constant-like"
  );
  static_assert(  //
      std::is_integral<LastType>::value || is_integral_constant_like_v<LastType>,
      "'LastType' must be an integer type or model integral-constant-like"
  );
  static_assert(  //
      std::is_integral<StrideType>::value || is_integral_constant_like_v<StrideType>,
      "'StrideType' must be an integer type or model integral-constant-like"
  );

  /// @brief first index type
  using first_type = FirstType;
  /// @brief last index type
  using last_type = LastType;
  /// @brief stride type
  using stride_type = StrideType;

  /// @brief start index
  /// First index (inclusive) in this slice.
  /// @attention This value must be non-negative to represent a valid slice.
  first_type first{};
  /// @brief last index
  /// Last index (exclusive) in this slice.
  last_type last{};
  /// @brief stride of indices
  /// Stride (or step) between indices in this slice.
  /// @attention This value must be positive to represent a valid slice.
  stride_type stride{};
};

// parasoft-end-suppress AUTOSAR-M11_0_1-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SLICE_HPP_
