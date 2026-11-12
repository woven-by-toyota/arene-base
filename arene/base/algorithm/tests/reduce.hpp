// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_REDUCE_HPP_

#include <gtest/gtest.h>

#include "arene/base/algorithm/reduce.hpp"
#include "arene/base/algorithm/tests/generated_test_data.hpp"
#include "arene/base/algorithm/tests/reduce_fwd.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/for_each.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace reduce_test {

/// @brief specifies the version of @c reduce under test
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::reduce_test::config::version<::reduce_test::config::override>
///   prior to including this header
///
/// @return enum value of @c ::reduce_test::config::flavor
///
template <class Override = reduce_test::config::override>
constexpr auto version() noexcept -> reduce_test::config::flavor {
  return ::reduce_test::config::version<Override>;
}
/// @brief provides the @c reduce algorithm as a function object
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::reduce_test::config::function_under_test<::reduce_test::config::override>
///   prior to including this header
///
/// @return reference to a function object
///
template <class Override = reduce_test::config::override>
constexpr auto function_under_test() noexcept -> auto& {
  return ::reduce_test::config::function_under_test<Override>;
}
/// @brief obtains the constexpr configuration of tests
/// @tparam Override parameter used to obtain the test configuration
///
/// Determine if tests should be invoked in a constant expression.
///
/// @return @c true if @c version() is not equal to @c std_accumulate,
///   otherwise @c false
///
template <class Override = reduce_test::config::override>
constexpr auto run_test_in_constexpr() noexcept -> bool {
  return version() != reduce_test::config::flavor::std_accumulate;
}

// invoke @c reduce and check:
// * reduce is only invoked N times
// where N is range.size()
//
// @{

template <class T>
struct counted_reduce_result_type {
  T result{};
  std::size_t reduce_count{};
};

template <class R, class T, class Reduce>
constexpr auto counted_reduce(R&& range, T value, Reduce reduce) -> counted_reduce_result_type<T> {
  auto reduce_count = std::size_t{};

  auto out = counted_reduce_result_type<T>{
      reduce_test::function_under_test()(  //
          range.begin(),
          range.end(),
          std::move(value),
          testing::counted<Reduce>{std::move(reduce), reduce_count}
      ),
      0U
  };

  out.reduce_count = reduce_count;

  return out;
}

// function object that returns @c std::max(lhs, rhs)
struct max {
  template <class T>
  constexpr auto operator()(T const& lhs, T const& rhs) const noexcept -> T const& {
    return std::max(lhs, rhs);
  }
};

// function object that always returns a default-constructed instance of the template parameter
template <class Constant>
struct always_returns {
  template <class... Ts>
  constexpr auto operator()(Ts const&...) const noexcept -> Constant {
    return {};
  }
};

// returns sum of 1...n
// https://en.wikipedia.org/wiki/Summation
template <class T>
constexpr auto sum_of_sequence(T n) -> T {
  return (n * (n + T{1})) / T{2};
}

// @}

}  // namespace reduce_test

namespace {

template <class T>
struct Reduce : ::testing::Test {};

template <class T, std::size_t N>
struct test_case {
  using value_type = T;
  static constexpr auto size = N;
};

using test_types = ::testing::Types<  //
    test_case<int, 0>,
    test_case<int, 1>,
    test_case<int, 2>,
    test_case<int, 3>,
    test_case<int, 5>,
    test_case<int, 10>>;

TYPED_TEST_SUITE(Reduce, test_types, );

/// @test @c reduce on a range correctly applies the
///   reduce operation the correct number of times.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Reduce, Sum, reduce_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  constexpr auto values = arene::base::sequential_values_from<value_type, value_type{1}, size>;

  auto const out = reduce_test::counted_reduce(  //
      values,
      value_type{},
      std::plus<>{}
  );

  CONSTEXPR_ASSERT(reduce_test::sum_of_sequence(static_cast<value_type>(size)) == out.result);
  CONSTEXPR_ASSERT(size == out.reduce_count);
}

/// @test @c reduce without specifying the reduce
/// operation gives the same result as using @c std::plus<>.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Reduce, SumWithDefaultOps, reduce_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  constexpr auto values = arene::base::sequential_values_from<value_type, value_type{1}, size>;

  auto const result1 = reduce_test::function_under_test()(  //
      values.cbegin(),
      values.cend(),
      value_type{},
      std::plus<>{}
  );

  auto const result2 = reduce_test::function_under_test()(  //
      values.cbegin(),
      values.cend(),
      value_type{}
  );

  CONSTEXPR_ASSERT(result1 == result2);
}

