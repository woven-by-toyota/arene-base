// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file helpers.hpp
/// @brief files containing helpers used across span_test
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_TESTS_SPAN_TEST_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_TESTS_SPAN_TEST_HELPERS_HPP_

#include <gtest/gtest.h>

// We specifically keep thease headers since Bazel will specify the underlying
// span value type *PER* test
// IWYU pragma: begin_keep
#include "arene/base/byte/byte.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
// IWYU pragma: end_keep

#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/flat_map.hpp"

namespace arene {
namespace base {
namespace tests {
namespace span_test {

using ::arene::base::span;
using ::arene::base::type_lists::concat_t;
using ::arene::base::type_lists::concat_unique_t;
using ::arene::base::type_lists::flat_map_t;

// These aliases are used by the Bazel BUILD file
using uchar = unsigned char;
using uint = unsigned int;

constexpr std::size_t max_data_size{10U};

template <typename T>
using make_static_extent_spans_tl = ::testing::Types<span<T, 1>, span<T, max_data_size / 2>, span<T, max_data_size>>;

template <typename T>
using make_dynamic_extent_spans_tl = ::testing::Types<span<T>>;

template <typename T>
using make_empty_spans_tl = ::testing::Types<span<T>, span<T, 0>>;

template <typename T>
using add_const_tl = ::testing::Types<T const>;

// 'TEST_VALUE_TYPE' is defined by the Bazel BUILD file
using mutable_constexpr_types_tl = ::testing::Types<TEST_VALUE_TYPE>;

using const_constexpr_types_tl = flat_map_t<mutable_constexpr_types_tl, add_const_tl>;

using mutable_static_extent_spans = flat_map_t<mutable_constexpr_types_tl, make_static_extent_spans_tl>;

using const_static_extent_spans = flat_map_t<const_constexpr_types_tl, make_static_extent_spans_tl>;

using mutable_dynamic_extent_spans = flat_map_t<mutable_constexpr_types_tl, make_dynamic_extent_spans_tl>;

using const_dynamic_extent_spans = flat_map_t<const_constexpr_types_tl, make_dynamic_extent_spans_tl>;

using mutable_empty_spans = flat_map_t<mutable_constexpr_types_tl, make_empty_spans_tl>;

using const_empty_spans = flat_map_t<const_constexpr_types_tl, make_empty_spans_tl>;

using all_dynamic_extent_spans = concat_t<mutable_dynamic_extent_spans, const_dynamic_extent_spans>;

using all_static_extent_spans = concat_t<mutable_static_extent_spans, const_static_extent_spans>;

using non_empty_spans = concat_t<all_dynamic_extent_spans, all_static_extent_spans>;

using all_spans = concat_unique_t<non_empty_spans, mutable_empty_spans, const_empty_spans>;

// NOLINTBEGIN(hicpp-avoid-c-arrays) Explicitly need c-array storage.
template <typename T>
struct make_value {
  constexpr auto operator()(int value) const noexcept -> T { return static_cast<T>(value); }
};

template <>
struct make_value<::arene::base::byte> {
  constexpr auto operator()(int value) const noexcept -> ::arene::base::byte { return ::arene::base::to_byte(value); }
};

template <typename T>
class Data {
 public:
  constexpr auto begin() noexcept { return std::begin(data_); }
  constexpr auto end() noexcept { return std::end(data_); }
  constexpr auto cbegin() const noexcept { return std::cbegin(data_); }
  constexpr auto cend() const noexcept { return std::cend(data_); }
  static constexpr std::integral_constant<std::size_t, max_data_size> size{};
  constexpr auto as_c_array() const noexcept -> T const (&)[size()] { return data_; }
  constexpr auto as_c_array() noexcept -> T (&)[size()] { return data_; }

 private:
  T data_[size()]{
      make_value<T>{}(1),
      make_value<T>{}(2),
      make_value<T>{}(3),
      make_value<T>{}(4),
      make_value<T>{}(5),
      make_value<T>{}(6),
      make_value<T>{}(7),
      make_value<T>{}(8),
      make_value<T>{}(9),
      make_value<T>{}(10)
  };
};
// NOLINTEND(hicpp-avoid-c-arrays) Need constexpr compatible contiguous storage without relying on base::array.

template <typename T>
constexpr std::integral_constant<std::size_t, max_data_size> Data<T>::size;

template <typename T>
class SpanTest;

template <typename T, std::size_t Extent>
class SpanTest<span<T, Extent>> : public ::testing::Test {
 public:
  Data<T> backing_data{};
  using template_type = T;
  // this has to be a function otherwise you get linker errors under the sanitizers.
  static constexpr auto template_extent() -> std::size_t { return Extent; }
};

template <typename T>
class SubspanTest : public span_test::SpanTest<T> {
 public:
  using typename span_test::SpanTest<T>::template_type;
  using span_test::SpanTest<T>::template_extent;

  static constexpr std::size_t zero_offset = 0U;
  static constexpr std::size_t max_offset = std::min(span_test::max_data_size, template_extent());
  static constexpr std::size_t between_zero_and_max_offset = std::min<std::size_t>(1U, max_offset);
};

template <typename T>
constexpr std::size_t SubspanTest<T>::zero_offset;

template <typename T>
constexpr std::size_t SubspanTest<T>::max_offset;

template <typename T>
constexpr std::size_t SubspanTest<T>::between_zero_and_max_offset;

}  // namespace span_test
}  // namespace tests
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_TESTS_SPAN_TEST_HELPERS_HPP_
