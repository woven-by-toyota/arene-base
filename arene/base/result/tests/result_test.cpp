// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/result.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/monostate/monostate.hpp"
#include "arene/base/result/detail/in_place_error.hpp"
#include "arene/base/result/detail/in_place_value.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/hash.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/reference_wrapper.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/utility/as_const.hpp"

namespace {

using arene::base::constraints;
using arene::base::error_result;
using arene::base::in_place_error;
using arene::base::in_place_error_t;
using arene::base::in_place_value;
using arene::base::in_place_value_t;
using arene::base::monostate;
using arene::base::result;
using arene::base::value_result;

template <class T, class U>
constexpr bool is_same_v = std::is_same<T, U>::value;

template <class T, class... P>
constexpr bool is_constructible_v = std::is_constructible<T, P...>::value;

template <class T>
constexpr bool is_default_constructible_v = std::is_default_constructible<T>::value;

template <class T>
constexpr bool is_nothrow_default_constructible_v = std::is_nothrow_default_constructible<T>::value;

template <class T>
constexpr bool is_copy_constructible_v = std::is_copy_constructible<T>::value;

template <class T>
constexpr bool is_nothrow_copy_constructible_v = std::is_nothrow_copy_constructible<T>::value;

template <class T>
constexpr bool is_trivially_copy_constructible_v = std::is_trivially_copy_constructible<T>::value;

template <class T>
constexpr bool is_nothrow_copy_assignable_v = std::is_nothrow_copy_assignable<T>::value;

template <class T>
constexpr bool is_move_constructible_v = std::is_move_constructible<T>::value;

template <class T>
constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible<T>::value;

template <class T>
constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable<T>::value;

/// @brief Helper type which stores the sum of its arguments
///
/// This helper type adds the two arguments to the ocnstructor and stores the result as a data member. This is used to
/// verify that the in_place constructors forreclty forward arguments.
struct value_sum_constructor_args {
  explicit value_sum_constructor_args(int& lhs, int&& rhs)
      : value(lhs + rhs) {}
  int value;
};

// Dummy type used as value1 Result error_type
// The constructor just adds the arguments and stores the result as a
// data memberes. This, and the paramerror_ter types, are used to verify that the
// in_places constructors correctly forward arguments.
struct error_sum_constructor_args {
  explicit error_sum_constructor_args(std::int64_t& lhs, std::int64_t&& rhs)
      : value(lhs + rhs) {}
  std::int64_t value;
};

// A move-only type to be used as the value_type of Result
struct move_only_value_sum_constructor_args {
  explicit move_only_value_sum_constructor_args(int& lhs, int&& rhs)
      : value(lhs + rhs) {}
  move_only_value_sum_constructor_args(move_only_value_sum_constructor_args&&) = default;
  auto operator=(move_only_value_sum_constructor_args&&) -> move_only_value_sum_constructor_args& = default;

  move_only_value_sum_constructor_args(move_only_value_sum_constructor_args const&) = delete;
  auto operator=(move_only_value_sum_constructor_args const&) -> move_only_value_sum_constructor_args& = delete;
  ~move_only_value_sum_constructor_args() = default;
  int value;
};

// A move-only type to be used as the error_type of Result
struct move_only_error_sum_constructor_args {
  explicit move_only_error_sum_constructor_args(std::int64_t& lhs, std::int64_t&& rhs)
      : value(lhs + rhs) {}
  move_only_error_sum_constructor_args(move_only_error_sum_constructor_args&&) = default;
  auto operator=(move_only_error_sum_constructor_args&&) -> move_only_error_sum_constructor_args& = default;

  move_only_error_sum_constructor_args(move_only_error_sum_constructor_args const&) = delete;
  auto operator=(move_only_error_sum_constructor_args const&) -> move_only_error_sum_constructor_args& = delete;
  ~move_only_error_sum_constructor_args() = default;
  std::int64_t value;
};

// A type with a default constructor that may throw
struct fallible_default_constructible {
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  fallible_default_constructible() noexcept(false) {}
};

// A type with a trivial copy constructor
struct trivially_copy_constructible {
  explicit trivially_copy_constructible(int val) noexcept
      : value(val) {}
  int value;
};

static_assert(is_trivially_copy_constructible_v<trivially_copy_constructible>, "");
static_assert(is_nothrow_copy_constructible_v<trivially_copy_constructible>, "");

// A type with a copy constructor that may throw
struct fallible_copy_constructible {
  explicit fallible_copy_constructible(int val) noexcept
      : value(val) {}

  fallible_copy_constructible(fallible_copy_constructible const& other) noexcept(false)
      : value(other.value) {
    static_cast<void>(other);
  }

  fallible_copy_constructible(fallible_copy_constructible&&) = default;

  auto operator=(fallible_copy_constructible const&) -> fallible_copy_constructible& = default;
  auto operator=(fallible_copy_constructible&&) -> fallible_copy_constructible& = default;
  ~fallible_copy_constructible() = default;

  int value;
};

static_assert(is_copy_constructible_v<fallible_copy_constructible>, "");
static_assert(!is_nothrow_copy_constructible_v<fallible_copy_constructible>, "");

// A type with a move constructor that may throw
struct fallible_move_constructible {
  fallible_move_constructible(fallible_move_constructible&&) noexcept(false) {}
  auto operator=(fallible_move_constructible&&) noexcept -> fallible_move_constructible& { return *this; }

  fallible_move_constructible(fallible_move_constructible const&) = default;
  auto operator=(fallible_move_constructible const&) -> fallible_move_constructible& = default;
  ~fallible_move_constructible() noexcept = default;
};

// a type which is not default constructible
struct no_default_ctor {
  no_default_ctor() = delete;
};

static_assert(is_move_constructible_v<fallible_move_constructible>, "");
static_assert(!is_nothrow_move_constructible_v<fallible_move_constructible>, "");
/// @test @c arene::base::result<T,E>::value_type is @c T
TEST(ResultMemberTypes, ValueTypeIsTTemplateParam) {
  // Make sure that the value_type member exists and is correct
  ::testing::StaticAssertTypeEq<result<int, char>::value_type, int>();
  ::testing::StaticAssertTypeEq<result<void, char>::value_type, void>();
}
/// @test @c arene::base::result<T,E>::error_type is @c E
TEST(ResultMemberTypes, ErrorTypeIsETemplateParam) {
  // Make sure that the error_type member exists and is correct
  ::testing::StaticAssertTypeEq<result<int, char>::error_type, char>();
}

/// @test @c arene::base::result<T,E> satisfies @c std::is_default_constructable if both @c T and @c E are default
/// constructable.
TEST(ResultConstructor, IsDefaultConstructibleIfValueIsDefaultConstructible) {
  STATIC_ASSERT_TRUE((is_default_constructible_v<result<int, char>>));
  STATIC_ASSERT_TRUE((is_default_constructible_v<result<void, char>>));
  STATIC_ASSERT_TRUE((is_default_constructible_v<result<int, fallible_default_constructible>>));
  STATIC_ASSERT_TRUE((is_default_constructible_v<result<fallible_default_constructible, int>>));
  STATIC_ASSERT_TRUE(
      (is_default_constructible_v<result<fallible_default_constructible, fallible_default_constructible>>)
  );
  STATIC_ASSERT_TRUE((is_default_constructible_v<result<int, no_default_ctor>>));

  STATIC_ASSERT_FALSE((is_default_constructible_v<result<no_default_ctor, char>>));
}
/// @test @c arene::base::result<T,E> satisfies @c std::is_nothrow_default_constructible if both @c T and @c E are
/// default constructable.
TEST(ResultConstructor, IsNothrowDefaultConstructibleIfValueAndErrorAreNothrowDefaultConstructible) {
  STATIC_ASSERT_TRUE((is_nothrow_default_constructible_v<result<void, char>>));
  STATIC_ASSERT_TRUE((is_nothrow_default_constructible_v<result<int, char>>));
  STATIC_ASSERT_TRUE((is_nothrow_default_constructible_v<result<int, fallible_default_constructible>>));
  STATIC_ASSERT_FALSE((is_nothrow_default_constructible_v<result<fallible_default_constructible, int>>));
  STATIC_ASSERT_TRUE((is_nothrow_default_constructible_v<result<int, no_default_ctor>>));

  STATIC_ASSERT_FALSE(
      (is_nothrow_default_constructible_v<result<fallible_default_constructible, fallible_default_constructible>>)
  );
  STATIC_ASSERT_FALSE((is_nothrow_default_constructible_v<result<no_default_ctor, char>>));
}
/// @test Post default construction, @c arene::base::result<T,E>::has_value returns @c true .
TEST(ResultConstructor, DefaultConstructorDefaultConstructsTheValueChannel) {
  // Actual default construction (makes sure value-initialization)
  {
    result<int, int> const res;
    EXPECT_EQ(res.value(), 0);
  }

  // Default construction (void)
  {
    result<void, int> const res;
    EXPECT_TRUE(res.has_value());
  }
}
/// @test Given an @c arene::base::result<T,E> constructed using @c in_place_value,
/// @c arene::base::result<T,E>::has_value returns @c true .
TEST(ResultConstructor, InPlaceValuePopulatesTheValueChannel) {
  {
    int value1 = 1;
    int value2 = 2;
    using result_t = result<value_sum_constructor_args, error_sum_constructor_args>;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result_t res{in_place_value, value1, std::move(value2)};
    ASSERT_EQ(res.value().value, 3);
  }

  STATIC_ASSERT_FALSE(
      (is_constructible_v<result<value_sum_constructor_args, error_sum_constructor_args>, in_place_value_t, int, int>)
  );
}
/// @test @c arene::base::result<void,E> has an @c in_place_value constructor which consumes @c arene::base::monostate,
/// and after it is invoked @c arene::base::result<T,E>::has_value returns @c true .
TEST(ResultConstructor, InPlaceValueForVoidAcceptsMonostate) {
  constexpr result<void, char> res{in_place_value, monostate{}};
  STATIC_ASSERT_TRUE(res.has_value());
}
/// @test Invoking @c arene::base::result<T,E> 's @c in_place_value constructor perfectly forwards the arguments to the
/// constructor of @c T .
TEST(ResultConstructor, InPlaceValueProperlyForwardsArguments) {
  {
    int value1 = 1;
    int value2 = 2;
    using result_t = result<value_sum_constructor_args, error_sum_constructor_args>;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result_t res{in_place_value, value1, std::move(value2)};
    auto const& const_r = res;

    // Useful for checking the return type is perfect-forwarded.
    decltype(auto) r_v = res.value();
    decltype(auto) const_r_v = const_r.value();
    decltype(auto) rvalue_r_v = std::move(res).value();
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    decltype(auto) const_rvalue_r_v = std::move(const_r).value();

    // Check the types
    EXPECT_TRUE((is_same_v<decltype(r_v), value_sum_constructor_args&>));
    EXPECT_TRUE((is_same_v<decltype(const_r_v), value_sum_constructor_args const&>));
    EXPECT_TRUE((is_same_v<decltype(rvalue_r_v), value_sum_constructor_args&&>));
    EXPECT_TRUE((is_same_v<decltype(const_rvalue_r_v), value_sum_constructor_args const&&>));

    // Check the values
    EXPECT_EQ(r_v.value, 3);
    EXPECT_EQ(const_r_v.value, 3);
    EXPECT_EQ(rvalue_r_v.value, 3);
    EXPECT_EQ(const_rvalue_r_v.value, 3);
  }
}
/// @test Given an @c arene::base::result<T,E> constructed using @c in_place_error,
/// @c arene::base::result<T,E>::has_error returns @c true .
TEST(ResultConstructor, InPlaceErrorPopulatesErrorChannel) {
  {
    std::int64_t value1 = 1;
    std::int64_t value2 = 2;
    using result_t = result<value_sum_constructor_args, error_sum_constructor_args>;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result_t res{in_place_error, value1, std::move(value2)};
    ASSERT_EQ(res.error().value, 3);
  }

  STATIC_ASSERT_FALSE((is_constructible_v<
                       result<value_sum_constructor_args, error_sum_constructor_args>,
                       in_place_error_t,
                       std::int64_t,
                       std::int64_t>));
}
/// @test Invoking @c arene::base::result<T,E> 's @c in_place_error constructor perfectly forwards the arguments to the
/// constructor of @c E .
TEST(ResultConstructor, InPlaceErrorProperlyForwardsArguments) {
  {
    std::int64_t value1 = 1;
    std::int64_t value2 = 2;
    using result_t = result<value_sum_constructor_args, error_sum_constructor_args>;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result_t res{in_place_error, value1, std::move(value2)};
    auto const& const_r = res;

    // Useful for checking the return type is perfect-forwarded.
    decltype(auto) r_v = res.error();
    decltype(auto) const_r_v = const_r.error();
    decltype(auto) rvalue_r_v = std::move(res).error();
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    decltype(auto) const_rvalue_r_v = std::move(const_r).error();

    // Check the types
    EXPECT_TRUE((is_same_v<decltype(r_v), error_sum_constructor_args&>));
    EXPECT_TRUE((is_same_v<decltype(const_r_v), error_sum_constructor_args const&>));
    EXPECT_TRUE((is_same_v<decltype(rvalue_r_v), error_sum_constructor_args&&>));
    EXPECT_TRUE((is_same_v<decltype(const_rvalue_r_v), error_sum_constructor_args const&&>));

    // Check the values
    EXPECT_EQ(r_v.value, 3);
    EXPECT_EQ(const_r_v.value, 3);
    EXPECT_EQ(rvalue_r_v.value, 3);
    EXPECT_EQ(const_rvalue_r_v.value, 3);
  }
}
/// @test @c ::arene::base::result<T,E> is copy constructible if both
/// @c T and @c E are copy constructible.
TEST(ResultConstructor, IsCopyConstructibleIfValueAndErrorAreCopyConstructible) {
  using value_t = trivially_copy_constructible;
  using error_t = trivially_copy_constructible;
  // Value objects
  {
    int const value1 = 42;
    result<value_t, error_t> source_r{in_place_value, value1};
    result<value_t, error_t> const res{source_r};
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value().value, value1);
    ASSERT_TRUE(source_r.has_value());
    EXPECT_EQ(source_r.value().value, value1);
  }

