// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/begin_end.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/compiler_support/platform_queries/stdlib_info.hpp"
#include "arene/base/compiler_support/preprocessor/define_configuration.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/add_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/add_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <array>
#endif

namespace {

#if ARENE_IS_ON(ARENE_STDLIB_LIBARENECXX)
template <typename T, std::size_t N>
using std_array = arene::base::array<T, N>;
#else
template <typename T, std::size_t N>
using std_array = std::array<T, N>;
#endif

using element_t = std::int32_t;

enum class is_noexcept { yes, no };

/// @brief A range with member `begin` and `end` methods, which may be marked `noexcept` or not.
template <is_noexcept IsNoexcept>
struct member_range {
  element_t* begin_{};
  element_t* end_{};

  constexpr auto begin() noexcept(IsNoexcept == is_noexcept::yes) -> element_t* { return begin_; }
  constexpr auto begin() const noexcept(IsNoexcept == is_noexcept::yes) -> element_t const* { return begin_; }
  constexpr auto end() noexcept(IsNoexcept == is_noexcept::yes) -> element_t* { return end_; }
  constexpr auto end() const noexcept(IsNoexcept == is_noexcept::yes) -> element_t const* { return end_; }
};

// NOLINTNEXTLINE(hicpp-avoid-c-arrays) A C-array type is required to test C-array invocability.
using c_array = element_t[3U];

namespace adl_range {

/// @brief A range with argument-dependent `begin` and `end` functions, which may be marked `noexcept` or not.
template <is_noexcept IsNoexcept>
struct range {
  element_t* begin_{};
  element_t* end_{};
};

/// @brief Argument-dependent `begin` function for `adl_range::range`.
template <is_noexcept IsNoexcept>
constexpr auto begin(range<IsNoexcept>& input) noexcept(IsNoexcept == is_noexcept::yes) -> element_t* {
  return input.begin_;
}

/// @brief Argument-dependent `begin` function for `adl_range::range`.
template <is_noexcept IsNoexcept>
constexpr auto begin(range<IsNoexcept> const& input) noexcept(IsNoexcept == is_noexcept::yes) -> element_t const* {
  return input.begin_;
}

/// @brief Argument-dependent `end` function for `adl_range::range`.
template <is_noexcept IsNoexcept>
constexpr auto end(range<IsNoexcept>& input) noexcept(IsNoexcept == is_noexcept::yes) -> element_t* {
  return input.end_;
}

/// @brief Argument-dependent `end` function for `adl_range::range`.
template <is_noexcept IsNoexcept>
constexpr auto end(range<IsNoexcept> const& input) noexcept(IsNoexcept == is_noexcept::yes) -> element_t const* {
  return input.end_;
}

}  // namespace adl_range

template <typename TypeParam>
struct test_data {
  arene::base::array<element_t, 3> underlying{1, 2, 3};
  TypeParam range{&*underlying.begin(), &*underlying.end()};
};

template <>
struct test_data<c_array> {
  c_array range{1, 2, 3};
};

template <std::size_t N>
struct test_data<std_array<element_t, N>> {
  std_array<element_t, N> range{};
};

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
template <std::size_t N>
struct test_data<arene::base::array<element_t, N>> {
  arene::base::array<element_t, N> range{};
};
#endif

using begin_func_t = decltype(::arene::base::ranges::begin);
using end_func_t = decltype(::arene::base::ranges::end);

template <typename TypeParam>
class RangeAccessorsTest : public ::testing::Test {};

template <typename TypeParam>
class RangeAccessorsCArrayTest : public RangeAccessorsTest<TypeParam> {};

using c_array_types = ::testing::Types<c_array>;

// Ideally this would just be a fixture since there's only one type under test, but CONSTEXPR_TEST doesn't support
// fixtures.
TYPED_TEST_SUITE(RangeAccessorsCArrayTest, c_array_types, );

/// @test Given a C-array, when `arene::base::ranges::begin` is called, then it returns a pointer to the first element
/// and is unconditionally noexcept.
CONSTEXPR_TYPED_TEST(RangeAccessorsCArrayTest, BeginReturnsTheFirstElement) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  ::testing::StaticAssertTypeEq<arene::base::invoke_result_t<begin_func_t, decltype(range)>, decltype(&range[0])>();
  ::testing::
      StaticAssertTypeEq<arene::base::invoke_result_t<begin_func_t, decltype(const_range)>, decltype(&const_range[0])>(
      );
  CONSTEXPR_ASSERT(noexcept(::arene::base::ranges::begin(range)));
  CONSTEXPR_ASSERT(noexcept(::arene::base::ranges::begin(const_range)));
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(range), &range[0]);
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(const_range), &const_range[0]);
}

