// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/utility/as_const.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/functional"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

struct not_default_constructible {
  std::int32_t value_;

 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  not_default_constructible(std::int32_t value) noexcept
      : value_(value) {}

  auto get_value() const noexcept -> std::int32_t { return value_; }
};

struct throwing_default_constructible {
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  throwing_default_constructible() noexcept(false) {}
};

struct throwable_copy_construction {
  constexpr throwable_copy_construction() = default;
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  constexpr throwable_copy_construction(throwable_copy_construction const&) noexcept(false) {}
  constexpr throwable_copy_construction(throwable_copy_construction&&) noexcept(false) {}
  constexpr auto operator=(throwable_copy_construction const&) -> throwable_copy_construction& = default;
  constexpr auto operator=(throwable_copy_construction&&) -> throwable_copy_construction& = default;
  ~throwable_copy_construction() = default;
};

class constexpr_comparable {
  friend constexpr auto operator==(constexpr_comparable const&, constexpr_comparable const&) noexcept -> bool {
    return true;
  }
  friend constexpr auto operator<(constexpr_comparable const&, constexpr_comparable const&) noexcept -> bool {
    return false;
  }
};

struct not_copy_constructible {
  not_copy_constructible() = default;
  ~not_copy_constructible() = default;

  not_copy_constructible(not_copy_constructible const&) = delete;
  auto operator=(not_copy_constructible const&) -> not_copy_constructible& = delete;

  not_copy_constructible(not_copy_constructible&&) = default;
  auto operator=(not_copy_constructible&&) -> not_copy_constructible& = default;
};

class construction_tracker {
 public:
  enum class state : std::uint8_t {
    default_constructed,
    copy_constructed,
    move_constructed,
    direct_constructed,
    direct_rvalue_constructed,
    copy_assigned,
    move_assigned,
    swapped_lhs,
    swapped_rhs
  };

  struct some_arg_type {};
  struct explicit_arg_type {};

  construction_tracker() noexcept
      : state_(state::default_constructed) {}
  construction_tracker(construction_tracker const&) noexcept
      : state_(state::copy_constructed) {}
  construction_tracker(construction_tracker&&) noexcept
      : state_(state::move_constructed) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  construction_tracker(some_arg_type const&) noexcept
      : state_(state::direct_constructed) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  construction_tracker(some_arg_type&&) noexcept
      : state_(state::direct_rvalue_constructed) {}

  auto operator=(construction_tracker&&) noexcept -> construction_tracker& {
    state_ = state::move_assigned;
    return *this;
  }
  auto operator=(construction_tracker const&) noexcept -> construction_tracker& {
    state_ = state::copy_assigned;
    return *this;
  }

  ~construction_tracker() = default;

  auto get_tracked_state() const noexcept -> state { return state_; }

  void swap(construction_tracker& other) noexcept {
    state_ = state::swapped_lhs;
    other.state_ = state::swapped_rhs;
  }

 private:
  state state_;
};

void swap(construction_tracker& lhs, construction_tracker& rhs) noexcept { lhs.swap(rhs); }

template <typename T>
class convertible_from {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  convertible_from(T) noexcept {}
};

template <typename T>
class explicitly_convertible_from {
 public:
  explicit explicitly_convertible_from(T) noexcept {}
};

template <typename T>
class assignable_from {
 public:
  template <typename U = T, arene::base::constraints<std::enable_if_t<std::is_copy_assignable_v<U>>> = nullptr>
  auto operator=(T const&) noexcept(std::is_nothrow_copy_assignable_v<U>) -> assignable_from& {
    return *this;
  }
  template <typename U = T, arene::base::constraints<std::enable_if_t<std::is_move_assignable_v<U>>> = nullptr>
  auto operator=(T&&) noexcept(std::is_nothrow_move_assignable_v<U>) -> assignable_from& {
    return *this;
  }
};

template <typename T>
class convertible_to {
  T value_;

 public:
  template <typename U>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload,hicpp-explicit-conversions)
  constexpr convertible_to(U&& value)
      : value_(std::forward<U>(value)) {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T const&() const noexcept { return value_; }
};

template <typename T>
class rvalue_convertible_to {
  T value_;

 public:
  template <typename U>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload,hicpp-explicit-conversions)
  constexpr rvalue_convertible_to(U&& value)
      : value_(std::forward<U>(value)) {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T&&() && noexcept { return static_cast<T&&>(value_); }
};

template <typename T>
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
T dummy_value{};

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
not_default_constructible dummy_value<not_default_constructible>{42};

template <typename First, typename Second>
struct pair_test_args {
  using first = First;
  using second = Second;
};

using pair_test_types = ::testing::Types<
    pair_test_args<std::int32_t, std::int32_t>,
    pair_test_args<std::int32_t, not_default_constructible>,
    pair_test_args<not_default_constructible, std::int32_t>,
    pair_test_args<not_default_constructible, not_default_constructible>,
    pair_test_args<void*, void*>,
    pair_test_args<std::int32_t, void*>,
    pair_test_args<void*, std::int32_t>,
    pair_test_args<std::int32_t*, construction_tracker>,
    pair_test_args<float, construction_tracker>,
    pair_test_args<construction_tracker, std::int32_t>,
    pair_test_args<construction_tracker, float>,
    pair_test_args<not_copy_constructible, std::int32_t>,
    pair_test_args<std::uint32_t, not_copy_constructible>,
    pair_test_args<construction_tracker, not_copy_constructible>,
    pair_test_args<testing::not_move_constructible, std::int32_t>,
    pair_test_args<construction_tracker, testing::not_move_constructible>,
    pair_test_args<not_copy_constructible, testing::not_move_constructible>,
    pair_test_args<std::int32_t, testing::throwable_move_construction>,
    pair_test_args<testing::throwable_move_construction, std::int32_t>,
    pair_test_args<testing::throwable_move_construction, testing::throwable_move_construction>,
    pair_test_args<std::int32_t, throwable_copy_construction>,
    pair_test_args<throwable_copy_construction, std::int32_t>,
    pair_test_args<throwable_copy_construction, throwable_copy_construction>,
    pair_test_args<throwable_copy_construction, testing::not_move_assignable>,
    pair_test_args<constexpr_comparable, std::uint64_t>,
    pair_test_args<std::uint64_t, constexpr_comparable>,
    pair_test_args<constexpr_comparable, constexpr_comparable>,
    pair_test_args<testing::not_move_assignable, std::uint64_t>,
    pair_test_args<testing::throwable_move_assignment, std::uint64_t>,
    pair_test_args<std::int32_t, testing::throwable_move_assignment>>;

template <typename TypeParam>
class StdPairTests : public testing::Test {
 public:
  using pair_type = std::pair<typename TypeParam::first, typename TypeParam::second>;
};

TYPED_TEST_SUITE(StdPairTests, pair_test_types, );

/// @test Ensure that the type aliases for the members are correct
TYPED_TEST(StdPairTests, MemberTypesAreCorrect) {
  testing::StaticAssertTypeEq<typename TestFixture::pair_type::first_type, typename TypeParam::first>();
  testing::StaticAssertTypeEq<typename TestFixture::pair_type::second_type, typename TypeParam::second>();
}

/// @test Ensure that the data members have the right types
TYPED_TEST(StdPairTests, DataMembersAreCorrect) {
  testing::
      StaticAssertTypeEq<decltype(std::declval<typename TestFixture::pair_type&>().first), typename TypeParam::first>();
  testing::
      StaticAssertTypeEq<decltype(std::declval<typename TestFixture::pair_type&>().second), typename TypeParam::second>(
      );
}

/// @test tuple_size<pair<>> is 2
TYPED_TEST(StdPairTests, TupleSize) {
  static_assert(
      testing::is_unambiguously_publicly_derived_from_v<
          std::tuple_size<typename TestFixture::pair_type>,
          std::integral_constant<std::size_t, 2>>,
      "tuple_size<pair> has base characteristic integral_constant<size_t, 2>"
  );
}

/// @test tuple_element<pair<>> matches first_type and second_type
TYPED_TEST(StdPairTests, TupleElement) {
  testing::StaticAssertTypeEq<
      typename std::tuple_element<0, typename TestFixture::pair_type>::type,
      typename TestFixture::pair_type::first_type>();
  testing::StaticAssertTypeEq<
      typename std::tuple_element<1, typename TestFixture::pair_type>::type,
      typename TestFixture::pair_type::second_type>();
}

/// @test A pair is default-constructible if and only if the fields are default-constructible
TYPED_TEST(StdPairTests, IsDefaultConstructibleIfArgsAreDefaultConstructible) {
  static_assert(
      std::is_default_constructible_v<typename TestFixture::pair_type> ==
          (std::is_default_constructible_v<typename TypeParam::first> &&
           std::is_default_constructible_v<typename TypeParam::second>),
      "Pair is default constructible if args are"
  );
}

/// @test A pair is nothrow-default-constructible if and only if the fields are nothrow-default-constructible
TYPED_TEST(StdPairTests, IsNothrowDefaultConstructibleIfArgsAreNothrowDefaultConstructible) {
  static_assert(
      std::is_nothrow_default_constructible_v<typename TestFixture::pair_type> ==
          (std::is_nothrow_default_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_default_constructible_v<typename TypeParam::second>),
      "Pair is nothrow default constructible if args are"
  );
}

/// @test Arithmetic types are initialized to zero
CONDITIONAL_TYPED_TEST(StdPairTests, DefaultConstructorInitializesIntegralFirstToZero, std::is_arithmetic_v<typename TypeParam::first>&& std::is_default_constructible_v<typename TypeParam::second>) {
  typename TestFixture::pair_type const pair1;

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.first, static_cast<typename TypeParam::first>(0));
  ARENE_IGNORE_END();
}

/// @test Arithmetic types are initialized to zero
CONDITIONAL_TYPED_TEST(StdPairTests, DefaultConstructorInitializesIntegralSecondToZero, std::is_arithmetic_v<typename TypeParam::second>&& std::is_default_constructible_v<typename TypeParam::first>) {
  typename TestFixture::pair_type const pair1;

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.second, static_cast<typename TypeParam::second>(0));
  ARENE_IGNORE_END();
}

/// @test Pointer types are initialized to null pointer
CONDITIONAL_TYPED_TEST(StdPairTests, DefaultConstructorInitializesPointerFirstToNullptr, std::is_pointer_v<typename TypeParam::first>&& std::is_default_constructible_v<typename TypeParam::second>) {
  typename TestFixture::pair_type const pair1;

  ASSERT_EQ(pair1.first, static_cast<typename TypeParam::first>(nullptr));
}

/// @test Pointer types are initialized to null pointer
CONDITIONAL_TYPED_TEST(StdPairTests, DefaultConstructorInitializesPointerSecondToNullptr, std::is_pointer_v<typename TypeParam::second>&& std::is_default_constructible_v<typename TypeParam::first>) {
  typename TestFixture::pair_type const pair1;

  ASSERT_EQ(pair1.second, static_cast<typename TypeParam::second>(nullptr));
}

template <typename T>
constexpr auto constexpr_check(T&&) -> bool {
  return true;
}

/// @test the default constructor is usable in a constant expression if the members are
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    DefaultConstructorInConstantExpressionIfArgsAre,
    (std::is_trivially_default_constructible_v<typename TypeParam::first> &&
     std::is_trivially_default_constructible_v<typename TypeParam::second>)
) {
  static_assert(
      constexpr_check(typename TestFixture::pair_type{}),
      "Must be default-constructible in a constant expression"
  );
}

/// @test the default constructor initializes class type members with their default constructor
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    DefaultConstructorInitializesClassFirstWithDefaultConstructor,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_default_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1;

  ASSERT_EQ(pair1.first.get_tracked_state(), construction_tracker::state::default_constructed);
}

/// @test the default constructor initializes class type members with their default constructor
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    DefaultConstructorInitializesClassSecondWithDefaultConstructor,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_default_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1;

  ASSERT_EQ(pair1.second.get_tracked_state(), construction_tracker::state::default_constructed);
}