  // Error objects
  {
    int const value1 = 42;
    result<value_t, error_t> source_r{in_place_error, value1};
    result<value_t, error_t> const res{source_r};
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().value, value1);
    ASSERT_FALSE(source_r.has_value());
    EXPECT_EQ(source_r.error().value, value1);
  }

  EXPECT_TRUE((is_copy_constructible_v<result<value_t, error_t>>));
  EXPECT_TRUE((is_nothrow_copy_constructible_v<result<value_t, error_t>>));
  EXPECT_FALSE((is_nothrow_copy_constructible_v<result<fallible_copy_constructible, error_t>>));
  EXPECT_FALSE((is_nothrow_copy_constructible_v<result<value_t, fallible_copy_constructible>>));
  EXPECT_FALSE((is_nothrow_copy_constructible_v<result<fallible_copy_constructible, fallible_copy_constructible>>));
}
/// @test @c arene::base::result<T,E> 's copy constructor copies the currently populated channel .
TEST(ResultConstructor, CopyOnlyCopiesActiveChannel) {
  static int value_copies = 0;
  struct mock_value {
    mock_value() = default;
    mock_value(mock_value const&) { ++value_copies; }
    mock_value(mock_value&&) = delete;

    auto operator=(mock_value const&) -> mock_value& = delete;
    auto operator=(mock_value&&) -> mock_value& = delete;
    ~mock_value() = default;
  };
  static int error_copies = 0;
  struct mock_error {
    mock_error() = default;
    mock_error(mock_error const&) { ++error_copies; }

    mock_error(mock_error&&) = delete;

    auto operator=(mock_error const&) -> mock_error& = delete;
    auto operator=(mock_error&&) -> mock_error& = delete;
    ~mock_error() = default;
  };

  // Value Objects
  {
    value_copies = 0;
    error_copies = 0;
    result<mock_value, mock_error> const r_source{in_place_value};
    EXPECT_EQ(value_copies, 0);
    EXPECT_EQ(error_copies, 0);
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    result<mock_value, mock_error> const res{r_source};
    EXPECT_EQ(value_copies, 1);
    EXPECT_EQ(error_copies, 0);
  }
  // Error Objects
  {
    value_copies = 0;
    error_copies = 0;
    result<mock_value, mock_error> const r_source{in_place_error};
    EXPECT_EQ(value_copies, 0);
    EXPECT_EQ(error_copies, 0);
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    result<mock_value, mock_error> const res{r_source};
    EXPECT_EQ(value_copies, 0);
    EXPECT_EQ(error_copies, 1);
  }
}

/// @test @c arene::base::result<T,E> is move-constructable if both @c T and @c E are move-constructable, and move
/// construction moves the currently populated channel.
TEST(ResultConstructor, IsMoveConstructibleIfValueAndErrorAreMoveConstructible) {
  using value_t = move_only_value_sum_constructor_args;
  using error_t = move_only_error_sum_constructor_args;
  // Value objects
  {
    int value1 = 1;
    int value2 = 2;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> source_r{in_place_value, value1, std::move(value2)};
    result<value_t, error_t> const res = std::move(source_r);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value().value, 3);
  }

  // Error objects
  {
    std::int64_t value1 = 1;
    std::int64_t value2 = 2;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> source_r{in_place_error, value1, std::move(value2)};
    result<value_t, error_t> const res = std::move(source_r);
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().value, 3);
  }

  EXPECT_FALSE((is_nothrow_move_constructible_v<result<fallible_move_constructible, error_t>>));
  EXPECT_FALSE((is_nothrow_move_constructible_v<result<value_t, fallible_move_constructible>>));
  EXPECT_FALSE((is_nothrow_move_constructible_v<result<fallible_move_constructible, fallible_move_constructible>>));
}

/// @test @c arene::base::result<T,E> may be constructed from types for either @c T or @c E which have non-trivial
/// destructors.
TEST(ResultConstructor, MayBeConstructedWitTypeWithNonTrivialDestructor) {
  struct non_trivial {
    explicit non_trivial(std::reference_wrapper<int> x_arg)
        : value(x_arg) {}
    ~non_trivial() { value = 0; }
    non_trivial(non_trivial const&) = default;
    non_trivial(non_trivial&&) = default;
    auto operator=(non_trivial const&) -> non_trivial& = delete;
    auto operator=(non_trivial&&) -> non_trivial& = delete;

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    int& value;
  };
  using value_t = non_trivial;
  using error_t = int;
  int value1 = 1;
  int value2 = 2;
  EXPECT_EQ(value1, 1);
  EXPECT_EQ(value2, 2);
  {
    result<value_t, error_t> const value_a(in_place_value, value1);
    result<value_t, error_t> const value_b(in_place_value, value2);
    EXPECT_EQ(value_a.value().value, 1);
    EXPECT_EQ(value_b.value().value, 2);
  }
  EXPECT_EQ(value1, 0);
  EXPECT_EQ(value2, 0);
}

/// @test Given an @c arene::base::result<T,E> , @c has_value() returns @c true if the result's value channel is
/// populated.
TEST(ResultHasValue, IsTrueIfValueChannelIsPopulated) {
  // Positive cases
  {
    constexpr result<int, int> res(in_place_value);
    STATIC_ASSERT_TRUE(res.has_value());

    constexpr result<void, int> res_void(in_place_value);
    STATIC_ASSERT_TRUE(res_void.has_value());
  }

  // Negative cases
  {
    constexpr result<int, int> res(in_place_error);
    STATIC_ASSERT_FALSE(res.has_value());

    constexpr result<void, int> res_void(in_place_error);
    STATIC_ASSERT_FALSE(res_void.has_value());
  }
}
/// @test @c arene::base::result<T,E>::has_value() is @c noexcept
TEST(ResultHasValue, IsNoExcept) { STATIC_ASSERT_TRUE(noexcept(result<int, int>().has_value())); }
/// @test Given an @c arene::base::result<T,E> , @c has_value(value) returns @c true if the result's value channel is
/// populated and the value in it compares equal to the input @c value.
TEST(ResultHasValueWithArgument, IsTrueIFFResultValueIsPopulatedAndEqualToArgument) {
  constexpr int actual_value = 10;
  constexpr result<int, int> value_result{in_place_value, actual_value};
  STATIC_ASSERT_TRUE(value_result.has_value(actual_value));
  STATIC_ASSERT_FALSE(value_result.has_value(actual_value + 1));
  constexpr result<int, int> error_result{in_place_error, actual_value};
  STATIC_ASSERT_FALSE(error_result.has_value(actual_value));
}
/// @test @c arene::base::result<T,E>::has_value(value) is @c noexcept
TEST(ResultHasValueWithArgument, IsNoExcept) {
  STATIC_ASSERT_TRUE(noexcept(result<int, int>().has_value(std::declval<int>())));
}

/// @test Given an @c arene::base::result<T,E> , @c has_error() returns @c true if the result's error channel is
/// populated.
TEST(ResultHasError, IsTrueIfErrorChannelIsPopulated) {
  constexpr result<int, int> res(in_place_error);
  STATIC_ASSERT_TRUE(res.has_error());
}
/// @test Given an @c arene::base::result<T,E> , @c has_error() returns @c false if the result's error channel is
/// not populated.
TEST(ResultHasError, IsFalseIfErrorChannelIsNotPopulated) {
  constexpr result<int, int> res(in_place_value);
  STATIC_ASSERT_FALSE(res.has_error());
}
/// @test @c arene::base::result<T,E>::has_error() is @c noexcept
TEST(ResultHasError, IsNoExcept) { STATIC_ASSERT_TRUE(noexcept(result<int, int>().has_error())); }
/// @test Given an @c arene::base::result<T,E> , @c has_error(error) returns @c true if the result's error channel is
/// populated and the value in it compares equal to the input @c value.
TEST(ResultHasErrorWithArgument, IsTrueIFFResultErrorIsPopulatedAndEqualToArgument) {
  constexpr int actual_error = 10;
  constexpr result<int, int> error_result{in_place_error, actual_error};
  STATIC_ASSERT_TRUE(error_result.has_error(actual_error));
  STATIC_ASSERT_FALSE(error_result.has_error(actual_error + 1));
  constexpr result<int, int> value_result{in_place_value, actual_error};
  STATIC_ASSERT_FALSE(value_result.has_error(actual_error));
}
/// @test @c arene::base::result<T,E>::has_error(error) is @c noexcept
TEST(ResultHasErrorWithArgument, IsNoExcept) {
  STATIC_ASSERT_TRUE(noexcept(result<int, int>().has_error(std::declval<int>())));
}
/// @test Given an @c arene::base::result<T,E> with its value channel populated, converting it to @c bool is equivalent
/// to @c arene::base::result<T,E>::has_value()
TEST(ResultBooleanConversion, IsTrueIfValueChannelIsPopulated) {
  // Make sure that the conversion exists but is explicit
  STATIC_ASSERT_TRUE((std::is_constructible<bool, result<int, int>>::value));
  STATIC_ASSERT_FALSE((std::is_convertible<result<int, int>, bool>::value));

  // Positive cases
  {
    constexpr result<int, int> res(in_place_value);
    STATIC_ASSERT_TRUE(static_cast<bool>(res));

    constexpr result<void, int> res_void(in_place_value);
    STATIC_ASSERT_TRUE(static_cast<bool>(res_void));
  }
}
/// @test Given an @c arene::base::result<T,E> with its error channel populated, converting it to @c bool is equivalent
/// to @c arene::base::result<T,E>::has_value()
TEST(ResultBooleanConversion, IsFalseIfValueChannelIsNotPopulated) {
  constexpr result<int, int> res(in_place_error);
  STATIC_ASSERT_FALSE(static_cast<bool>(res));

  constexpr result<void, int> res_void(in_place_error);
  STATIC_ASSERT_FALSE(static_cast<bool>(res_void));
}
/// @test @c arene::base::result<T,E> 's boolean conversion operator is @c noexcept .
TEST(ResultBooleanConversion, IsNoExcept) {
  static_assert(noexcept(static_cast<bool>(result<int, int>())), "operator bool should be noexcept");
}

