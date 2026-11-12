// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/size.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/ranges/disable_sized_range.hpp"
#include "arene/base/ranges/tests/ranges_test_helpers.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @brief A range that provides only a @c size() member function.
struct member_only {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto size() const noexcept -> std::size_t { return 3U; }
};

/// @brief A range that provides a @c size() member that may throw.
///
/// @note @c size returns a runtime member rather than a literal because it prevents GCC 8
/// from constant-folding the call and spuriously treating it as non-throwing.
struct throwing_member {
  std::size_t value_{4U};
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto size() const noexcept(false) -> std::size_t { return value_; }
};

namespace adl_ns {
/// @brief A range whose size is found only via argument-dependent lookup.
struct adl_only {};

constexpr auto size(adl_only /*unused*/) noexcept -> std::size_t { return 5U; }

/// @brief A range whose ADL @c size() may throw.
///
/// @note @c size returns a runtime member rather than a literal because it prevents GCC 8
/// from constant-folding the call and spuriously treating it as non-throwing.
struct throwing_adl {
  std::size_t value_{7U};
};

constexpr auto size(throwing_adl input) -> std::size_t { return input.value_; }
}  // namespace adl_ns

/// @brief A range whose member @c size() returns a non-integer type. Because the result is not integer-like, the member
/// path must be ignored; with no @c begin()/@c end() to fall back on, @c ranges::size does not participate.
struct non_integer_member {
  struct not_an_integer {};
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto size() const noexcept -> not_an_integer { return not_an_integer{}; }
};

namespace non_integer_adl_ns {
/// @brief A range whose ADL @c size() returns a non-integer type, so the ADL path must be ignored.
struct non_integer_adl {};

struct not_an_integer {};
constexpr auto size(non_integer_adl /*unused*/) noexcept -> not_an_integer { return not_an_integer{}; }
}  // namespace non_integer_adl_ns

/// @brief A range whose member @c size() returns a non-integer type but which also exposes @c begin()/@c end(). The
/// non-integer member must be ignored, so @c ranges::size falls through to the @c end() - @c begin() path.
struct non_integer_member_with_fallback {
  struct not_an_integer {};
  int const* first;
  int const* last;

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto size() const noexcept -> not_an_integer { return not_an_integer{}; }
  constexpr auto begin() const noexcept -> int const* { return first; }
  constexpr auto end() const noexcept -> int const* { return last; }
};

/// @brief Which subtraction directions a @c configurable_range 's iterator/sentinel pair supports.
enum class subtraction_support {
  none,                     ///< Neither @c sentinel - iterator nor @c iterator - sentinel is defined.
  sentinel_minus_iterator,  ///< Only @c sentinel - iterator is defined.
  iterator_minus_sentinel,  ///< Only @c iterator - sentinel is defined.
  both,                     ///< Both directions are defined (models a sized sentinel).
};

/// @brief @c true if @p support enables the @c sentinel - iterator direction.
constexpr auto has_sentinel_minus_iterator(subtraction_support support) noexcept -> bool {
  return support == subtraction_support::sentinel_minus_iterator || support == subtraction_support::both;
}

/// @brief @c true if @p support enables the @c iterator - sentinel direction.
constexpr auto has_iterator_minus_sentinel(subtraction_support support) noexcept -> bool {
  return support == subtraction_support::iterator_minus_sentinel || support == subtraction_support::both;
}

/// @brief A range whose iterator/sentinel subtraction directions, subtraction result type, and iterator difference type
/// are parameterized, so a single template can exercise every combination the sized-sentinel check must accept or
/// reject, as well as the exact result type @c ranges::size derives from the fallback path.
///
/// @tparam Support which of the two subtraction directions the pair defines.
/// @tparam DifferenceType the result type of the enabled subtraction(s); when it differs from the iterator's
///         @c difference_type , the pair must NOT be treated as a sized sentinel.
/// @tparam IteratorDifferenceType the iterator's @c difference_type ; when it matches @c DifferenceType and the pair is
///         a sized sentinel, @c ranges::size yields its @c make_unsigned_t .
template <
    subtraction_support Support,
    typename DifferenceType = std::ptrdiff_t,
    typename IteratorDifferenceType = std::ptrdiff_t>