/// @test @c std::pair can be constructed from two values if the types are copy-constructible
TYPED_TEST(StdPairTests, IsValueConstructibleIfArgsAreCopyConstructible) {
  static_assert(
      std::is_constructible_v<
          typename TestFixture::pair_type,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are copy constructible"
  );
}

/// @test @c std::pair can be constructed from two values without throwing if the types are nothrow-copy-constructible
TYPED_TEST(StdPairTests, IsNothrowValueConstructibleIfArgsAreCopyConstructible) {
  static_assert(
      std::is_nothrow_constructible_v<
          typename TestFixture::pair_type,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is nothrow constructible from pair of values if args are nothrow copy constructible"
  );
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForIntegralFirstCopiesValues,
    (std::is_integral_v<typename TypeParam::first> && std::is_copy_constructible_v<typename TypeParam::second>)
) {
  auto first_value = static_cast<typename TypeParam::first>(42);

  typename TestFixture::pair_type const pair1{first_value, dummy_value<typename TypeParam::second>};

  ASSERT_EQ(pair1.first, first_value);
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForIntegralSecondCopiesValues,
    (std::is_integral_v<typename TypeParam::second> && std::is_copy_constructible_v<typename TypeParam::first>)
) {
  auto second_value = static_cast<typename TypeParam::second>(42);

  typename TestFixture::pair_type const pair1{dummy_value<typename TypeParam::first>, second_value};

  ASSERT_EQ(pair1.second, second_value);
}

/// assert properties of @c std::get on a @c std::pair
/// @tparam I index to get
/// @tparam RefQual type to cast the input to
/// @tparam Pair pair type
/// @tparam Member data member pointer type of a @c std::pair
/// @param value reference to a @c std::pair
/// @param member member of the @c std::pair to access
///
/// Check that the following are @c true:
/// * get on the pair returns the same value as accessing the data member
/// * get returns the same ref-qualified type as accessing the data member
/// * get is always noexcept
///
template <std::size_t I, class RefQual, class Pair, class Member>
auto assert_valid_get_with(Pair& value, Member member) -> void {
  ASSERT_EQ(std::get<I>(static_cast<RefQual>(value)), static_cast<RefQual>(value).*member);
  ASSERT_TRUE(std::is_same_v<                                      //
              decltype(std::get<I>(static_cast<RefQual>(value))),  //
              decltype((std::declval<RefQual>().*member))          //
              >);
  ASSERT_TRUE(noexcept(std::get<I>(static_cast<RefQual&>(value))));
}

/// @test a @c std::pair 's first value can be obtained using @c std::get
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    Get0,
    (std::is_integral_v<typename TypeParam::first> && std::is_copy_constructible_v<typename TypeParam::second>)
) {
  auto first_value = static_cast<typename TypeParam::first>(42);

  using pair = typename TestFixture::pair_type;

  auto pair1 = pair{first_value, dummy_value<typename TypeParam::second>};

  assert_valid_get_with<0, pair&>(pair1, &pair::first);
  assert_valid_get_with<0, pair const&>(pair1, &pair::first);
  assert_valid_get_with<0, pair&&>(pair1, &pair::first);
  assert_valid_get_with<0, pair const&&>(pair1, &pair::first);
}

/// @test a @c std::pair 's second value can be obtained using @c std::get
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    Get1,
    (std::is_integral_v<typename TypeParam::second> && std::is_copy_constructible_v<typename TypeParam::first>)
) {
  auto second_value = static_cast<typename TypeParam::second>(42);

  using pair = typename TestFixture::pair_type;

  auto pair1 = pair{dummy_value<typename TypeParam::first>, second_value};

  assert_valid_get_with<1, pair&>(pair1, &pair::second);
  assert_valid_get_with<1, pair const&>(pair1, &pair::second);
  assert_valid_get_with<1, pair&&>(pair1, &pair::second);
  assert_valid_get_with<1, pair const&&>(pair1, &pair::second);
}