/// @test @c reduce without specifying the reduce
/// operation gives the same result as using @c std::plus<> and
/// on generated data
///
TEST(Reduce, SumWithDefaultOpsOnGeneratedData) {
  arene::base::for_each(  //
      algorithm_test::test_data,
      [](auto const& values) {
        auto const result1 = reduce_test::function_under_test()(  //
            values.cbegin(),
            values.cend(),
            int{},
            std::plus<>{}
        );

        auto const result2 = reduce_test::function_under_test()(  //
            values.cbegin(),
            values.cend(),
            int{}
        );

        CONSTEXPR_ASSERT(result1 == result2);
      }
  );
}

/// @test @c reduce can be used with a Value type that differs from
/// the range value types and the return type of the reduce function
///
TYPED_TEST(Reduce, DifferentValueType) {
  constexpr auto size = TypeParam::size;

  auto const out = reduce_test::counted_reduce(  //
      arene::base::sequential_values_from<std::uint16_t, std::uint16_t{1}, size>,
      std::uint64_t{},
      [](auto lhs, auto rhs) { return static_cast<std::uint32_t>(lhs + rhs); }
  );

  CONSTEXPR_ASSERT_EQ(reduce_test::sum_of_sequence(static_cast<std::uint64_t>(size)), out.result);
  CONSTEXPR_ASSERT_EQ(size, out.reduce_count);
}

// The reduce function must be type-wise commutative so we need to handle each
// arg being 'only_move_constructible_type' OR 'int'.
template <class R>
struct commutative_reduce_max {
  static auto value(int value) noexcept -> int { return value; }
  static auto value(R const& wrapper) noexcept -> int { return wrapper.value; }

  template <class T, class U>
  auto operator()(T const& lhs, U const& rhs) const noexcept -> R {
    return std::max(value(lhs), value(rhs));
  }
};

/// @test @c reduce can be used with a Value type that is only move
/// constructible
///
/// This test cannot be 'constexpr' as the value type is not move assignable -
/// which requires 'reduce' to wrap the accumulated value in an
/// 'optional' and repeatedly move construct it with a new value instead of
/// using move assignment. This repeated construction is implemented using
/// 'optional::emplace' which is not 'constexpr'.
TYPED_TEST(Reduce, OnlyMoveConstructible) {
  constexpr auto values = arene::base::sequential_values_from<int, 1, 10>;

  // This is still compiled even if the condition is false, so we provide a
  // dummy when the function under test is not 'reduce'.
  using only_move_constructible_type = arene::base::conditional_t<
      reduce_test::version() == reduce_test::config::flavor::reduce,  //
      testing::configurable_value<
          int,
          testing::throws_on::nothing,
          testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_assign>,
      testing::configurable_value<int>>;

  // Invoke the function, even with the dummy type for 'accumulate' flavors,
  // so we don't get coverage misses.
  auto const out = reduce_test::function_under_test()(  //
      values.cbegin(),
      values.cend(),
      only_move_constructible_type{},
      commutative_reduce_max<only_move_constructible_type>{}
  );

  ASSERT_EQ(values.back(), out.value);
}

/// @test all @c reduce flavors will use move assignment when updating
/// the accumulator if the @c reduce result is an rvalue.
///
TYPED_TEST(Reduce, UsesMoveAssignment) {
  using counting_wrapper = ::testing::counting_wrapper<int>;

  auto const values = arene::base::array<counting_wrapper, 10>{};

  counting_wrapper::count = {};
  auto const out = reduce_test::counted_reduce(  //
      values,
      counting_wrapper{},
      reduce_test::always_returns<counting_wrapper>{}
  );

  ASSERT_EQ(counting_wrapper::count.move_assign, 10);
  ASSERT_EQ(counting_wrapper::count.copy_assign, 0);
}

/// @test @c accumulate applies operations in order
CONDITIONAL_TYPED_TEST(
    Reduce,
    AppliesOperationsInOrder,
    reduce_test::version() != reduce_test::config::flavor::reduce
) {
  auto const values = arene::base::array<int, 10>{};

  struct visitor {
    int const* prev{};
    bool in_order{true};
  };

  struct in_order_visitor_reduce {
    auto operator()(visitor const& lhs, int const& rhs) const -> visitor {
      auto const* rhs_ptr = std::addressof(rhs);
      return {rhs_ptr, lhs.in_order && (arene::base::next(lhs.prev) == rhs_ptr)};
    }
  };

  // verify that the visitor reduce function sets 'in_order' to 'false' if we
  // visit the same element twice or elements in reverse order
  ASSERT_TRUE(!(in_order_visitor_reduce{}(  //
                    visitor{values.data(), true},
                    *values.data()
                ))
                   .in_order);
  ASSERT_TRUE(!(in_order_visitor_reduce{}(  //
                    visitor{&values[1], true},
                    *values.data()
                ))
                   .in_order);

  auto const out = reduce_test::function_under_test()(  //
      arene::base::next(values.cbegin()),
      values.cend(),
      visitor{values.data(), true},
      in_order_visitor_reduce{}
  );

  ASSERT_TRUE(out.in_order);
}

