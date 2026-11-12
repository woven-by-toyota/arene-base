// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/equal_elements.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/negate.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/detail/tuple_transform.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {
/// @test It should not be possible to invoke @c equal_elements with arguments that are not instantiations of @c mdspan
TEST(EqualElements, NotInvocableWithNonMdspan) {
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), int, int>));
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::linalg::equal_elements),
                arene::base::array<int, 5>,
                arene::base::array<int, 5>>));
}

/// @test It should be possible to invoke @c equal_elements with arguments that are identical instantiations of @c
/// mdspan
TEST(EqualElements, InvocableWithIdenticalMdspan) {
  using span1 = arene::base::mdspan<std::uint32_t const, arene::base::extents<std::uint32_t, 5, 7, 3>>;
  using span2 = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span1, span1>));
  ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span2, span2>));
}

/// @test It should not be possible to invoke @c equal_elements with arguments that are instantiations of @c mdspan that
/// do not have compatible extents
TEST(EqualElements, NotInvocableWithMismatchedMdspanExtents) {
  using span1 = arene::base::mdspan<std::uint32_t const, arene::base::extents<std::uint32_t, 5, 7, 3>>;
  using span2 = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span3 = arene::base::mdspan<std::uint32_t const, arene::base::extents<std::uint32_t, 5, 7>>;
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span1, span2>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span2, span1>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span1, span3>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span2, span3>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span3, span2>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span3, span1>));
}

/// @test It should not be possible to invoke @c equal_elements with arguments that are instantiations of @c mdspan that
/// do not have comparable types
TEST(EqualElements, NotInvocableWithNonComparableMdspanElementType) {
  struct other {};
  using span1 = arene::base::mdspan<std::uint32_t const, arene::base::extents<std::uint32_t, 5, 7, 3>>;
  using span2 = arene::base::mdspan<other const, arene::base::extents<std::uint32_t, 5, 7, 3>>;
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span1, span2>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span2, span1>));
}

/// @test If the elements match when calling @c equal_elements with @c mdspan instantiations with the same contents, the
/// result should be @c true
TEST(EqualElements, ReturnsTrueIfElementsMatchFixedExtents) {
  using span1_t = arene::base::mdspan<std::uint32_t const, arene::base::extents<std::uint32_t, 5, 7, 3>>;

  auto const data1 = arene::base::sequential_values<std::uint32_t, static_cast<std::size_t>(5U * 7U * 3U)>;
  auto const data2{data1};

  span1_t const span1{data1.data()};
  span1_t const span2{data2.data()};

  ASSERT_EQ(data1.size(), span1.size());
  ASSERT_EQ(data2.size(), span2.size());

  ASSERT_TRUE(arene::base::linalg::equal_elements(span1, span2));
}

/// @test If the elements do not match when calling @c equal_elements with @c mdspan instantiations with different
/// contents, the result should be @c false
TEST(EqualElements, ReturnsFalseIfElementsDoNotMatchFixedExtents) {
  using span1_t = arene::base::mdspan<std::uint32_t const, arene::base::extents<std::uint32_t, 5, 7, 3>>;

  auto const data1 = arene::base::sequential_values<std::uint32_t, static_cast<std::size_t>(5U * 7U * 3U)>;

  auto const data2 = [&data1] {
    auto data = data1;
    data[1] = 999U;
    return data;
  }();

  span1_t const span1{data1.data()};
  span1_t const span2{data2.data()};

  ASSERT_EQ(data1.size(), span1.size());
  ASSERT_EQ(data2.size(), span2.size());

  ASSERT_FALSE(arene::base::linalg::equal_elements(span1, span2));
}

class as_float_with_offset {
 public:
  constexpr auto operator()(int index) const noexcept -> float { return static_cast<float>(index) + 0.42F; }
};

/// @test If the elements match when calling @c equal_elements with @c mdspan instantiations with the same contents, the
/// result should be @c true
CONSTEXPR_TEST(EqualElements, ReturnsTrueIfElementsMatchDynamicExtents) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  auto const data0 = arene::base::sequential_values<int, static_cast<std::size_t>(5U * 6U * 3U)>;
  auto const data1 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_float_with_offset{});
  auto const data2{data1};

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 3};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data2.data(), extents};

  CONSTEXPR_ASSERT_EQ(data1.size(), span1.size());
  CONSTEXPR_ASSERT_EQ(data2.size(), span2.size());
  CONSTEXPR_ASSERT_EQ(span1.extents(), span2.extents());

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(span1, span2));
}

