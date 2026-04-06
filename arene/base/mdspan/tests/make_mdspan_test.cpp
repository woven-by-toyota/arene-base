// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/make_mdspan.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {
enum class some_enum : std::uint8_t;
struct some_class;

template <class Type, Type Value>
struct integral_constant_like {
  static constexpr auto value = Value;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator Type() const noexcept { return value; }
};

template <typename T, typename TypeList, typename = arene::base::constraints<>>
extern constexpr bool can_call_make_mdspan_impl = false;

template <typename T, typename... Args>
extern constexpr bool can_call_make_mdspan_impl<
    T,
    arene::base::type_list<Args...>,
    arene::base::constraints<decltype(arene::base::make_mdspan(std::declval<T>(), std::declval<Args>()...))>> = true;

template <typename T, typename... Args>
extern constexpr bool can_call_make_mdspan = can_call_make_mdspan_impl<T, arene::base::type_list<Args...>>;

struct some_class {};

enum class some_enum : std::uint8_t { enumerator };

using make_mdspan_types = ::testing::Types<
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t,
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t,
    float,
    double,
    some_class,
    some_enum,
    some_class*>;

template <typename Mdspan>
class MakeMdspanTestSuite : public testing::Test {};

TYPED_TEST_SUITE(MakeMdspanTestSuite, make_mdspan_types, );

/// @test The @c make_mdspan function handles C arrays
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, CArrayUsesArrayExtent) {
  constexpr std::size_t count = 42;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam values[count] = {};

  auto span = arene::base::make_mdspan(values);

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values)));

  ::testing::
      StaticAssertTypeEq<decltype(span), arene::base::mdspan<TypeParam, arene::base::extents<std::size_t, count>>>();

  CONSTEXPR_ASSERT_EQ(span.size(), count);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), &values[0]);

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  CONSTEXPR_ASSERT(can_call_make_mdspan<TypeParam(&)[count]>);
}

/// @test The @c make_mdspan function does not accept multi-dimensional arrays
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, CArrayNoMultiDimensionalArrays) {
  constexpr std::size_t count = 42;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  CONSTEXPR_ASSERT_FALSE(can_call_make_mdspan<TypeParam(&)[count][count]>);
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  CONSTEXPR_ASSERT_FALSE(can_call_make_mdspan<TypeParam(&)[][count]>);
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  CONSTEXPR_ASSERT_FALSE(can_call_make_mdspan<TypeParam(&)[count][count][count]>);
}

/// @test The @c make_mdspan function creates a zero-dimensional @c mdspan if passed a plain pointer
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, ZeroDimensionalSpanFromPointer) {
  TypeParam value{};
  CONSTEXPR_ASSERT(can_call_make_mdspan<TypeParam*>);

  auto span = arene::base::make_mdspan(&value);

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(&value)));

  ::testing::StaticAssertTypeEq<decltype(span), arene::base::mdspan<TypeParam, arene::base::extents<std::size_t>>>();

  CONSTEXPR_ASSERT_EQ(span.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), &value);
}

/// @test The @c make_mdspan function creates a one-dimensional @c mdspan from a plain pointer and a size
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, OneDimensionalSpanFromPointerAndSize) {
  constexpr std::size_t count = 42;
  arene::base::array<TypeParam, count> values{};
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, std::size_t>));

  auto span = arene::base::make_mdspan(values.data(), count);

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), count)));

  ::testing::StaticAssertTypeEq<
      decltype(span),
      arene::base::mdspan<TypeParam, arene::base::extents<std::size_t, arene::base::dynamic_extent>>>();

  CONSTEXPR_ASSERT_EQ(span.size(), count);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span.extents().extent(0), count);
}

/// @test The @c make_mdspan function creates a two-dimensional @c mdspan from a plain pointer and two sizes
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, TwoDimensionalSpanFromPointerAndSizes) {
  constexpr std::size_t count = 42;
  constexpr std::size_t count2 = 99;
  arene::base::array<TypeParam, count * count2> values{};
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, std::size_t, std::size_t>));

  auto span = arene::base::make_mdspan(values.data(), count, count2);

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), count, count2)));

  ::testing::StaticAssertTypeEq<
      decltype(span),
      arene::base::mdspan<
          TypeParam,
          arene::base::extents<std::size_t, arene::base::dynamic_extent, arene::base::dynamic_extent>>>();

  CONSTEXPR_ASSERT_EQ(span.size(), count * count2);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span.extents().extent(1), count2);
}