/// @test @c reduce can be used with bidirectional iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Reduce, WithBidirectionalIterators, reduce_test::run_test_in_constexpr()) {
  auto const values = arene::base::sequential_values_from<int, 1, 10>;
  auto const view = testing::demoted_view(std::bidirectional_iterator_tag{}, values.begin(), values.end());

  auto const out = reduce_test::counted_reduce(  //
      view,
      0,
      std::plus<>{}
  );

  CONSTEXPR_ASSERT(reduce_test::sum_of_sequence(static_cast<int>(10)) == out.result);
  CONSTEXPR_ASSERT(10 == out.reduce_count);
}

/// @test @c reduce can be used with forward iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Reduce, WithForwardIterators, reduce_test::run_test_in_constexpr()) {
  auto const values = arene::base::sequential_values_from<int, 1, 10>;
  auto const view = testing::demoted_view(std::forward_iterator_tag{}, values.begin(), values.end());

  auto const out = reduce_test::counted_reduce(  //
      view,
      0,
      std::plus<>{}
  );

  CONSTEXPR_ASSERT(reduce_test::sum_of_sequence(static_cast<int>(10)) == out.result);
  CONSTEXPR_ASSERT(10 == out.reduce_count);
}

/// @test @c reduce can be used with input iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Reduce, WithInputIterators, reduce_test::run_test_in_constexpr()) {
  auto const values = arene::base::sequential_values_from<int, 1, 10>;
  auto const view = testing::demoted_view(std::input_iterator_tag{}, values.begin(), values.end());

  auto const out = reduce_test::counted_reduce(  //
      view,
      0,
      std::plus<>{}
  );

  CONSTEXPR_ASSERT(reduce_test::sum_of_sequence(static_cast<int>(10)) == out.result);
  CONSTEXPR_ASSERT(10 == out.reduce_count);
}

/// @test @c reduce is @c noexcept(false) if iterator operations can throw
///
TEST(Reduce, NoexceptIfIteratorOpsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant) {
    using iterator = testing::noexcept_configurable_input_iterator<specifier_constant>;

    return noexcept(reduce_test::function_under_test()(  //
        std::declval<iterator>(),
        std::declval<iterator>(),
        typename iterator::value_type{},
        std::plus<>{}
    ));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment));
}

/// @test @c reduce is @c noexcept(false) if the reduce op can throw
///
TEST(Reduce, NoexceptIfReduceOpCanThrow) {
  auto const is_noexcept_with_reduce = [](auto reduce) {
    return noexcept(reduce_test::function_under_test()(  //
        std::declval<int*>(),
        std::declval<int*>(),
        int{},
        std::declval<decltype(reduce)>()
    ));
  };

  auto const throwing_binary_op = [](auto&&...) noexcept(false) -> int { return {}; };

  ASSERT_TRUE(is_noexcept_with_reduce(std::plus<>{}));
  ASSERT_FALSE(is_noexcept_with_reduce(throwing_binary_op));
}

// returns whether or not reduce is noexcept when called with non-throwing operations, an init of type
// `decltype(value)`, and iterators that dereference to `decltype(value)`
/// @cond
constexpr struct {
  template <class T>
  constexpr auto operator()(T value) const noexcept -> bool {
    using iterator_value_type = decltype(value);
    using iterator_type = iterator_value_type*;
    using value_type = iterator_value_type;
    return noexcept(reduce_test::function_under_test()(  //
        std::declval<iterator_type>(),
        std::declval<iterator_type>(),
        std::declval<value_type>(),
        std::plus<>{}
    ));
  }
} is_noexcept_with_value{};
/// @endcond

/// @test @c reduce is @c noexcept(false) if the Value type can throw
/// on move construction
///
TEST(Reduce, NoexceptIfValueThrowsOnMoveConstruction) {
  ASSERT_TRUE(is_noexcept_with_value(int{}));
  ASSERT_TRUE(is_noexcept_with_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::move_construct>{}));
}

/// @test @c reduce is @c noexcept(false) if the Value type is move
/// assignable and can throw on move assignment
///
TEST(Reduce, NoexceptIfValueThrowsOnMoveAssignment) {
  ASSERT_TRUE(is_noexcept_with_value(int{}));
  ASSERT_TRUE(is_noexcept_with_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::move_assign>{}));
}