/// @test If the elements do not match when calling @c equal_elements with @c mdspan instantiations with different
/// contents, the result should be @c false
CONSTEXPR_TEST(EqualElements, ReturnsFalseIfElementsDoNotMatchDynamicExtents) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  auto const data0 = arene::base::sequential_values<int, static_cast<std::size_t>(5U * 6U * 3U)>;
  auto const data1 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_float_with_offset{});
  auto data2 = data1;
  data2[83] = 999.42F;

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 3};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data2.data(), extents};

  CONSTEXPR_ASSERT_EQ(data1.size(), span1.size());
  CONSTEXPR_ASSERT_EQ(data2.size(), span2.size());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::equal_elements(span1, span2));
}

/// @test If the elements match when calling @c equal_elements with @c mdspan instantiations with no contents, the
/// result should be @c true
TEST(EqualElements, ReturnsTrueForEmptySpans) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  arene::base::array<float, 0> const data1{};

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 0};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data1.data(), extents};

  ASSERT_EQ(data1.size(), span1.size());
  ASSERT_EQ(data1.size(), span2.size());

  ASSERT_TRUE(arene::base::linalg::equal_elements(span1, span2));
}

/// @test If the elements match when calling @c equal_elements with @c mdspan instantiations with no contents, the
/// result should be @c true
TEST(EqualElements, ReturnsTrueForEmptySpansDifferentTypes) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      std::int32_t const,
      arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  arene::base::array<float, 0> const data1{};
  arene::base::array<std::int32_t, 0> const data2{};

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 0};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data2.data(), extents};

  ASSERT_EQ(data1.size(), span1.size());
  ASSERT_EQ(data2.size(), span2.size());

  ASSERT_TRUE(arene::base::linalg::equal_elements(span1, span2));
}

class is_equal_negative {
 public:
  template <typename Lhs, typename Rhs>
  constexpr auto operator()(Lhs&& lhs, Rhs&& rhs) const {
    return std::equal_to<>{}(lhs, -rhs);
  }
};

/// @test If the elements match with a custom comparator when calling @c equal_elements with @c mdspan instantiations
/// with the same contents, the result should be @c true
CONSTEXPR_TEST(EqualElements, ReturnsTrueIfElementsMatchDynamicExtentsWithCustomComparator) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  auto const data0 = arene::base::sequential_values<int, static_cast<std::size_t>(5U * 6U * 3U)>;
  auto const data1 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_float_with_offset{});
  auto const data2 = arene::base::tuple_detail::tuple_transform_to_array<>(data1, std::negate<>{});

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 3};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data2.data(), extents};

  CONSTEXPR_ASSERT_EQ(data1.size(), span1.size());
  CONSTEXPR_ASSERT_EQ(data2.size(), span2.size());
  CONSTEXPR_ASSERT_EQ(span1.extents(), span2.extents());

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(span1, span2, is_equal_negative{}));
}

/// @test If the elements do not match with a custom comparator when calling @c equal_elements with @c mdspan
/// instantiations with different contents, the result should be @c false
CONSTEXPR_TEST(EqualElements, ReturnsFalseIfElementsDoNotMatchDynamicExtentsWithCustomComparator) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  auto const data0 = arene::base::sequential_values<int, static_cast<std::size_t>(5U * 6U * 3U)>;
  auto const data1 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_float_with_offset{});
  auto const data2{data1};

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 3};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data2.data(), extents};

  CONSTEXPR_ASSERT_EQ(data1.size(), span1.size());
  CONSTEXPR_ASSERT_EQ(data2.size(), span2.size());

  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::equal_elements(span1, span2, is_equal_negative{}));
}

class throwing_equality {
 public:
  template <typename Lhs, typename Rhs>
  auto operator()(Lhs&& lhs, Rhs&& rhs) const -> bool {
    return std::equal_to<>{}(lhs, rhs);
  }
};

class nothrow_equality {
 public:
  template <typename Lhs, typename Rhs>
  auto operator()(Lhs&& lhs, Rhs&& rhs) const noexcept -> bool {
    return std::equal_to<>{}(lhs, rhs);
  }
};

template <typename ElementType>
class throwing_accessor {
 public:
  using offset_policy = throwing_accessor;
  using element_type = ElementType;
  using reference = ElementType&;
  using data_handle_type = ElementType*;

  constexpr throwing_accessor() noexcept = default;
  template <class OtherElementType>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr throwing_accessor(throwing_accessor<OtherElementType>) noexcept {}
  constexpr auto access(data_handle_type data, std::size_t index) const -> reference { return data[index]; }
  constexpr auto offset(data_handle_type data, std::size_t index) const noexcept -> data_handle_type {
    return data + index;
  }
};