/// @test @c std::get returns ref-collapsed references using the ref
/// qualification of the @c std::pair
TEST(StdPairTests, GetWithReferenceTypes) {
  auto first = 1;
  auto second = 2;

  auto const test_with_inputs = [&first, &second](auto& lvalue_ref, auto&& rvalue_ref) {
    using pair_type = std::pair<decltype(lvalue_ref), decltype(rvalue_ref)>;

    auto values =
        pair_type{std::forward<decltype(lvalue_ref)>(lvalue_ref), std::forward<decltype(rvalue_ref)>(rvalue_ref)};

    static_assert(
        std::is_same_v<typename pair_type::first_type, decltype(std::get<0>(std::declval<pair_type&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );
    static_assert(
        std::is_same_v<typename pair_type::first_type, decltype(std::get<0>(std::declval<pair_type const&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );
    static_assert(
        std::is_same_v<typename pair_type::first_type, decltype(std::get<0>(std::declval<pair_type&&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );
    static_assert(
        std::is_same_v<typename pair_type::first_type, decltype(std::get<0>(std::declval<pair_type const&&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );

    static_assert(
        std::is_same_v<typename pair_type::second_type&, decltype(std::get<1>(std::declval<pair_type&>()))>,
        "get<> of an rvalue reference type returns an lvalue reference if the argument is an lvalue reference"
    );
    static_assert(
        std::is_same_v<typename pair_type::second_type&, decltype(std::get<1>(std::declval<pair_type const&>()))>,
        "get<> of an rvalue reference type returns an lvalue reference if the argument is an lvalue reference"
    );
    static_assert(
        std::is_same_v<typename pair_type::second_type, decltype(std::get<1>(std::declval<pair_type&&>()))>,
        "get<> of an rvalue reference type returns an rvalue reference if the argument is an rvalue reference"
    );
    static_assert(
        std::is_same_v<typename pair_type::second_type, decltype(std::get<1>(std::declval<pair_type const&&>()))>,
        "get<> of an rvalue reference type returns an rvalue reference if the argument is an rvalue reference"
    );

    ASSERT_EQ(&std::get<0>(values), &first);
    ASSERT_EQ(&std::get<1>(values), &second);
  };

  // NOLINTBEGIN(hicpp-move-const-arg)
  test_with_inputs(first, std::move(second));
  test_with_inputs(arene::base::as_const(first), std::move(arene::base::as_const(second)));
  // NOLINTEND(hicpp-move-const-arg)
}

/// assert properties of @c std::get on a @c std::pair
/// @tparam T type to get
/// @tparam RefQual type to cast the input to
/// @tparam Pair pair type
/// @tparam Member data member pointer type of a @c std::pair
/// @param value reference to a @c std::pair
/// @param member member of the @c std::pair to access
///
/// Check that the following are @c true:
/// * get on the pair returns the same value as accessing the data member
/// * get returns the same ref-qualified type as accessing the data member
/// * get is always noexcept
///
template <class T, class RefQual, class Pair, class Member>
auto assert_valid_get_with(Pair& value, Member member) -> void {
  ASSERT_EQ(std::get<T>(static_cast<RefQual>(value)), static_cast<RefQual>(value).*member);
  ASSERT_TRUE(std::is_same_v<                                      //
              decltype(std::get<T>(static_cast<RefQual>(value))),  //
              decltype((std::declval<RefQual>().*member))          //
              >);
  ASSERT_TRUE(noexcept(std::get<T>(static_cast<RefQual&>(value))));
}

/// @test a @c std::pair 's first value can be obtained using @c std::get
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GetFirstType,
    (std::is_integral_v<typename TypeParam::first> && std::is_copy_constructible_v<typename TypeParam::second> &&
     !std::is_same_v<typename TypeParam::first, typename TypeParam::second>)
) {
  auto first_value = static_cast<typename TypeParam::first>(42);

  using pair = typename TestFixture::pair_type;

  auto pair1 = pair{first_value, dummy_value<typename TypeParam::second>};

  assert_valid_get_with<typename TypeParam::first, pair&>(pair1, &pair::first);
  assert_valid_get_with<typename TypeParam::first, pair const&>(pair1, &pair::first);
  assert_valid_get_with<typename TypeParam::first, pair&&>(pair1, &pair::first);
  assert_valid_get_with<typename TypeParam::first, pair const&&>(pair1, &pair::first);
}

/// @test a @c std::pair 's second value can be obtained using @c std::get
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GetSecondType,
    (std::is_integral_v<typename TypeParam::second> && std::is_copy_constructible_v<typename TypeParam::first> &&
     !std::is_same_v<typename TypeParam::first, typename TypeParam::second>)
) {
  auto second_value = static_cast<typename TypeParam::second>(42);

  using pair = typename TestFixture::pair_type;

  auto pair1 = pair{dummy_value<typename TypeParam::first>, second_value};

  assert_valid_get_with<typename TypeParam::second, pair&>(pair1, &pair::second);
  assert_valid_get_with<typename TypeParam::second, pair const&>(pair1, &pair::second);
  assert_valid_get_with<typename TypeParam::second, pair&&>(pair1, &pair::second);
  assert_valid_get_with<typename TypeParam::second, pair const&&>(pair1, &pair::second);
}

/// @test @c std::get returns ref-collapsed references using the ref
/// qualification of the @c std::pair
TEST(StdPairTests, GetByTypeWithReferenceTypes) {
  auto first = 1;
  auto second = 2;

  auto const test_with_inputs = [&first, &second](auto& lvalue_ref, auto&& rvalue_ref) {
    using pair_type = std::pair<decltype(lvalue_ref), decltype(rvalue_ref)>;

    auto values =
        pair_type{std::forward<decltype(lvalue_ref)>(lvalue_ref), std::forward<decltype(rvalue_ref)>(rvalue_ref)};

    static_assert(
        std::is_same_v<
            typename pair_type::first_type,
            decltype(std::get<typename pair_type::first_type>(std::declval<pair_type&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );
    static_assert(
        std::is_same_v<
            typename pair_type::first_type,
            decltype(std::get<typename pair_type::first_type>(std::declval<pair_type const&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );
    static_assert(
        std::is_same_v<
            typename pair_type::first_type,
            decltype(std::get<typename pair_type::first_type>(std::declval<pair_type&&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );
    static_assert(
        std::is_same_v<
            typename pair_type::first_type,
            decltype(std::get<typename pair_type::first_type>(std::declval<pair_type const&&>()))>,
        "get<> of an lvalue reference type always returns an lvalue reference"
    );

    static_assert(
        std::is_same_v<
            typename pair_type::second_type&,
            decltype(std::get<typename pair_type::second_type>(std::declval<pair_type&>()))>,
        "get<> of an rvalue reference type returns an lvalue reference if the argument is an lvalue reference"
    );
    static_assert(
        std::is_same_v<
            typename pair_type::second_type&,
            decltype(std::get<typename pair_type::second_type>(std::declval<pair_type const&>()))>,
        "get<> of an rvalue reference type returns an lvalue reference if the argument is an lvalue reference"
    );
    static_assert(
        std::is_same_v<
            typename pair_type::second_type,
            decltype(std::get<typename pair_type::second_type>(std::declval<pair_type&&>()))>,
        "get<> of an rvalue reference type returns an rvalue reference if the argument is an rvalue reference"
    );
    static_assert(
        std::is_same_v<
            typename pair_type::second_type,
            decltype(std::get<typename pair_type::second_type>(std::declval<pair_type const&&>()))>,
        "get<> of an rvalue reference type returns an rvalue reference if the argument is an rvalue reference"
    );

    ASSERT_EQ(&std::get<0>(values), &first);
    ASSERT_EQ(&std::get<typename pair_type::second_type>(values), &second);
  };

  // NOLINTBEGIN(hicpp-move-const-arg)
  test_with_inputs(first, std::move(second));
  test_with_inputs(arene::base::as_const(first), std::move(arene::base::as_const(second)));
  // NOLINTEND(hicpp-move-const-arg)
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForClassFirstCopiesValues,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  construction_tracker const source{};
  typename TestFixture::pair_type const pair1{source, dummy_value<typename TypeParam::second>};

  ASSERT_EQ(pair1.first.get_tracked_state(), construction_tracker::state::copy_constructed);
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForClassSecondCopiesValues,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  construction_tracker const source{};
  typename TestFixture::pair_type const pair1{dummy_value<typename TypeParam::first>, source};

  ASSERT_EQ(pair1.second.get_tracked_state(), construction_tracker::state::copy_constructed);
}

/// @test Constructing a @c std::pair from a pair of values moves rvalues from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForClassFirstMovesRValues,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1{construction_tracker{}, dummy_value<typename TypeParam::second>};

  ASSERT_EQ(pair1.first.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test Constructing a @c std::pair from a pair of values moves rvalues from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForClassSecondMovesRValues,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1{dummy_value<typename TypeParam::first>, construction_tracker{}};

  ASSERT_EQ(pair1.second.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test A pair is explicitly constructible from values that are constructible from the supplied arguments
TYPED_TEST(StdPairTests, IsExplicitlyConstructibleIfTypesAreConstructible) {
  static_assert(
      std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first const&>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == std::is_copy_constructible_v<typename TypeParam::second>,
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first&>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<typename TypeParam::first, explicitly_convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second const&>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == std::is_copy_constructible_v<typename TypeParam::first>,
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second&>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<
              explicitly_convertible_from<typename TypeParam::first>,
              explicitly_convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  struct local {};

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );
}

/// @test A pair is implicitly constructible from values that are convertible from the supplied arguments
TYPED_TEST(StdPairTests, IsImplicitlyConstructibleIfTypesAreConvertible) {
  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is convertible from values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not convertible from values if args are not implicitly convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first const&>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == std::is_copy_constructible_v<typename TypeParam::second>,
      "Pair is convertible from values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first&>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not convertible from values if args are not convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is convertible from values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, explicitly_convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not convertible from values if args are not implicitly convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second const&>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == std::is_copy_constructible_v<typename TypeParam::first>,
      "Pair is convertible from values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second&>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not convertible from values if args are not convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is convertible from values if args are convertible"
  );

  struct local {};

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from values if args are not convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from values if args are not convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from values if args are not convertible"
  );
}

/// @test A pair is nothrow-constructible from values that are nothrow-convertible from the supplied arguments
TYPED_TEST(StdPairTests, IsNothrowConvertibleConstructibleIfAppropriate) {
  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first const&>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == std::is_nothrow_copy_constructible_v<typename TypeParam::second>,
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first&>, typename TypeParam::second>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second const&>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == std::is_nothrow_copy_constructible_v<typename TypeParam::first>,
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second&>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&> == (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
                                                 std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  struct local {};

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          typename TypeParam::first const&,
          typename TypeParam::second const&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );
}

/// @test Constructing a @c std::pair from a pair of values initializes the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForClassFirstConstructsWithArgs,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  construction_tracker::some_arg_type const source{};
  typename TestFixture::pair_type const pair1{source, dummy_value<typename TypeParam::second>};

  ASSERT_EQ(pair1.first.get_tracked_state(), construction_tracker::state::direct_constructed);
  typename TestFixture::pair_type const
      pair2{construction_tracker::some_arg_type{}, dummy_value<typename TypeParam::second>};

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::direct_rvalue_constructed);
}

/// @test Constructing a @c std::pair from a pair of values initializes the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionForClassSecondConstructsWithArgs,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  construction_tracker::some_arg_type const source{};
  typename TestFixture::pair_type const pair1{dummy_value<typename TypeParam::first>, source};

  ASSERT_EQ(pair1.second.get_tracked_state(), construction_tracker::state::direct_constructed);
  typename TestFixture::pair_type const pair2{
      dummy_value<typename TypeParam::first>,
      construction_tracker::some_arg_type{}
  };

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::direct_rvalue_constructed);
}

/// @test @c pair is copy-constructible if and only if the types are
TYPED_TEST(StdPairTests, IsCopyConstructibleIfArgsAreCopyConstructible) {
  static_assert(
      std::is_copy_constructible_v<typename TestFixture::pair_type> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is copy constructible if args are copy constructible"
  );
}

/// @test @c pair is nothrow-copy-constructible if and only if the types are
TYPED_TEST(StdPairTests, IsNothrowCopyConstructibleIfArgsAreCopyConstructible) {
  static_assert(
      std::is_nothrow_copy_constructible_v<typename TestFixture::pair_type> ==
          (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is nothrow copy constructible if args are nothrow copy constructible"
  );
}

/// @test @c pair is trivially-copy-constructible if and only if the types are
TYPED_TEST(StdPairTests, IsTriviallyCopyConstructibleIfTypesAre) {
  static_assert(
      std::is_trivially_copy_constructible_v<typename TestFixture::pair_type> ==
          (std::is_trivially_copy_constructible_v<typename TypeParam::first> &&
           std::is_trivially_copy_constructible_v<typename TypeParam::second>),
      "Pair is trivially copy constructible if args are trivially copy constructible"
  );
}

/// @test @c pair is move-constructible if and only if the types are
TYPED_TEST(StdPairTests, IsMoveConstructibleIfArgsAreMoveConstructible) {
  static_assert(
      std::is_move_constructible_v<typename TestFixture::pair_type> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is move constructible if args are move constructible"
  );
}

/// @test @c pair is nothrow-move-constructible if and only if the types are
TYPED_TEST(StdPairTests, IsNothrowMoveConstructibleIfArgsAreMoveConstructible) {
  static_assert(
      std::is_nothrow_move_constructible_v<typename TestFixture::pair_type> ==
          (std::is_nothrow_move_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_move_constructible_v<typename TypeParam::second>),
      "Pair is nothrow move constructible if args are nothrow move constructible"
  );
}

/// @test @c pair is trivially-move-constructible if and only if the types are
TYPED_TEST(StdPairTests, IsTriviallyMoveConstructibleIfTypesAre) {
  static_assert(
      std::is_trivially_move_constructible_v<typename TestFixture::pair_type> ==
          (std::is_trivially_move_constructible_v<typename TypeParam::first> &&
           std::is_trivially_move_constructible_v<typename TypeParam::second>),
      "Pair is trivially move constructible if args are trivially move constructible"
  );
}

/// @test @c pair is explicitly constructible from a pair of other types if the corresponding types are explicitly
/// constructible
TYPED_TEST(StdPairTests, IsExplicitlyConstructibleConstructibleFromConstPairIfAppropriate) {
  static_assert(
      std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<typename TypeParam::first, explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<
              explicitly_convertible_from<typename TypeParam::first>,
              explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  struct local {};

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<explicitly_convertible_from<local>, explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, explicitly_convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<explicitly_convertible_from<local>, explicitly_convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );
}

/// @test @c pair is explicitly constructible from an rvalue pair of other types if the corresponding types are
/// explicitly constructible
TYPED_TEST(StdPairTests, IsExplicitlyConstructibleConstructibleFromRValuePairIfAppropriate) {
  static_assert(
      std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<typename TypeParam::first, explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<
              explicitly_convertible_from<typename TypeParam::first>,
              explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are constructible"
  );

  struct local {};

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<explicitly_convertible_from<local>, explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, explicitly_convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<explicitly_convertible_from<local>, explicitly_convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not constructible"
  );
}

/// @test @c pair is convertible from a pair of other types if the corresponding types are convertible
TYPED_TEST(StdPairTests, IsConvertibleConstructibleFromConstPairIfAppropriate) {
  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is convertible from pair of values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not convertible from pair of values if args are not implicitly convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is convertible from pair of values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not convertible from pair of values if args are not implicitly convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_constructible_v<typename TypeParam::first> &&
           std::is_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  struct local {};

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not convertible from pair of values if args are not convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not convertible from pair of values if args are not convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not convertible from pair of values if args are not convertible"
  );
}

/// @test @c pair is convertible from an rvalue pair of other types if the corresponding types are convertible
TYPED_TEST(StdPairTests, IsConvertibleConstructibleFromRvaluePairIfAppropriate) {
  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is convertible from pair of values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<explicitly_convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not convertible from pair of values if args are not implicitly convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is convertible from pair of values if args are convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<typename TypeParam::first, explicitly_convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not convertible from pair of values if args are not implicitly convertible"
  );

  static_assert(
      arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_constructible_v<typename TypeParam::first> &&
           std::is_move_constructible_v<typename TypeParam::second>),
      "Pair is convertible from pair of values if args are convertible"
  );

  struct local {};

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not convertible from pair of values if args are not convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not convertible from pair of values if args are not convertible"
  );

  static_assert(
      !arene::base::is_implicitly_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not convertible from pair of values if args are not convertible"
  );
}

/// @test @c pair is nothrow convertible from a pair of other types if the corresponding types are nothrow convertible
TYPED_TEST(StdPairTests, IsNothrowConvertibleConstructibleFromConstPairIfAppropriate) {
  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_nothrow_copy_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_copy_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  struct local {};

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );
}

/// @test @c pair is nothrow convertible from an rvalue pair of other types if the corresponding types are nothrow
/// convertible
TYPED_TEST(StdPairTests, IsNothrowConvertibleConstructibleFromRValuePairIfAppropriate) {
  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_nothrow_move_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<typename TypeParam::first, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_nothrow_move_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  static_assert(
      std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_nothrow_move_constructible_v<typename TypeParam::first> &&
           std::is_nothrow_move_constructible_v<typename TypeParam::second>),
      "Pair is constructible from pair of values if args are convertible"
  );

  struct local {};

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<local>, convertible_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<typename TypeParam::first>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );

  static_assert(
      !std::is_nothrow_constructible_v<
          std::pair<convertible_from<local>, convertible_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not constructible from pair of values if args are not convertible"
  );
}

/// @test the value constructor is usable in a constant expression if the members are
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructorUsableInConstantExpressionIfArgsAre,
    (std::is_trivially_copy_constructible_v<typename TypeParam::first> &&
     std::is_trivially_copy_constructible_v<typename TypeParam::second> &&
     std::is_trivially_default_constructible_v<typename TypeParam::first> &&
     std::is_trivially_default_constructible_v<typename TypeParam::second>)
) {
  static_assert(
      constexpr_check(typename TestFixture::pair_type{typename TypeParam::first{}, typename TypeParam::second{}}),
      "Must be value-constructible in a constant expression"
  );
}

/// @test the value constructor is usable in a constant expression if the members are
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    NonTemplateValueConstructorUsableInConstantExpressionIfArgsAre,
    (std::is_trivially_copy_constructible_v<typename TypeParam::first> &&
     std::is_trivially_copy_constructible_v<typename TypeParam::second> &&
     std::is_trivially_default_constructible_v<typename TypeParam::first> &&
     std::is_trivially_default_constructible_v<typename TypeParam::second>)
) {
  static_assert(
      constexpr_check(typename TestFixture::pair_type{{}, {}}),
      "Must be value-constructible in a constant expression"
  );
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassFirstCopiesValues,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  std::pair<convertible_to<construction_tracker>, typename TypeParam::second> const
      pair1{construction_tracker{}, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const pair2{pair1};

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::copy_constructed);
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassSecondCopiesValues,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  std::pair<typename TypeParam::first, convertible_to<construction_tracker>> const pair1{
      dummy_value<typename TypeParam::first>,
      construction_tracker{}
  };
  typename TestFixture::pair_type const pair2{pair1};

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::copy_constructed);
}

/// @test Constructing a @c std::pair from a pair of values moves rvalues from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassFirstMovesRValues,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  std::pair<rvalue_convertible_to<construction_tracker>, typename TypeParam::second>
      pair1{construction_tracker{}, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const pair2{std::move(pair1)};

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test Constructing a @c std::pair from a pair of values moves rvalues from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassSecondMovesRValues,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  std::pair<typename TypeParam::first, rvalue_convertible_to<construction_tracker>> pair1{
      dummy_value<typename TypeParam::first>,
      construction_tracker{}
  };
  typename TestFixture::pair_type const pair2{std::move(pair1)};

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassFirstCopiesLValueRef,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  // NOLINTNEXTLINE(misc-const-correctness)
  construction_tracker source{};
  std::pair<construction_tracker&, typename TypeParam::second> pair1{source, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const pair2{std::move(pair1)};

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::copy_constructed);
}