/// @test @c reduce is @c noexcept(false) if @c operator+ can throw
///
TEST(Reduce, NoexceptIfOperatorPlusThrows) {
  ASSERT_TRUE(is_noexcept_with_value(int{}));
  ASSERT_TRUE(is_noexcept_with_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::plus>{}));
}

/// @test @c reduce is @c noexcept(false) if the result of @c reduce
/// can throw when converted to Value
///
TEST(Reduce, NoexceptIfValueThrowsOnConvertingConstruction) {
  auto const is_noexcept_with_int_ranges_and_value = [](auto value) {
    using iterator_type = int*;

    return noexcept(reduce_test::function_under_test()(  //
        std::declval<iterator_type>(),
        std::declval<iterator_type>(),
        std::declval<decltype(value)>(),
        reduce_test::always_returns<std::integral_constant<int, 0>>{}
    ));
  };

  ASSERT_TRUE(is_noexcept_with_int_ranges_and_value(int{}));
  ASSERT_TRUE(is_noexcept_with_int_ranges_and_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_int_ranges_and_value(testing::configurable_value<int, throws_on::value_construct>{}));
}

/// @test @c reduce requires input iterators as arguments
///
TEST(Reduce, RequiresInputIterator) {
  ASSERT_TRUE((arene::base::is_invocable_v<decltype(reduce_test::function_under_test()), int*, int*, int, std::plus<>>)
  );

  ASSERT_FALSE((arene::base::is_invocable_v<decltype(reduce_test::function_under_test()), int, int*, int, std::plus<>>)
  );
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(reduce_test::function_under_test()), int*, int, int, std::plus<>>)
  );
}

/// @test @c reduce requires the value type to be move constructible
/// and @c accumulate requires the value type to be copyable
///
TEST(Reduce, RequiresValueToBeCopyableOrMoveConstructible) {
  constexpr bool is_accumulate_flavor{reduce_test::version() != reduce_test::config::flavor::reduce};

  auto const is_invocable_with = [](auto const& value) {
    using value_type = arene::base::remove_cvref_t<decltype(value)>;

    return arene::base::is_invocable_v<  //
        decltype(reduce_test::function_under_test()),
        value_type*,
        value_type*,
        value_type,
        reduce_test::always_returns<std::integral_constant<int, 0>>>;
  };

  using copyable_type = testing::configurable_value<int>;

  ASSERT_TRUE(is_invocable_with(copyable_type{}));

  using missing_copy_construct_type = testing::configurable_value<  //
      int,
      testing::throws_on::nothing,
      testing::disable::copy_construct>;

  ASSERT_TRUE(is_accumulate_flavor != is_invocable_with(missing_copy_construct_type{}));

  using missing_copy_assign_type = testing::configurable_value<  //
      int,
      testing::throws_on::nothing,
      testing::disable::copy_assign>;

  ASSERT_TRUE(is_accumulate_flavor != is_invocable_with(missing_copy_assign_type{}));

  using movable_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct |  //
       testing::disable::copy_assign)>;

  ASSERT_TRUE(is_accumulate_flavor != is_invocable_with(movable_type{}));

  using move_constructible_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct |  //
       testing::disable::copy_assign |     //
       testing::disable::move_assign)>;

  ASSERT_TRUE(is_accumulate_flavor != is_invocable_with(move_constructible_type{}));

  using immobile_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct |  //
       testing::disable::copy_assign |     //
       testing::disable::move_construct |  //
       testing::disable::move_assign)>;

  ASSERT_FALSE(is_invocable_with(immobile_type{}));
}

/// @test @c reduce requires @c reduce to accept both @c init and
/// @c *first as the argument type for *each* argument but @c accumulate does not
///
TEST(Reduce, RequiresReduceToBeTypeCommutativeInvocable) {
  constexpr bool is_accumulate_flavor{reduce_test::version() != reduce_test::config::flavor::reduce};

  struct dummy {};

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(reduce_test::function_under_test()),
               int*,
               int*,
               dummy,
               reduce_test::always_returns<dummy>>));

  auto const dummy_left_int_right = [](dummy, int) -> dummy { return {}; };
  ASSERT_EQ(
      is_accumulate_flavor,
      (arene::base::is_invocable_v<
          decltype(reduce_test::function_under_test()),
          int*,
          int*,
          dummy,
          decltype(dummy_left_int_right)>)
  );
}