struct configurable_range {
  /// @brief The iterator of the range. Its @c difference_type is @c IteratorDifferenceType .
  struct iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = int;
    using difference_type = IteratorDifferenceType;
    using pointer = int const*;
    using reference = int const&;

    int const* ptr;

    constexpr auto operator*() const noexcept -> reference { return *ptr; }
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) A pointer-backed test iterator.
    constexpr auto operator++() noexcept -> iterator& {
      ++ptr;
      return *this;
    }
    constexpr auto operator++(int) noexcept -> iterator {
      iterator const copy{*this};
      ++ptr;
      return copy;
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    constexpr auto operator==(iterator other) const noexcept -> bool { return ptr == other.ptr; }
    constexpr auto operator!=(iterator other) const noexcept -> bool { return ptr != other.ptr; }
  };

  /// @brief The sentinel of the range.
  struct sentinel {
    int const* last;

    /// @brief @c sentinel - iterator , enabled only when @c Support permits that direction.
    template <
        subtraction_support SfinaeSupport = Support,
        arene::base::constraints<std::enable_if_t<has_sentinel_minus_iterator(SfinaeSupport)>> = nullptr>
    friend constexpr auto operator-(sentinel lhs, iterator rhs) noexcept -> DifferenceType {
      return static_cast<DifferenceType>(lhs.last - rhs.ptr);
    }

    /// @brief @c iterator - sentinel , enabled only when @c Support permits that direction.
    template <
        subtraction_support SfinaeSupport = Support,
        arene::base::constraints<std::enable_if_t<has_iterator_minus_sentinel(SfinaeSupport)>> = nullptr>
    friend constexpr auto operator-(iterator lhs, sentinel rhs) noexcept -> DifferenceType {
      return static_cast<DifferenceType>(lhs.ptr - rhs.last);
    }

    constexpr auto operator==(iterator other) const noexcept -> bool { return last == other.ptr; }
    constexpr auto operator!=(iterator other) const noexcept -> bool { return last != other.ptr; }
    friend constexpr auto operator==(iterator lhs, sentinel rhs) noexcept -> bool { return lhs.ptr == rhs.last; }
    friend constexpr auto operator!=(iterator lhs, sentinel rhs) noexcept -> bool { return lhs.ptr != rhs.last; }
  };

  int const* first;
  int const* last;

  constexpr auto begin() const noexcept -> iterator { return iterator{first}; }
  constexpr auto end() const noexcept -> sentinel { return sentinel{last}; }
};

/// @brief Alias yielding the result type of @c arene::base::ranges::size(t) , well-formed only when that call is valid.
/// @tparam Range the type of the range
template <typename Range>
using size_result_t = decltype(arene::base::ranges::size(std::declval<Range&>()));

/// @brief A range with both a member @c size() and an ADL @c size(): member must win.
namespace both_ns {
struct both {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto size() const noexcept -> std::size_t { return 6U; }
};

// This free function must NOT be selected because the member function takes priority.
constexpr auto size(both /*unused*/) noexcept -> int { return -1; }
}  // namespace both_ns

namespace adl_beats_fallback_ns {
/// @brief A range with an ADL @c size() reporting a deliberately wrong value alongside @c begin()/@c end()
struct adl_beats_fallback {
  int const* first;
  int const* last;

  constexpr auto begin() const noexcept -> int const* { return first; }
  constexpr auto end() const noexcept -> int const* { return last; }
};

constexpr auto size(adl_beats_fallback /*unused*/) noexcept -> std::size_t { return 999U; }
}  // namespace adl_beats_fallback_ns

/// @brief A range with a member @c size() that reports a deliberately wrong value, alongside @c begin()/@c end().
///
/// It opts out of the member/ADL @c size() path via @c disable_sized_range , so @c size must ignore the member and
/// compute @c end() - @c begin() instead.
struct disabled_member_range {
  int const* first;
  int const* last;

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto size() const noexcept -> std::size_t { return 999U; }
  constexpr auto begin() const noexcept -> int const* { return first; }
  constexpr auto end() const noexcept -> int const* { return last; }
};

