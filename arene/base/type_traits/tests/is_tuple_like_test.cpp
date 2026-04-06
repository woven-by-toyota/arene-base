// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_tuple_like.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::is_pair_like_v;
using ::arene::base::is_tuple_like_v;

static_assert(
    arene::base::tuple_like_detail::tuple_element_constructible_from_get<arene::base::array<int, 1U>, 0U>,
    ""
);

/// @brief Featureless user-defined type that does not implement the tuple protocol
struct user_defined {};

/// @brief User-defined type defines type traits but doesn't define get
class has_traits_missing_get {};

/// @brief User-defined type that has ADL @c get<Idx>() but doesn't define the required type traits
template <std::size_t Size>
class has_adl_get {};

template <std::size_t Idx, std::size_t Size>
auto get(has_adl_get<Size>&) noexcept -> std::size_t& {
  static std::size_t value = Idx < Size ? Idx : static_cast<std::size_t>(-1);
  return value;
}

template <std::size_t Idx, std::size_t Size>
auto get(has_adl_get<Size> const&) noexcept -> std::size_t const& {
  static std::size_t value = Idx < Size ? Idx : static_cast<std::size_t>(-1);
  return value;
}

template <std::size_t Idx, std::size_t Size>
auto get(has_adl_get<Size>&&) noexcept -> std::size_t&& {
  static std::size_t value;
  value = Idx < Size ? Idx : static_cast<std::size_t>(-1);
  return std::move(value);  // NOLINT(hicpp-move-const-arg) This function needs to return an rvalue reference
}

template <std::size_t Idx, std::size_t Size>
auto get(has_adl_get<Size> const&&) noexcept -> std::size_t const&& {
  static std::size_t value;
  value = Idx < Size ? Idx : static_cast<std::size_t>(-1);
  return std::move(value);  // NOLINT(hicpp-move-const-arg) This function needs to return an rvalue reference
}

/// @brief User-defined type that has ADL @c get<Idx>() and also defines the required type traits
template <std::size_t Size>
class has_adl_get_and_traits : public has_adl_get<Size> {};

/// @brief User-defined type that has member @c get<Idx>() but doesn't define the required type traits
template <std::size_t Size>
class has_member_get {
  mutable int data_;

 public:
  template <std::size_t Idx>
  constexpr auto get() & noexcept -> int& {
    data_ = Idx;
    return data_;
  }

  template <std::size_t Idx>
  constexpr auto get() const& noexcept -> int const& {
    data_ = Idx;
    return data_;
  }

  template <std::size_t Idx>
  constexpr auto get() && noexcept -> int&& {
    data_ = Idx;
    return std::move(data_);  // NOLINT(hicpp-move-const-arg) This function needs to return an rvalue reference
  }

  template <std::size_t Idx>
  constexpr auto get() const&& noexcept -> int const&& {
    data_ = Idx;
    return std::move(data_);  // NOLINT(hicpp-move-const-arg) This function needs to return an rvalue reference
  }
};

/// @brief User-defined type that has member @c get<Idx>() and also defines the required type traits
template <std::size_t Size>
class has_member_get_and_traits : public has_member_get<Size> {};

/// @brief User-defined type that has member @c get<Idx>() and also defines the required type traits, but get is not
/// ref-qualified
template <std::size_t Size>
class has_simple_member_get_and_traits {
 public:
  template <std::size_t>
  auto get() const noexcept -> int {
    return {};
  }
};

}  // namespace

// Drop into the global namespace to specialize std:: traits because the anonymous namespace is not related to ::std

template <std::size_t Size>
class std::tuple_size<has_adl_get_and_traits<Size>> {
 public:
  static constexpr std::size_t value = Size;
};

template <std::size_t Idx, std::size_t Size>
class std::tuple_element<Idx, has_adl_get_and_traits<Size>> {
 public:
  using type = std::size_t;
};

template <std::size_t Size>
class std::tuple_size<has_member_get_and_traits<Size>> : public std::integral_constant<std::size_t, Size> {};

template <std::size_t Idx, std::size_t Size>
class std::tuple_element<Idx, has_member_get_and_traits<Size>> {
 public:
  using type = int;
};

template <>
class std::tuple_size<has_traits_missing_get> : public std::integral_constant<std::size_t, 2> {};

template <std::size_t I>
class std::tuple_element<I, has_traits_missing_get> {
 public:
  using type = int;
};

template <std::size_t Size>
class std::tuple_size<has_simple_member_get_and_traits<Size>> : public std::integral_constant<std::size_t, Size> {};