/// @test Given a C-array, when `arene::base::ranges::end` is called, then it returns a pointer one past the last
/// element and is unconditionally noexcept.
CONSTEXPR_TYPED_TEST(RangeAccessorsCArrayTest, EndReturnsOnePastTheLastElement) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  ::testing::StaticAssertTypeEq<arene::base::invoke_result_t<end_func_t, decltype(range)>, decltype(&range[3])>();
  ::testing::
      StaticAssertTypeEq<arene::base::invoke_result_t<end_func_t, decltype(const_range)>, decltype(&const_range[3])>();
  CONSTEXPR_ASSERT(noexcept(::arene::base::ranges::end(range)));
  CONSTEXPR_ASSERT(noexcept(::arene::base::ranges::end(const_range)));
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::end(range), &range[3]);
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::end(const_range), &const_range[3]);
}

template <typename TypeParam>
class RangeAccessorsEmptyStdArrayTest : public RangeAccessorsTest<TypeParam> {};

using empty_std_array_container_types = ::testing::Types<std_array<element_t, 0U>>;

TYPED_TEST_SUITE(RangeAccessorsEmptyStdArrayTest, empty_std_array_container_types, );

/// @test Given a zero-sized `std::array`, when `arene::base::ranges::begin` is called, then it returns
/// a value equivalent to invoking the member begin.
CONSTEXPR_TYPED_TEST(RangeAccessorsEmptyStdArrayTest, BeginIsEqualToEnd) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(range), ::arene::base::ranges::end(range));
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(const_range), ::arene::base::ranges::end(const_range));
}

template <typename TypeParam>
class RangeAccessorsNonEmptyStdArrayTest : public RangeAccessorsTest<TypeParam> {};

using non_empty_std_array_container_types =
    ::arene::base::type_lists::remove_duplicates_t<::testing::Types<std_array<element_t, 3U>>>;

TYPED_TEST_SUITE(RangeAccessorsNonEmptyStdArrayTest, non_empty_std_array_container_types, );

/// @test Given a non-zero-sized `std::array`, when `arene::base::ranges::begin` is called, then it returns
/// a value equivalent to invoking the member begin.
CONSTEXPR_TYPED_TEST(RangeAccessorsNonEmptyStdArrayTest, BeginReturnsEquivalentToArrayBegin) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  using std::get;
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(range), &get<0>(range));
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(const_range), &get<0>(const_range));
#endif
}

/// @test Given a non-zero-sized `std::array`, when `arene::base::ranges::end` is called, then it returns a value
/// equivalent to invoking the member end.
CONSTEXPR_TYPED_TEST(RangeAccessorsNonEmptyStdArrayTest, EndReturnsEquivalentToArrayEnd) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  using std::get;
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::end(range), &get<0>(range) + std::tuple_size<TypeParam>::value);
  CONSTEXPR_ASSERT_EQ(
      ::arene::base::ranges::end(const_range),
      &get<0>(const_range) + std::tuple_size<TypeParam>::value
  );
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
#endif
}

template <typename TypeParam>
class RangeAccessorsMemberRangeTest : public RangeAccessorsTest<TypeParam> {};

using member_range_container_types = ::arene::base::type_lists::concat_t<::testing::Types<
    ::arene::base::array<element_t, 0U>,
    ::arene::base::array<element_t, 3U>,
    member_range<is_noexcept::yes>,
    member_range<is_noexcept::no>>>;

TYPED_TEST_SUITE(RangeAccessorsMemberRangeTest, member_range_container_types, );

/// @test Given a range with a member `begin` method, when `arene::base::ranges::begin` is called, then it returns the
/// iterator produced by the member method.
CONSTEXPR_TYPED_TEST(RangeAccessorsMemberRangeTest, EndReturnsTheMemberBeginningIterator) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  ::testing::StaticAssertTypeEq<arene::base::invoke_result_t<begin_func_t, decltype(range)>, decltype(range.begin())>();
  ::testing::StaticAssertTypeEq<
      arene::base::invoke_result_t<begin_func_t, decltype(const_range)>,
      decltype(const_range.begin())>();
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(range), range.begin());
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(const_range), const_range.begin());
}