namespace disabled_adl_ns {
/// @brief A range with an ADL @c size() that reports a deliberately wrong value, alongside @c begin()/@c end().
///
/// It opts out of the member/ADL @c size() path via @c disable_sized_range , so @c size must ignore the ADL free
/// function and compute @c end() - @c begin() instead.
struct disabled_adl_range {
  int const* first;
  int const* last;

  constexpr auto begin() const noexcept -> int const* { return first; }
  constexpr auto end() const noexcept -> int const* { return last; }
};

constexpr auto size(disabled_adl_range /*unused*/) noexcept -> std::size_t { return 999U; }
}  // namespace disabled_adl_ns

}  // namespace

template <>
struct arene::base::ranges::disable_sized_range<disabled_member_range> : std::true_type {};
template <>
struct arene::base::ranges::disable_sized_range<disabled_adl_ns::disabled_adl_range> : std::true_type {};

namespace {

// Namespace-scope test data so it has a constant address usable at compile time. A `static constexpr` local would be a
// C++23 extension inside the constexpr test bodies, and a non-static local's address is not a constant expression.
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr int span_data[4] = {1, 2, 3, 4};
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr int array_data[7] = {};

/// @test Invoking @c arene::base::ranges::size on a type with a member @c size() returns the member result.
CONSTEXPR_TEST(Size, MemberSize) {
  constexpr member_only value{};
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(value)), std::size_t>();
  CONSTEXPR_ASSERT_EQ(arene::base::ranges::size(value), 3U);
}

/// @test Invoking @c arene::base::ranges::size on @c arene::base::span uses the member @c size().
CONSTEXPR_TEST(Size, SpanMemberSize) {
  constexpr arene::base::span<int const, 4> view{span_data};
  CONSTEXPR_ASSERT_EQ(arene::base::ranges::size(view), 4U);
}

/// @test Invoking @c arene::base::ranges::size on @c arene::base::array uses its callable member @c size.
CONSTEXPR_TEST(Size, ArrayMemberSize) {
  constexpr arene::base::array<int, 3> value{{1, 2, 3}};
  CONSTEXPR_ASSERT_EQ(arene::base::ranges::size(value), 3U);
}

/// @test Invoking @c arene::base::ranges::size on a type with only an ADL @c size() uses the free function.
CONSTEXPR_TEST(Size, AdlSize) {
  constexpr adl_ns::adl_only value{};
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(value)), std::size_t>();
  CONSTEXPR_ASSERT_EQ(arene::base::ranges::size(value), 5U);
}

/// @test Invoking @c arene::base::ranges::size on a C array returns its extent via the end-begin path.
CONSTEXPR_TEST(Size, CArray) {
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(array_data)), std::size_t>();
  CONSTEXPR_ASSERT_EQ(arene::base::ranges::size(array_data), 7U);
}

/// @test A member @c size() takes priority over an ADL @c size().
CONSTEXPR_TEST(Size, MemberBeatsAdl) {
  constexpr both_ns::both value{};
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(value)), std::size_t>();
  CONSTEXPR_ASSERT_EQ(arene::base::ranges::size(value), 6U);
  // Sanity check that the ADL overload exists and differs from the member; it must not be the one selected above.
  CONSTEXPR_ASSERT_EQ(size(value), -1);
}

/// @test A viable ADL @c size() takes priority over the @c end() - @c begin() fallback.
CONSTEXPR_TEST(Size, AdlBeatsSentinelFallback) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int const data[3] = {1, 2, 3};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  adl_beats_fallback_ns::adl_beats_fallback const value{&data[0], &data[0] + 3};
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(value)), std::size_t>();
  // ADL size() reports 999; the end() - begin() fallback would report 3. The ADL path must win.
  CONSTEXPR_ASSERT_EQ(arene::base::ranges::size(value), 999U);
}