/// @test The @c make_mdspan function creates a five-dimensional @c mdspan from a plain pointer and five sizes, some of
/// which are <c>std::integral_constant</c>s and others which are integral-constant-like
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, FiveDimensionalSpanFromPointerAndSizes) {
  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<TypeParam, count * count2 * count3 * count4 * count5> values{};
  CONSTEXPR_ASSERT((can_call_make_mdspan<
                    TypeParam*,
                    std::size_t,
                    std::size_t,
                    std::integral_constant<std::uint8_t, count3>,
                    integral_constant_like<std::int32_t, count4>,
                    std::size_t>));

  auto span = arene::base::make_mdspan(
      values.data(),
      count,
      count2,
      std::integral_constant<std::uint8_t, count3>{},
      std::integral_constant<std::int32_t, count4>{},
      count5
  );

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(
      values.data(),
      count,
      count2,
      integral_constant_like<std::uint8_t, count3>{},
      std::integral_constant<std::int32_t, count4>{},
      count5
  )));

  ::testing::StaticAssertTypeEq<
      decltype(span),
      arene::base::mdspan<
          TypeParam,
          arene::base::extents<
              std::size_t,
              arene::base::dynamic_extent,
              arene::base::dynamic_extent,
              count3,
              count4,
              arene::base::dynamic_extent>>>();

  CONSTEXPR_ASSERT_EQ(span.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span.extents().extent(2), count3);
  CONSTEXPR_ASSERT_EQ(span.extents().extent(3), count4);
  CONSTEXPR_ASSERT_EQ(span.extents().extent(4), count5);
}

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a plain pointer and a fixed-size @c span
/// of extents
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, NDimensionalSpanFromPointerAndSpanOfExtents) {
  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<TypeParam, count * count2 * count3 * count4 * count5> values{};
  CONSTEXPR_ASSERT_FALSE((can_call_make_mdspan<TypeParam*, arene::base::span<std::size_t>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::span<std::size_t, 0>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::span<std::size_t, 1>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::span<std::uint8_t, 2>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::span<std::int32_t, 3>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::span<std::size_t, 5>>));

  arene::base::array<std::size_t, 5> const extents{count, count2, count3, count4, count5};

  auto span = arene::base::make_mdspan(values.data(), arene::base::span<std::size_t, 0>{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), arene::base::span<std::size_t, 0>{})));

  ::testing::StaticAssertTypeEq<decltype(span), arene::base::mdspan<TypeParam, arene::base::dextents<std::size_t, 0>>>(
  );

  CONSTEXPR_ASSERT_EQ(span.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), values.data());

  auto span2 = arene::base::make_mdspan(values.data(), arene::base::span<std::size_t const, 1>{extents.data(), 1});

  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(values.data(), arene::base::span<std::size_t const, 1>{extents.data(), 1}))
  );

  ::testing::StaticAssertTypeEq<decltype(span2), arene::base::mdspan<TypeParam, arene::base::dextents<std::size_t, 1>>>(
  );

  CONSTEXPR_ASSERT_EQ(span2.size(), count);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span2.extents().extent(0), count);

  auto span3 = arene::base::make_mdspan(values.data(), arene::base::span<std::size_t const, 5>{extents.data(), 5});

  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(values.data(), arene::base::span<std::size_t const, 5>{extents.data(), 5}))
  );

  ::testing::StaticAssertTypeEq<decltype(span3), arene::base::mdspan<TypeParam, arene::base::dextents<std::size_t, 5>>>(
  );

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(2), count3);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(3), count4);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(4), count5);
}

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a plain pointer and an @c array of extents
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, NDimensionalSpanFromPointerAndArrayOfExtents) {
  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<TypeParam, count * count2 * count3 * count4 * count5> values{};
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::array<std::size_t, 0>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::array<std::size_t, 1>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::array<std::uint8_t, 2>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::array<std::int32_t, 3>>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, arene::base::array<std::size_t, 5>>));

  arene::base::array<std::size_t, 0> const extents{};

  auto span = arene::base::make_mdspan(values.data(), extents);

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents)));

  ::testing::StaticAssertTypeEq<decltype(span), arene::base::mdspan<TypeParam, arene::base::dextents<std::size_t, 0>>>(
  );

  CONSTEXPR_ASSERT_EQ(span.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), values.data());

  arene::base::array<std::size_t, 1> const extents2{count};
  auto span2 = arene::base::make_mdspan(values.data(), extents2);

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents2)));

  ::testing::StaticAssertTypeEq<decltype(span2), arene::base::mdspan<TypeParam, arene::base::dextents<std::size_t, 1>>>(
  );

  CONSTEXPR_ASSERT_EQ(span2.size(), count);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span2.extents().extent(0), count);

  arene::base::array<std::size_t, 5> const extents3{count, count2, count3, count4, count5};
  auto span3 = arene::base::make_mdspan(values.data(), extents3);
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents3)));

  ::testing::StaticAssertTypeEq<decltype(span3), arene::base::mdspan<TypeParam, arene::base::dextents<std::size_t, 5>>>(
  );

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(2), count3);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(3), count4);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(4), count5);
}

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a plain pointer and a layout mapping
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, NDimensionalSpanFromPointerAndMapping) {
  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  arene::base::array<TypeParam, count * count2 * count3> values{};

  using layout1 = arene::base::layout_left::mapping<arene::base::extents<std::size_t>>;
  using layout2 = arene::base::layout_left::mapping<arene::base::dextents<std::size_t, 2>>;
  using layout3 = arene::base::layout_left::mapping<arene::base::extents<std::size_t, count, count2, count3>>;
  using layout4 = arene::base::layout_right::mapping<arene::base::extents<std::size_t>>;
  using layout5 = arene::base::layout_right::mapping<arene::base::dextents<std::size_t, 2>>;
  using layout6 = arene::base::layout_right::mapping<arene::base::extents<std::size_t, count, count2, count3>>;

  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, layout1 const&>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, layout2 const&>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, layout3 const&>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, layout4 const&>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, layout5 const&>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, layout6 const&>));

  auto span = arene::base::make_mdspan(values.data(), layout1{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), layout1{})));

  ::testing::
      StaticAssertTypeEq<decltype(span), arene::base::mdspan<TypeParam, layout1::extents_type, layout1::layout_type>>();

  CONSTEXPR_ASSERT_EQ(span.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), values.data());

  auto span2 = arene::base::make_mdspan(values.data(), layout2{layout2::extents_type{count, count2}});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), layout2{layout2::extents_type{count, count2}})));

  ::testing::
      StaticAssertTypeEq<decltype(span2), arene::base::mdspan<TypeParam, layout2::extents_type, layout2::layout_type>>(
      );

  CONSTEXPR_ASSERT_EQ(span2.size(), count * count2);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span2.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span2.extents().extent(1), count2);

  auto span3 = arene::base::make_mdspan(values.data(), layout3{layout3::extents_type{}});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), layout3{layout3::extents_type{}})));

  ::testing::
      StaticAssertTypeEq<decltype(span3), arene::base::mdspan<TypeParam, layout3::extents_type, layout3::layout_type>>(
      );

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(2), count3);

  auto span4 = arene::base::make_mdspan(values.data(), layout4{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), layout4{})));

  ::testing::
      StaticAssertTypeEq<decltype(span4), arene::base::mdspan<TypeParam, layout4::extents_type, layout4::layout_type>>(
      );

  CONSTEXPR_ASSERT_EQ(span4.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span4.empty());
  CONSTEXPR_ASSERT_EQ(span4.data_handle(), values.data());

  auto span5 = arene::base::make_mdspan(values.data(), layout5{layout5::extents_type{count, count2}});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), layout5{layout5::extents_type{count, count2}})));

  ::testing::
      StaticAssertTypeEq<decltype(span5), arene::base::mdspan<TypeParam, layout5::extents_type, layout5::layout_type>>(
      );

  CONSTEXPR_ASSERT_EQ(span5.size(), count * count2);
  CONSTEXPR_ASSERT_FALSE(span5.empty());
  CONSTEXPR_ASSERT_EQ(span5.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span5.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span5.extents().extent(1), count2);

  auto span6 = arene::base::make_mdspan(values.data(), layout6{layout6::extents_type{}});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), layout6{layout6::extents_type{}})));

  ::testing::
      StaticAssertTypeEq<decltype(span6), arene::base::mdspan<TypeParam, layout6::extents_type, layout6::layout_type>>(
      );

  CONSTEXPR_ASSERT_EQ(span6.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span6.empty());
  CONSTEXPR_ASSERT_EQ(span6.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span6.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span6.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span6.extents().extent(2), count3);
}

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a plain pointer and an @c extents object
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, NDimensionalSpanFromPointerAndExtents) {
  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<TypeParam, count * count2 * count3 * count4 * count5> values{};

  using extents1 = arene::base::extents<std::size_t>;
  using extents2 = arene::base::extents<std::size_t, count>;
  using extents3 = arene::base::extents<std::uint8_t, count, count2, count3>;
  using extents4 = arene::base::dextents<std::int32_t, 3>;
  using extents5 = arene::base::extents<
      std::uint64_t,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      count3,
      count4,
      arene::base::dynamic_extent>;
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, extents1>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, extents2>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, extents3>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, extents4>));
  CONSTEXPR_ASSERT((can_call_make_mdspan<TypeParam*, extents5>));

  auto span = arene::base::make_mdspan(values.data(), extents1{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents1{})));

  ::testing::StaticAssertTypeEq<decltype(span), arene::base::mdspan<TypeParam, extents1>>();

  CONSTEXPR_ASSERT_EQ(span.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span.empty());
  CONSTEXPR_ASSERT_EQ(span.data_handle(), values.data());

  auto span2 = arene::base::make_mdspan(values.data(), extents2{});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents2{})));

  ::testing::StaticAssertTypeEq<decltype(span2), arene::base::mdspan<TypeParam, extents2>>();

  CONSTEXPR_ASSERT_EQ(span2.size(), count);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span2.extents().extent(0), count);

  auto span3 = arene::base::make_mdspan(values.data(), extents3{});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents3{})));

  ::testing::StaticAssertTypeEq<decltype(span3), arene::base::mdspan<TypeParam, extents3>>();

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span3.extents().extent(2), count3);

  auto span4 = arene::base::make_mdspan(values.data(), extents4{count, count2, count3});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents4{count, count2, count3})));

  ::testing::StaticAssertTypeEq<decltype(span4), arene::base::mdspan<TypeParam, extents4>>();

  CONSTEXPR_ASSERT_EQ(span4.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span4.empty());
  CONSTEXPR_ASSERT_EQ(span4.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span4.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span4.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span4.extents().extent(2), count3);

  auto span5 = arene::base::make_mdspan(values.data(), extents5{count, count2, count5});

  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), extents5{count, count2, count5})));

  ::testing::StaticAssertTypeEq<decltype(span5), arene::base::mdspan<TypeParam, extents5>>();

  CONSTEXPR_ASSERT_EQ(span5.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span5.empty());
  CONSTEXPR_ASSERT_EQ(span5.data_handle(), values.data());
  CONSTEXPR_ASSERT_EQ(span5.extents().extent(0), count);
  CONSTEXPR_ASSERT_EQ(span5.extents().extent(1), count2);
  CONSTEXPR_ASSERT_EQ(span5.extents().extent(2), count3);
  CONSTEXPR_ASSERT_EQ(span5.extents().extent(3), count4);
  CONSTEXPR_ASSERT_EQ(span5.extents().extent(4), count5);
}

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a pointer, a layout mapping, and an
/// accessor
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, MakeFromDataRightMappingAndDefaultAccessor) {
  using element_type = TypeParam;
  using accessor_type = arene::base::default_accessor<element_type>;

  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<element_type, count * count2 * count3 * count4 * count5> values{};

  using extents1 = arene::base::extents<std::size_t>;
  using extents2 = arene::base::extents<std::size_t, count>;
  using extents3 = arene::base::extents<std::uint8_t, count, count2, count3>;
  using extents4 = arene::base::dextents<std::int32_t, 3>;
  using extents5 = arene::base::extents<
      std::uint64_t,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      count3,
      count4,
      arene::base::dynamic_extent>;

  using mapping_type1 = arene::base::layout_right::mapping<extents1>;
  using mapping_type2 = arene::base::layout_right::mapping<extents2>;
  using mapping_type3 = arene::base::layout_right::mapping<extents3>;
  using mapping_type4 = arene::base::layout_right::mapping<extents4>;
  using mapping_type5 = arene::base::layout_right::mapping<extents5>;

  auto span1 = arene::base::make_mdspan(values.data(), mapping_type1{extents1{}}, accessor_type{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), mapping_type1{extents1{}}, accessor_type{})));

  ::testing::StaticAssertTypeEq<
      decltype(span1),
      arene::base::mdspan<element_type, extents1, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span1.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span1.empty());
  CONSTEXPR_ASSERT_EQ(span1.data_handle(), values.data());

  auto span2 = arene::base::make_mdspan(values.data(), mapping_type2{extents2{}}, accessor_type{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), mapping_type2{extents2{}}, accessor_type{})));

  ::testing::StaticAssertTypeEq<
      decltype(span2),
      arene::base::mdspan<element_type, extents2, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span2.size(), count);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), values.data());

  auto span3 = arene::base::make_mdspan(values.data(), mapping_type3{extents3{}}, accessor_type{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), mapping_type3{extents3{}}, accessor_type{})));

  ::testing::StaticAssertTypeEq<
      decltype(span3),
      arene::base::mdspan<element_type, extents3, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), values.data());

  auto span4 = arene::base::make_mdspan(values.data(), mapping_type4{extents4{count, count2, count3}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(values.data(), mapping_type4{extents4{count, count2, count3}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span4),
      arene::base::mdspan<element_type, extents4, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span4.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span4.empty());
  CONSTEXPR_ASSERT_EQ(span4.data_handle(), values.data());

  auto span5 = arene::base::make_mdspan(values.data(), mapping_type5{extents5{count, count2, count5}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(values.data(), mapping_type5{extents5{count, count2, count5}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span5),
      arene::base::mdspan<element_type, extents5, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span5.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span5.empty());
  CONSTEXPR_ASSERT_EQ(span5.data_handle(), values.data());
}

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a pointer, a layout mapping, and an
/// accessor
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, MakeFromDataLeftMappingAndDefaultAccessor) {
  using element_type = TypeParam;
  using accessor_type = arene::base::default_accessor<element_type>;

  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<element_type, count * count2 * count3 * count4 * count5> values{};

  using extents1 = arene::base::extents<std::size_t>;
  using extents2 = arene::base::extents<std::size_t, count>;
  using extents3 = arene::base::extents<std::uint8_t, count, count2, count3>;
  using extents4 = arene::base::dextents<std::int32_t, 3>;
  using extents5 = arene::base::extents<
      std::uint64_t,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      count3,
      count4,
      arene::base::dynamic_extent>;

  using mapping_type1 = arene::base::layout_left::mapping<extents1>;
  using mapping_type2 = arene::base::layout_left::mapping<extents2>;
  using mapping_type3 = arene::base::layout_left::mapping<extents3>;
  using mapping_type4 = arene::base::layout_left::mapping<extents4>;
  using mapping_type5 = arene::base::layout_left::mapping<extents5>;

  auto span1 = arene::base::make_mdspan(values.data(), mapping_type1{extents1{}}, accessor_type{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), mapping_type1{extents1{}}, accessor_type{})));

  ::testing::StaticAssertTypeEq<
      decltype(span1),
      arene::base::mdspan<element_type, extents1, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span1.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span1.empty());
  CONSTEXPR_ASSERT_EQ(span1.data_handle(), values.data());

  auto span2 = arene::base::make_mdspan(values.data(), mapping_type2{extents2{}}, accessor_type{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), mapping_type2{extents2{}}, accessor_type{})));

  ::testing::StaticAssertTypeEq<
      decltype(span2),
      arene::base::mdspan<element_type, extents2, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span2.size(), count);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), values.data());

  auto span3 = arene::base::make_mdspan(values.data(), mapping_type3{extents3{}}, accessor_type{});
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(values.data(), mapping_type3{extents3{}}, accessor_type{})));

  ::testing::StaticAssertTypeEq<
      decltype(span3),
      arene::base::mdspan<element_type, extents3, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), values.data());

  auto span4 = arene::base::make_mdspan(values.data(), mapping_type4{extents4{count, count2, count3}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(values.data(), mapping_type4{extents4{count, count2, count3}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span4),
      arene::base::mdspan<element_type, extents4, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span4.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span4.empty());
  CONSTEXPR_ASSERT_EQ(span4.data_handle(), values.data());

  auto span5 = arene::base::make_mdspan(values.data(), mapping_type5{extents5{count, count2, count5}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(values.data(), mapping_type5{extents5{count, count2, count5}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span5),
      arene::base::mdspan<element_type, extents5, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span5.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span5.empty());
  CONSTEXPR_ASSERT_EQ(span5.data_handle(), values.data());
}

template <typename ElementType>
class custom_accessor {
 public:
  struct element_type {
    ElementType element;
  };

  class data_handle_type {
    element_type* ptr_;

   public:
    constexpr explicit data_handle_type(element_type* ptr) noexcept
        : ptr_(ptr) {}

    friend constexpr auto operator==(data_handle_type lhs, data_handle_type rhs) noexcept -> bool {
      return lhs.ptr_ == rhs.ptr_;
    }
  };

  class reference {
    element_type& ref_;

   public:
    constexpr explicit reference(element_type& ref) noexcept
        : ref_(ref) {}

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator element_type&() const noexcept { return ref_; }
  };

  using offset_policy = custom_accessor;

  custom_accessor() = default;

  constexpr auto access(data_handle_type handle, std::size_t index) const noexcept -> reference {
    return reference{handle.ptr_[index]};
  }

  constexpr auto offset(data_handle_type handle, std::size_t delta) const noexcept -> data_handle_type {
    return data_handle_type{handle.ptr_ + delta};
  }
};

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a pointer, a layout mapping, and an
/// accessor
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, MakeFromDataRightMappingAndCustomAccessor) {
  using accessor_type = custom_accessor<TypeParam>;
  using element_type = typename accessor_type::element_type;
  using data_handle_type = typename accessor_type::data_handle_type;

  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<element_type, count * count2 * count3 * count4 * count5> values{};

  using extents1 = arene::base::extents<std::size_t>;
  using extents2 = arene::base::extents<std::size_t, count>;
  using extents3 = arene::base::extents<std::uint8_t, count, count2, count3>;
  using extents4 = arene::base::dextents<std::int32_t, 3>;
  using extents5 = arene::base::extents<
      std::uint64_t,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      count3,
      count4,
      arene::base::dynamic_extent>;

  using mapping_type1 = arene::base::layout_right::mapping<extents1>;
  using mapping_type2 = arene::base::layout_right::mapping<extents2>;
  using mapping_type3 = arene::base::layout_right::mapping<extents3>;
  using mapping_type4 = arene::base::layout_right::mapping<extents4>;
  using mapping_type5 = arene::base::layout_right::mapping<extents5>;

  auto span1 = arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type1{extents1{}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type1{extents1{}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span1),
      arene::base::mdspan<element_type, extents1, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span1.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span1.empty());
  CONSTEXPR_ASSERT_EQ(span1.data_handle(), data_handle_type{values.data()});

  auto span2 = arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type2{extents2{}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type2{extents2{}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span2),
      arene::base::mdspan<element_type, extents2, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span2.size(), count);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), data_handle_type{values.data()});

  auto span3 = arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type3{extents3{}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type3{extents3{}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span3),
      arene::base::mdspan<element_type, extents3, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), data_handle_type{values.data()});

  auto span4 = arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type4{extents4{count, count2, count3}},
      accessor_type{}
  );
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type4{extents4{count, count2, count3}},
      accessor_type{}
  )));

  ::testing::StaticAssertTypeEq<
      decltype(span4),
      arene::base::mdspan<element_type, extents4, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span4.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span4.empty());
  CONSTEXPR_ASSERT_EQ(span4.data_handle(), data_handle_type{values.data()});

  auto span5 = arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type5{extents5{count, count2, count5}},
      accessor_type{}
  );
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type5{extents5{count, count2, count5}},
      accessor_type{}
  )));

  ::testing::StaticAssertTypeEq<
      decltype(span5),
      arene::base::mdspan<element_type, extents5, arene::base::layout_right, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span5.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span5.empty());
  CONSTEXPR_ASSERT_EQ(span5.data_handle(), data_handle_type{values.data()});
}