/// @test Given a range with a member `begin` method, when `arene::base::ranges::begin` is called, then its
/// noexcept-ness matches that of the member method.
CONSTEXPR_TYPED_TEST(RangeAccessorsMemberRangeTest, BeginNoexceptMatchesMember) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::begin(range)), noexcept(range.begin()));
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::begin(const_range)), noexcept(const_range.begin()));
}

/// @test Given a range with a member `end` method, when `arene::base::ranges::end` is called, then it returns the
/// iterator produced by the member method.
CONSTEXPR_TYPED_TEST(RangeAccessorsMemberRangeTest, EndReturnsTheMemberEndingIterator) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  ::testing::StaticAssertTypeEq<arene::base::invoke_result_t<end_func_t, decltype(range)>, decltype(range.end())>();
  ::testing::
      StaticAssertTypeEq<arene::base::invoke_result_t<end_func_t, decltype(const_range)>, decltype(const_range.end())>(
      );
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::end(range), range.end());
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::end(const_range), const_range.end());
}

/// @test Given a range with a member `end` method, when `arene::base::ranges::end` is called, then its
/// noexcept-ness matches that of the member method.
CONSTEXPR_TYPED_TEST(RangeAccessorsMemberRangeTest, EndNoexceptMatchesMember) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::end(range)), noexcept(range.end()));
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::end(const_range)), noexcept(const_range.end()));
}

template <typename TypeParam>
class RangeAccessorsAdlRangeTest : public RangeAccessorsTest<TypeParam> {};

using adl_range_container_types =
    ::testing::Types<adl_range::range<is_noexcept::yes>, adl_range::range<is_noexcept::no>>;

TYPED_TEST_SUITE(RangeAccessorsAdlRangeTest, adl_range_container_types, );

/// @test Given a range with an argument-dependent `begin` function, when `arene::base::ranges::begin` is called, then
/// it returns the iterator produced by the argument-dependent function.
CONSTEXPR_TYPED_TEST(RangeAccessorsAdlRangeTest, BeginReturnsTheAdlBeginningIterator) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  ::testing::StaticAssertTypeEq<arene::base::invoke_result_t<begin_func_t, decltype(range)>, decltype(begin(range))>();
  ::testing::StaticAssertTypeEq<
      arene::base::invoke_result_t<begin_func_t, decltype(const_range)>,
      decltype(begin(const_range))>();
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(range), begin(range));
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::begin(const_range), begin(const_range));
}

/// @test Given a range with an argument-dependent `begin` function, when `arene::base::ranges::begin` is called, its
/// noexcept-ness matches that of the argument-dependent function.
CONSTEXPR_TYPED_TEST(RangeAccessorsAdlRangeTest, BeginNoexceptMatchesAdl) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::begin(range)), noexcept(begin(range)));
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::begin(const_range)), noexcept(begin(const_range)));
}

/// @test Given a range with an argument-dependent `end` function, when `arene::base::ranges::end` is called, then it
/// returns the iterator produced by the argument-dependent function.
CONSTEXPR_TYPED_TEST(RangeAccessorsAdlRangeTest, EndReturnsTheAdlEndingIterator) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  ::testing::StaticAssertTypeEq<arene::base::invoke_result_t<end_func_t, decltype(range)>, decltype(end(range))>();
  ::testing::
      StaticAssertTypeEq<arene::base::invoke_result_t<end_func_t, decltype(const_range)>, decltype(end(const_range))>();
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::end(range)), noexcept(end(range)));
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::end(const_range)), noexcept(end(const_range)));
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::end(range), end(range));
  CONSTEXPR_ASSERT_EQ(::arene::base::ranges::end(const_range), end(const_range));
}

/// @test Given a range with an argument-dependent `end` function, when `arene::base::ranges::end` is called, its
/// noexcept-ness matches that of the argument-dependent function.
CONSTEXPR_TYPED_TEST(RangeAccessorsAdlRangeTest, EndNoexceptMatchesAdl) {
  auto data = test_data<TypeParam>{};
  auto& range = data.range;
  auto const& const_range = range;
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::end(range)), noexcept(end(range)));
  CONSTEXPR_ASSERT_EQ(noexcept(::arene::base::ranges::end(const_range)), noexcept(end(const_range)));
}

/// @brief A type which has none of the requirements to be considered a range.
struct no_iterators_at_all {};

struct iterator_is_not_a_valid_iterator {
  struct iterator {
    ARENE_MAYBE_UNUSED friend constexpr auto operator==(iterator const&, iterator const&) -> bool { return true; }
    ARENE_MAYBE_UNUSED friend constexpr auto operator!=(iterator const& lhs, iterator const& rhs) -> bool {
      return !(lhs == rhs);
    }
  };
  auto begin() -> iterator;
  auto end() -> iterator;
  auto begin() const -> iterator;
  auto end() const -> iterator;
};