template <std::size_t Idx, std::size_t Size>
class std::tuple_element<Idx, has_simple_member_get_and_traits<Size>> {
 public:
  using type = int;
};

namespace {

/// @brief A test expectation used for parameterized testing
/// @tparam T The type being tested
/// @tparam ShouldBeTupleLike Expectation of whether or not @c T is expected to be tuple-like
/// @tparam ShouldBePairLike Expectation of whether or not @c T is expected to be pair-like
template <typename T, bool ShouldBeTupleLike, bool ShouldBePairLike>
struct expectation {
  using type = T;
  static constexpr bool tuple_like = ShouldBeTupleLike;
  static constexpr bool pair_like = ShouldBePairLike;
};

template <typename T, bool ShouldBeTupleLike, bool ShouldBePairLike>
constexpr bool expectation<T, ShouldBeTupleLike, ShouldBePairLike>::tuple_like;
template <typename T, bool ShouldBeTupleLike, bool ShouldBePairLike>
constexpr bool expectation<T, ShouldBeTupleLike, ShouldBePairLike>::pair_like;

template <class T>
class IsTupleLikeTest : public ::testing::Test {};

using test_types = ::testing::Types<
    expectation<int, false, false>,
    expectation<double, false, false>,
    expectation<user_defined, false, false>,
    expectation<std::tuple<>, true, false>,
    expectation<std::tuple<double>, true, false>,
    expectation<std::tuple<double, int>, true, true>,
    expectation<std::tuple<int, int, int>, true, false>,
    expectation<std::tuple<double, int, int, double, double>, true, false>,
    expectation<std::pair<int, double>, true, true>,
    expectation<arene::base::array<int, 0>, true, false>,
    expectation<arene::base::array<int, 1>, true, false>,
    expectation<arene::base::array<int, 2>, true, true>,
    expectation<arene::base::array<int, 3>, true, false>,
    expectation<arene::base::array<int, 5>, true, false>,
    expectation<has_adl_get<0U>, false, false>,
    expectation<has_adl_get<2U>, false, false>,
    expectation<has_adl_get<5U>, false, false>,
    expectation<has_adl_get_and_traits<0U>, true, false>,
    expectation<has_adl_get_and_traits<2U>, true, true>,
    expectation<has_adl_get_and_traits<5U>, true, false>,
    expectation<has_member_get<0U>, false, false>,
    expectation<has_member_get<2U>, false, false>,
    expectation<has_member_get<5U>, false, false>,
    expectation<has_member_get_and_traits<0U>, true, false>,
    expectation<has_member_get_and_traits<2U>, true, true>,
    expectation<has_member_get_and_traits<5U>, true, false>,
    expectation<has_traits_missing_get, false, false>,
    expectation<has_simple_member_get_and_traits<0U>, true, false>,
    expectation<has_simple_member_get_and_traits<2U>, true, true>,
    expectation<has_simple_member_get_and_traits<5U>, true, false>>;

TYPED_TEST_SUITE(IsTupleLikeTest, test_types, );

/// @test The tested type and its cv-qualified forms are tuple-like and/or pair-like according to the test expectation
TYPED_TEST(IsTupleLikeTest, TraitsAreAsExpected) {
  STATIC_ASSERT_EQ(is_tuple_like_v<typename TypeParam::type>, TypeParam::tuple_like);
  STATIC_ASSERT_EQ(is_tuple_like_v<typename TypeParam::type&>, TypeParam::tuple_like);
  STATIC_ASSERT_EQ(is_tuple_like_v<typename TypeParam::type const&>, TypeParam::tuple_like);
  STATIC_ASSERT_EQ(is_tuple_like_v<typename TypeParam::type&&>, TypeParam::tuple_like);
  STATIC_ASSERT_EQ(is_tuple_like_v<typename TypeParam::type const&&>, TypeParam::tuple_like);

  STATIC_ASSERT_EQ(is_pair_like_v<typename TypeParam::type>, TypeParam::pair_like);
  STATIC_ASSERT_EQ(is_pair_like_v<typename TypeParam::type&>, TypeParam::pair_like);
  STATIC_ASSERT_EQ(is_pair_like_v<typename TypeParam::type const&>, TypeParam::pair_like);
  STATIC_ASSERT_EQ(is_pair_like_v<typename TypeParam::type&&>, TypeParam::pair_like);
  STATIC_ASSERT_EQ(is_pair_like_v<typename TypeParam::type const&&>, TypeParam::pair_like);
}

}  // namespace