/// @test Constructing a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassSecondCopiesLValueRef,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  // NOLINTNEXTLINE(misc-const-correctness)
  construction_tracker source{};
  std::pair<typename TypeParam::first, construction_tracker&> pair1{dummy_value<typename TypeParam::first>, source};
  typename TestFixture::pair_type const pair2{std::move(pair1)};

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::copy_constructed);
}

/// @test Constructing a @c std::pair from a pair of values moves rvalues from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassFirstMovesRValueRef,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  // NOLINTNEXTLINE(misc-const-correctness)
  construction_tracker source{};
  std::pair<construction_tracker&&, typename TypeParam::second>
      pair1{std::move(source), dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const pair2{std::move(pair1)};

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test Constructing a @c std::pair from a pair of values moves rvalues from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueConstructionFromOtherPairForClassSecondMovesRValueRef,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  // NOLINTNEXTLINE(misc-const-correctness)
  construction_tracker source{};
  std::pair<typename TypeParam::first, construction_tracker&&> pair1{
      dummy_value<typename TypeParam::first>,
      std::move(source)
  };
  typename TestFixture::pair_type const pair2{std::move(pair1)};

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test the pair move constructor is usable in a constant expression if the members are
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    NonTemplateValueConstructorFromRValueOtherPairUsableInConstantExpressionIfArgsAre,
    (std::is_trivially_copy_constructible_v<typename TypeParam::first> &&
     std::is_trivially_copy_constructible_v<typename TypeParam::second> &&
     std::is_trivially_default_constructible_v<typename TypeParam::first> &&
     std::is_trivially_default_constructible_v<typename TypeParam::second>)
) {
  static_assert(
      constexpr_check(typename TestFixture::pair_type{
          std::pair<convertible_to<typename TypeParam::first>, convertible_to<typename TypeParam::second>>{
              typename TypeParam::first{},
              typename TypeParam::second{}
          }
      }),
      "Must be value-constructible in a constant expression"
  );
}

/// @test the pair copy constructor is usable in a constant expression if the members are
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    NonTemplateValueConstructorFromLValueOtherPairUsableInConstantExpressionIfArgsAre,
    (std::is_trivially_copy_constructible_v<typename TypeParam::first> &&
     std::is_trivially_copy_constructible_v<typename TypeParam::second> &&
     std::is_trivially_default_constructible_v<typename TypeParam::first> &&
     std::is_trivially_default_constructible_v<typename TypeParam::second>)
) {
  constexpr std::pair<convertible_to<typename TypeParam::first>, convertible_to<typename TypeParam::second>> source{
      typename TypeParam::first{},
      typename TypeParam::second{}
  };
  static_assert(
      constexpr_check(typename TestFixture::pair_type{source}),
      "Must be value-constructible in a constant expression"
  );
}

template <bool NoExcept = true>
class multi_arg_constructible {
 public:
  std::int32_t first_arg;
  std::int32_t second_arg;
  std::int32_t third_arg;

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr multi_arg_constructible(std::int32_t first, std::int32_t second, std::int32_t third) noexcept(NoExcept)
      : first_arg{first},
        second_arg{second},
        third_arg{third} {}
};

/// @test piecewise_construct_t can be default constructed
TEST(StdPairTests, PiecewiseConstructTag) {
  static_assert(std::is_default_constructible_v<std::piecewise_construct_t>, "Should be default constructible");
}

/// @test piecewise construction is disabled when types are not constructible
TEST(StdPairTests, PiecewiseConstructionConstraints) {
  static_assert(
      !std::is_constructible_v<
          std::pair<not_default_constructible, std::int32_t>,
          std::piecewise_construct_t,
          std::tuple<>,
          std::tuple<std::int32_t>>,
      "Should not be constructible without required args"
  );

  static_assert(
      !std::is_constructible_v<
          std::pair<std::int32_t, not_default_constructible>,
          std::piecewise_construct_t,
          std::tuple<std::int32_t>,
          std::tuple<>>,
      "Should not be constructible without required args"
  );

  static_assert(
      std::is_constructible_v<
          std::pair<not_default_constructible, std::int32_t>,
          std::piecewise_construct_t,
          std::tuple<std::int32_t>,
          std::tuple<std::int32_t>>,
      "Should be constructible with required args"
  );
}

/// @test piecewise construction works
TEST(StdPairTests, PiecewiseConstructionWorksAsExpected) {
  auto const pair_from_empty_tuples =
      std::pair<std::int32_t, std::int32_t>{std::piecewise_construct, std::tuple<>{}, std::tuple<>{}};
  ASSERT_EQ(pair_from_empty_tuples.first, 0);
  ASSERT_EQ(pair_from_empty_tuples.second, 0);

  auto const pair_from_single_tuples = std::pair<std::int32_t, std::int32_t>{
      std::piecewise_construct,
      std::tuple<std::int32_t>{42},
      std::tuple<std::int32_t>{99}
  };
  ASSERT_EQ(pair_from_single_tuples.first, 42);
  ASSERT_EQ(pair_from_single_tuples.second, 99);

  auto const pair_from_multi_tuples = std::pair<multi_arg_constructible<>, multi_arg_constructible<>>{
      std::piecewise_construct,
      std::tuple<std::int32_t, std::int32_t, std::int32_t>{1, 2, 3},
      std::tuple<std::int32_t, std::int32_t, std::int32_t>{4, 5, 6}
  };
  ASSERT_EQ(pair_from_multi_tuples.first.first_arg, 1);
  ASSERT_EQ(pair_from_multi_tuples.first.second_arg, 2);
  ASSERT_EQ(pair_from_multi_tuples.first.third_arg, 3);
  ASSERT_EQ(pair_from_multi_tuples.second.first_arg, 4);
  ASSERT_EQ(pair_from_multi_tuples.second.second_arg, 5);
  ASSERT_EQ(pair_from_multi_tuples.second.third_arg, 6);
}

template <class Dummy>
constexpr auto test_piecewise_construct_in_constexpr() -> bool {
  auto const pair = std::pair<std::int32_t, std::int32_t>{
      std::piecewise_construct,
      std::tuple<std::int32_t>{42},
      std::tuple<std::int32_t>{99}
  };
  std::ignore = pair;
  return true;
}

template <class Dummy>
using tests_construct = std::integral_constant<bool, test_piecewise_construct_in_constexpr<Dummy>()>;

/// @test piecewise construction should not be constexpr
TEST(StdPairTests, PiecewiseConstructionIsNotConstexpr) {
  ASSERT_FALSE(arene::base::substitution_succeeds<tests_construct, void>);
}

/// @test std::piecewise_construct does not have an explicit constructor
TEST(StdPairTests, PiecewiseConstructDoesNotHaveExplicitConstructor) {
  auto const pair_from_single_tuples =
      std::pair<std::int32_t, std::int32_t>{{}, std::tuple<std::int32_t>{42}, std::tuple<std::int32_t>{99}};
  ASSERT_EQ(pair_from_single_tuples.first, 42);
  ASSERT_EQ(pair_from_single_tuples.second, 99);
}

/// @test piecewise construction noexcept specification is correct
TEST(StdPairTests, PiecewiseConstructionNoexceptSpecification) {
  static_assert(
      noexcept(std::pair<multi_arg_constructible<true>, std::int32_t>{
          std::piecewise_construct,
          std::tuple<std::int32_t, std::int32_t, std::int32_t>{1, 2, 3},
          std::tuple<std::int32_t>{3}
      }),
      "Should not be noexcept for throwing constructible types"
  );

  static_assert(
      !noexcept(std::pair<multi_arg_constructible<false>, std::int32_t>{
          std::piecewise_construct,
          std::tuple<std::int32_t, std::int32_t, std::int32_t>{1, 2, 3},
          std::tuple<std::int32_t>{3}
      }),
      "Should not be noexcept for throwing constructible types"
  );
}

/// @test piecewise construction forwards arguments correctly
TEST(StdPairTests, PiecewiseConstructionForwardsArguments) {
  auto tracker_tuple1 = std::tuple<construction_tracker::some_arg_type>{construction_tracker::some_arg_type{}};
  auto tracker_tuple2 = std::tuple<construction_tracker::some_arg_type>{construction_tracker::some_arg_type{}};

  // NOLINTBEGIN(hicpp-move-const-arg)
  std::pair<construction_tracker, construction_tracker> const piecewise_pair{
      std::piecewise_construct,
      std::move(tracker_tuple1),
      std::move(tracker_tuple2)
  };
  // NOLINTEND(hicpp-move-const-arg)

  ASSERT_EQ(piecewise_pair.first.get_tracked_state(), construction_tracker::state::direct_rvalue_constructed);
  ASSERT_EQ(piecewise_pair.second.get_tracked_state(), construction_tracker::state::direct_rvalue_constructed);
}

struct move_only {
  move_only() = default;
  ~move_only() = default;

  explicit move_only(std::int32_t input)
      : value{input} {}

  move_only(move_only const&) = delete;
  auto operator=(move_only const&) -> move_only& = delete;

  move_only(move_only&&) noexcept = default;
  auto operator=(move_only&&) noexcept -> move_only& = default;

  std::int32_t value{};
};

/// @test piecewise construction with reference types
TEST(StdPairTests, PiecewiseConstructionWithReferenceTypes) {
  auto first = move_only{42};
  auto second = move_only{99};

  auto const pair = std::pair<move_only, move_only>{
      std::piecewise_construct,
      std::tuple<move_only&&>{std::move(first)},
      std::tuple<move_only&&>{std::move(second)}
  };

  ASSERT_EQ(pair.first.value, 42);
  ASSERT_EQ(pair.second.value, 99);
}

/// @test the pair copy assignment operator is declared if and only if both types are copy assignable
TYPED_TEST(StdPairTests, IsCopyAssignableIfBothMembersAre) {
  static_assert(
      std::is_copy_assignable_v<typename TestFixture::pair_type> ==
          (std::is_copy_assignable_v<typename TypeParam::first> &&
           std::is_copy_assignable_v<typename TypeParam::second>),
      "Pair is copy assignable if and only if both members are"
  );
}

/// @test the pair copy assignment operator is noexcept if and only if both types are nothrow copy assignable
TYPED_TEST(StdPairTests, IsNothrowCopyAssignableIfBothMembersAre) {
  static_assert(
      std::is_nothrow_copy_assignable_v<typename TestFixture::pair_type> ==
          (std::is_nothrow_copy_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_copy_assignable_v<typename TypeParam::second>),
      "Pair is copy assignable if and only if both members are"
  );
}

/// @test the pair move assignment operator is declared if and only if both types are move assignable
TYPED_TEST(StdPairTests, IsMoveAssignableIfBothMembersAre) {
  static_assert(
      std::is_move_assignable_v<typename TestFixture::pair_type> ==
          (std::is_move_assignable_v<typename TypeParam::first> &&
           std::is_move_assignable_v<typename TypeParam::second>),
      "Pair is move assignable if and only if both members are"
  );
}

/// @test the pair move assignment operator is noexcept if and only if both types are nothrow move assignable
TYPED_TEST(StdPairTests, IsNothrowMoveAssignableIfBothMembersAre) {
  static_assert(
      std::is_nothrow_move_assignable_v<typename TestFixture::pair_type> ==
          (std::is_nothrow_move_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_move_assignable_v<typename TypeParam::second>),
      "Pair is move assignable if and only if both members are"
  );
}