/// @test The @c make_mdspan function creates an N-dimensional @c mdspan from a pointer, a layout mapping, and an
/// accessor
CONSTEXPR_TYPED_TEST(MakeMdspanTestSuite, MakeFromDataLeftMappingAndCustomAccessor) {
  using accessor_type = custom_accessor<TypeParam>;
  using element_type = typename accessor_type::element_type;
  using data_handle_type = typename accessor_type::data_handle_type;

  constexpr std::size_t count = 4;
  constexpr std::size_t count2 = 9;
  constexpr std::size_t count3 = 3;
  constexpr std::size_t count4 = 42;
  constexpr std::size_t count5 = 5;
  arene::base::array<element_type, count * count2 * count3 * count4 * count5> values{};

  using extents1 = arene::base::extents<std::size_t>;
  using extents2 = arene::base::extents<std::size_t, count>;
  using extents3 = arene::base::extents<std::uint8_t, count, count2, count3>;
  using extents4 = arene::base::dextents<std::int32_t, 3>;
  using extents5 = arene::base::extents<
      std::uint64_t,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      count3,
      count4,
      arene::base::dynamic_extent>;

  using mapping_type1 = arene::base::layout_left::mapping<extents1>;
  using mapping_type2 = arene::base::layout_left::mapping<extents2>;
  using mapping_type3 = arene::base::layout_left::mapping<extents3>;
  using mapping_type4 = arene::base::layout_left::mapping<extents4>;
  using mapping_type5 = arene::base::layout_left::mapping<extents5>;

  auto span1 = arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type1{extents1{}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type1{extents1{}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span1),
      arene::base::mdspan<element_type, extents1, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span1.size(), 1);
  CONSTEXPR_ASSERT_FALSE(span1.empty());
  CONSTEXPR_ASSERT_EQ(span1.data_handle(), data_handle_type{values.data()});

  auto span2 = arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type2{extents2{}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type2{extents2{}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span2),
      arene::base::mdspan<element_type, extents2, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span2.size(), count);
  CONSTEXPR_ASSERT_FALSE(span2.empty());
  CONSTEXPR_ASSERT_EQ(span2.data_handle(), data_handle_type{values.data()});

  auto span3 = arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type3{extents3{}}, accessor_type{});
  CONSTEXPR_ASSERT(
      noexcept(arene::base::make_mdspan(data_handle_type{values.data()}, mapping_type3{extents3{}}, accessor_type{}))
  );

  ::testing::StaticAssertTypeEq<
      decltype(span3),
      arene::base::mdspan<element_type, extents3, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span3.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span3.empty());
  CONSTEXPR_ASSERT_EQ(span3.data_handle(), data_handle_type{values.data()});

  auto span4 = arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type4{extents4{count, count2, count3}},
      accessor_type{}
  );
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type4{extents4{count, count2, count3}},
      accessor_type{}
  )));

  ::testing::StaticAssertTypeEq<
      decltype(span4),
      arene::base::mdspan<element_type, extents4, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span4.size(), count * count2 * count3);
  CONSTEXPR_ASSERT_FALSE(span4.empty());
  CONSTEXPR_ASSERT_EQ(span4.data_handle(), data_handle_type{values.data()});

  auto span5 = arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type5{extents5{count, count2, count5}},
      accessor_type{}
  );
  CONSTEXPR_ASSERT(noexcept(arene::base::make_mdspan(
      data_handle_type{values.data()},
      mapping_type5{extents5{count, count2, count5}},
      accessor_type{}
  )));

  ::testing::StaticAssertTypeEq<
      decltype(span5),
      arene::base::mdspan<element_type, extents5, arene::base::layout_left, accessor_type>>();

  CONSTEXPR_ASSERT_EQ(span5.size(), count * count2 * count3 * count4 * count5);
  CONSTEXPR_ASSERT_FALSE(span5.empty());
  CONSTEXPR_ASSERT_EQ(span5.data_handle(), data_handle_type{values.data()});
}

}  // namespace