/// @test @c arene::base::result<T,E>::value() 's return type is @c T with matching @c cref qualification to the
/// instance it is invoked on.
TEST(ResultValue, ForNonVoidReturnTypeIsValueTypeAndMatchesCRefQualificationOfResult) {
  using result_t = result<int, int>;
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t&>().value()), result_t::value_type&>();
  }
  {
    SCOPED_TRACE("const lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t const&>().value()), result_t::value_type const&>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t&&>().value()), result_t::value_type&&>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t const&&>().value()), result_t::value_type const&&>();
  }
}
/// @test @c arene::base::result<void,E>::value() 's return type is @c void .
TEST(ResultValue, ForVoidReturnTypeIsVoid) {
  using result_t = result<void, int>;
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t&>().value()), void>();
  }
  {
    SCOPED_TRACE("const lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t const&>().value()), void>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t&&>().value()), void>();
  }
  {
    SCOPED_TRACE("const rvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t const&&>().value()), void>();
  }
}

/// @test @c arene::base::result<T,E>::error() 's return type is @c E with matching @c cref qualification to the
/// instance it is invoked on.
TEST(ResultError, ReturnTypeIsErrorTypeAndMatchesCRefQualificationOfResult) {
  using result_t = result<int, int>;
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t&>().error()), result_t::error_type&>();
  }
  {
    SCOPED_TRACE("const lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t const&>().error()), result_t::error_type const&>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t&&>().error()), result_t::error_type&&>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t const&&>().error()), result_t::error_type const&&>();
  }
}

/// @test Given an @c arene::base::result<T,E> for which @c has_value() returns @c true , @c value_or(default_value)
/// returns a reference to the held value in the @c result.
TEST(ResultValueOr, ReturnsValueIfValueChannelIsPopulated) {
  using result_t = result<int, int>;
  constexpr int original_value = 10;
  constexpr int default_value = 15;
  {
    SCOPED_TRACE("lvalue");
    constexpr result_t result{in_place_value, original_value};
    STATIC_ASSERT_EQ(result.value_or(default_value), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    STATIC_ASSERT_EQ((result_t{in_place_value, original_value}).value_or(default_value), original_value);
  }
}
/// @test Given an @c arene::base::result<T,E> for which @c has_error() returns @c true , @c value_or(default_value)
/// returns a copy of the @c default_value argument.
TEST(ResultValueOr, ReturnsDefaultValueIfErrorChannelIsPopulated) {
  using result_t = result<int, int>;
  constexpr int error = 10;
  constexpr int default_value = 15;
  {
    SCOPED_TRACE("lvalue");
    constexpr result_t result{in_place_error, error};
    STATIC_ASSERT_EQ(result.value_or(default_value), default_value);
  }
  {
    SCOPED_TRACE("rvalue");
    STATIC_ASSERT_EQ((result_t{in_place_error, error}).value_or(default_value), default_value);
  }
}
/// @test Given an @c arene::base::result<T,E>::value_or(default_value) can accept any @c default_value which is
/// convertible to @c T .
TEST(ResultValueOr, CanConsumeTypeConvertibleToValue) {
  using result_t = result<arene::base::string_view, int>;
  int error = 10;
  auto const* const default_value = "default";
  {
    SCOPED_TRACE("lvalue");
    result_t result{in_place_error, error};  // NOLINT(misc-const-correctness) this must be a non-const lvalue
    EXPECT_EQ(result.value_or(default_value), default_value);
  }
  {
    SCOPED_TRACE("rvalue");
    EXPECT_EQ((result_t{in_place_error, error}).value_or(default_value), default_value);
  }
}

/// @test Given an @c arene::base::result<T,E> for which @c has_value() returns @c true , @c operator->() returns a
/// pointer to the held value in the @c result.
TEST(ResultArrowOperator, ReturnsPointerToValue) {
  using result_t = result<int, int>;
  {
    SCOPED_TRACE("lvalue");
    result_t result{in_place_value, 10};
    EXPECT_EQ(result.operator->(), &result.value());
  }
  {
    SCOPED_TRACE("const lvalue");
    result_t const result{in_place_value, 10};
    EXPECT_EQ(result.operator->(), &result.value());
  }
}
/// @test The return type of @c arene::base::result<T,E>::operator->() matches the @c const -qualification of the
/// instance it is invoked on.
TEST(ResultArrowOperator, ConstQualificationOfReturnMatchesResults) {
  using result_t = result<int, int>;
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t&>().operator->()), result_t::value_type*>();
  }
  {
    SCOPED_TRACE("const lvalue");
    ::testing::StaticAssertTypeEq<decltype(std::declval<result_t const&>().operator->()), result_t::value_type const*>(
    );
  }
}

/// @test The return type of @c arene::base::result<T,E>::operator*() matches the @c cref -qualification of the instance
/// it is invoked on.
TEST(ResultDeferenceOperator, TypeOfReturnIsReferenceToValueWithCRefMatchingResult) {
  using result_t = result<int, float>;
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<decltype(*std::declval<result_t&>()), result_t::value_type&>();
  }
  {
    SCOPED_TRACE("const lvalue");
    ::testing::StaticAssertTypeEq<decltype(*std::declval<result_t const&>()), result_t::value_type const&>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<decltype(*std::declval<result_t&&>()), result_t::value_type&&>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<decltype(*std::declval<result_t const&&>()), result_t::value_type const&&>();
  }
}
/// @test Given an @c arene::base::result<T,E> for which
/// @c has_value() returns @c true , @c operator*() returns
/// a reference
/// to the held value in the @c result.
TEST(ResultDeferenceOperator, ReturnsValue) {
  using result_t = result<int, float>;
  constexpr int original_value = 10;
  {
    SCOPED_TRACE("lvalue");
    result_t result{in_place_value, original_value};
    EXPECT_EQ(*result, original_value);
  }
  {
    SCOPED_TRACE("const lvalue");
    result_t const result{in_place_value, original_value};
    EXPECT_EQ(*result, original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    result_t result{in_place_value, original_value};
    EXPECT_EQ(*std::move(result), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    result_t const result{in_place_value, original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing const rvalue
    EXPECT_EQ(*std::move(result), original_value);
  }
}
/// @test Given an @c arene::base::result<void,E> for which
/// @c has_value() returns @c true , it is valid to invoke
/// @c operator*() .
TEST(ResultDeferenceOperator, VoidValueResultReturnsVoid) {
  {
    SCOPED_TRACE("lvalue");
    result<void, int> result{};  // NOLINT(misc-const-correctness) this must be a non-const lvalue
    ::testing::StaticAssertTypeEq<decltype(*result), void>();
    static_cast<void>(*result);
    SUCCEED();
  }
  {
    SCOPED_TRACE("rvalue");
    result<void, int> result{};  // NOLINT(misc-const-correctness) this must be a non-const lvalue
    ::testing::StaticAssertTypeEq<decltype(*std::move(result)), void>();
    static_cast<void>(*std::move(result));
    SUCCEED();
  }
}

/// @test @c arene::base::result<T,E> is copy-assignable if both @c T and @c E are copy-assignable, and copy-assigning
/// an instance sets the active channel of the assigned-to instance to a copy of the currently active channel of the
/// assigned-from instance.
TEST(ResultAssignment, IsCopyAssignableIfValueAndErrorAreCopyAssignable) {
  using value_t = trivially_copy_constructible;
  using error_t = trivially_copy_constructible;

  // value lhs, value rhs
  {
    int const value1 = 1;
    result<value_t, error_t> source_r{in_place_value, value1};
    int const value3 = 3;
    result<value_t, error_t> res{in_place_value, value3};
    res = source_r;
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value().value, value1);
    ASSERT_TRUE(source_r.has_value());
    EXPECT_EQ(source_r.value().value, value1);
  }

  // error lhs, value rhs
  {
    int const value1 = 1;
    result<value_t, error_t> source_r{in_place_value, value1};
    int const value3 = 3;
    result<value_t, error_t> res{in_place_error, value3};
    res = source_r;
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value().value, value1);
    ASSERT_TRUE(source_r.has_value());
    EXPECT_EQ(source_r.value().value, value1);
  }

  // value lhs, error rhs
  {
    int const value1 = 1;
    result<value_t, error_t> source_r{in_place_error, value1};
    int const value3 = 3;
    result<value_t, error_t> res{in_place_value, value3};
    res = source_r;
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().value, value1);
    ASSERT_FALSE(source_r.has_value());
    EXPECT_EQ(source_r.error().value, value1);
  }

  // error lhs, error rhs
  {
    int const value1 = 1;
    result<value_t, error_t> source_r{in_place_error, value1};
    int const value3 = 3;
    result<value_t, error_t> res{in_place_error, value3};
    res = source_r;
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().value, value1);
    ASSERT_FALSE(source_r.has_value());
    EXPECT_EQ(source_r.error().value, value1);
  }

  // Check that all of the above were nothrow...
  EXPECT_TRUE((is_nothrow_copy_assignable_v<result<value_t, error_t>>));
}
/// @test It is possible to chain copy-assignments of @c arene::base::result<T,E> .
TEST(ResultAssignment, CopyAssignChaining) {
  result<int, int> const source_r{in_place_value, 42};
  result<int, int> res{in_place_value, 0};

  result<int, int>& rerror_t = (res = source_r);
  EXPECT_EQ(&rerror_t, &res);
}
/// @test @c arene::base::result<T,E> is move-assignable if both @c T and @c E are move-assignable, and move-assigning
/// an instance sets the active channel of the assigned-to instance to a value produced by moving the currently active
/// channel of the assigned-from instance.
TEST(ResultAssignment, IsMoveAssignableIfValueAndErrorAreMoveAssignable) {
  using value_t = move_only_value_sum_constructor_args;
  using error_t = move_only_error_sum_constructor_args;

  // value lhs, value rhs
  {
    int value1 = 1;
    int value2 = 2;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> source_r{in_place_value, value1, std::move(value2)};
    int value3 = 3;
    int value4 = 4;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> res{in_place_value, value3, std::move(value4)};
    res = std::move(source_r);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value().value, 3);
  }

  // error lhs, value rhs
  {
    int value1 = 1;
    int value2 = 2;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> source_r{in_place_value, value1, std::move(value2)};
    std::int64_t value3 = 3;
    std::int64_t value4 = 4;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> res{in_place_error, value3, std::move(value4)};
    res = std::move(source_r);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value().value, 3);
  }

  // value lhs, error rhs
  {
    std::int64_t value1 = 1;
    std::int64_t value2 = 2;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> source_r{in_place_error, value1, std::move(value2)};
    int value3 = 3;
    int value4 = 4;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> res{in_place_value, value3, std::move(value4)};
    res = std::move(source_r);
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().value, 3);
  }

  // error lhs, error rhs
  {
    std::int64_t value1 = 1;
    std::int64_t value2 = 2;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> source_r{in_place_error, value1, std::move(value2)};
    std::int64_t value3 = 3;
    std::int64_t value4 = 4;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result<value_t, error_t> res{in_place_error, value3, std::move(value4)};
    res = std::move(source_r);
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().value, 3);
  }

  // Check that all of the above were nothrow...
  EXPECT_TRUE((is_nothrow_move_assignable_v<result<value_t, error_t>>));
}
/// @test Given an instance of @c arene::base::result<T,E> for which @c has_value() is @c true and @c T is non-trivial,
/// using it as the right hand operand of a move-assignment actually moves the assigned-from value.
TEST(ResultAssignment, MoveAssignWithNonTrivialValueMovesValue) {
  result<arene::base::string_view, int> source{in_place_value, "hello"};
  result<arene::base::string_view, int> value_destination{in_place_value, "old"};
  result<arene::base::string_view, int> error_destination{in_place_error, 42};

  value_destination = std::move(source);
  ASSERT_EQ(*value_destination, "hello");
  source = {in_place_value, "hello"};
  error_destination = std::move(source);
  ASSERT_EQ(*error_destination, "hello");
}
/// @test Given an instance of @c arene::base::result<T,E> for which @c has_error() is @c true and @c E is non-trivial,
/// using it as the right hand operand of a move-assignment actually moves the assigned-from error.
TEST(ResultAssignment, MoveAssignWithNonTrivialErrorMovesError) {
  result<int, arene::base::string_view> source{in_place_error, "hello"};
  result<int, arene::base::string_view> value_destination{in_place_value, 42};
  result<int, arene::base::string_view> error_destination{in_place_error, "old"};

  value_destination = std::move(source);
  ASSERT_EQ(value_destination.error(), "hello");
  source = {in_place_error, "hello"};
  error_destination = std::move(source);
  ASSERT_EQ(error_destination.error(), "hello");
}
/// @test Given an instance of @c arene::base::result<void,E> for which @c has_value() is @c true , using it as the
/// right hand operand of an assignment will set the assigned to result to its value-channel.
TEST(ResultAssignment, VoidValueAssignmentFromValue) {
  using result_t = result<void, int>;
  {
    SCOPED_TRACE("lvalue");
    result_t const lvalue_value_assign_from{in_place_value};
    result_t initially_value{in_place_value};
    initially_value = lvalue_value_assign_from;
    EXPECT_TRUE(initially_value.has_value());

    result_t initially_error{in_place_error, 10};
    initially_error = lvalue_value_assign_from;
    EXPECT_TRUE(initially_error.has_value());
  }
  {
    SCOPED_TRACE("rvalue");
    result_t initially_value{in_place_value};
    initially_value = result_t{in_place_value};
    EXPECT_TRUE(initially_value.has_value());

    result_t initially_error{in_place_error, 10};
    initially_error = result_t{in_place_value};
    EXPECT_TRUE(initially_error.has_value());
  }
}
/// @test Given an instance of @c arene::base::result<void,E> for which @c has_value() is @c false , using it as the
/// right hand operand of an assignment will set the assigned to result to its error-channel.
TEST(ResultAssignment, VoidValueAssignmentFromError) {
  using result_t = result<void, int>;
  constexpr int expected_error = 10;
  {
    SCOPED_TRACE("lvalue");
    result_t const lvalue_error_assign_from{in_place_error, expected_error};

    result_t initially_value{in_place_value};
    initially_value = lvalue_error_assign_from;
    ASSERT_TRUE(initially_value.has_error());
    EXPECT_EQ(initially_value.error(), lvalue_error_assign_from.error());

    result_t initially_error{in_place_error, expected_error + 1};
    initially_error = lvalue_error_assign_from;
    ASSERT_TRUE(initially_error.has_error());
    EXPECT_EQ(initially_error.error(), lvalue_error_assign_from.error());
  }
  {
    SCOPED_TRACE("rvalue");
    result_t initially_value{in_place_value};
    initially_value = result_t{in_place_error, expected_error};
    ASSERT_TRUE(initially_value.has_error());
    EXPECT_EQ(initially_value.error(), expected_error);

    result_t initially_error{in_place_error, expected_error + 1};
    initially_error = result_t{in_place_error, expected_error};
    ASSERT_TRUE(initially_error.has_error());
    EXPECT_EQ(initially_error.error(), expected_error);
  }
}
/// @test It is possible to chain move-assignments of @c arene::base::result<T,E> .
TEST(ResultAssignment, MoveAssignChaining) {
  using value_t = move_only_value_sum_constructor_args;
  using error_t = move_only_error_sum_constructor_args;
  int value1 = 1;
  int value2 = 2;
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  result<value_t, error_t> source_r{in_place_value, value1, std::move(value2)};
  int value3 = 3;
  int value4 = 4;
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  result<value_t, error_t> res{in_place_value, value3, std::move(value4)};

  result<value_t, error_t>& rerror_t = (res = std::move(source_r));
  EXPECT_EQ(&rerror_t, &res);
}