/// @test @c pair is assignable from a pair of other types if the corresponding types are assignable
TYPED_TEST(StdPairTests, IsAssignableFromConstPairIfAppropriate) {
  static_assert(
      std::is_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_assignable_v<typename TypeParam::first> &&
           std::is_copy_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_assignable_v<
          std::pair<typename TypeParam::first, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_assignable_v<typename TypeParam::first> &&
           std::is_copy_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_copy_assignable_v<typename TypeParam::first> &&
           std::is_copy_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  struct local {};

  static_assert(
      !std::is_assignable_v<
          std::pair<assignable_from<local>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_assignable_v<
          std::pair<assignable_from<local>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );
}

/// @test @c pair is nothrow assignable from a pair of other types if the corresponding types are assignable
TYPED_TEST(StdPairTests, IsNothrowAssignableFromConstPairIfAppropriate) {
  static_assert(
      std::is_nothrow_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_nothrow_copy_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_copy_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_nothrow_assignable_v<
          std::pair<typename TypeParam::first, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_nothrow_copy_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_copy_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_nothrow_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&> ==
          (std::is_nothrow_copy_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_copy_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  struct local {};

  static_assert(
      !std::is_nothrow_assignable_v<
          std::pair<assignable_from<local>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_nothrow_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_nothrow_assignable_v<
          std::pair<assignable_from<local>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second> const&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );
}

/// @test Assigning a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueAssignmentFromOtherPairForClassFirstCopiesValues,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second> && std::is_copy_assignable_v<typename TypeParam::second>)
) {
  std::pair<convertible_to<construction_tracker>, typename TypeParam::second> const
      pair1{construction_tracker{}, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type pair2{construction_tracker{}, dummy_value<typename TypeParam::second>};

  pair2 = pair1;

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::copy_assigned);
}

/// @test Assigning a @c std::pair from a pair of values copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueAssignmentFromOtherPairForClassSecondCopiesValues,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first> && std::is_copy_assignable_v<typename TypeParam::first>)
) {
  std::pair<typename TypeParam::first, convertible_to<construction_tracker>> const pair1{
      dummy_value<typename TypeParam::first>,
      construction_tracker{}
  };
  typename TestFixture::pair_type pair2{dummy_value<typename TypeParam::first>, construction_tracker{}};

  pair2 = pair1;

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::copy_assigned);
}

/// @test @c pair is assignable from an rvalue pair of other types if the corresponding types are assignable
TYPED_TEST(StdPairTests, IsAssignableFromRValuePairIfAppropriate) {
  static_assert(
      std::is_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_assignable_v<typename TypeParam::first> &&
           std::is_move_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_assignable_v<
          std::pair<typename TypeParam::first, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_assignable_v<typename TypeParam::first> &&
           std::is_move_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_move_assignable_v<typename TypeParam::first> &&
           std::is_move_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  struct local {};

  static_assert(
      !std::is_assignable_v<
          std::pair<assignable_from<local>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_assignable_v<
          std::pair<assignable_from<local>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );
}

/// @test @c pair is nothrow assignable from an rvalue pair of other types if the corresponding types are assignable
TYPED_TEST(StdPairTests, IsNothrowAssignableFromRValuePairIfAppropriate) {
  static_assert(
      std::is_nothrow_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, typename TypeParam::second>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_nothrow_move_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_move_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_nothrow_assignable_v<
          std::pair<typename TypeParam::first, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_nothrow_move_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_move_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  static_assert(
      std::is_nothrow_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&> ==
          (std::is_nothrow_move_assignable_v<typename TypeParam::first> &&
           std::is_nothrow_move_assignable_v<typename TypeParam::second>),
      "Pair is assignable from pair of values if args are assignable"
  );

  struct local {};

  static_assert(
      !std::is_nothrow_assignable_v<
          std::pair<assignable_from<local>, assignable_from<typename TypeParam::second>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_nothrow_assignable_v<
          std::pair<assignable_from<typename TypeParam::first>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );

  static_assert(
      !std::is_nothrow_assignable_v<
          std::pair<assignable_from<local>, assignable_from<local>>,
          std::pair<typename TypeParam::first, typename TypeParam::second>&&>,
      "Pair is not assignable from pair of values if args are not assignable"
  );
}

/// @test Assigning a @c std::pair from a pair of values moves the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueAssignmentFromRValueOtherPairForClassFirstMovesValues,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second> && std::is_copy_assignable_v<typename TypeParam::second>)
) {
  std::pair<rvalue_convertible_to<construction_tracker>, typename TypeParam::second>
      pair1{construction_tracker{}, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type pair2{construction_tracker{}, dummy_value<typename TypeParam::second>};

  pair2 = std::move(pair1);

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::move_assigned);
}

/// @test Assigning a @c std::pair from a pair of values moves the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueAssignmentFromRValueOtherPairForClassSecondMovesValues,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first> && std::is_copy_assignable_v<typename TypeParam::first>)
) {
  std::pair<typename TypeParam::first, rvalue_convertible_to<construction_tracker>> pair1{
      dummy_value<typename TypeParam::first>,
      construction_tracker{}
  };
  typename TestFixture::pair_type pair2{dummy_value<typename TypeParam::first>, construction_tracker{}};

  pair2 = std::move(pair1);

  construction_tracker dummy{};
  dummy = rvalue_convertible_to<construction_tracker>{dummy};

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::move_assigned);
}

/// @test Assigning a @c std::pair from an rvalue pair of values with lvalue ref copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueAssignmentFromRValueOtherPairForClassFirstCopiesLValues,
    (std::is_same_v<typename TypeParam::first, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::second> && std::is_copy_assignable_v<typename TypeParam::second>)
) {
  // NOLINTNEXTLINE(misc-const-correctness)
  construction_tracker source{};
  std::pair<rvalue_convertible_to<construction_tracker&>, typename TypeParam::second>
      pair1{source, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type pair2{construction_tracker{}, dummy_value<typename TypeParam::second>};

  pair2 = std::move(pair1);

  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::copy_assigned);
}

/// @test Assigning a @c std::pair from an rvalue pair of values with lvalue ref copies the values from the initializer
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ValueAssignmentFromRValueOtherPairForClassSecondCopiesLValues,
    (std::is_same_v<typename TypeParam::second, construction_tracker> &&
     std::is_copy_constructible_v<typename TypeParam::first> && std::is_copy_assignable_v<typename TypeParam::first>)
) {
  // NOLINTNEXTLINE(misc-const-correctness)
  construction_tracker source{};
  std::pair<typename TypeParam::first, rvalue_convertible_to<construction_tracker&>> pair1{
      dummy_value<typename TypeParam::first>,
      source
  };
  typename TestFixture::pair_type pair2{dummy_value<typename TypeParam::first>, construction_tracker{}};

  pair2 = std::move(pair1);

  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::copy_assigned);
}

template <typename T, typename = arene::base::constraints<>>
extern constexpr bool is_member_swappable_v = false;

template <typename T>
extern constexpr bool
    is_member_swappable_v<T, arene::base::constraints<decltype(std::declval<T&>().swap(std::declval<T&>()))>> = true;

template <typename T, typename = arene::base::constraints<>>
extern constexpr bool is_nothrow_member_swappable_v = false;

template <typename T>
extern constexpr bool
    is_nothrow_member_swappable_v<T, arene::base::constraints<std::enable_if_t<is_member_swappable_v<T>>>> =
        noexcept(std::declval<T&>().swap(std::declval<T&>()));

/// @test @c std::pair is swappable if and only if both argument types are swappable
TYPED_TEST(StdPairTests, CanSwapIfAndOnlyIfTypesAreSwappable) {
  static_assert(
      arene::base::is_swappable_v<typename TestFixture::pair_type> ==
          (arene::base::is_swappable_v<typename TypeParam::first> &&
           arene::base::is_swappable_v<typename TypeParam::second>),
      "Should be swappable if types are"
  );
}

/// @test @c std::pair is swappable if and only if both argument types are swappable
TYPED_TEST(StdPairTests, CanSwapWithMemberIfAndOnlyIfTypesAreSwappable) {
  static_assert(
      is_member_swappable_v<typename TestFixture::pair_type> ==
          (arene::base::is_swappable_v<typename TypeParam::first> &&
           arene::base::is_swappable_v<typename TypeParam::second>),
      "Should be swappable if types are"
  );
}

/// @test @c std::pair is nothrow swappable if and only if both argument types are nothrow swappable
TYPED_TEST(StdPairTests, NoexceptSwapIfAndOnlyIfTypesAreNothrowSwappable) {
  static_assert(
      arene::base::is_nothrow_swappable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_swappable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_swappable_v<typename TypeParam::second>),
      "Should be swappable if types are"
  );
}

/// @test @c std::pair is nothrow swappable if and only if both argument types are nothrow swappable
TYPED_TEST(StdPairTests, NoexceptSwapWithMemberIfAndOnlyIfTypesAreNothrowSwappable) {
  static_assert(
      is_nothrow_member_swappable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_swappable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_swappable_v<typename TypeParam::second>),
      "Should be swappable if types are"
  );
}

/// @test swapping two pairs swaps their first value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ArithmeticFirstTypeSwapsIfOtherIsSwappable,
    (std::is_arithmetic_v<typename TypeParam::first> && arene::base::is_swappable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TypeParam::first const val1{42};
  typename TypeParam::first const val2{99};

  typename TestFixture::pair_type pair1{val1, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type pair2{val2, dummy_value<typename TypeParam::second>};

  pair1.swap(pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.first, val2);
  ASSERT_EQ(pair2.first, val1);
  ARENE_IGNORE_END();
}

/// @test swapping two pairs swaps their second value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ArithmeticSecondTypeSwapsIfOtherIsSwappable,
    (std::is_arithmetic_v<typename TypeParam::second> && arene::base::is_swappable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TypeParam::second const val1{42};
  typename TypeParam::second const val2{99};

  typename TestFixture::pair_type pair1{dummy_value<typename TypeParam::first>, val1};
  typename TestFixture::pair_type pair2{dummy_value<typename TypeParam::first>, val2};

  pair1.swap(pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.second, val2);
  ASSERT_EQ(pair2.second, val1);
  ARENE_IGNORE_END();
}

/// @test swapping two pairs swaps their first value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    TrackerFirstTypeSwapsIfOtherIsSwappable,
    (std::is_same_v<construction_tracker, typename TypeParam::first> &&
     arene::base::is_swappable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type pair1{construction_tracker{}, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type pair2{construction_tracker{}, dummy_value<typename TypeParam::second>};

  pair1.swap(pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.first.get_tracked_state(), construction_tracker::state::swapped_lhs);
  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::swapped_rhs);
  ARENE_IGNORE_END();
}

/// @test swapping two pairs swaps their second value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    TrackerSecondTypeSwapsIfOtherIsSwappable,
    (std::is_same_v<construction_tracker, typename TypeParam::second> &&
     arene::base::is_swappable_v<typename TypeParam::first> && std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type pair1{dummy_value<typename TypeParam::first>, construction_tracker{}};
  typename TestFixture::pair_type pair2{dummy_value<typename TypeParam::first>, construction_tracker{}};

  pair1.swap(pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.second.get_tracked_state(), construction_tracker::state::swapped_lhs);
  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::swapped_rhs);
  ARENE_IGNORE_END();
}