/// @test @c reduce can be used with a Reduce type that returns a type
/// different than Value but convertible to Value
///
TEST(Reduce, ReduceResultConvertibleToValue) {
  using only_movable_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      testing::disable::copy_construct | testing::disable::copy_assign>;

  // the result of the transform does not matter
  struct dummy {};

  struct convertible_to_only_movable_type {
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator only_movable_type() const { return {}; }
  };

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::reduce),
               only_movable_type*,
               only_movable_type*,
               only_movable_type,
               reduce_test::always_returns<convertible_to_only_movable_type>>));

  auto const reduce_returning_lvalue_ref = [](auto const&...) -> convertible_to_only_movable_type& {
    static auto value = convertible_to_only_movable_type{};
    return value;
  };

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::reduce),
               only_movable_type*,
               only_movable_type*,
               only_movable_type,
               decltype(reduce_returning_lvalue_ref)>));

  auto const reduce_returning_rvalue_ref = [](auto const&...) -> convertible_to_only_movable_type&& {
    static auto value = convertible_to_only_movable_type{};
    return std::move(value);  // NOLINT(hicpp-move-const-arg)
  };

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::reduce),
               only_movable_type*,
               only_movable_type*,
               only_movable_type,
               decltype(reduce_returning_rvalue_ref)>));
}

/// @test @c reduce requires the @c reduce to be convertible to @c
/// Value but @c accumulate requires @c reduce to be assignable
///
TEST(Reduce, RequiresReduceResultToBeConvertible) {
  constexpr bool is_accumulate_flavor{reduce_test::version() != reduce_test::config::flavor::reduce};

  struct assignable_with_int {
    ~assignable_with_int() = default;
    assignable_with_int() = default;
    assignable_with_int(assignable_with_int const&) = default;
    assignable_with_int(assignable_with_int&&) = default;
    auto operator=(assignable_with_int const&) -> assignable_with_int& = default;
    auto operator=(assignable_with_int&&) -> assignable_with_int& = default;

    auto operator=(int) -> assignable_with_int& { return *this; }
  };

  ASSERT_EQ(
      is_accumulate_flavor,
      (arene::base::is_invocable_v<
          decltype(reduce_test::function_under_test()),
          assignable_with_int*,
          assignable_with_int*,
          assignable_with_int,
          reduce_test::always_returns<int>>)
  );
}

/// @test only @c arene::base::accumulate can use a @c Reduce that requires
/// @c acc to be an rvalue reference
///
TEST(Reduce, RvalueReferenceAccReduceUsable) {
  constexpr bool is_arene_base_inner_product_flavor{reduce_test::version() == reduce_test::config::flavor::accumulate};

  struct dummy {};

  // This reduce function is only usable with the 'arene::base::accumulate'
  // flavor as it use 'std::move' the accumulated value.
  //
  // 'std::accumulate' does not use 'std::move', instead passing the
  // accumulated value as an lvalue reference and not an rvalue reference.
  //
  // The constraints on @c arene::base::reduce prevent this from being used:
  //
  // * <c> reduce(init, *first1) </c> is convertible to @c Value
  //
  // which is encoded as:
  //  std::enable_if_t<is_invocable_r_v<
  //    Value,
  //    Reduce&,
  //    Value&,
  //    iter_reference_t<InputIt>>>
  //
  // that is -- invocability is checked with the accumulator passed as an lvalue
  // reference.
  auto const reduce_requires_rvalue_references =
      [](auto&& lhs, ARENE_MAYBE_UNUSED auto&& rhs
      ) -> std::enable_if_t<std::is_rvalue_reference<decltype(lhs)>::value, dummy> { return {}; };

  ASSERT_EQ(
      is_arene_base_inner_product_flavor,
      (arene::base::is_invocable_v<
          decltype(reduce_test::function_under_test()),
          dummy*,
          dummy*,
          dummy,
          decltype(reduce_requires_rvalue_references)>)
  );
}

/// @test @c reduce and @c accumulate can be called with a range
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    Reduce,
    InvocableWithRange,
    reduce_test::version() != reduce_test::config::flavor::std_accumulate
) {
  auto const values = arene::base::sequential_values_from<int, 1U, TypeParam::size + 1U>;

  auto const out = reduce_test::counted_reduce(  //
      values,
      0,
      std::plus<>{}
  );

  auto reduce_count = std::size_t{};

  auto result = reduce_test::function_under_test()(  //
      values,
      0,
      testing::counted<std::plus<>>{std::plus<>{}, reduce_count}
  );

  CONSTEXPR_ASSERT_EQ(out.result, result);
  CONSTEXPR_ASSERT_EQ(out.reduce_count, reduce_count);
}

}  // namespace
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_REDUCE_HPP_