/// @test Given two instances of @c arene::base::result<T,E> @c lhs and @c rhs, they are equality comparable if both @c
/// T and @c E are equality comparable. The result of the comparison is:
/// * @c false if @c lhs.has_value() != @c rhs.has_value()
/// * Otherwise, equivalent to comparing instances of the currently active channel in @c lhs and @c rhs .
TEST(ResultComparison, IsEqualityComparable) {
  {
    // value_type and error_type to be used in the following tests
    using value_t = int;
    using error_t = int;

    {
      // LHS objects
      result<value_t, error_t> const value_0a(in_place_value, 0);
      result<value_t, error_t> const error_0a(in_place_error, 0);

      // RHS objects
      result<value_t, error_t> const value_0b(in_place_value, 0);
      result<value_t, error_t> const value_5b(in_place_value, 5);
      result<value_t, error_t> const error_0b(in_place_error, 0);
      result<value_t, error_t> const error_5b(in_place_error, 5);

      // Same value
      EXPECT_TRUE(value_0a == value_0b);
      EXPECT_FALSE(value_0a != value_0b);

      // Different value
      EXPECT_FALSE(value_0a == value_5b);
      EXPECT_TRUE(value_0a != value_5b);

      // Same error
      EXPECT_TRUE(error_0a == error_0b);
      EXPECT_FALSE(error_0a != error_0b);

      // Different error
      EXPECT_FALSE(error_0a == error_5b);
      EXPECT_TRUE(error_0a != error_5b);

      // Different kind (value lhs)
      EXPECT_FALSE(value_0a == error_0b);
      EXPECT_TRUE(value_0a != error_0b);

      // Different kind (error lhs)
      EXPECT_FALSE(error_0a == value_0b);
      EXPECT_TRUE(error_0a != value_0b);
    }
  }
}
/// @test Given two instances of @c arene::base::result<void,E> @c lhs and @c rhs, they are equality comparable if @c E
/// is equality comparable. The result of the comparison is:
/// * @c false if @c lhs.has_value() != @c rhs.has_value()
/// * @c true if @c lhs.has_value() and @c rhs.has_value()
/// * Otherwise, equivalent to comparing the @c E instances held in the error channels of @c lhs and @c rhs .
TEST(ResultCompare, ComparisonWithVoidValues) {
  {
    // value_type and error_type to be used in the following tests
    using value_t = void;
    using error_t = int;

    {
      // LHS objects
      result<value_t, error_t> const value_a(in_place_value);
      result<value_t, error_t> const error_a(in_place_error);

      // RHS objects
      result<value_t, error_t> const value_b(in_place_value);
      result<value_t, error_t> const error_b(in_place_error);

      // Same value
      EXPECT_TRUE(value_a == value_b);
      EXPECT_FALSE(value_a != value_b);

      // Same error
      EXPECT_TRUE(error_a == error_b);
      EXPECT_FALSE(error_a != error_b);

      // Different kind (value lhs)
      EXPECT_FALSE(value_a == error_b);
      EXPECT_TRUE(value_a != error_b);

      // Different kind (error lhs)
      EXPECT_FALSE(error_a == value_b);
      EXPECT_TRUE(error_a != value_b);
    }
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , the result of @c std::hash<arene::base::result<T,E>>{}() is
/// equivalent to invoking @c std::hash on the currently active channel.
TEST(ResultHash, StdHashIsHashOfActiveChannel) {
  {
    SCOPED_TRACE("non-void value");
    using result_t = result<int, double>;
    auto const value_result = result_t{in_place_value, 10};
    // we can't use STATIC_ASSERT_EQ because apparently hash<int>::operator() isn't constexpr in C++14.
    ASSERT_EQ(std::hash<result_t>{}(value_result), std::hash<result_t::value_type>{}(value_result.value()));
    auto const error_result = result_t{in_place_error, 1.1234};
    ASSERT_EQ(std::hash<result_t>{}(error_result), std::hash<result_t::error_type>{}(error_result.error()));
  }
  {
    SCOPED_TRACE("void value");
    using result_t = result<void, double>;
    auto const value_result = result_t{in_place_value};
    // we can't use STATIC_ASSERT_EQ because apparently hash<int>::operator() isn't constexpr in C++14.
    ASSERT_EQ(std::hash<result_t>{}(value_result), std::hash<arene::base::monostate>{}(arene::base::monostate{}));
    auto const error_result = result_t{in_place_error, 1.1234};
    ASSERT_EQ(std::hash<result_t>{}(error_result), std::hash<result_t::error_type>{}(error_result.error()));
  }
}

// A type which can only be direct initialized, and thus only emplaced.
template <typename T>
struct only_emplacible {
  T value;
  explicit only_emplacible(T val) noexcept
      : value(val) {}
  ~only_emplacible() = default;
  only_emplacible(only_emplacible const&) noexcept = delete;
  only_emplacible(only_emplacible&&) noexcept = delete;
  auto operator=(only_emplacible const&) noexcept -> only_emplacible& = delete;
  auto operator=(only_emplacible&&) noexcept -> only_emplacible& = delete;
};
/// @test Given an instance of @c arene::base::result<T,E> , invoking @c emplace(in_place_value,args...) populates the
/// value channel with an instance equivalent to in-place-constructing  @c T with @c args... . invoking
/// @c emplace(in_place_error,args...) populates the error channel with an instance equivalent to in-place-constructing
/// @c E with @c args... .
TEST(ResultEmplace, EmplacesContentIntoResult) {
  using result_t = result<only_emplacible<int>, only_emplacible<double>>;
  result_t result{in_place_value, 1};

  constexpr double expected_error{1.234};
  result.emplace(in_place_error, expected_error);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Not performing any floating point arithmetic");

  ASSERT_TRUE(result.has_error());
  EXPECT_EQ(result.error().value, expected_error);

  constexpr int expected_value{10};
  result.emplace(in_place_value, expected_value);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().value, expected_value);

  ARENE_IGNORE_END();
}
/// @test Given an instance of @c arene::base::result<T,E> , the return from @c emplace is a non- @c const reference to
/// the instance of @c T or @c E that was constructed.
TEST(ResultEmplace, ReturnsNonConstReferenceToEmplacedContent) {
  using result_t = result<only_emplacible<int>, only_emplacible<double>>;
  result_t result{in_place_value, 1};

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Not performing any floating point arithmetic");

  constexpr double expected_error{1.234};
  auto& emplaced_error = result.emplace(in_place_error, expected_error);
  EXPECT_EQ(emplaced_error.value, expected_error);
  emplaced_error.value = -expected_error;
  EXPECT_EQ(result.error().value, -expected_error);

  constexpr int expected_value{10};
  auto& emplaced_value = result.emplace(in_place_value, expected_value);
  EXPECT_EQ(emplaced_value.value, expected_value);
  emplaced_value.value = -expected_value;
  EXPECT_EQ(result.value().value, -expected_value);

  ARENE_IGNORE_END();
}

template <typename Res, typename Which, typename... Args>
using use_emplace = decltype(std::declval<Res>().emplace(Which{}, std::forward<Args>(std::declval<Args>())...));

/// @test @c arene::base::result<T,E>::emplace(in_place_value,args...) requires that @c T satisfy
/// @c std::is_nothrow_constructible<T,Args...> . @c arene::base::result<T,E>::emplace(in_place_error,args...) requires
/// that @c E satisfy @c std::is_nothrow_constructible<E,Args...> .
TEST(ResultEmplace, IsInvocableOnlyForNothrowConstructible) {
  // NOLINTNEXTLINE(hicpp-special-member-functions)
  struct throws_on_copy_constructor {
    throws_on_copy_constructor() = default;
    // NOLINTNEXTLINE(hicpp-use-equals-default) gcc11 will ignore the noexcept clause if this function is defaulted
    throws_on_copy_constructor(throws_on_copy_constructor const&) noexcept(false) {}
    throws_on_copy_constructor(throws_on_copy_constructor&&) = default;
  };

  using result_t = result<throws_on_copy_constructor, throws_on_copy_constructor>;
  STATIC_ASSERT_TRUE(
      (arene::base::substitution_succeeds<use_emplace, result_t, in_place_value_t, throws_on_copy_constructor&&>)
  );
  STATIC_ASSERT_TRUE(
      (arene::base::substitution_succeeds<use_emplace, result_t, in_place_error_t, throws_on_copy_constructor&&>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::substitution_succeeds<use_emplace, result_t, in_place_value_t, throws_on_copy_constructor const&>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::substitution_succeeds<use_emplace, result_t, in_place_error_t, throws_on_copy_constructor const&>)
  );
}

template <typename Result>
using use_swap = decltype(std::declval<Result&>().swap(std::declval<Result&>()));
/// @test @c arene::base::result<T,E>::swap(other) requires that both @c T and @c E satisfy @c arene::base::is_swappable
TEST(ResultSwap, IsInvocableOnlyIfBothValueAndErrorModelSwappable) {
  struct non_swappable {
    ~non_swappable() = default;
    non_swappable(non_swappable const&) = delete;
    non_swappable(non_swappable&&) = delete;
    auto operator=(non_swappable const&) -> non_swappable& = delete;
    auto operator=(non_swappable&&) -> non_swappable& = delete;
  };
  using value_not_swappable = result<non_swappable, int>;
  using error_not_swappable = result<int, non_swappable>;
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<use_swap, value_not_swappable>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<use_swap, error_not_swappable>);
}
/// @test @c arene::base::result<T,E>::swap(other) requires that both @c T and @c E satisfy
/// @c std::is_nothrow_move_constructable .
TEST(ResultSwap, IsInvocableOnlyIfBothValueAndErrorAreNothrowMoveConstructible) {
  struct throwing_move {
    ~throwing_move() = default;
    throwing_move(throwing_move const&) = delete;
    throwing_move(throwing_move&&) noexcept(false) = delete;
    auto operator=(throwing_move const&) -> throwing_move& = delete;
    auto operator=(throwing_move&&) noexcept(false) -> throwing_move& = default;
  };
  using value_not_nothrow_move_ctor = result<throwing_move, int>;
  using error_not_nothrow_move_ctor = result<int, throwing_move>;
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<use_swap, value_not_nothrow_move_ctor>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<use_swap, error_not_nothrow_move_ctor>);
}
/// @test Given two instances of @c arene::base::result<T,E>, @c lhs and @c rhs , where both instances' @c has_value()
/// returns @c true; when @c lhs.swap(rhs) is invoked, then it is equivalent to invoking
/// @c ::arene::base::swap(lhs.value(),rhs.value()).
TEST(ResultSwap, IfBothAreValueTheValuesAreSwapped) {
  constexpr int original_lhs = 10;
  constexpr int original_rhs = -10;

  result<int, int> lhs{in_place_value, original_lhs};
  result<int, int> rhs{in_place_value, original_rhs};
  {
    SCOPED_TRACE("member");
    lhs.swap(rhs);
    EXPECT_TRUE(lhs.has_value(original_rhs));
    EXPECT_TRUE(rhs.has_value(original_lhs));
  }
  {
    SCOPED_TRACE("free function");
    swap(lhs, rhs);
    EXPECT_TRUE(lhs.has_value(original_lhs));
    EXPECT_TRUE(rhs.has_value(original_rhs));
  }
}
/// @test Given two instances of @c arene::base::result<T,E>, @c lhs and @c rhs , where both instances' @c has_error()
/// returns @c true; when @c lhs.swap(rhs) is invoked, then it is equivalent to invoking
/// @c ::arene::base::swap(lhs.error(),rhs.error()).
TEST(ResultSwap, IfBothAreErrorTheErrorsAreSwapped) {
  constexpr int original_lhs = 10;
  constexpr int original_rhs = -10;

  result<int, int> lhs{in_place_error, original_lhs};
  result<int, int> rhs{in_place_error, original_rhs};
  {
    SCOPED_TRACE("member");
    lhs.swap(rhs);
    EXPECT_TRUE(lhs.has_error(original_rhs));
    EXPECT_TRUE(rhs.has_error(original_lhs));
  }
  {
    SCOPED_TRACE("free function");
    swap(lhs, rhs);
    EXPECT_TRUE(lhs.has_error(original_lhs));
    EXPECT_TRUE(rhs.has_error(original_rhs));
  }
}
/// @test Given two instances of @c arene::base::result<T,E>, @c lhs and @c rhs , where
/// @c lhs.has_value()!=rhs.has_value(); when @c lhs.swap(rhs) is invoked, then it is equivalent to invoking
/// @c lhs.emplace(...) with a moved-from instance of the value in @c rhs 's pre-swap active channel, and invoking
/// @c rhs.emplace(...) on a moved-from instance of the value in @c lhs 's pre-swap active channel.
TEST(ResultSwap, IfStatesAreNotTheSameIsEquivalentToEmplacingTheStatesIntoEachOther) {
  constexpr int original_value = 10;
  constexpr int original_error = -10;

  result<int, int> lhs{in_place_value, original_value};
  result<int, int> rhs{in_place_error, original_error};
  {
    SCOPED_TRACE("member");
    lhs.swap(rhs);
    EXPECT_TRUE(lhs.has_error(original_error));
    EXPECT_TRUE(rhs.has_value(original_value));
  }
  {
    SCOPED_TRACE("free function");
    swap(lhs, rhs);
    EXPECT_TRUE(lhs.has_value(original_value));
    EXPECT_TRUE(rhs.has_error(original_error));
  }
}
/// @test Given two instances of @c arene::base::result<void,E>, @c lhs and @c rhs , where both instances'
/// @c has_value() returns @c true; when @c lhs.swap(rhs) is invoked, it is a no-op.
TEST(ResultSwap, VoidValueIfBothAreValueSwapIsANoop) {
  result<void, int> lhs{in_place_value};
  result<void, int> rhs{in_place_value};
  {
    SCOPED_TRACE("member");
    lhs.swap(rhs);
    EXPECT_TRUE(lhs.has_value());
    EXPECT_TRUE(rhs.has_value());
  }
  {
    SCOPED_TRACE("free function");
    swap(lhs, rhs);
    EXPECT_TRUE(lhs.has_value());
    EXPECT_TRUE(rhs.has_value());
  }
}
/// @test Given two instances of @c arene::base::result<void,E>, @c lhs and @c rhs , where both instances'
/// @c has_error() returns @c true; when @c lhs.swap(rhs) is invoked, then it is equivalent to invoking
/// @c ::arene::base::swap(lhs.error(),rhs.error()).
TEST(ResultSwap, VoidValueIfBothAreErrorTheErrorsAreSwapped) {
  constexpr int original_lhs = 10;
  constexpr int original_rhs = -10;

  result<void, int> lhs{in_place_error, original_lhs};
  result<void, int> rhs{in_place_error, original_rhs};
  {
    SCOPED_TRACE("member");
    lhs.swap(rhs);
    EXPECT_TRUE(lhs.has_error(original_rhs));
    EXPECT_TRUE(rhs.has_error(original_lhs));
  }
  {
    SCOPED_TRACE("free function");
    swap(lhs, rhs);
    EXPECT_TRUE(lhs.has_error(original_lhs));
    EXPECT_TRUE(rhs.has_error(original_rhs));
  }
}
/// @test Given two instances of @c arene::base::result<void,E>, @c lhs and @c rhs , where
/// @c lhs.has_value()!=rhs.has_value(); when @c lhs.swap(rhs) is invoked, then it is equivalent to invoking
/// @c lhs.emplace(...) with a moved-from instance of the value in @c rhs 's pre-swap active channel, and invoking
/// @c rhs.emplace(...) on a moved-from instance of the value in @c lhs 's pre-swap active channel.
TEST(ResultSwap, VoidValueIfStatesAreNotTheSameIsEquivalentToEmplacingTheStatesIntoEachOther) {
  constexpr int original_error = -10;

  result<void, int> lhs{in_place_value};
  result<void, int> rhs{in_place_error, original_error};
  {
    SCOPED_TRACE("member");
    lhs.swap(rhs);
    EXPECT_TRUE(lhs.has_error(original_error));
    EXPECT_TRUE(rhs.has_value());
  }
  {
    SCOPED_TRACE("free function");
    swap(lhs, rhs);
    EXPECT_TRUE(lhs.has_value());
    EXPECT_TRUE(rhs.has_error(original_error));
  }
}