/// @test swapping two pairs swaps their first value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ArithmeticFirstTypeNonMemberSwapsIfOtherIsSwappable,
    (std::is_arithmetic_v<typename TypeParam::first> && arene::base::is_swappable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TypeParam::first const val1{42};
  typename TypeParam::first const val2{99};

  typename TestFixture::pair_type pair1{val1, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type pair2{val2, dummy_value<typename TypeParam::second>};

  std::swap(pair1, pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.first, val2);
  ASSERT_EQ(pair2.first, val1);
  ARENE_IGNORE_END();
}

/// @test swapping two pairs swaps their second value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    ArithmeticSecondTypeNonMemberSwapsIfOtherIsSwappable,
    (std::is_arithmetic_v<typename TypeParam::second> && arene::base::is_swappable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TypeParam::second const val1{42};
  typename TypeParam::second const val2{99};

  typename TestFixture::pair_type pair1{dummy_value<typename TypeParam::first>, val1};
  typename TestFixture::pair_type pair2{dummy_value<typename TypeParam::first>, val2};

  std::swap(pair1, pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.second, val2);
  ASSERT_EQ(pair2.second, val1);
  ARENE_IGNORE_END();
}

/// @test swapping two pairs swaps their first value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    TrackerFirstTypeNonMemberSwapsIfOtherIsSwappable,
    (std::is_same_v<construction_tracker, typename TypeParam::first> &&
     arene::base::is_swappable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type pair1{construction_tracker{}, dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type pair2{construction_tracker{}, dummy_value<typename TypeParam::second>};

  std::swap(pair1, pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.first.get_tracked_state(), construction_tracker::state::swapped_lhs);
  ASSERT_EQ(pair2.first.get_tracked_state(), construction_tracker::state::swapped_rhs);
  ARENE_IGNORE_END();
}

/// @test swapping two pairs swaps their second value
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    TrackerSecondTypeNonMemberSwapsIfOtherIsSwappable,
    (std::is_same_v<construction_tracker, typename TypeParam::second> &&
     arene::base::is_swappable_v<typename TypeParam::first> && std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type pair1{dummy_value<typename TypeParam::first>, construction_tracker{}};
  typename TestFixture::pair_type pair2{dummy_value<typename TypeParam::first>, construction_tracker{}};

  std::swap(pair1, pair2);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  ASSERT_EQ(pair1.second.get_tracked_state(), construction_tracker::state::swapped_lhs);
  ASSERT_EQ(pair2.second.get_tracked_state(), construction_tracker::state::swapped_rhs);
  ARENE_IGNORE_END();
}

/// @test In C++14 @c std::pair has comparison operators that are always defined, even for non-comparable members
TYPED_TEST(StdPairTests, ComparisonOperatorsAlwaysDefined) {
  static_assert(
      arene::base::is_less_than_comparable_v<typename TestFixture::pair_type>,
      "Must be less-than-comparable"
  );
  static_assert(
      arene::base::is_greater_than_comparable_v<typename TestFixture::pair_type>,
      "Must be greater-than-comparable"
  );
  static_assert(arene::base::is_equality_comparable_v<typename TestFixture::pair_type>, "Must be equality-comparable");
  static_assert(
      arene::base::is_inequality_comparable_v<typename TestFixture::pair_type>,
      "Must be inequality-comparable"
  );
  static_assert(
      arene::base::is_less_than_or_equal_comparable_v<typename TestFixture::pair_type>,
      "Must be less-than-or-equal-comparable"
  );
  static_assert(
      arene::base::is_greater_than_or_equal_comparable_v<typename TestFixture::pair_type>,
      "Must be greater-than-or-equal-comparable"
  );
}

/// @test The comparison operators must be namespace scope
TYPED_TEST(StdPairTests, ComparisonOperatorsAlwaysDefinedAtNamespaceScope) {
  static_assert(
      std::is_same_v<
          decltype(std::operator<(
              std::declval<typename TestFixture::pair_type const&>(),
              std::declval<typename TestFixture::pair_type const&>()
          )),
          bool>,
      "Must be less-than-comparable"
  );
  static_assert(
      std::is_same_v<
          decltype(std::operator>(
              std::declval<typename TestFixture::pair_type const&>(),
              std::declval<typename TestFixture::pair_type const&>()
          )),
          bool>,
      "Must be greater-than-comparable"
  );
  static_assert(
      std::is_same_v<
          decltype(std::operator<=(
              std::declval<typename TestFixture::pair_type const&>(),
              std::declval<typename TestFixture::pair_type const&>()
          )),
          bool>,
      "Must be less-than-or-equal-comparable"
  );
  static_assert(
      std::is_same_v<
          decltype(std::operator>=(
              std::declval<typename TestFixture::pair_type const&>(),
              std::declval<typename TestFixture::pair_type const&>()
          )),
          bool>,
      "Must be greater-than-or-equal-comparable"
  );
  static_assert(
      std::is_same_v<
          decltype(std::operator==(
              std::declval<typename TestFixture::pair_type const&>(),
              std::declval<typename TestFixture::pair_type const&>()
          )),
          bool>,
      "Must be equality-comparable"
  );
  static_assert(
      std::is_same_v<
          decltype(std::operator!=(
              std::declval<typename TestFixture::pair_type const&>(),
              std::declval<typename TestFixture::pair_type const&>()
          )),
          bool>,
      "Must be inequality-comparable"
  );
}

template <typename T>
class wrapped_with_less {
  T value_;

 public:
  explicit wrapped_with_less(T value) noexcept
      : value_(value) {}

  friend auto operator<(wrapped_with_less const& lhs, wrapped_with_less const& rhs) noexcept -> bool {
    return lhs.value_ < rhs.value_;
  }
};

template <typename T>
class wrapped_with_equal {
  T value_;

 public:
  explicit wrapped_with_equal(T value) noexcept
      : value_(value) {}

  friend auto operator==(wrapped_with_equal const& lhs, wrapped_with_equal const& rhs) noexcept -> bool {
    return lhs.value_ == rhs.value_;
  }
};

/// @test The less than operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOperatorConsistentWithLexicalComparisonOfFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const
      pair1{static_cast<typename TypeParam::first>(42), dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const
      pair2{static_cast<typename TypeParam::first>(99), dummy_value<typename TypeParam::second>};

  ASSERT_TRUE(pair1 < pair2);
  ASSERT_FALSE(pair2 < pair1);
  ASSERT_FALSE(pair1 < pair1);
}

/// @test The less than operator orders pairs based on the @c second data member if the @c first data members are equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOperatorConsistentWithLexicalComparisonOfSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(99)
  };

  ASSERT_TRUE(pair1 < pair2);
  ASSERT_FALSE(pair2 < pair1);
  ASSERT_FALSE(pair1 < pair1);
}

/// @test The less than operator orders pairs based on the @c first data member and then the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOperatorConsistentWithLexicalComparisonOfBothTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(99)
  };
  typename TestFixture::pair_type const pair3{
      static_cast<typename TypeParam::first>(99),
      static_cast<typename TypeParam::second>(42)
  };

  ASSERT_TRUE(pair1 < pair2);
  ASSERT_TRUE(pair2 < pair3);
  ASSERT_TRUE(pair1 < pair3);
  ASSERT_FALSE(pair2 < pair1);
  ASSERT_FALSE(pair3 < pair2);
  ASSERT_FALSE(pair3 < pair1);
}

/// @test The less than operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOperatorConsistentWithLexicalComparisonOfWrappedFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using wrapped_pair = std::pair<first_wrapped, typename TypeParam::second>;
  wrapped_pair const
      pair1{first_wrapped{static_cast<typename TypeParam::first>(42)}, dummy_value<typename TypeParam::second>};
  wrapped_pair const
      pair2{first_wrapped{static_cast<typename TypeParam::first>(99)}, dummy_value<typename TypeParam::second>};

  ASSERT_TRUE(pair1 < pair2);
  ASSERT_FALSE(pair2 < pair1);
  ASSERT_FALSE(pair1 < pair1);
}

/// @test The less than operator orders pairs based on the @c second data member if the @c first data members are equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOperatorConsistentWithLexicalComparisonOfWrappedSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<typename TypeParam::first, second_wrapped>;
  wrapped_pair const pair1{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };

  ASSERT_TRUE(pair1 < pair2);
  ASSERT_FALSE(pair2 < pair1);
  ASSERT_FALSE(pair1 < pair1);
}

/// @test The less than operator orders pairs based on the @c first data member and then the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOperatorConsistentWithLexicalComparisonOfBothWrappedTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<first_wrapped, second_wrapped>;
  wrapped_pair const pair1{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };
  wrapped_pair const pair3{
      first_wrapped{static_cast<typename TypeParam::first>(99)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };

  ASSERT_TRUE(pair1 < pair2);
  ASSERT_TRUE(pair2 < pair3);
  ASSERT_TRUE(pair1 < pair3);
  ASSERT_FALSE(pair2 < pair1);
  ASSERT_FALSE(pair3 < pair2);
  ASSERT_FALSE(pair3 < pair1);
}

/// @test The less than or equals operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOrEqualsOperatorConsistentWithLexicalComparisonOfFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const
      pair1{static_cast<typename TypeParam::first>(42), dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const
      pair2{static_cast<typename TypeParam::first>(99), dummy_value<typename TypeParam::second>};

  ASSERT_TRUE(pair1 <= pair2);
  ASSERT_FALSE(pair2 <= pair1);
  ASSERT_TRUE(pair1 <= pair1);
}

/// @test The less than or equals operator orders pairs based on the @c second data member if the @c first data members
/// are equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOrEqualsOperatorConsistentWithLexicalComparisonOfSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(99)
  };

  ASSERT_TRUE(pair1 <= pair2);
  ASSERT_FALSE(pair2 <= pair1);
  ASSERT_TRUE(pair1 <= pair1);
}

/// @test The less than or equals operator orders pairs based on the @c first data member and then the @c second data
/// member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOrEqualsOperatorConsistentWithLexicalComparisonOfBothTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(99)
  };
  typename TestFixture::pair_type const pair3{
      static_cast<typename TypeParam::first>(99),
      static_cast<typename TypeParam::second>(42)
  };

  ASSERT_TRUE(pair1 <= pair2);
  ASSERT_TRUE(pair2 <= pair3);
  ASSERT_TRUE(pair1 <= pair3);
  ASSERT_FALSE(pair2 <= pair1);
  ASSERT_FALSE(pair3 <= pair2);
  ASSERT_FALSE(pair3 <= pair1);
}

/// @test The less than or equals operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOrEqualsOperatorConsistentWithLexicalComparisonOfWrappedFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using wrapped_pair = std::pair<first_wrapped, typename TypeParam::second>;
  wrapped_pair const
      pair1{first_wrapped{static_cast<typename TypeParam::first>(42)}, dummy_value<typename TypeParam::second>};
  wrapped_pair const
      pair2{first_wrapped{static_cast<typename TypeParam::first>(99)}, dummy_value<typename TypeParam::second>};

  ASSERT_TRUE(pair1 <= pair2);
  ASSERT_FALSE(pair2 <= pair1);
  ASSERT_TRUE(pair1 <= pair1);
}

/// @test The less than or equals operator orders pairs based on the @c second data member if the @c first data members
/// are equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOrEqualsOperatorConsistentWithLexicalComparisonOfWrappedSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<typename TypeParam::first, second_wrapped>;
  wrapped_pair const pair1{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };

  ASSERT_TRUE(pair1 <= pair2);
  ASSERT_FALSE(pair2 <= pair1);
  ASSERT_TRUE(pair1 <= pair1);
}

/// @test The less than or equals operator orders pairs based on the @c first data member and then the @c second data
/// member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    LessThanOrEqualsOperatorConsistentWithLexicalComparisonOfBothWrappedTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<first_wrapped, second_wrapped>;
  wrapped_pair const pair1{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };
  wrapped_pair const pair3{
      first_wrapped{static_cast<typename TypeParam::first>(99)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };

  ASSERT_TRUE(pair1 <= pair2);
  ASSERT_TRUE(pair2 <= pair3);
  ASSERT_TRUE(pair1 <= pair3);
  ASSERT_FALSE(pair2 <= pair1);
  ASSERT_FALSE(pair3 <= pair2);
  ASSERT_FALSE(pair3 <= pair1);
}

/// @test The equals operator compares the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualsOperatorConsistentWithComparisonOfFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_equality_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const
      pair1{static_cast<typename TypeParam::first>(42), dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const
      pair2{static_cast<typename TypeParam::first>(99), dummy_value<typename TypeParam::second>};

  ASSERT_FALSE(pair1 == pair2);
  ASSERT_FALSE(pair2 == pair1);
  ASSERT_TRUE(pair1 == pair1);
  ASSERT_TRUE(pair2 == pair2);
}

/// @test The equals operator compares the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualsOperatorConsistentWithComparisonOfSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_equality_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(99)
  };

  ASSERT_FALSE(pair1 == pair2);
  ASSERT_FALSE(pair2 == pair1);
  ASSERT_TRUE(pair1 == pair1);
  ASSERT_TRUE(pair2 == pair2);
}

/// @test The equals operator compares the @c first data member and the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualsOperatorConsistentWithComparisonOfBothTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(99)
  };
  typename TestFixture::pair_type const pair3{
      static_cast<typename TypeParam::first>(99),
      static_cast<typename TypeParam::second>(42)
  };

  ASSERT_FALSE(pair1 == pair2);
  ASSERT_FALSE(pair2 == pair3);
  ASSERT_FALSE(pair1 == pair3);
  ASSERT_FALSE(pair2 == pair1);
  ASSERT_FALSE(pair3 == pair2);
  ASSERT_FALSE(pair3 == pair1);
}