struct end_iterator_is_not_a_sentinel_to_begin {
  auto begin() -> int*;
  auto end() -> double*;
  auto begin() const -> int const*;
  auto end() const -> double const*;
};

struct end_iterator_is_not_semiregular_default_ctor {
  using begin_iterator = int*;
  struct end_iterator {
    // semi-regular requires default contractibility
    constexpr explicit end_iterator(int) {}
    ~end_iterator() = default;
    constexpr end_iterator(end_iterator const&) = default;
    constexpr end_iterator(end_iterator&&) = default;
    constexpr auto operator=(end_iterator const&) -> end_iterator& = default;
    constexpr auto operator=(end_iterator&&) -> end_iterator& = default;

    ARENE_MAYBE_UNUSED friend constexpr auto operator==(end_iterator const&, end_iterator const&) -> bool {
      return true;
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator!=(end_iterator const& lhs, end_iterator const& rhs) -> bool {
      return !(lhs == rhs);
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator==(end_iterator const&, begin_iterator const&) -> bool {
      return true;
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator!=(end_iterator const& lhs, begin_iterator const& rhs) -> bool {
      return !(lhs == rhs);
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator==(begin_iterator const&, end_iterator const&) -> bool {
      return true;
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator!=(begin_iterator const& lhs, end_iterator const& rhs) -> bool {
      return !(lhs == rhs);
    }
  };

  auto begin() -> begin_iterator;
  auto end() -> end_iterator;
  auto begin() const -> begin_iterator const&;
  auto end() const -> end_iterator const&;
};

struct end_iterator_is_not_semiregular_non_copyable {
  using begin_iterator = int*;
  struct end_iterator {
    end_iterator() = default;
    ~end_iterator() = default;
    // semi-regular requires copyable
    constexpr end_iterator(end_iterator const&) = delete;
    constexpr end_iterator(end_iterator&&) = default;
    constexpr auto operator=(end_iterator const&) -> end_iterator& = delete;
    constexpr auto operator=(end_iterator&&) -> end_iterator& = default;
    ARENE_MAYBE_UNUSED friend constexpr auto operator==(end_iterator const&, end_iterator const&) -> bool {
      return true;
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator!=(end_iterator const& lhs, end_iterator const& rhs) -> bool {
      return !(lhs == rhs);
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator==(end_iterator const&, begin_iterator const&) -> bool {
      return true;
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator!=(end_iterator const& lhs, begin_iterator const& rhs) -> bool {
      return !(lhs == rhs);
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator==(begin_iterator const&, end_iterator const&) -> bool {
      return true;
    }
    ARENE_MAYBE_UNUSED friend constexpr auto operator!=(begin_iterator const& lhs, end_iterator const& rhs) -> bool {
      return !(lhs == rhs);
    }
  };
  auto begin() -> begin_iterator;
  auto end() -> end_iterator;
  auto begin() const -> begin_iterator const&;
  auto end() const -> end_iterator const&;
};

using non_borrowed_range_types =
    ::arene::base::type_lists::concat_t<member_range_container_types, adl_range_container_types>;

using rvalue_non_borrowed_range_types =
    ::arene::base::type_lists::apply_each_t<non_borrowed_range_types, std::add_rvalue_reference_t>;

using const_rvalue_non_borrowed_range_types =
    ::arene::base::type_lists::apply_each_t<rvalue_non_borrowed_range_types, std::add_const_t>;

using non_lvalue_non_borrowed_range_types = ::arene::base::type_lists::remove_duplicates_t<
    ::arene::base::type_lists::concat_t<rvalue_non_borrowed_range_types, const_rvalue_non_borrowed_range_types>>;

using non_valid_begin_types = ::testing::Types<no_iterators_at_all, iterator_is_not_a_valid_iterator>;

using lvalue_non_valid_begin_types =
    ::arene::base::type_lists::apply_each_t<non_valid_begin_types, std::add_lvalue_reference_t>;
using rvalue_non_valid_begin_types =
    ::arene::base::type_lists::apply_each_t<non_valid_begin_types, std::add_rvalue_reference_t>;
using const_lvalue_non_valid_begin_types =
    ::arene::base::type_lists::apply_each_t<lvalue_non_valid_begin_types, std::add_const_t>;
using const_rvalue_non_valid_begin_types =
    ::arene::base::type_lists::apply_each_t<rvalue_non_valid_begin_types, std::add_const_t>;
using all_non_valid_begin_types = ::arene::base::type_lists::concat_t<
    lvalue_non_valid_begin_types,
    rvalue_non_valid_begin_types,
    const_lvalue_non_valid_begin_types,
    const_rvalue_non_valid_begin_types,
    non_lvalue_non_borrowed_range_types>;

template <typename TypeParam>
class BeginInvocabilityTest : public RangeAccessorsTest<TypeParam> {};

TYPED_TEST_SUITE(BeginInvocabilityTest, non_lvalue_non_borrowed_range_types, );

/// @test Given a type which is not a range or is a non-lvalue, non-borrowed range, or where the begin iterator does not
/// model `std::input_or_output_iterator`, then `arene::base::ranges::begin` is not invocable.
TYPED_TEST(BeginInvocabilityTest, BeginIsNotInvocable) {
  STATIC_ASSERT_FALSE((!std::is_same<end_iterator_is_not_a_sentinel_to_begin, ::arene::base::remove_cvref_t<TypeParam>>::value) && arene::base::is_invocable_v<begin_func_t, TypeParam>);
}

using non_valid_end_types = ::testing::Types<
    end_iterator_is_not_a_sentinel_to_begin,
    end_iterator_is_not_semiregular_non_copyable,
    end_iterator_is_not_semiregular_default_ctor>;

using lvalue_non_valid_end_types =
    ::arene::base::type_lists::apply_each_t<non_valid_end_types, std::add_lvalue_reference_t>;
using rvalue_non_valid_end_types =
    ::arene::base::type_lists::apply_each_t<non_valid_end_types, std::add_rvalue_reference_t>;
using const_lvalue_non_valid_end_types =
    ::arene::base::type_lists::apply_each_t<lvalue_non_valid_end_types, std::add_const_t>;
using const_rvalue_non_valid_end_types =
    ::arene::base::type_lists::apply_each_t<rvalue_non_valid_end_types, std::add_const_t>;
using all_non_valid_end_types = ::arene::base::type_lists::concat_t<
    lvalue_non_valid_end_types,
    rvalue_non_valid_end_types,
    const_lvalue_non_valid_end_types,
    const_rvalue_non_valid_end_types,
    non_lvalue_non_borrowed_range_types>;

template <typename TypeParam>
class EndNonInvocabilityTest : public RangeAccessorsTest<TypeParam> {};

TYPED_TEST_SUITE(EndNonInvocabilityTest, non_lvalue_non_borrowed_range_types, );
/// @test Given a type which is not a range or is a non-lvalue, non-borrowed range, or where the end iterator does not
/// model `std::sentinel_for<iterator_t<Rng>, EndItr>`, then `arene::base::ranges::end` is not invocable.
TYPED_TEST(EndNonInvocabilityTest, EndIsNotInvocableIfRange) {
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<end_func_t, TypeParam>);
}

using all_valid_ranges = ::arene::base::type_lists::remove_duplicates_t<::arene::base::type_lists::concat_t<
    c_array_types,
    empty_std_array_container_types,
    non_empty_std_array_container_types,
    member_range_container_types,
    adl_range_container_types>>;

template <typename T>
class RangeIteratorTraitsTest : public ::testing::Test {};

TYPED_TEST_SUITE(RangeIteratorTraitsTest, all_valid_ranges, );

/// @test Given a range `Rng`, then `arene::base::ranges::iterator_t<Rng>` is equivalent to
/// `decltype(::arene::base::ranges::begin(std::declval<Rng&>()))`.
TYPED_TEST(RangeIteratorTraitsTest, IteratorTIsEquivalentToReturnFromBegin) {
  ::testing::StaticAssertTypeEq<
      ::arene::base::ranges::iterator_t<TypeParam>,
      decltype(::arene::base::ranges::begin(std::declval<TypeParam&>()))>();
}

/// @test Given a range `Rng`, then `arene::base::ranges::sentinel_t<Rng>` is equivalent to
/// `decltype(::arene::base::ranges::end(std::declval<Rng&>()))`.
TYPED_TEST(RangeIteratorTraitsTest, SentinelTIsEquivalentToReturnFromEnd) {
  ::testing::StaticAssertTypeEq<
      ::arene::base::ranges::sentinel_t<TypeParam>,
      decltype(::arene::base::ranges::end(std::declval<TypeParam&>()))>();
}

}  // namespace