/// @test If everything is @c noexcept , @c equal_elements is too. If any of the required operations are not @c noexcept
/// , @c equal_elements is not either.
TEST(EqualElements, NoexceptMatchesOperations) {
  using span1_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using span2_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>,
      arene::base::layout_right,
      throwing_accessor<float const>>;

  ASSERT_TRUE((arene::base::is_nothrow_invocable_v<decltype(arene::base::linalg::equal_elements), span1_t, span1_t>));
  ASSERT_FALSE(
      (arene::base::
           is_nothrow_invocable_v<decltype(arene::base::linalg::equal_elements), span1_t, span1_t, throwing_equality>)
  );
  ASSERT_TRUE(
      (arene::base::
           is_nothrow_invocable_v<decltype(arene::base::linalg::equal_elements), span1_t, span1_t, nothrow_equality>)
  );
  ASSERT_FALSE(
      (arene::base::
           is_nothrow_invocable_v<decltype(arene::base::linalg::equal_elements), span2_t, span2_t, nothrow_equality>)
  );
  ASSERT_TRUE(
      (arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span2_t, span2_t, nothrow_equality>)
  );
  ASSERT_FALSE(
      (arene::base::
           is_nothrow_invocable_v<decltype(arene::base::linalg::equal_elements), span1_t, span2_t, nothrow_equality>)
  );
  ASSERT_TRUE(
      (arene::base::is_invocable_v<decltype(arene::base::linalg::equal_elements), span1_t, span2_t, nothrow_equality>)
  );
}

struct wrapper1 {
  std::uint32_t value;
};
struct wrapper2 {
  std::uint32_t value;
};

constexpr auto operator==(wrapper1 lhs, wrapper2 rhs) noexcept -> bool { return lhs.value == rhs.value; }

template <typename Target>
class as_wrapper {
 public:
  template <typename Source>
  constexpr auto operator()(Source source) const noexcept -> Target {
    return Target{source};
  }
};

struct convertible_wrapper {
  std::uint32_t value;

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator wrapper1() const noexcept { return wrapper1{value}; }
};

/// @test If the elements match with a custom equality operator when calling @c equal_elements with @c mdspan
/// instantiations with the same contents, the result should be @c true
CONSTEXPR_TEST(EqualElements, ReturnsTrueIfElementsMatchWithCustomEquality) {
  using span1_t = arene::base::mdspan<wrapper1 const, arene::base::extents<std::uint32_t, 5U, 6U, 3U>>;
  using span2_t = arene::base::mdspan<wrapper2 const, arene::base::extents<std::uint32_t, 5U, 6U, 3U>>;

  auto const data0 = arene::base::sequential_values<std::uint32_t, static_cast<std::size_t>(5U * 6U * 3U)>;
  auto const data1 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_wrapper<wrapper1>{});
  auto const data2 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_wrapper<wrapper2>{});

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 3};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data2.data(), extents};

  CONSTEXPR_ASSERT_EQ(data1.size(), span1.size());
  CONSTEXPR_ASSERT_EQ(data2.size(), span2.size());
  CONSTEXPR_ASSERT_EQ(span1.extents(), span2.extents());

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(span1, span2));
}

/// @test If the elements match with a custom equality operator when calling @c equal_elements with @c mdspan
/// instantiations with the same contents, the result should be @c true
CONSTEXPR_TEST(EqualElements, ReturnsTrueIfElementsMatchWithConversion) {
  using span1_t = arene::base::mdspan<convertible_wrapper const, arene::base::extents<std::uint32_t, 5U, 6U, 3U>>;
  using span2_t = arene::base::mdspan<wrapper2 const, arene::base::extents<std::uint32_t, 5U, 6U, 3U>>;

  auto const data0 = arene::base::sequential_values<std::uint32_t, static_cast<std::size_t>(5U * 6U * 3U)>;
  auto const data1 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_wrapper<convertible_wrapper>{});
  auto const data2 = arene::base::tuple_detail::tuple_transform_to_array<>(data0, as_wrapper<wrapper2>{});

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 3};
  span1_t const span1{data1.data(), extents};
  span2_t const span2{data2.data(), extents};

  CONSTEXPR_ASSERT_EQ(data1.size(), span1.size());
  CONSTEXPR_ASSERT_EQ(data2.size(), span2.size());
  CONSTEXPR_ASSERT_EQ(span1.extents(), span2.extents());

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(span1, span2));
}

}  // namespace