/// @test The equals operator compares the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualsOperatorConsistentWithComparisonOfWrappedFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_equality_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_equal<typename TypeParam::first>;
  using wrapped_pair = std::pair<first_wrapped, typename TypeParam::second>;
  wrapped_pair const
      pair1{first_wrapped{static_cast<typename TypeParam::first>(42)}, dummy_value<typename TypeParam::second>};
  wrapped_pair const
      pair2{first_wrapped{static_cast<typename TypeParam::first>(99)}, dummy_value<typename TypeParam::second>};

  ASSERT_FALSE(pair1 == pair2);
  ASSERT_FALSE(pair2 == pair1);
  ASSERT_TRUE(pair1 == pair1);
  ASSERT_TRUE(pair2 == pair2);
}

/// @test The equals operator compares the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualsOperatorConsistentWithComparisonOfWrappedSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_equality_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  using second_wrapped = wrapped_with_equal<typename TypeParam::second>;
  using wrapped_pair = std::pair<typename TypeParam::first, second_wrapped>;
  wrapped_pair const pair1{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };

  ASSERT_FALSE(pair1 == pair2);
  ASSERT_FALSE(pair2 == pair1);
  ASSERT_TRUE(pair1 == pair1);
  ASSERT_TRUE(pair2 == pair2);
}

/// @test The equals operator compares the @c first data member and the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualsOperatorConsistentWithComparisonOfBothWrappedTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_equal<typename TypeParam::first>;
  using second_wrapped = wrapped_with_equal<typename TypeParam::second>;
  using wrapped_pair = std::pair<first_wrapped, second_wrapped>;
  wrapped_pair const pair1{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };
  wrapped_pair const pair3{
      first_wrapped{static_cast<typename TypeParam::first>(99)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };

  ASSERT_FALSE(pair1 == pair2);
  ASSERT_FALSE(pair2 == pair3);
  ASSERT_FALSE(pair1 == pair3);
  ASSERT_FALSE(pair2 == pair1);
  ASSERT_FALSE(pair3 == pair2);
  ASSERT_FALSE(pair3 == pair1);
}

/// @test The not equals operator compares the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    InequalityOperatorConsistentWithComparisonOfFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_equality_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const
      pair1{static_cast<typename TypeParam::first>(42), dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const
      pair2{static_cast<typename TypeParam::first>(99), dummy_value<typename TypeParam::second>};

  ASSERT_TRUE(pair1 != pair2);
  ASSERT_TRUE(pair2 != pair1);
  ASSERT_FALSE(pair1 != pair1);
  ASSERT_FALSE(pair2 != pair2);
}

/// @test The not equals operator compares the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    InequalityOperatorConsistentWithComparisonOfSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_equality_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(99)
  };

  ASSERT_TRUE(pair1 != pair2);
  ASSERT_TRUE(pair2 != pair1);
  ASSERT_FALSE(pair1 != pair1);
  ASSERT_FALSE(pair2 != pair2);
}

/// @test The not equals operator compares the @c first data member and the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    InequalityOperatorConsistentWithComparisonOfBothTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(99)
  };
  typename TestFixture::pair_type const pair3{
      static_cast<typename TypeParam::first>(99),
      static_cast<typename TypeParam::second>(42)
  };

  ASSERT_TRUE(pair1 != pair2);
  ASSERT_TRUE(pair2 != pair3);
  ASSERT_TRUE(pair1 != pair3);
  ASSERT_TRUE(pair2 != pair1);
  ASSERT_TRUE(pair3 != pair2);
  ASSERT_TRUE(pair3 != pair1);
}

/// @test The not equals operator compares the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    InequalityOperatorConsistentWithComparisonOfWrappedFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_equality_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_equal<typename TypeParam::first>;
  using wrapped_pair = std::pair<first_wrapped, typename TypeParam::second>;
  wrapped_pair const
      pair1{first_wrapped{static_cast<typename TypeParam::first>(42)}, dummy_value<typename TypeParam::second>};
  wrapped_pair const
      pair2{first_wrapped{static_cast<typename TypeParam::first>(99)}, dummy_value<typename TypeParam::second>};

  ASSERT_TRUE(pair1 != pair2);
  ASSERT_TRUE(pair2 != pair1);
  ASSERT_FALSE(pair1 != pair1);
  ASSERT_FALSE(pair2 != pair2);
}

/// @test The not equals operator compares the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    InequalityOperatorConsistentWithComparisonOfWrappedSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_equality_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  using second_wrapped = wrapped_with_equal<typename TypeParam::second>;
  using wrapped_pair = std::pair<typename TypeParam::first, second_wrapped>;
  wrapped_pair const pair1{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };

  ASSERT_TRUE(pair1 != pair2);
  ASSERT_TRUE(pair2 != pair1);
  ASSERT_FALSE(pair1 != pair1);
  ASSERT_FALSE(pair2 != pair2);
}

/// @test The not equals operator compares the @c first data member and the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    InequalityOperatorConsistentWithComparisonOfBothWrappedTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_equal<typename TypeParam::first>;
  using second_wrapped = wrapped_with_equal<typename TypeParam::second>;
  using wrapped_pair = std::pair<first_wrapped, second_wrapped>;
  wrapped_pair const pair1{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };
  wrapped_pair const pair3{
      first_wrapped{static_cast<typename TypeParam::first>(99)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };

  ASSERT_TRUE(pair1 != pair2);
  ASSERT_TRUE(pair2 != pair3);
  ASSERT_TRUE(pair1 != pair3);
  ASSERT_TRUE(pair2 != pair1);
  ASSERT_TRUE(pair3 != pair2);
  ASSERT_TRUE(pair3 != pair1);
}

/// @test The greater than operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOperatorConsistentWithLexicalComparisonOfFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const
      pair1{static_cast<typename TypeParam::first>(42), dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const
      pair2{static_cast<typename TypeParam::first>(99), dummy_value<typename TypeParam::second>};

  ASSERT_FALSE(pair1 > pair2);
  ASSERT_TRUE(pair2 > pair1);
  ASSERT_FALSE(pair1 > pair1);
}

/// @test The greater than operator orders pairs based on the @c second data member if the @c first data members are
/// equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOperatorConsistentWithLexicalComparisonOfSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(99)
  };

  ASSERT_FALSE(pair1 > pair2);
  ASSERT_TRUE(pair2 > pair1);
  ASSERT_FALSE(pair1 > pair1);
}

/// @test The greater than operator orders pairs based on the @c first data member and then the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOperatorConsistentWithLexicalComparisonOfBothTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(99)
  };
  typename TestFixture::pair_type const pair3{
      static_cast<typename TypeParam::first>(99),
      static_cast<typename TypeParam::second>(42)
  };

  ASSERT_FALSE(pair1 > pair2);
  ASSERT_FALSE(pair2 > pair3);
  ASSERT_FALSE(pair1 > pair3);
  ASSERT_TRUE(pair2 > pair1);
  ASSERT_TRUE(pair3 > pair2);
  ASSERT_TRUE(pair3 > pair1);
}

/// @test The greater than operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOperatorConsistentWithLexicalComparisonOfWrappedFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using wrapped_pair = std::pair<first_wrapped, typename TypeParam::second>;
  wrapped_pair const
      pair1{first_wrapped{static_cast<typename TypeParam::first>(42)}, dummy_value<typename TypeParam::second>};
  wrapped_pair const
      pair2{first_wrapped{static_cast<typename TypeParam::first>(99)}, dummy_value<typename TypeParam::second>};

  ASSERT_FALSE(pair1 > pair2);
  ASSERT_TRUE(pair2 > pair1);
  ASSERT_FALSE(pair1 > pair1);
}

/// @test The greater than operator orders pairs based on the @c second data member if the @c first data members are
/// equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOperatorConsistentWithLexicalComparisonOfWrappedSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<typename TypeParam::first, second_wrapped>;
  wrapped_pair const pair1{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };

  ASSERT_FALSE(pair1 > pair2);
  ASSERT_TRUE(pair2 > pair1);
  ASSERT_FALSE(pair1 > pair1);
}

/// @test The greater than operator orders pairs based on the @c first data member and then the @c second data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOperatorConsistentWithLexicalComparisonOfBothWrappedTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<first_wrapped, second_wrapped>;
  wrapped_pair const pair1{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };
  wrapped_pair const pair3{
      first_wrapped{static_cast<typename TypeParam::first>(99)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };

  ASSERT_FALSE(pair1 > pair2);
  ASSERT_FALSE(pair2 > pair3);
  ASSERT_FALSE(pair1 > pair3);
  ASSERT_TRUE(pair2 > pair1);
  ASSERT_TRUE(pair3 > pair2);
  ASSERT_TRUE(pair3 > pair1);
}

/// @test The greater than or equals operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOrEqualsOperatorConsistentWithLexicalComparisonOfFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const
      pair1{static_cast<typename TypeParam::first>(42), dummy_value<typename TypeParam::second>};
  typename TestFixture::pair_type const
      pair2{static_cast<typename TypeParam::first>(99), dummy_value<typename TypeParam::second>};

  ASSERT_FALSE(pair1 >= pair2);
  ASSERT_TRUE(pair2 >= pair1);
  ASSERT_TRUE(pair1 >= pair1);
  ASSERT_TRUE(pair2 >= pair2);
}

/// @test The greater than or equals operator orders pairs based on the @c second data member if the @c first data
/// members are equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOrEqualsOperatorConsistentWithLexicalComparisonOfSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  typename TestFixture::pair_type const pair1{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      dummy_value<typename TypeParam::first>,
      static_cast<typename TypeParam::second>(99)
  };

  ASSERT_FALSE(pair1 >= pair2);
  ASSERT_TRUE(pair2 >= pair1);
  ASSERT_TRUE(pair1 >= pair1);
}

/// @test The greater than oe equals operator orders pairs based on the @c first data member and then the @c second data
/// member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOrEqualsOperatorConsistentWithLexicalComparisonOfBothTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  typename TestFixture::pair_type const pair1{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(42)
  };
  typename TestFixture::pair_type const pair2{
      static_cast<typename TypeParam::first>(42),
      static_cast<typename TypeParam::second>(99)
  };
  typename TestFixture::pair_type const pair3{
      static_cast<typename TypeParam::first>(99),
      static_cast<typename TypeParam::second>(42)
  };

  ASSERT_FALSE(pair1 >= pair2);
  ASSERT_FALSE(pair2 >= pair3);
  ASSERT_FALSE(pair1 >= pair3);
  ASSERT_TRUE(pair2 >= pair1);
  ASSERT_TRUE(pair3 >= pair2);
  ASSERT_TRUE(pair3 >= pair1);
}

/// @test The greater than or equals operator orders pairs based on the @c first data member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOrEqualsOperatorConsistentWithLexicalComparisonOfWrappedFirst,
    (std::is_arithmetic_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second> &&
     std::is_copy_constructible_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using wrapped_pair = std::pair<first_wrapped, typename TypeParam::second>;
  wrapped_pair const
      pair1{first_wrapped{static_cast<typename TypeParam::first>(42)}, dummy_value<typename TypeParam::second>};
  wrapped_pair const
      pair2{first_wrapped{static_cast<typename TypeParam::first>(99)}, dummy_value<typename TypeParam::second>};

  ASSERT_FALSE(pair1 >= pair2);
  ASSERT_TRUE(pair2 >= pair1);
  ASSERT_TRUE(pair1 >= pair1);
}

/// @test The greater than or equals operator orders pairs based on the @c second data member if the @c first data
/// members are equal
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOrEqualsOperatorConsistentWithLexicalComparisonOfWrappedSecond,
    (std::is_arithmetic_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     std::is_copy_constructible_v<typename TypeParam::first>)
) {
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<typename TypeParam::first, second_wrapped>;
  wrapped_pair const pair1{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      dummy_value<typename TypeParam::first>,
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };

  ASSERT_FALSE(pair1 >= pair2);
  ASSERT_TRUE(pair2 >= pair1);
  ASSERT_TRUE(pair1 >= pair1);
}