struct visitor_base {
  using result_t = result<int, int>;
  enum parameter_qualification { lvalue, clvalue, rvalue, crvalue, not_called };
  parameter_qualification selected_overload = not_called;
};

struct monadic_visitor : visitor_base {
  ///
  /// @brief Overloads for introspecting a visited result.
  /// @return a result containing the value the functor was called with.
  /// @post selected_overload will be set to the state matching the overload that was selected.
  /// @{
  auto operator()(int& arg) -> result_t {
    selected_overload = parameter_qualification::lvalue;
    return result_t{in_place_value, arg};
  }
  auto operator()(int const& arg) -> result_t {
    selected_overload = parameter_qualification::clvalue;
    return result_t{in_place_value, arg};
  }
  auto operator()(int&& arg) -> result_t {
    selected_overload = parameter_qualification::rvalue;
    return result_t{in_place_value, arg};
  }
  auto operator()(int const&& arg) -> result_t {
    selected_overload = parameter_qualification::crvalue;
    return result_t{in_place_value, arg};
  }
  /// @}
};
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.and_then(value_handler) is invoked the provided @c value_handler callback is invoked
/// with a type matching the return type equivalent to that of invoking @c result.value() .
TEST(ResultAndThen, CallsProvidedVisitorWithValueQualifiedAsResultIfValueIsPopulated) {
  {
    SCOPED_TRACE("lvalue");
    monadic_visitor handle_value;
    monadic_visitor::result_t error_result{in_place_error, 10};
    error_result.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::not_called);
    handle_value = {};
    monadic_visitor::result_t value_result{in_place_value, 10};
    value_result.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::lvalue);
  }
  {
    SCOPED_TRACE("const-lvalue");
    monadic_visitor handle_value;
    monadic_visitor::result_t const error_result{in_place_error, 10};
    error_result.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::not_called);
    handle_value = {};
    monadic_visitor::result_t const result{in_place_value, 10};
    result.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::clvalue);
  }
  {
    SCOPED_TRACE("rvalue");
    monadic_visitor handle_value;
    monadic_visitor::result_t error_result{in_place_error, 10};
    std::move(error_result).and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::not_called);
    handle_value = {};
    monadic_visitor::result_t result{in_place_value, 10};
    std::move(result).and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::rvalue);
  }
  {
    SCOPED_TRACE("const-rvalue");
    monadic_visitor handle_value;
    monadic_visitor::result_t const error_result{in_place_error, 10};
    std::move(error_result).and_then(handle_value);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::not_called);
    handle_value = {};
    monadic_visitor::result_t const result{in_place_value, 10};
    std::move(result).and_then(handle_value);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_value.selected_overload, monadic_visitor::parameter_qualification::crvalue);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.and_then(value_handler) is invoked the provided @c value_handler callback is invoked
/// with the a value equivalent to invoking @c result.value() , matching reference qualification, and its return is
/// returned.
TEST(ResultAndThen, TheVisitorIsCalledWithTheHeldValue) {
  auto const original_value = monadic_visitor::result_t::value_type{10};
  monadic_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto result = monadic_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.and_then(handle_value).value(), original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = monadic_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.and_then(handle_value).value(), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = monadic_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(std::move(result).and_then(handle_value).value(), original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = monadic_visitor::result_t{in_place_value, original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).and_then(handle_value).value(), original_value);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_value() returns @c false ,
/// when @c result.and_then(value_handler) is invoked, @c value_handler is not invoked and a result equivalent to
/// copy-constructing the return type of @c value_handler from @c result.error() .
TEST(ResultAndThen, AResultWithACopyOfErrorIsReturnedIfValueIsNotPopulated) {
  auto const original_error = monadic_visitor::result_t::error_type{10};
  monadic_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto result = monadic_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.and_then(handle_value).error(), original_error);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = monadic_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.and_then(handle_value).error(), original_error);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = monadic_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(std::move(result).and_then(handle_value).error(), original_error);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = monadic_visitor::result_t{in_place_error, original_error};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).and_then(handle_value).error(), original_error);
  }
}
/// @test Given an instance of @c arene::base::result<void,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.and_then(value_handler) is invoked the provided @c value_handler callback is invoked
/// with no arguments.
TEST(ResultAndThen, TheProvidedHandlerIsInvokedWithoutArgumentsForVoidValue) {
  constexpr int return_value = 10;
  {
    SCOPED_TRACE("lvalue");
    result<void, int> void_result{in_place_value};
    EXPECT_TRUE(void_result.and_then([&return_value]() {
                             return result<int, int>{in_place_value, return_value};
                           }
    ).has_value(return_value));
  }
  {
    SCOPED_TRACE("const-lvalue");
    result<void, int> const void_result{in_place_value};
    EXPECT_TRUE(void_result.and_then([&return_value]() {
                             return result<int, int>{in_place_value, return_value};
                           }
    ).has_value(return_value));
  }
  {
    SCOPED_TRACE("rvalue");
    result<void, int> void_result{in_place_value};
    EXPECT_TRUE(std::move(void_result)
                    .and_then([&return_value]() {
                      return result<int, int>{in_place_value, return_value};
                    })
                    .has_value(return_value));
  }
  {
    SCOPED_TRACE("const-rvalue");
    result<void, int> void_result{in_place_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_TRUE(std::move(void_result)
                    .and_then([&return_value]() {
                      return result<int, int>{in_place_value, return_value};
                    })
                    .has_value(return_value));
  }
}

/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_error() returns @c true ,
/// when @c result.or_else(error_handler) is invoked the provided @c error_handler callback is invoked
/// with a type matching the return type equivalent to that of invoking @c result.error() .
TEST(ResultOrElse, CallsProvidedVisitorWithErrorQualifiedAsResultIfErrorIsPopulated) {
  {
    SCOPED_TRACE("lvalue");
    monadic_visitor handle_error;
    monadic_visitor::result_t error_result{in_place_error, 10};
    error_result.or_else(handle_error);
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::lvalue);
    handle_error = {};
    monadic_visitor::result_t value_result{in_place_value, 10};
    value_result.or_else(handle_error);
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-lvalue");
    monadic_visitor handle_error;
    monadic_visitor::result_t const error_result{in_place_error, 10};
    error_result.or_else(handle_error);
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::clvalue);
    handle_error = {};
    monadic_visitor::result_t const result{in_place_value, 10};
    result.or_else(handle_error);
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("rvalue");
    monadic_visitor handle_error;
    monadic_visitor::result_t error_result{in_place_error, 10};
    std::move(error_result).or_else(handle_error);
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::rvalue);
    handle_error = {};
    monadic_visitor::result_t result{in_place_value, 10};
    std::move(result).or_else(handle_error);
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-rvalue");
    monadic_visitor handle_error;
    monadic_visitor::result_t const error_result{in_place_error, 10};
    std::move(error_result).or_else(handle_error);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::crvalue);
    handle_error = {};
    monadic_visitor::result_t const result{in_place_value, 10};
    std::move(result).or_else(handle_error);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_error.selected_overload, monadic_visitor::parameter_qualification::not_called);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_error() returns @c true ,