/// @test A member @c size() returning a non-integer type is ignored; with no @c begin()/@c end() fallback,
/// @c arene::base::ranges::size does not participate for that type.
CONSTEXPR_TEST(Size, NonIntegerMemberSizeIsIgnored) {
  CONSTEXPR_ASSERT_FALSE(arene::base::substitution_succeeds<size_result_t, non_integer_member>);
}

/// @test An ADL @c size() returning a non-integer type is ignored; with no @c begin()/@c end() fallback,
/// @c arene::base::ranges::size does not participate for that type.
CONSTEXPR_TEST(Size, NonIntegerAdlSizeIsIgnored) {
  CONSTEXPR_ASSERT_FALSE(arene::base::substitution_succeeds<size_result_t, non_integer_adl_ns::non_integer_adl>);
  // Odr-use the non-integer ADL overload so it is emitted; it exists yet is what gets rejected above.
  constexpr non_integer_adl_ns::non_integer_adl value{};
  constexpr non_integer_adl_ns::not_an_integer result{size(value)};
  static_cast<void>(result);
}

/// @test A member @c size() returning a non-integer type is ignored in favour of the @c end() - @c begin() path, which
/// yields the true extent as @c std::size_t .
CONSTEXPR_TEST(Size, NonIntegerMemberFallsThroughToSentinel) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int const data[5] = {1, 2, 3, 4, 5};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  non_integer_member_with_fallback const value{&data[0], &data[0] + 5};
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(value)), std::size_t>();
  CONSTEXPR_ASSERT(arene::base::ranges::size(value) == 5U);
}

/// @test @c arene::base::ranges::size accepts a range whose iterator/sentinel pair supports subtraction in both
/// directions with the iterator's difference type, and computes @c end() - @c begin().
CONSTEXPR_TEST(Size, SizedSentinelBothDirections) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int const data[3] = {1, 2, 3};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  configurable_range<subtraction_support::both> const value{&data[0], &data[0] + 3};
  ::testing::StaticAssertTypeEq<size_result_t<decltype(value)>, std::size_t>();
  CONSTEXPR_ASSERT(arene::base::ranges::size(value) == 3U);
}

/// @test @c arene::base::ranges::size rejects a range whose iterator/sentinel pair supports subtraction in only one
/// direction, even though that single direction compiles: a sized sentinel must be subtractable both ways.
CONSTEXPR_TEST(Size, SizedSentinelRequiresBothDirections) {
  CONSTEXPR_ASSERT_FALSE(arene::base::substitution_succeeds<
                         size_result_t,
                         configurable_range<subtraction_support::sentinel_minus_iterator>>);
  CONSTEXPR_ASSERT_FALSE(arene::base::substitution_succeeds<
                         size_result_t,
                         configurable_range<subtraction_support::iterator_minus_sentinel>>);
  CONSTEXPR_ASSERT_FALSE(arene::base::
                             substitution_succeeds<size_result_t, configurable_range<subtraction_support::none>>);
  CONSTEXPR_ASSERT(arene::base::substitution_succeeds<size_result_t, configurable_range<subtraction_support::both>>);
}

/// @test @c arene::base::ranges::size rejects a range whose subtraction yields a type other than the iterator's
/// difference type, even when both directions are otherwise available.
CONSTEXPR_TEST(Size, SizedSentinelRequiresExactDifferenceType) {
  CONSTEXPR_ASSERT_FALSE(arene::base::substitution_succeeds<
                         size_result_t,
                         configurable_range<subtraction_support::both, float>>);
}

/// @test @c ranges::size returns the unsigned counterpart of the iterator's own difference type
CONSTEXPR_TEST(Size, FallbackResultTracksDifferenceType) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int const data[3] = {1, 2, 3};
  using narrow_range = configurable_range<subtraction_support::both, std::int16_t, std::int16_t>;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  narrow_range const value{&data[0], &data[0] + 3};
  ::testing::StaticAssertTypeEq<size_result_t<narrow_range>, std::uint16_t>();
  CONSTEXPR_ASSERT(arene::base::ranges::size(value) == 3U);
}