/// @test The greater than or eqauls operator orders pairs based on the @c first data member and then the @c second data
/// member
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    GreaterThanOrEqualsOperatorConsistentWithLexicalComparisonOfBothWrappedTypes,
    (std::is_arithmetic_v<typename TypeParam::first> && std::is_arithmetic_v<typename TypeParam::second>)
) {
  using first_wrapped = wrapped_with_less<typename TypeParam::first>;
  using second_wrapped = wrapped_with_less<typename TypeParam::second>;
  using wrapped_pair = std::pair<first_wrapped, second_wrapped>;
  wrapped_pair const pair1{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };
  wrapped_pair const pair2{
      first_wrapped{static_cast<typename TypeParam::first>(42)},
      second_wrapped{static_cast<typename TypeParam::second>(99)}
  };
  wrapped_pair const pair3{
      first_wrapped{static_cast<typename TypeParam::first>(99)},
      second_wrapped{static_cast<typename TypeParam::second>(42)}
  };

  ASSERT_FALSE(pair1 >= pair2);
  ASSERT_FALSE(pair2 >= pair3);
  ASSERT_FALSE(pair1 >= pair3);
  ASSERT_TRUE(pair2 >= pair1);
  ASSERT_TRUE(pair3 >= pair2);
  ASSERT_TRUE(pair3 >= pair1);
}

/// @test The ordering comparison operators are noexcept if the basis comparison operator is noexcept
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    OrderingComparisonsAreConditionallyNoexcept,
    (arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second>)
) {
  static_assert(
      arene::base::is_nothrow_less_than_comparable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::second>),
      "Nothrow less than comparable if both members are"
  );
  static_assert(
      arene::base::is_nothrow_greater_than_comparable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::second>),
      "Nothrow greater than comparable if both members are nothrow less than comparable"
  );
  static_assert(
      arene::base::is_nothrow_less_than_or_equal_comparable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::second>),
      "Nothrow less than or equal comparable if both members are nothrow less than comparable"
  );
  static_assert(
      arene::base::is_nothrow_greater_than_or_equal_comparable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_less_than_comparable_v<typename TypeParam::second>),
      "Nothrow greater than or equal comparable if both members are nothrow less than comparable"
  );
}

/// @test The equality comparison operators are noexcept if the basis comparison operator is noexcept
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualityComparisonsAreConditionallyNoexcept,
    (arene::base::is_equality_comparable_v<typename TypeParam::first> &&
     arene::base::is_equality_comparable_v<typename TypeParam::second>)
) {
  static_assert(
      arene::base::is_nothrow_equality_comparable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_equality_comparable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_equality_comparable_v<typename TypeParam::second>),
      "Nothrow equality comparable if both members are"
  );
  static_assert(
      arene::base::is_nothrow_inequality_comparable_v<typename TestFixture::pair_type> ==
          (arene::base::is_nothrow_equality_comparable_v<typename TypeParam::first> &&
           arene::base::is_nothrow_equality_comparable_v<typename TypeParam::second>),
      "Nothrow inequality comparable if both members are nothrow equality comparable"
  );
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "float equal comparison is not invoked here");
template <typename T, bool = constexpr_check(T{} == T{})>
constexpr auto constexpr_equals_check(std::int32_t) -> std::true_type;
ARENE_IGNORE_END();

template <typename T>
constexpr auto constexpr_equals_check(not_default_constructible) -> std::false_type;

template <typename T>
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
constexpr extern bool constexpr_equals_supported_v =
    std::is_same_v<decltype(constexpr_equals_check<T>(0)), std::true_type>;

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "float equal comparison is not invoked here");
template <typename T, bool = constexpr_check(T{} != T{})>
constexpr auto constexpr_not_equals_check(std::int32_t) -> std::true_type;
ARENE_IGNORE_END();

template <typename T>
constexpr auto constexpr_not_equals_check(not_default_constructible) -> std::false_type;

template <typename T>
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
constexpr extern bool constexpr_not_equals_supported_v =
    std::is_same_v<decltype(constexpr_not_equals_check<T>(0)), std::true_type>;

/// @test the equality comparison operators are usable in a constant expression if the members are
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    EqualityComparisonUsableInConstexprContextIfTypesAre,
    (std::is_trivially_default_constructible_v<typename TypeParam::first> &&
     std::is_trivially_default_constructible_v<typename TypeParam::second> &&
     arene::base::is_equality_comparable_v<typename TypeParam::first> &&
     arene::base::is_equality_comparable_v<typename TypeParam::second>)
) {
  static_assert(
      constexpr_equals_supported_v<typename TestFixture::pair_type> ==
          (constexpr_equals_supported_v<typename TypeParam::first> &&
           constexpr_equals_supported_v<typename TypeParam::second>),
      "Must be comparable in a constant expression if types are"
  );
  static_assert(
      constexpr_not_equals_supported_v<typename TestFixture::pair_type> ==
          (constexpr_equals_supported_v<typename TypeParam::first> &&
           constexpr_equals_supported_v<typename TypeParam::second>),
      "Must be comparable in a constant expression if types are"
  );
}

template <typename T, bool = constexpr_check(T{} < T{})>
constexpr auto constexpr_less_than_check(std::int32_t) -> std::true_type;

template <typename T>
constexpr auto constexpr_less_than_check(not_default_constructible) -> std::false_type;

template <typename T>
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
constexpr extern bool constexpr_less_than_supported_v =
    std::is_same_v<decltype(constexpr_less_than_check<T>(0)), std::true_type>;

template <typename T, bool = constexpr_check(T{} > T{})>
constexpr auto constexpr_greater_than_check(std::int32_t) -> std::true_type;

template <typename T>
constexpr auto constexpr_greater_than_check(not_default_constructible) -> std::false_type;

template <typename T>
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
constexpr extern bool constexpr_greater_than_supported_v =
    std::is_same_v<decltype(constexpr_greater_than_check<T>(0)), std::true_type>;

template <typename T, bool = constexpr_check(T{} <= T{})>
constexpr auto constexpr_less_than_or_equal_check(std::int32_t) -> std::true_type;

template <typename T>
constexpr auto constexpr_less_than_or_equal_check(not_default_constructible) -> std::false_type;

template <typename T>
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
constexpr extern bool constexpr_less_than_or_equal_supported_v =
    std::is_same_v<decltype(constexpr_less_than_or_equal_check<T>(0)), std::true_type>;

template <typename T, bool = constexpr_check(T{} >= T{})>
constexpr auto constexpr_greater_than_or_equal_check(std::int32_t) -> std::true_type;

template <typename T>
constexpr auto constexpr_greater_than_or_equal_check(not_default_constructible) -> std::false_type;

template <typename T>
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
constexpr extern bool constexpr_greater_than_or_equal_supported_v =
    std::is_same_v<decltype(constexpr_greater_than_or_equal_check<T>(0)), std::true_type>;

/// @test the ordering comparison operators are usable in a constant expression if the members are
CONDITIONAL_TYPED_TEST(
    StdPairTests,
    OrderingComparisonsUsableInConstexprContextIfTypesAre,
    (std::is_trivially_default_constructible_v<typename TypeParam::first> &&
     std::is_trivially_default_constructible_v<typename TypeParam::second> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::first> &&
     arene::base::is_less_than_comparable_v<typename TypeParam::second>)
) {
  static_assert(
      constexpr_less_than_supported_v<typename TestFixture::pair_type> ==
          (constexpr_less_than_supported_v<typename TypeParam::first> &&
           constexpr_less_than_supported_v<typename TypeParam::second>),
      "Must be comparable in a constant expression if types are"
  );
  static_assert(
      constexpr_greater_than_supported_v<typename TestFixture::pair_type> ==
          (constexpr_less_than_supported_v<typename TypeParam::first> &&
           constexpr_less_than_supported_v<typename TypeParam::second>),
      "Must be comparable in a constant expression if types are"
  );
  static_assert(
      constexpr_less_than_or_equal_supported_v<typename TestFixture::pair_type> ==
          (constexpr_less_than_supported_v<typename TypeParam::first> &&
           constexpr_less_than_supported_v<typename TypeParam::second>),
      "Must be comparable in a constant expression if types are"
  );
  static_assert(
      constexpr_greater_than_or_equal_supported_v<typename TestFixture::pair_type> ==
          (constexpr_less_than_supported_v<typename TypeParam::first> &&
           constexpr_less_than_supported_v<typename TypeParam::second>),
      "Must be comparable in a constant expression if types are"
  );
}

using arene::base::type_lists::concat_unique_t;
using arene::base::type_lists::remove_duplicates_t;

using make_pair_types = remove_duplicates_t<
    ::testing::extend_with_all_t<concat_unique_t<::testing::object_types, ::testing::compound_types>>>;

template <class T1, class T2>
using make_pair_return_type_t = decltype(std::make_pair(std::declval<T1>(), std::declval<T2>()));

template <typename TypeParam>
class StdMakePairTests : public testing::Test {};

TYPED_TEST_SUITE(StdMakePairTests, make_pair_types, );

/// @test The @c make_pair function decays both of the input arguments
TYPED_TEST(StdMakePairTests, MakePairDecaysInputValueTypes) {
  using pair_type = make_pair_return_type_t<TypeParam, TypeParam>;

  testing::StaticAssertTypeEq<typename pair_type::first_type, std::decay_t<TypeParam>>();
  testing::StaticAssertTypeEq<typename pair_type::second_type, std::decay_t<TypeParam>>();
}

/// @test The @c make_pair function extracts a reference type from a @c std::reference_wrapper
CONDITIONAL_TYPED_TEST(StdMakePairTests, MakePairExtractsFromReferenceWrapper, !std::is_reference_v<TypeParam>) {
  using reference_type = std::reference_wrapper<TypeParam>;
  using pair_type = make_pair_return_type_t<reference_type, reference_type>;

  testing::StaticAssertTypeEq<typename pair_type::first_type, typename reference_type::type&>();
  testing::StaticAssertTypeEq<typename pair_type::second_type, typename reference_type::type&>();
}

/// @test The @c make_pair function forwards the arguments it is given.
TEST(StdMakePairTests, MakePairForwardsTheArguments) {
  auto first_tracker = construction_tracker{};
  auto second_tracker = construction_tracker{};
  auto const copied = std::make_pair(first_tracker, second_tracker);

  ASSERT_EQ(copied.first.get_tracked_state(), construction_tracker::state::copy_constructed);
  ASSERT_EQ(copied.second.get_tracked_state(), construction_tracker::state::copy_constructed);

  auto const moved = std::make_pair(std::move(first_tracker), std::move(second_tracker));

  ASSERT_EQ(moved.first.get_tracked_state(), construction_tracker::state::move_constructed);
  ASSERT_EQ(moved.second.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test The @c make_pair function is usable in a constant expression if the arguments are
CONDITIONAL_TYPED_TEST(
    StdMakePairTests,
    MakePairUsableInConstantExpressionIfArgsAre,
    (std::is_trivially_copy_constructible_v<std::decay_t<TypeParam>> &&
     std::is_trivially_default_constructible_v<std::decay_t<TypeParam>> &&
     std::is_default_constructible_v<std::decay_t<TypeParam>>)
) {
  static_assert(
      constexpr_check(std::make_pair(std::decay_t<TypeParam>{}, std::decay_t<TypeParam>{})),
      "Must be callable in a constant expression"
  );
}

template <class T1, class T2>
constexpr auto is_nothrow_invocable_with() -> bool {
  return noexcept(std::make_pair(std::declval<T1>(), std::declval<T2>()));
}

/// @test The @c std::make_pair is noexcept if and only if a pair can be nothrow constructed from the given arguments.
CONSTEXPR_TEST(StdMakePairTests, MakePairNoexceptIfConstructionIs) {
  ASSERT_TRUE(noexcept(std::make_pair(std::declval<std::int32_t>(), std::declval<std::int32_t>())));
  ASSERT_FALSE(noexcept(
      std::make_pair(std::declval<throwable_copy_construction&>(), std::declval<throwable_copy_construction&>())
  ));
}

}  // namespace