/// when @c result.or_else(error_handler) is invoked the provided @c error_handler callback is invoked
/// with the a value equivalent to invoking @c result.error() , matching reference qualification, and its return is
/// returned.
TEST(ResultOrElse, TheVisitorIsCalledWithTheHeldError) {
  auto const original_error = monadic_visitor::result_t::error_type{10};
  monadic_visitor handle_error;
  // this looks a little weird, but the handler for the error returns a value-populated result.
  {
    SCOPED_TRACE("lvalue");
    auto result = monadic_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.or_else(handle_error).value(), original_error);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = monadic_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.or_else(handle_error).value(), original_error);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = monadic_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(std::move(result).or_else(handle_error).value(), original_error);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = monadic_visitor::result_t{in_place_error, original_error};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).or_else(handle_error).value(), original_error);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_error() returns @c false ,
/// when @c result.or_else(error_handler) is invoked, @c error_handler is not invoked and a result equivalent to
/// copy-constructing the return type of @c error_handler from @c result.value() .
TEST(ResultOrElse, AResultWithACopyOfValueIsReturnedIfErrorIsNotPopulated) {
  auto const original_value = monadic_visitor::result_t::value_type{10};
  monadic_visitor handle_error;
  {
    SCOPED_TRACE("lvalue");
    auto result = monadic_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.or_else(handle_error).value(), original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = monadic_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.or_else(handle_error).value(), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = monadic_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(std::move(result).or_else(handle_error).value(), original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = monadic_visitor::result_t{in_place_value, original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).or_else(handle_error).value(), original_value);
  }
}

struct transform_visitor : visitor_base {
  ///
  /// @brief Overloads for introspecting a visited result.
  /// @return a result containing the value the functor was called with.
  /// @post selected_overload will be set to the state matching the overload that was selected.
  /// @{
  auto operator()(int& arg) -> int {
    selected_overload = parameter_qualification::lvalue;
    return arg;
  }
  auto operator()(int const& arg) -> int {
    selected_overload = parameter_qualification::clvalue;
    return arg;
  }
  auto operator()(int&& arg) -> int {
    selected_overload = parameter_qualification::rvalue;
    return arg;
  }
  auto operator()(int const&& arg) -> int {
    selected_overload = parameter_qualification::crvalue;
    return arg;
  }
  /// @}
};
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.transform(value_transformer) is invoked the provided @c value_transformer callback is invoked
/// with a type matching the return type equivalent to that of invoking @c result.value() .
TEST(ResultTransform, CallsProvidedVisitorWithValueQualifiedAsResultIfValueIsPopulated) {
  {
    SCOPED_TRACE("lvalue");
    transform_visitor handle_value;
    transform_visitor::result_t error_result{in_place_error, 10};
    error_result.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::not_called);
    handle_value = {};
    transform_visitor::result_t value_result{in_place_value, 10};
    value_result.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::lvalue);
  }
  {
    SCOPED_TRACE("const-lvalue");
    transform_visitor handle_value;
    transform_visitor::result_t const error_result{in_place_error, 10};
    error_result.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::not_called);
    handle_value = {};
    transform_visitor::result_t const result{in_place_value, 10};
    result.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::clvalue);
  }
  {
    SCOPED_TRACE("rvalue");
    transform_visitor handle_value;
    transform_visitor::result_t error_result{in_place_error, 10};
    std::move(error_result).transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::not_called);
    handle_value = {};
    transform_visitor::result_t result{in_place_value, 10};
    std::move(result).transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::rvalue);
  }
  {
    SCOPED_TRACE("const-rvalue");
    transform_visitor handle_value;
    transform_visitor::result_t const error_result{in_place_error, 10};
    std::move(error_result).transform(handle_value);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::not_called);
    handle_value = {};
    transform_visitor::result_t const result{in_place_value, 10};
    std::move(result).transform(handle_value);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_value.selected_overload, transform_visitor::parameter_qualification::crvalue);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.transform(value_transformer) is invoked the provided @c value_transformer callback is invoked
/// with the a value equivalent to invoking @c result.value() , matching reference qualification, and a @c result<U,E>
/// produced from its return is returned.
TEST(ResultTransform, TheVisitorIsCalledWithTheHeldValue) {
  auto const original_value = transform_visitor::result_t::value_type{10};
  transform_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto result = transform_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.transform(handle_value).value(), original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = transform_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.transform(handle_value).value(), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = transform_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(std::move(result).transform(handle_value).value(), original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = transform_visitor::result_t{in_place_value, original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).transform(handle_value).value(), original_value);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_value() returns @c false ,
/// when @c result.transform(value_transformer) is invoked, @c value_transformer is not invoked and a result equivalent
/// to copy-constructing the return type of @c value_transformer from @c result.error()
TEST(ResultTransform, AResultWithACopyOfErrorIsReturnedIfValueIsNotPopulated) {
  auto const original_error = transform_visitor::result_t::error_type{10};
  transform_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto result = transform_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.transform(handle_value).error(), original_error);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = transform_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.transform(handle_value).error(), original_error);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = transform_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(std::move(result).transform(handle_value).error(), original_error);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = transform_visitor::result_t{in_place_error, original_error};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).transform(handle_value).error(), original_error);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.transform(value_transformer) is invoked with a @c value_transformer whose return type is @c void, the