/// @test A type opting out via @c disable_sized_range has its member @c size() ignored; @c size computes
/// @c end() - @c begin() instead.
CONSTEXPR_TEST(Size, DisableSizedRangeSkipsMember) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int const data[5] = {1, 2, 3, 4, 5};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  disabled_member_range const value{&data[0], &data[0] + 5};
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(value)), std::size_t>();
  // The member size() would report 999; the end - begin fallback reports the true extent.
  CONSTEXPR_ASSERT(arene::base::ranges::size(value) == 5U);
  CONSTEXPR_ASSERT_EQ(value.size(), 999U);
}

/// @test A type opting out via @c disable_sized_range has its ADL @c size() ignored; @c size computes
/// @c end() - @c begin() instead.
CONSTEXPR_TEST(Size, DisableSizedRangeSkipsAdl) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int const data[5] = {1, 2, 3, 4, 5};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  disabled_adl_ns::disabled_adl_range const value{&data[0], &data[0] + 5};
  ::testing::StaticAssertTypeEq<decltype(arene::base::ranges::size(value)), std::size_t>();
  // The ADL size() would report 999; the end - begin fallback reports the true extent.
  CONSTEXPR_ASSERT(arene::base::ranges::size(value) == 5U);
  CONSTEXPR_ASSERT_EQ(size(value), 999U);
}

/// @test @c arene::base::ranges::size propagates the @c noexcept of the selected operation.
///
/// @note Two GCC 8 pre-C++17 quirks are sidestepped here so the declared exception specification is honoured across
/// supported toolchains. First, the operands use @c std::declval rather than @c constexpr locals: a call on a constexpr
/// local operand is itself a constant expression, which GCC 8 treated as non-throwing regardless of its declared
/// specification. Second, the throwing test types (@c throwing_member, @c adl_ns::throwing_adl) return a runtime
/// data member rather than a literal, since GCC 8 constant-folds a manifestly-constant @c constexpr body and then also
/// treats that folded call as non-throwing. Either quirk alone would make @c noexcept(size(...)) spuriously @c true .
TEST(Size, NoexceptPropagation) {
  // Must be noexcept when the member is noexcept.
  STATIC_ASSERT_TRUE(noexcept(arene::base::ranges::size(std::declval<member_only const&>())));
  // Must not be noexcept when the member may throw.
  STATIC_ASSERT_FALSE(noexcept(arene::base::ranges::size(std::declval<throwing_member const&>())));
  // Must be noexcept when the ADL size is noexcept.
  STATIC_ASSERT_TRUE(noexcept(arene::base::ranges::size(std::declval<adl_ns::adl_only const&>())));
  // Must not be noexcept when the ADL size may throw.
  STATIC_ASSERT_FALSE(noexcept(arene::base::ranges::size(std::declval<adl_ns::throwing_adl const&>())));
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  STATIC_ASSERT_TRUE(noexcept(arene::base::ranges::size(std::declval<int(&)[2]>())));
  // NOLINTEND(hicpp-avoid-c-arrays)

  // Call these functions so clang doesn't mark them as unused.
  EXPECT_EQ(arene::base::ranges::size(throwing_member{}), 4U);
  EXPECT_EQ(arene::base::ranges::size(adl_ns::throwing_adl{}), 7U);
}

template <typename TypeParam>
class SizeForRangeTypes : public ::testing::Test {};

TYPED_TEST_SUITE(SizeForRangeTypes, ranges_test::range_types, );

/// @test Given any standard range shape (member @c size() , ADL @c begin()/@c end() , or a C array), @c ranges::size
/// participates in overload resolution and yields @c std::size_t regardless of which path is selected.
TYPED_TEST(SizeForRangeTypes, ParticipatesAndYieldsSizeType) {
  STATIC_ASSERT_TRUE((arene::base::substitution_succeeds<size_result_t, TypeParam>));
  ::testing::StaticAssertTypeEq<size_result_t<TypeParam>, std::size_t>();
}

}  // namespace