/// provided @c value_transformer callback is invoked with the equivalent to invoking @c result.value() , then
/// @c result<void,E> is returned with its value channel populated.
TEST(ResultTransform, InvokesProvidedHandlerWithValueAndReturnsVoidResultIfVoidReturn) {
  constexpr int original_value = 10;
  {
    SCOPED_TRACE("lvalue");
    result<int, int> void_result{in_place_value, original_value};
    int called_with{};
    void_result.transform([&called_with](int arg) { called_with = arg; });
    EXPECT_EQ(called_with, original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    result<int, int> const void_result{in_place_value, original_value};
    int called_with{};
    void_result.transform([&called_with](int arg) { called_with = arg; });
    EXPECT_EQ(called_with, original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    result<int, int> void_result{in_place_value, original_value};
    int called_with{};
    std::move(void_result).transform([&called_with](int arg) { called_with = arg; });
    EXPECT_EQ(called_with, original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    result<int, int> const void_result{in_place_value, original_value};
    int called_with{};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    std::move(void_result).transform([&called_with](int arg) { called_with = arg; });
    EXPECT_EQ(called_with, original_value);
  }
}
/// @test Given an instance of @c arene::base::result<void,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.transform(value_transformer) is invoked the provided @c value_transformer callback is invoked
/// with no arguments and a @c result<U,E> produced from its return is returned.
TEST(ResultTransform, VoidValueInvokesProvidedHandlerAndReturnsResultWithValueIfNonVoidReturn) {
  constexpr int return_value = 10;
  {
    SCOPED_TRACE("lvalue");
    result<void, int> void_result{in_place_value};
    EXPECT_TRUE(void_result.transform([]() { return return_value; }).has_value(return_value));
  }
  {
    SCOPED_TRACE("const-lvalue");
    result<void, int> const void_result{in_place_value};
    EXPECT_TRUE(void_result.transform([]() { return return_value; }).has_value(return_value));
  }
  {
    SCOPED_TRACE("rvalue");
    result<void, int> void_result{in_place_value};
    EXPECT_TRUE(std::move(void_result).transform([]() { return return_value; }).has_value(return_value));
  }
  {
    SCOPED_TRACE("const-rvalue");
    result<void, int> void_result{in_place_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_TRUE(std::move(void_result).transform([]() { return return_value; }).has_value(return_value));
  }
}
/// @test Given an instance of @c arene::base::result<void,E> , @c result, where @c result.has_value() returns @c true ,
/// when @c result.transform(value_transformer) is invoked with a @c value_transformer whose return type is @c void, the
/// provided @c value_transformer callback is invoked with no arguments , then @c result<void,E> is returned with its
/// value channel populated.
TEST(ResultTransform, VoidValueInvokesProvidedHandlerAndReturnsVoidResultIfVoidReturn) {
  {
    SCOPED_TRACE("lvalue");
    result<void, int> void_result{in_place_value};
    bool was_called = false;
    void_result.transform([&was_called]() { was_called = true; });
    EXPECT_TRUE(was_called);
  }
  {
    SCOPED_TRACE("const-lvalue");
    result<void, int> const void_result{in_place_value};
    bool was_called = false;
    void_result.transform([&was_called]() { was_called = true; });
    EXPECT_TRUE(was_called);
  }
  {
    SCOPED_TRACE("rvalue");
    result<void, int> void_result{in_place_value};
    bool was_called = false;
    std::move(void_result).transform([&was_called]() { was_called = true; });
    EXPECT_TRUE(was_called);
  }
  {
    SCOPED_TRACE("const-rvalue");
    result<void, int> const void_result{in_place_value};
    bool was_called = false;
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    std::move(void_result).transform([&was_called]() { was_called = true; });
    EXPECT_TRUE(was_called);
  }
}

/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_error() returns @c true ,
/// when @c result.transform_error(error_transformer) is invoked the provided @c error_transformer callback is invoked
/// with a type matching the return type equivalent to that of invoking @c result.error() .
TEST(ResultTransformError, CallsProvidedVisitorWithErrorQualifiedAsResultIfErrorIsPopulated) {
  {
    SCOPED_TRACE("lvalue");
    transform_visitor handle_error;
    transform_visitor::result_t error_result{in_place_error, 10};
    error_result.transform_error(handle_error);
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::lvalue);
    handle_error = {};
    transform_visitor::result_t value_result{in_place_value, 10};
    value_result.transform_error(handle_error);
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-lvalue");
    transform_visitor handle_error;
    transform_visitor::result_t const error_result{in_place_error, 10};
    error_result.transform_error(handle_error);
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::clvalue);
    handle_error = {};
    transform_visitor::result_t const result{in_place_value, 10};
    result.transform_error(handle_error);
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("rvalue");
    transform_visitor handle_error;
    transform_visitor::result_t error_result{in_place_error, 10};
    std::move(error_result).transform_error(handle_error);
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::rvalue);
    handle_error = {};
    transform_visitor::result_t result{in_place_value, 10};
    std::move(result).transform_error(handle_error);
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-rvalue");
    transform_visitor handle_error;
    transform_visitor::result_t const error_result{in_place_error, 10};
    std::move(error_result).transform_error(handle_error);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::crvalue);
    handle_error = {};
    transform_visitor::result_t const result{in_place_value, 10};
    std::move(result).transform_error(handle_error);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_error.selected_overload, transform_visitor::parameter_qualification::not_called);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_error() returns @c true ,
/// when @c result.transform_error(error_transformer) is invoked the provided @c error_transformer callback is invoked
/// with the a value equivalent to invoking @c result.error() , matching reference qualification, and a @c result<T,U>
/// produced from its return is returned.
TEST(ResultTransformError, TheVisitorIsCalledWithTheHeldError) {
  auto const original_error = transform_visitor::result_t::error_type{10};
  transform_visitor handle_error;
  {
    SCOPED_TRACE("lvalue");
    auto result = transform_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.transform_error(handle_error).error(), original_error);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = transform_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(result.transform_error(handle_error).error(), original_error);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = transform_visitor::result_t{in_place_error, original_error};
    EXPECT_EQ(std::move(result).transform_error(handle_error).error(), original_error);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = transform_visitor::result_t{in_place_error, original_error};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).transform_error(handle_error).error(), original_error);
  }
}
/// @test Given an instance of @c arene::base::result<T,E> , @c result, where @c result.has_error() returns @c false ,
/// when @c result.transform_error(error_transformer) is invoked, @c error_transformer is not invoked and a
/// @c result<T,U> copy-constructed from invoking @c result.value()
TEST(ResultTransformError, AResultWithACopyOfValueIsReturnedIfErrorIsNotPopulated) {
  auto const original_value = transform_visitor::result_t::value_type{10};
  transform_visitor handle_error;
  {
    SCOPED_TRACE("lvalue");
    auto result = transform_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.transform_error(handle_error).value(), original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const result = transform_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(result.transform_error(handle_error).value(), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    auto result = transform_visitor::result_t{in_place_value, original_value};
    EXPECT_EQ(std::move(result).transform_error(handle_error).value(), original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const result = transform_visitor::result_t{in_place_value, original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(result).transform_error(handle_error).value(), original_value);
  }
}

/// A struct that is a "literal type"
struct literal_struct {
  std::uint32_t val;

  friend constexpr auto operator==(literal_struct const& lhs, literal_struct const& rhs) noexcept -> bool {
    return lhs.val == rhs.val;
  }
};

/// Another struct that is a "literal type"
struct other_literal_struct {
  std::uint16_t val;
  friend constexpr auto operator==(other_literal_struct const& lhs, other_literal_struct const& rhs) noexcept -> bool {
    return lhs.val == rhs.val;
  }
};

enum class some_error_code { error1, error2 };

using result_holding_literal_struct = result<literal_struct, other_literal_struct>;

constexpr auto make_value(std::uint32_t val) -> result_holding_literal_struct {
  return value_result(literal_struct{val});
}

constexpr auto make_error(std::uint16_t val) -> result_holding_literal_struct {
  return error_result(other_literal_struct{val});
}

using builtin_result = result<std::uint32_t, some_error_code>;

constexpr auto make_builtin_value(std::uint32_t val) -> builtin_result { return value_result(val); }

constexpr auto make_builtin_error(some_error_code val) -> builtin_result { return error_result(val); }

template <typename Result>
inline constexpr auto assign_to_value(Result res, typename Result::value_type value) -> Result {
  res.value() = value;
  return res;
}
/// @test Validates that @c arene::base::result<T,E> 's constructors, @c has_value(), @c has_error , @c value() and
/// @c error() work in @c constexpr when both @c T and @c E are literal types.
TEST(ResultConstexpr, BasicOperationsWorkWhenValueAndErrorTypesAreStructsThatAreLiteralTypes) {
  static constexpr std::uint32_t val1 = 0x42424242;
  static constexpr result_holding_literal_struct res_with_value = make_value(val1);
  static constexpr std::uint16_t val2 = 0x9999;
  static constexpr result_holding_literal_struct res_with_error = make_error(val2);

  static constexpr result_holding_literal_struct default_constructed{};

  STATIC_ASSERT_TRUE(res_with_value.has_value());
  STATIC_ASSERT_TRUE(res_with_value);
  STATIC_ASSERT_EQ(res_with_value.value().val, val1);
  STATIC_ASSERT_EQ(res_with_value->val, val1);
  STATIC_ASSERT_EQ((*res_with_value).val, val1);
  STATIC_ASSERT_EQ(res_with_value, res_with_value);
  STATIC_ASSERT_FALSE(res_with_error.has_value());
  STATIC_ASSERT_FALSE(res_with_error);
  STATIC_ASSERT_EQ(res_with_error.error().val, val2);
  STATIC_ASSERT_EQ(res_with_error, res_with_error);
  STATIC_ASSERT_NE(res_with_error, res_with_value);

  STATIC_ASSERT_TRUE(default_constructed.has_value());
  STATIC_ASSERT_TRUE(default_constructed);
  STATIC_ASSERT_EQ(default_constructed.value().val, 0);
  STATIC_ASSERT_EQ(default_constructed->val, 0);
  STATIC_ASSERT_EQ((*default_constructed).val, 0);
}
/// @test Validates that @c arene::base::result<T,E> 's assignment operators work in @c constexpr when both @c T and
/// @c E are literal types.
TEST(ResultConstexpr, CanAssignValueWithLiteralStructs) {
  static constexpr std::uint32_t val1 = 0x42424242;
  static constexpr std::uint32_t val3 = 0xABCDEF;
  static constexpr result_holding_literal_struct assigned_result =
      assign_to_value(make_value(val3), literal_struct{val1});

  STATIC_ASSERT_TRUE(assigned_result.has_value());
  STATIC_ASSERT_TRUE(assigned_result);
  STATIC_ASSERT_EQ(assigned_result->val, val1);
}

// NOLINTNEXTLINE(hicpp-special-member-functions))
struct move_tracker {
  enum class state_type {
    default_constructed,
    move_constructed,
    copy_constructed,
    const_move_constructed,
    non_const_copy_constructed
  };

  state_type state;

  constexpr move_tracker() noexcept
      : state(state_type::default_constructed) {}
  constexpr move_tracker(move_tracker&&) noexcept
      : state(state_type::move_constructed) {}
  constexpr move_tracker(move_tracker const&) noexcept
      : state(state_type::copy_constructed) {}
  constexpr move_tracker(move_tracker const&&) noexcept
      : state(state_type::const_move_constructed) {}
  constexpr move_tracker(move_tracker&) noexcept
      : state(state_type::non_const_copy_constructed) {}
};

struct move_tracker_results {
  move_tracker::state_type copy;
  move_tracker::state_type const_copy;
  move_tracker::state_type moved;
  move_tracker::state_type const_moved;
};

constexpr auto move_around_value() -> move_tracker_results {
  using mt_result = result<move_tracker, int>;

  mt_result initial{};
  mt_result copy{in_place_value, initial.value()};
  mt_result const_copy{in_place_value, as_const(initial).value()};
  mt_result moved{in_place_value, std::move(initial).value()};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  mt_result const_moved{in_place_value, std::move(as_const(initial)).value()};

  return {copy->state, const_copy->state, moved->state, const_moved->state};
}
/// @test Validates that it is possible to move from a @c arene::base::result<T,E>::value() 's in @c constexpr when
/// both @c T and @c E are @c constexpr -compatible types.
TEST(ResultConstexpr, CanMoveFromStoredValue) {
  static constexpr move_tracker_results states = move_around_value();

  STATIC_ASSERT_EQ(states.copy, move_tracker::state_type::non_const_copy_constructed);
  STATIC_ASSERT_EQ(states.const_copy, move_tracker::state_type::copy_constructed);
  STATIC_ASSERT_EQ(states.moved, move_tracker::state_type::move_constructed);
  STATIC_ASSERT_EQ(states.const_moved, move_tracker::state_type::const_move_constructed);
}

constexpr auto move_around_deref() -> move_tracker_results {
  using mt_result = result<move_tracker, int>;

  mt_result initial{};
  mt_result copy{in_place_value, *initial};
  mt_result const_copy{in_place_value, *as_const(initial)};
  mt_result moved{in_place_value, *std::move(initial)};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  mt_result const_moved{in_place_value, *std::move(as_const(initial))};

  return {copy->state, const_copy->state, moved->state, const_moved->state};
}
/// @test Validates that it is possible to move from a @c arene::base::result<T,E> 's @c operator* in @c constexpr when
/// both @c T and @c E are @c constexpr -compatible types.
TEST(ResultConstexpr, CanMoveFromStoredValueUsingDerefOperator) {
  static constexpr move_tracker_results states = move_around_deref();

  STATIC_ASSERT_EQ(states.copy, move_tracker::state_type::non_const_copy_constructed);
  STATIC_ASSERT_EQ(states.const_copy, move_tracker::state_type::copy_constructed);
  STATIC_ASSERT_EQ(states.moved, move_tracker::state_type::move_constructed);
  STATIC_ASSERT_EQ(states.const_moved, move_tracker::state_type::const_move_constructed);
}

constexpr auto move_around_error() -> move_tracker_results {
  using mt_result = result<void, move_tracker>;

  mt_result initial{in_place_error};
  mt_result copy{in_place_error, initial.error()};
  mt_result const_copy{in_place_error, as_const(initial).error()};
  mt_result moved{in_place_error, std::move(initial).error()};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  mt_result const_moved{in_place_error, std::move(as_const(initial)).error()};

  return {copy.error().state, const_copy.error().state, moved.error().state, const_moved.error().state};
}
/// @test Validates that it is possible to move from a @c arene::base::result<T,E>::error() 's in @c constexpr when
/// both @c T and @c E are @c constexpr -compatible types.
TEST(ResultConstexpr, CanMoveFromStoredError) {
  static constexpr move_tracker_results states = move_around_error();

  STATIC_ASSERT_EQ(states.copy, move_tracker::state_type::non_const_copy_constructed);
  STATIC_ASSERT_EQ(states.const_copy, move_tracker::state_type::copy_constructed);
  STATIC_ASSERT_EQ(states.moved, move_tracker::state_type::move_constructed);
  STATIC_ASSERT_EQ(states.const_moved, move_tracker::state_type::const_move_constructed);
}
/// @test Validates that @c arene::base::result<T,E> 's constructors, @c has_value(), @c has_error , @c value() and
/// @c error() work in @c constexpr when both @c T and @c E are builtin types.
TEST(ResultConstexpr, BasicOperationsWorkWithBuiltinValueAndErrorTypes) {
  static constexpr std::uint32_t val1 = 0x42424242;
  static constexpr builtin_result res_with_value = make_builtin_value(val1);
  static constexpr some_error_code val2 = some_error_code::error2;
  static constexpr builtin_result res_with_error = make_builtin_error(val2);

  static constexpr builtin_result default_constructed{};

  STATIC_ASSERT_TRUE(res_with_value.has_value());
  STATIC_ASSERT_TRUE(res_with_value);
  STATIC_ASSERT_EQ(res_with_value.value(), val1);
  STATIC_ASSERT_EQ(*res_with_value, val1);
  STATIC_ASSERT_EQ(res_with_value, res_with_value);
  STATIC_ASSERT_FALSE(res_with_error.has_value());
  STATIC_ASSERT_FALSE(res_with_error);
  STATIC_ASSERT_EQ(res_with_error.error(), val2);
  STATIC_ASSERT_EQ(res_with_error, res_with_error);
  STATIC_ASSERT_NE(res_with_error, res_with_value);

  STATIC_ASSERT_TRUE(default_constructed.has_value());
  STATIC_ASSERT_TRUE(default_constructed);
  STATIC_ASSERT_EQ(default_constructed.value(), 0);
  STATIC_ASSERT_EQ(*default_constructed, 0);
}
/// @test Validates that @c arene::base::result<T,E> 's assignment operators work in @c constexpr when both @c T and
/// @c E are literal types.
TEST(ResultConstexpr, CanAssignValueWithLiteralStructsBuiltins) {
  static constexpr std::uint32_t val1 = 0x42424242;
  static constexpr std::uint32_t val3 = 0xABCDEF;
  static constexpr builtin_result assigned_result = assign_to_value(make_builtin_value(val3), val1);

  STATIC_ASSERT_TRUE(assigned_result.has_value());
  STATIC_ASSERT_TRUE(assigned_result);
  STATIC_ASSERT_EQ(*assigned_result, val1);
}

// A function object used to verify that @c expand_result properly perfect-forwards (lvalue form).
template <class T>
struct verify_call_l_value {
  explicit verify_call_l_value() = default;
  explicit verify_call_l_value(std::remove_reference_t<T>* expected)
      : expected_ptr(expected) {}

  template <class U, constraints<std::enable_if_t<is_same_v<U&&, T>>> = nullptr>
  auto operator()(U&& value) & -> bool {
    ARENE_PRECONDITION(!was_called);
    was_called = true;
    return expected_ptr == std::addressof(value);
  }
  std::remove_reference_t<T>* expected_ptr = nullptr;
  bool was_called = false;

  // needs to be non-trivial
  ~verify_call_l_value() = default;
  verify_call_l_value(verify_call_l_value const&) = default;
  verify_call_l_value(verify_call_l_value&&) noexcept {}
  auto operator=(verify_call_l_value const&) -> verify_call_l_value& = default;
  auto operator=(verify_call_l_value&&) noexcept -> verify_call_l_value& {}
};

// A function object used to verify that @c expand_result properly
// perfect-forwards (rvalue form).
template <class T>
struct verify_call_r_value {
  verify_call_r_value() = default;
  explicit verify_call_r_value(std::remove_reference_t<T>* expected)
      : expected_ptr(expected) {}

  template <class U, constraints<std::enable_if_t<is_same_v<U&&, T>>> = nullptr>
  auto operator()(U&& value) && -> bool {
    ARENE_PRECONDITION(!was_called);
    was_called = true;
    return expected_ptr == std::addressof(value);
  }
  std::remove_reference_t<T>* expected_ptr = nullptr;
  bool was_called = false;

  // needs to be non-trivial
  ~verify_call_r_value() = default;
  verify_call_r_value(verify_call_r_value const&) = default;
  verify_call_r_value(verify_call_r_value&&) noexcept {}
  auto operator=(verify_call_r_value const&) -> verify_call_r_value& = default;
  auto operator=(verify_call_r_value&&) noexcept -> verify_call_r_value& {}
};
/// @test Given an lvalue reference to an instance of @c arene::base::result<T,E> ,
/// @c arene::base::expand_result(result,handle_value,handle_error) will invoke the handler that matches the currently
/// populated channel in @c result with an lvalue instance of the value in that channel.
TEST(ExpandResult, CanVisitLvalueCallable) {
  using value_t = int;
  using error_t = char;

  // lvalue value
  {
    result<value_t, error_t> res{in_place_value};
    verify_call_l_value<value_t&> f_v{std::addressof(res.value())};
    verify_call_l_value<error_t&> f_e{};
    EXPECT_TRUE(arene::base::expand_result(res, f_v, f_e));
    EXPECT_TRUE(f_v.was_called);
    EXPECT_FALSE(f_e.was_called);
  }

  // const lvalue value
  {
    result<value_t, error_t> const res{in_place_value};
    verify_call_l_value<value_t const&> f_v{std::addressof(res.value())};
    verify_call_l_value<error_t const&> f_e{};
    EXPECT_TRUE(arene::base::expand_result(res, f_v, f_e));
    EXPECT_TRUE(f_v.was_called);
    EXPECT_FALSE(f_e.was_called);
  }

  // rvalue value
  {
    result<value_t, error_t> res{in_place_value};
    verify_call_l_value<value_t&&> f_v{std::addressof(res.value())};
    verify_call_l_value<error_t&&> f_e{};
    EXPECT_TRUE(arene::base::expand_result(std::move(res), f_v, f_e));
    EXPECT_TRUE(f_v.was_called);
    EXPECT_FALSE(f_e.was_called);
  }

  // const rvalue value
  {
    result<value_t, error_t> const res{in_place_value};
    verify_call_l_value<value_t const&&> f_v{std::addressof(res.value())};
    verify_call_l_value<error_t const&&> f_e{};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    EXPECT_TRUE(arene::base::expand_result(std::move(res), f_v, f_e));
    EXPECT_TRUE(f_v.was_called);
    EXPECT_FALSE(f_e.was_called);
  }

  // lvalue error
  {
    result<value_t, error_t> res{in_place_error};
    verify_call_l_value<value_t&> f_v{};
    verify_call_l_value<error_t&> f_e{std::addressof(res.error())};
    EXPECT_TRUE(arene::base::expand_result(res, f_v, f_e));
    EXPECT_FALSE(f_v.was_called);
    EXPECT_TRUE(f_e.was_called);
  }

  // const lvalue error
  {
    result<value_t, error_t> const res{in_place_error};
    verify_call_l_value<value_t const&> f_v{};
    verify_call_l_value<error_t const&> f_e{std::addressof(res.error())};
    EXPECT_TRUE(arene::base::expand_result(res, f_v, f_e));
    EXPECT_FALSE(f_v.was_called);
    EXPECT_TRUE(f_e.was_called);
  }

  // rvalue error
  {
    result<value_t, error_t> res{in_place_error};
    verify_call_l_value<value_t&&> f_v{};
    verify_call_l_value<error_t&&> f_e{std::addressof(res.error())};
    EXPECT_TRUE(arene::base::expand_result(std::move(res), f_v, f_e));
    EXPECT_FALSE(f_v.was_called);
    EXPECT_TRUE(f_e.was_called);
  }

  // const rvalue error
  {
    result<value_t, error_t> const res{in_place_error};
    verify_call_l_value<value_t const&&> f_v{};
    verify_call_l_value<error_t const&&> f_e{std::addressof(res.error())};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    EXPECT_TRUE(arene::base::expand_result(std::move(res), f_v, f_e));
    EXPECT_FALSE(f_v.was_called);
    EXPECT_TRUE(f_e.was_called);
  }
}
/// @test Given an rvalue reference to an instance of @c arene::base::result<T,E> ,
/// @c arene::base::expand_result(result,handle_value,handle_error) will invoke the handler that matches the currently
/// populated channel in @c result with an rvalue instance of the value in that channel.
TEST(ExpandResult, VisitRvalueCallable) {
  using value_t = int;
  using error_t = char;

  // lvalue value
  {
    result<value_t, error_t> res{in_place_value};
    verify_call_r_value<value_t&> f_v{std::addressof(res.value())};
    verify_call_r_value<error_t&> f_e{};
    EXPECT_TRUE(arene::base::expand_result(res, std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_e.was_called);
  }

  // const lvalue value
  {
    result<value_t, error_t> const res{in_place_value};
    verify_call_r_value<value_t const&> f_v{std::addressof(res.value())};
    verify_call_r_value<error_t const&> f_e{};
    EXPECT_TRUE(arene::base::expand_result(res, std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_e.was_called);
  }

  // rvalue value
  {
    result<value_t, error_t> res{in_place_value};
    verify_call_r_value<value_t&&> f_v{std::addressof(res.value())};
    verify_call_r_value<error_t&&> f_e{};
    EXPECT_TRUE(arene::base::expand_result(std::move(res), std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_e.was_called);
  }

  // const rvalue value
  {
    result<value_t, error_t> const res{in_place_value};
    verify_call_r_value<value_t const&&> f_v{std::addressof(res.value())};
    verify_call_r_value<error_t const&&> f_e{};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    EXPECT_TRUE(arene::base::expand_result(std::move(res), std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_e.was_called);
  }

  // lvalue error
  {
    result<value_t, error_t> res{in_place_error};
    verify_call_r_value<value_t&> f_v{};
    verify_call_r_value<error_t&> f_e{std::addressof(res.error())};
    EXPECT_TRUE(arene::base::expand_result(res, std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_e.was_called);
  }

  // const lvalue error
  {
    result<value_t, error_t> const res{in_place_error};
    verify_call_r_value<value_t const&> f_v{};
    verify_call_r_value<error_t const&> f_e{std::addressof(res.error())};
    EXPECT_TRUE(arene::base::expand_result(res, std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_e.was_called);
  }

  // rvalue error
  {
    result<value_t, error_t> res{in_place_error};
    verify_call_r_value<value_t&&> f_v{};
    verify_call_r_value<error_t&&> f_e{std::addressof(res.error())};
    EXPECT_TRUE(arene::base::expand_result(std::move(res), std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_e.was_called);
  }

  // const rvalue error
  {
    result<value_t, error_t> const res{in_place_error};
    verify_call_r_value<value_t const&&> f_v{};
    verify_call_r_value<error_t const&&> f_e{std::addressof(res.error())};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    EXPECT_TRUE(arene::base::expand_result(std::move(res), std::move(f_v), std::move(f_e)));
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_FALSE(f_v.was_called);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    EXPECT_TRUE(f_e.was_called);
  }
}

template <typename Res, typename Value, typename Error>
using use_expand_result =
    decltype(arene::base::expand_result(std::declval<Res>(), std::declval<Value>(), std::declval<Error>()));
/// @test @c arene::base::expand_result(result,handle_value,handle_error) does not participate in overload resolution if
/// @c handle_value is not invocable with the return of @c result.value() or @c handle_error is not invocable with the
/// return of @c result.error() .
TEST(ExpandResult, DoesNotParticipateInOverloadResolutionIfValueVisitorIsNotInvocableWithValue) {
  using result_t = result<arene::base::string_view, arene::base::string_view>;
  auto const valid_handler = [](arene::base::string_view const&) {};
  auto const needs_lval = [](arene::base::string_view&) {};
  auto const needs_rval = [](arene::base::string_view&&) {};
  STATIC_ASSERT_FALSE(
      (arene::base::
           substitution_succeeds<use_expand_result, result_t const&, decltype(needs_lval), decltype(valid_handler)>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::
           substitution_succeeds<use_expand_result, result_t const&, decltype(valid_handler), decltype(needs_lval)>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::substitution_succeeds<use_expand_result, result_t&&, decltype(needs_lval), decltype(valid_handler)>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::substitution_succeeds<use_expand_result, result_t&&, decltype(valid_handler), decltype(needs_lval)>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::
           substitution_succeeds<use_expand_result, result_t const&, decltype(needs_rval), decltype(valid_handler)>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::
           substitution_succeeds<use_expand_result, result_t const&, decltype(valid_handler), decltype(needs_rval)>)
  );
}

constexpr auto process_value(literal_struct& val) -> result_holding_literal_struct {
  return value_result(literal_struct{val.val * 2});
}
constexpr auto process_error(other_literal_struct& val) -> result_holding_literal_struct {
  return error_result(other_literal_struct{static_cast<std::uint16_t>(val.val * 3)});
}

constexpr auto do_expand(result_holding_literal_struct initial) -> result_holding_literal_struct {
  return arene::base::expand_result(initial, process_value, process_error);
}
/// @test Validates @c arene::base::expand_result(result,handle_value,handle_error) works in @c constexpr .
TEST(ExpandResult, WorksInConstexpr) {
  static constexpr std::uint32_t val = 0x42424242;
  static constexpr std::uint16_t error = 0x1212;

  STATIC_ASSERT_EQ(do_expand(make_value(val))->val, val * 2);
  STATIC_ASSERT_EQ(do_expand(make_error(error)).error().val, error * 3);
}

/// Dummy container types to test typing on value and error
struct dummy_value {
  int value;
};
struct dummy_error {
  int value;
};

/// @test Given an instance of @c arene::base::value_result<T> , it is possible to construct any
/// @c arene::base::result<T,E> using it as an argument, and the resulting @c result<T,E> will be as if constructed by
/// the @c T instance held in the @c value_result .
TEST(ValueResult, CanInitializeResultFromValueResult) {
  constexpr int value = 42;
  constexpr result<dummy_value, dummy_error> res = value_result(dummy_value{value});
  STATIC_ASSERT_TRUE(res.has_value());
  STATIC_ASSERT_EQ(res.value().value, value);
}
/// @test Given an instance of @c arene::base::value_result<void> , it is possible to construct any
/// @c arene::base::result<void,E> using it as an argument, and the resulting @c result<void,E> will be as if
/// constructed with its value channel populated.
TEST(ValueResult, SupportsVoidValues) {
  constexpr result<void, dummy_error> res = value_result();
  STATIC_ASSERT_TRUE(res.has_value());
}
/// @test Given an instance of @c arene::base::value_result<T> , if an lvalue instance is used to construct an
/// @c arene::base::result<T,E> using it as an argument, the @c T instance in the @c value_result is not consumed.
TEST(ValueResult, LValueCanBeUsedToInitializeMultipleResults) {
  constexpr int value = 42;

  auto tmp = value_result(dummy_value{value});
  result<dummy_value, dummy_error> const first_res = tmp;
  EXPECT_TRUE(first_res.has_value());
  EXPECT_EQ(first_res.value().value, value);

  result<dummy_value, dummy_error> const second_res = tmp;
  EXPECT_TRUE(second_res.has_value());
  EXPECT_EQ(second_res.value().value, value);
}

/// @test Given an instance of @c arene::base::error_result<E> , it is possible to construct any
/// @c arene::base::result<T,E> using it as an argument, and the resulting @c result<T,E> will be as if constructed by
/// the @c E instance held in the @c error_result .
TEST(ErrorResult, CanInitializeResultFromErrorResult) {
  constexpr int err = 24;
  constexpr result<dummy_value, dummy_error> res = error_result(dummy_error{err});
  STATIC_ASSERT_FALSE(res.has_value());
  STATIC_ASSERT_EQ(res.error().value, err);
}
/// @test Given an instance of @c arene::base::error_result<E> , if an lvalue instance is used to construct an
/// @c arene::base::result<T,E> using it as an argument, the @c E instance in the @c error_result is not consumed.
TEST(ErrorResult, LValueCanBeUsedToInitializeMultipleResults) {
  constexpr int err = 24;

  auto tmp = error_result(dummy_error{err});
  result<dummy_value, dummy_error> const first_res = tmp;
  EXPECT_TRUE(first_res.has_error());
  EXPECT_EQ(first_res.error().value, err);

  result<dummy_value, dummy_error> const second_res = tmp;
  EXPECT_TRUE(second_res.has_error());
  EXPECT_EQ(second_res.error().value, err);
}

}  // namespace
