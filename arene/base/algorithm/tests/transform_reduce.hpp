// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TRANSFORM_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TRANSFORM_REDUCE_HPP_

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/generated_test_data.hpp"
#include "arene/base/algorithm/tests/transform_reduce_fwd.hpp"
#include "arene/base/algorithm/transform.hpp"
#include "arene/base/algorithm/transform_reduce.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
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

namespace transform_reduce_test {

/// @brief specifies the version of @c transform_reduce under test
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::transform_reduce_test::config::version<::transform_reduce_test::config::override>
///   prior to including this header
///
/// @return enum value of @c ::transform_reduce_test::config::flavor
///
template <class Override = transform_reduce_test::config::override>
constexpr auto version() noexcept -> transform_reduce_test::config::flavor {
  return ::transform_reduce_test::config::version<Override>;
}
/// @brief provides the @c transform_reduce algorithm as a function object
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::transform_reduce_test::config::function_under_test<::transform_reduce_test::config::override>
///   prior to including this header
///
/// @return reference to a function object
///
template <class Override = transform_reduce_test::config::override>
constexpr auto function_under_test() noexcept -> auto& {
  return ::transform_reduce_test::config::function_under_test<Override>;
}
/// @brief obtains the constexpr configuration of tests
/// @tparam Override parameter used to obtain the test configuration
///
/// Determine if tests should be invoked in a constant expression.
///
/// @return @c true if @c version() is not equal to @c std_inner_product,
///   otherwise @c false
///
template <class Override = transform_reduce_test::config::override>
constexpr auto run_test_in_constexpr() noexcept -> bool {
  return version() != transform_reduce_test::config::flavor::std_inner_product;
}

// invoke @c transform_reduce and check:
// * reduce is only invoked N times
// * transform is only invoked N times
// where N is range1.size()
//
// @{

template <class T>
struct counted_transform_reduce_result_type {
  T result{};
  std::size_t reduce_count{};
  std::size_t transform_count{};
};

template <class R, class I, class T, class Reduce, class Transform>
constexpr auto counted_transform_reduce(R&& range1, I first2, T value, Reduce reduce, Transform transform)
    -> counted_transform_reduce_result_type<T> {
  auto reduce_count = std::size_t{};
  auto transform_count = std::size_t{};

  auto out = counted_transform_reduce_result_type<T>{
      transform_reduce_test::function_under_test()(  //
          range1.begin(),
          range1.end(),
          std::move(first2),
          std::move(value),
          testing::counted<Reduce>{std::move(reduce), reduce_count},
          testing::counted<Transform>{std::move(transform), transform_count}
      ),
      0U,
      0U
  };

  out.reduce_count = reduce_count;
  out.transform_count = transform_count;

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

// returns sum of squares for 1...n
// https://math.stackexchange.com/questions/48080/sum-of-first-n-squares-equals-fracnn12n16
template <class T>
constexpr auto sum_of_squares(T n) -> T {
  return (n * (n + T{1}) * (T{2} * n + T{1})) / T{6};
}

// @}

}  // namespace transform_reduce_test

namespace {

template <class T>
struct TransformReduce : ::testing::Test {};

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

TYPED_TEST_SUITE(TransformReduce, test_types, );

/// @test @c transform_reduce on a range correctly applies the transform and
///   reduce operations the correct number of times.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(TransformReduce, InnerProduct, transform_reduce_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  constexpr auto values = arene::base::sequential_values_from<value_type, value_type{1}, size>;

  auto const out = transform_reduce_test::counted_transform_reduce(  //
      values,
      values.begin(),
      value_type{},
      std::plus<>{},
      std::multiplies<>{}
  );

  CONSTEXPR_ASSERT(transform_reduce_test::sum_of_squares(static_cast<value_type>(size)) == out.result);
  CONSTEXPR_ASSERT(size == out.reduce_count);
  CONSTEXPR_ASSERT(size == out.transform_count);
}

/// @test @c transform_reduce without specifying the reduce and transform
/// operations gives the same result as using @c std::plus<> and
/// @c std::multiplies
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    TransformReduce,
    InnerProductWithDefaultOps,
    transform_reduce_test::run_test_in_constexpr()
) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  constexpr auto values = arene::base::sequential_values_from<value_type, value_type{1}, size>;

  auto const result1 = transform_reduce_test::function_under_test()(  //
      values.cbegin(),
      values.cend(),
      values.cbegin(),
      value_type{},
      std::plus<>{},
      std::multiplies<>{}
  );

  auto const result2 = transform_reduce_test::function_under_test()(  //
      values.cbegin(),
      values.cend(),
      values.cbegin(),
      value_type{}
  );

  CONSTEXPR_ASSERT(result1 == result2);
}

/// @test @c transform_reduce without specifying the reduce and transform
/// operations gives the same result as using @c std::plus<> and
/// @c std::multiplies on generated data
///
TEST(TransformReduce, InnerProductWithDefaultOpsOnGeneratedData) {
  arene::base::for_each(  //
      algorithm_test::test_data,
      [](auto const& values) {
        auto const result1 = transform_reduce_test::function_under_test()(  //
            values.cbegin(),
            values.cend(),
            values.cbegin(),
            int{},
            std::plus<>{},
            std::multiplies<>{}
        );

        auto const result2 = transform_reduce_test::function_under_test()(  //
            values.cbegin(),
            values.cend(),
            values.cbegin(),
            int{}
        );

        CONSTEXPR_ASSERT(result1 == result2);
      }
  );
}

/// @test @c transform_reduce without specifying the reduce and transform
/// operations can be used if all the value types (range1 value type, range2
/// value type, and accumulator value type) are different.
///
/// @note There is no @c CONDITIONALLY_CONSTEXPR_TEST macro
TEST(TransformReduce, DefaultOpsAndDifferentValueTypes) {
  constexpr auto size = std::size_t{5};

  struct value_type2 {
    int value{};
  };

  struct value_type3 {
    int value{};

    // NOLINTNEXTLINE(readability-make-member-function-const)
    constexpr auto operator+(value_type3 const& rhs) -> value_type3 {  //
      return value_type3{value + rhs.value};
    }
    constexpr auto operator+(value_type3 const& rhs) const -> value_type3 {  //
      return value_type3{value + rhs.value};
    }
  };

  struct value_type1 {
    int value{};

    constexpr auto operator*(value_type2 rhs) const -> value_type3 {  //
      return value_type3{value * rhs.value};
    }
  };

  constexpr auto values1 = arene::base::array<value_type1, size>{
      value_type1{1},
      value_type1{2},
      value_type1{3},
      value_type1{4},
      value_type1{5}
  };
  constexpr auto values2 = arene::base::array<value_type2, size>{
      value_type2{1},
      value_type2{2},
      value_type2{3},
      value_type2{4},
      value_type2{5}
  };

  auto const result = transform_reduce_test::function_under_test()(  //
      values1.cbegin(),
      values1.cend(),
      values2.cbegin(),
      value_type3{}
  );

  ASSERT_EQ(  //
      transform_reduce_test::sum_of_squares(size),
      static_cast<std::size_t>(result.value)
  );
}

/// @test @c transform_reduce can be used with a Value type that differs from
/// the range value types and the return type of the reduce function
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    TransformReduce,
    DifferentValueType,
    transform_reduce_test::run_test_in_constexpr()
) {
  using value_type = std::uint32_t;
  constexpr auto size = TypeParam::size;

  auto const out = transform_reduce_test::counted_transform_reduce(  //
      arene::base::sequential_values_from<value_type, value_type{1}, size>,
      arene::base::sequential_values_from<std::uint16_t, std::uint16_t{1}, size>.begin(),
      std::uint64_t{},
      std::plus<>{},
      std::multiplies<>{}
  );

  CONSTEXPR_ASSERT(transform_reduce_test::sum_of_squares(static_cast<std::uint64_t>(size)) == out.result);
  CONSTEXPR_ASSERT(size == out.reduce_count);
  CONSTEXPR_ASSERT(size == out.transform_count);
}

/// @test @c transform_reduce can be used with two different range value types
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    TransformReduce,
    MaxOfFilteredValues,
    transform_reduce_test::run_test_in_constexpr()
) {
  using value_type = typename TypeParam::value_type;

  // we don't test with an empty range in the case, otherwise that results in
  // coverage misses
  constexpr auto size = TypeParam::size + 1U;

  constexpr auto values = arene::base::sequential_values_from<value_type, value_type{1}, size>;

  struct is_even {
    constexpr auto operator()(value_type const& value) const -> bool {
      return static_cast<std::size_t>(value) % 2U == 0U;
    }
  };

  auto evens = arene::base::array<bool, size>{};
  arene::base::transform(values.cbegin(), values.cend(), evens.begin(), is_even{});

  struct value_if {
    constexpr auto operator()(value_type const& value, bool forward) const -> value_type {
      return forward ? value : value_type{};
    }
  };

  auto const out = transform_reduce_test::counted_transform_reduce(  //
      values,
      evens.cbegin(),
      value_type{},
      transform_reduce_test::max{},
      value_if{}
  );

  auto const max_value = (size % 2U == 0U ? size : size - 1U);

  CONSTEXPR_ASSERT(max_value == out.result);
  CONSTEXPR_ASSERT(size == out.reduce_count);
  CONSTEXPR_ASSERT(size == out.transform_count);
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

/// @test @c transform_reduce can be used with a Value type that is only move
/// constructible
///
/// This test cannot be 'constexpr' as the value type is not move assignable -
/// which requires 'transform_reduce' to wrap the accumulated value in an
/// 'optional' and repeatedly move construct it with a new value instead of
/// using move assignment. This repeated construction is implemented using
/// 'optional::emplace' which is not 'constexpr'.
TYPED_TEST(TransformReduce, OnlyMoveConstructible) {
  constexpr auto values = arene::base::sequential_values_from<int, 1, 10>;

  // This is still compiled even if the condition is false, so we provide a
  // dummy when the function under test is not 'transform_reduce'.
  using only_move_constructible_type = arene::base::conditional_t<
      transform_reduce_test::version() == transform_reduce_test::config::flavor::transform_reduce,  //
      testing::configurable_value<
          int,
          testing::throws_on::nothing,
          testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_assign>,
      testing::configurable_value<int>>;

  // Invoke the function, even with the dummy type for 'inner_product' flavors,
  // so we don't get coverage misses.
  auto const out = transform_reduce_test::function_under_test()(  //
      values.cbegin(),
      values.cend(),
      values.cbegin(),
      only_move_constructible_type{},
      commutative_reduce_max<only_move_constructible_type>{},
      transform_reduce_test::max{}
  );

  ASSERT_EQ(values.back(), out.value);
}

/// @test all @c transform_reduce flavors will use move assignment when updating
/// the accumulator if the @c reduce result is an rvalue.
///
TYPED_TEST(TransformReduce, UsesMoveAssignment) {
  using counting_wrapper = ::testing::counting_wrapper<int>;

  auto const values = arene::base::array<counting_wrapper, 10>{};

  counting_wrapper::count = {};
  auto const out = transform_reduce_test::counted_transform_reduce(  //
      values,
      values.cbegin(),
      counting_wrapper{},
      transform_reduce_test::always_returns<counting_wrapper>{},
      transform_reduce_test::always_returns<counting_wrapper>{}
  );

  ASSERT_EQ(counting_wrapper::count.move_assign, 10);
  ASSERT_EQ(counting_wrapper::count.copy_assign, 0);
}

/// @test @c inner_product applies operations in order
/// @note This test is not 'CONSTEXPR_*' since it must also work for 'std::inner_product'
CONDITIONAL_TYPED_TEST(
    TransformReduce,
    AppliesOperationsInOrder,
    transform_reduce_test::version() != transform_reduce_test::config::flavor::transform_reduce
) {
  auto const values = arene::base::array<int, 10>{};

  struct visitor {
    int const* prev{};
    bool in_order{true};
  };

  struct forward_address_transform {
    static auto non_constexpr_function() -> void {}

    constexpr auto operator()(int const& lhs, int const& rhs) const noexcept -> int const* {
      if (&lhs != &rhs) {
        non_constexpr_function();
      }

      return &lhs;
    }
  };

  struct in_order_visitor_reduce {
    constexpr auto operator()(visitor const& lhs, int const* rhs) const -> visitor {
      return {rhs, lhs.in_order && (arene::base::next(lhs.prev) == rhs)};
    }
  };

  // verify that the visitor reduce function sets 'in_order' to 'false' if we
  // visit the same element twice or elements in reverse order
  ASSERT_TRUE(!(in_order_visitor_reduce{}(  //
                    visitor{values.data(), true},
                    values.data()
                ))
                   .in_order);
  ASSERT_TRUE(!(in_order_visitor_reduce{}(  //
                    visitor{&values[1], true},
                    values.data()
                ))
                   .in_order);

  auto const out = transform_reduce_test::function_under_test()(  //
      arene::base::next(values.cbegin()),
      values.cend(),
      arene::base::next(values.cbegin()),
      visitor{values.data(), true},
      in_order_visitor_reduce{},
      forward_address_transform{}
  );

  ASSERT_TRUE(out.in_order);
}

/// @test @c transform_reduce can be used with bidirectional iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    TransformReduce,
    WithBidirectionalIterators,
    transform_reduce_test::run_test_in_constexpr()
) {
  auto const values = arene::base::sequential_values_from<int, 1, 10>;
  auto const view = testing::demoted_view(std::bidirectional_iterator_tag{}, values.begin(), values.end());

  auto const out = transform_reduce_test::counted_transform_reduce(  //
      view,
      view.begin(),
      0,
      std::plus<>{},
      std::multiplies<>{}
  );

  CONSTEXPR_ASSERT(transform_reduce_test::sum_of_squares(static_cast<int>(10)) == out.result);
  CONSTEXPR_ASSERT(10 == out.reduce_count);
  CONSTEXPR_ASSERT(10 == out.transform_count);
}

/// @test @c transform_reduce can be used with forward iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    TransformReduce,
    WithForwardIterators,
    transform_reduce_test::run_test_in_constexpr()
) {
  auto const values = arene::base::sequential_values_from<int, 1, 10>;
  auto const view = testing::demoted_view(std::forward_iterator_tag{}, values.begin(), values.end());

  auto const out = transform_reduce_test::counted_transform_reduce(  //
      view,
      view.begin(),
      0,
      std::plus<>{},
      std::multiplies<>{}
  );

  CONSTEXPR_ASSERT(transform_reduce_test::sum_of_squares(static_cast<int>(10)) == out.result);
  CONSTEXPR_ASSERT(10 == out.reduce_count);
  CONSTEXPR_ASSERT(10 == out.transform_count);
}

/// @test @c transform_reduce can be used with input iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    TransformReduce,
    WithInputIterators,
    transform_reduce_test::run_test_in_constexpr()
) {
  auto const values = arene::base::sequential_values_from<int, 1, 10>;
  auto const view = testing::demoted_view(std::input_iterator_tag{}, values.begin(), values.end());

  auto const out = transform_reduce_test::counted_transform_reduce(  //
      view,
      view.begin(),
      0,
      std::plus<>{},
      std::multiplies<>{}
  );

  CONSTEXPR_ASSERT(transform_reduce_test::sum_of_squares(static_cast<int>(10)) == out.result);
  CONSTEXPR_ASSERT(10 == out.reduce_count);
  CONSTEXPR_ASSERT(10 == out.transform_count);
}

/// @test @c transform_reduce is @c noexcept(false) if iterator operations can throw
///
TEST(TransformReduce, NoexceptIfIteratorOpsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant1, auto specifier_constant2) {
    using iterator1 = testing::noexcept_configurable_input_iterator<specifier_constant1>;
    using iterator2 = testing::noexcept_configurable_input_iterator<specifier_constant2>;

    return noexcept(transform_reduce_test::function_under_test()(  //
        std::declval<iterator1>(),
        std::declval<iterator1>(),
        std::declval<iterator2>(),
        typename iterator1::value_type{},
        std::plus<>{},
        std::multiplies<>{}
    ));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal, throws_on::nothing));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference, throws_on::nothing));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::nothing, throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::nothing, throws_on::pre_increment));
}

/// @test @c transform_reduce is @c noexcept(false) if the reduce or transform op can throw
///
TEST(TransformReduce, NoexceptIfReduceOrTransformOpCanThrow) {
  auto const is_noexcept_with_reduce_and_transform = [](auto reduce, auto transform) {
    return noexcept(transform_reduce_test::function_under_test()(  //
        std::declval<int*>(),
        std::declval<int*>(),
        std::declval<int*>(),
        int{},
        std::declval<decltype(reduce)>(),
        std::declval<decltype(transform)>()
    ));
  };

  auto const throwing_binary_op = [](auto&&...) noexcept(false) -> int { return {}; };

  ASSERT_TRUE(is_noexcept_with_reduce_and_transform(std::plus<>{}, std::multiplies<>{}));

  ASSERT_FALSE(is_noexcept_with_reduce_and_transform(std::plus<>{}, throwing_binary_op));
  ASSERT_FALSE(is_noexcept_with_reduce_and_transform(throwing_binary_op, std::multiplies<>{}));
  ASSERT_FALSE(is_noexcept_with_reduce_and_transform(throwing_binary_op, throwing_binary_op));
}

// returns whether or not transform_reduce is noexcept when called with non-throwing operations, an init of type
// `decltype(value)`, and iterators that dereference to `decltype(value)`
constexpr struct {
  template <class T>
  constexpr auto operator()(T value) const noexcept -> bool {
    using iterator_value_type = decltype(value);
    using iterator_type = iterator_value_type*;
    using value_type = iterator_value_type;
    return noexcept(transform_reduce_test::function_under_test()(  //
        std::declval<iterator_type>(),
        std::declval<iterator_type>(),
        std::declval<iterator_type>(),
        std::declval<value_type>(),
        std::plus<>{},
        std::multiplies<>{}
    ));
  }
} is_noexcept_with_value{};

/// @test @c transform_reduce is @c noexcept(false) if the Value type can throw
/// on move construction
///
TEST(TransformReduce, NoexceptIfValueThrowsOnMoveConstruction) {
  ASSERT_TRUE(is_noexcept_with_value(int{}));
  ASSERT_TRUE(is_noexcept_with_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::move_construct>{}));
}

/// @test @c transform_reduce is @c noexcept(false) if the Value type is move
/// assignable and can throw on move assignment
///
TEST(TransformReduce, NoexceptIfValueThrowsOnMoveAssignment) {
  ASSERT_TRUE(is_noexcept_with_value(int{}));
  ASSERT_TRUE(is_noexcept_with_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::move_assign>{}));
}

/// @test @c transform_reduce is @c noexcept(false) if the @c operator+ or operator* can throw
///
TEST(TransformReduce, NoexceptIfOperatorPlusMultipliesThrows) {
  ASSERT_TRUE(is_noexcept_with_value(int{}));
  ASSERT_TRUE(is_noexcept_with_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::plus>{}));
  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::multiply>{}));
  ASSERT_FALSE(is_noexcept_with_value(testing::configurable_value<int, throws_on::plus | throws_on::multiply>{}));
}

/// @test @c transform_reduce is @c noexcept(false) if the result of @c reduce
/// can throw when converted to Value
///
TEST(TransformReduce, NoexceptIfValueThrowsOnConvertingConstruction) {
  auto const is_noexcept_with_int_ranges_and_value = [](auto value) {
    using iterator_type = int*;

    return noexcept(transform_reduce_test::function_under_test()(  //
        std::declval<iterator_type>(),
        std::declval<iterator_type>(),
        std::declval<iterator_type>(),
        std::declval<decltype(value)>(),
        transform_reduce_test::always_returns<std::integral_constant<int, 0>>{},
        std::multiplies<>{}
    ));
  };

  ASSERT_TRUE(is_noexcept_with_int_ranges_and_value(int{}));
  ASSERT_TRUE(is_noexcept_with_int_ranges_and_value(testing::configurable_value<int>{}));

  using testing::throws_on;

  ASSERT_FALSE(is_noexcept_with_int_ranges_and_value(testing::configurable_value<int, throws_on::value_construct>{}));
}

/// @test @c transform_reduce requires input iterators as arguments
///
TEST(TransformReduce, RequiresInputIterator) {
  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(transform_reduce_test::function_under_test()),
               int*,
               int*,
               int*,
               int,
               std::plus<>,
               std::multiplies<>>));

  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(transform_reduce_test::function_under_test()),
                int,
                int*,
                int*,
                int,
                std::plus<>,
                std::multiplies<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(transform_reduce_test::function_under_test()),
                int*,
                int,
                int*,
                int,
                std::plus<>,
                std::multiplies<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(transform_reduce_test::function_under_test()),
                int*,
                int*,
                int,
                int,
                std::plus<>,
                std::multiplies<>>));
}

/// @test @c transform_reduce requires @c transform to be invocable with the
/// reference type of each range
///
TEST(TransformReduce, RequiresTransformInvocableWithIterReferences) {
  struct dummy {};

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(transform_reduce_test::function_under_test()),
               int*,
               int*,
               dummy*,
               int,
               transform_reduce_test::always_returns<int>,
               transform_reduce_test::always_returns<dummy>>));

  auto const int_left_dummy_right = [](int, dummy) -> dummy { return {}; };
  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(transform_reduce_test::function_under_test()),
               int*,
               int*,
               dummy*,
               int,
               transform_reduce_test::always_returns<int>,
               decltype(int_left_dummy_right)>));

  auto const dummy_left_int_right = [](dummy, int) -> dummy { return {}; };
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(transform_reduce_test::function_under_test()),
                int*,
                int*,
                dummy*,
                int,
                transform_reduce_test::always_returns<int>,
                decltype(dummy_left_int_right)>));

  auto const takes_dummy_rvalue_reference = [](int, dummy&&) -> dummy { return {}; };
  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(transform_reduce_test::function_under_test()),
                int*,
                int*,
                dummy*,
                int,
                transform_reduce_test::always_returns<int>,
                decltype(takes_dummy_rvalue_reference)>));
}

/// @test @c transform_reduce requires the value type to be move constructible
/// and @c inner_product requires the value type to be copyable
///
TEST(TransformReduce, RequiresValueToBeCopyableOrMoveConstructible) {
  constexpr bool is_inner_product_flavor{
      transform_reduce_test::version() != transform_reduce_test::config::flavor::transform_reduce
  };

  auto const is_invocable_with = [](auto const& value) {
    using value_type = arene::base::remove_cvref_t<decltype(value)>;

    return arene::base::is_invocable_v<  //
        decltype(transform_reduce_test::function_under_test()),
        value_type*,
        value_type*,
        value_type*,
        value_type,
        transform_reduce_test::always_returns<std::integral_constant<int, 0>>,
        transform_reduce_test::always_returns<std::integral_constant<int, 0>>>;
  };

  using copyable_type = testing::configurable_value<int>;

  ASSERT_TRUE(is_invocable_with(copyable_type{}));

  using missing_copy_construct_type = testing::configurable_value<  //
      int,
      testing::throws_on::nothing,
      testing::disable::copy_construct>;

  ASSERT_TRUE(is_inner_product_flavor != is_invocable_with(missing_copy_construct_type{}));

  using missing_copy_assign_type = testing::configurable_value<  //
      int,
      testing::throws_on::nothing,
      testing::disable::copy_assign>;

  ASSERT_TRUE(is_inner_product_flavor != is_invocable_with(missing_copy_assign_type{}));

  using movable_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct |  //
       testing::disable::copy_assign)>;

  ASSERT_TRUE(is_inner_product_flavor != is_invocable_with(movable_type{}));

  using move_constructible_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct |  //
       testing::disable::copy_assign |     //
       testing::disable::move_assign)>;

  ASSERT_TRUE(is_inner_product_flavor != is_invocable_with(move_constructible_type{}));

  using immobile_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct |  //
       testing::disable::copy_assign |     //
       testing::disable::move_construct |  //
       testing::disable::move_assign)>;

  ASSERT_FALSE(is_invocable_with(immobile_type{}));
}

/// @test @c transform_reduce requires @c reduce to accept both @c init and
/// @c transform(...) as the argument type for *each* argument but @c inner_product does not
///
TEST(TransformReduce, RequiresReduceToBeTypeCommutativeInvocable) {
  constexpr bool is_inner_product_flavor{
      transform_reduce_test::version() != transform_reduce_test::config::flavor::transform_reduce
  };

  struct dummy {};

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(transform_reduce_test::function_under_test()),
               int*,
               int*,
               int*,
               dummy,
               transform_reduce_test::always_returns<dummy>,
               transform_reduce_test::always_returns<int>>));

  auto const dummy_left_int_right = [](dummy, int) -> dummy { return {}; };
  ASSERT_EQ(
      is_inner_product_flavor,
      (arene::base::is_invocable_v<
          decltype(transform_reduce_test::function_under_test()),
          int*,
          int*,
          int*,
          dummy,
          decltype(dummy_left_int_right),
          transform_reduce_test::always_returns<int>>)
  );
}

/// @test @c transform_reduce can be used with a Reduce type that returns a type
/// different than Value but convertible to Value
///
TEST(TransformReduce, ReduceResultConvertibleToValue) {
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
               decltype(arene::base::transform_reduce),
               only_movable_type*,
               only_movable_type*,
               only_movable_type*,
               only_movable_type,
               transform_reduce_test::always_returns<convertible_to_only_movable_type>,
               transform_reduce_test::always_returns<dummy>>));

  auto const reduce_returning_lvalue_ref = [](auto const&...) -> convertible_to_only_movable_type& {
    static auto value = convertible_to_only_movable_type{};
    return value;
  };

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::transform_reduce),
               only_movable_type*,
               only_movable_type*,
               only_movable_type*,
               only_movable_type,
               decltype(reduce_returning_lvalue_ref),
               transform_reduce_test::always_returns<dummy>>));

  auto const reduce_returning_rvalue_ref = [](auto const&...) -> convertible_to_only_movable_type&& {
    static auto value = convertible_to_only_movable_type{};
    return std::move(value);  // NOLINT(hicpp-move-const-arg)
  };

  ASSERT_TRUE((arene::base::is_invocable_v<
               decltype(arene::base::transform_reduce),
               only_movable_type*,
               only_movable_type*,
               only_movable_type*,
               only_movable_type,
               decltype(reduce_returning_rvalue_ref),
               transform_reduce_test::always_returns<dummy>>));
}

/// @test @c transform_reduce requires the @c reduce to be convertible to @c
/// Value but @c inner_product requires @c reduce to be assignable
///
TEST(TransformReduce, RequiresReduceResultToBeConvertible) {
  constexpr bool is_inner_product_flavor{
      transform_reduce_test::version() != transform_reduce_test::config::flavor::transform_reduce
  };

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
      is_inner_product_flavor,
      (arene::base::is_invocable_v<
          decltype(transform_reduce_test::function_under_test()),
          assignable_with_int*,
          assignable_with_int*,
          assignable_with_int*,
          assignable_with_int,
          transform_reduce_test::always_returns<int>,
          transform_reduce_test::always_returns<int>>)
  );
}

/// @test only @c arene::base::inner_product can use a @c Reduce that requires
/// @c acc to be an rvalue reference
///
TEST(TransformReduce, RvalueReferenceAccReduceUsable) {
  constexpr bool is_arene_base_inner_product_flavor{
      transform_reduce_test::version() == transform_reduce_test::config::flavor::inner_product
  };

  struct dummy {};

  // This reduce function is only usable with the 'arene::base::inner_product'
  // flavor as it use 'std::move' the accumulated value.
  //
  // 'std::inner_product' does not use 'std::move', instead passing the
  // accumulated value as an lvalue reference and not an rvalue reference.
  //
  // The constraints on @c arene::base::transform_reduce prevent this from being used:
  //
  // * <c> reduce(init, transform(*first1, *first2)) </c> is convertible to @c Value
  //
  // which is encoded as:
  //  std::enable_if_t<is_invocable_r_v<
  //    Value,
  //    Reduce&,
  //    Value&,
  //    iter_result_t<Transform&, InputIt1, InputIt2>>>
  //
  // that is -- invocability is checked with the accumulator passed as an lvalue
  // reference.
  auto const reduce_requires_rvalue_references = [](auto&& lhs, auto&& rhs)
      -> std::enable_if_t<
          std::is_rvalue_reference<decltype(lhs)>::value && std::is_rvalue_reference<decltype(rhs)>::value,
          dummy> { return {}; };

  ASSERT_EQ(
      is_arene_base_inner_product_flavor,
      (arene::base::is_invocable_v<
          decltype(transform_reduce_test::function_under_test()),
          dummy*,
          dummy*,
          dummy*,
          dummy,
          decltype(reduce_requires_rvalue_references),
          transform_reduce_test::always_returns<dummy>>)
  );
}

/// @test @c transform_reduce requires range value types to be multipliable
/// if the transform is not specified
///
TEST(TransformReduce, RangeValueTypesMultipliable) {
  struct type1 {};
  struct type2 {
    auto operator*(type1) const -> int { return {}; }
  };

  ASSERT_FALSE((arene::base::is_invocable_v<  //
                decltype(transform_reduce_test::function_under_test()),
                type1*,
                type1*,
                type2*,
                int>));

  ASSERT_TRUE((arene::base::is_invocable_v<  //
               decltype(transform_reduce_test::function_under_test()),
               type2*,
               type2*,
               type1*,
               int>));
}

/// @test @c transform_reduce requires the accumulator value type to be addable
/// to the multiplication result
///
TEST(TransformReduce, AccumulatorValueTypeAddable) {
  constexpr bool is_inner_product_flavor{
      transform_reduce_test::version() != transform_reduce_test::config::flavor::transform_reduce
  };

  struct not_addable_with_int {};

  ASSERT_FALSE((arene::base::is_invocable_v<  //
                decltype(transform_reduce_test::function_under_test()),
                int*,
                int*,
                int*,
                not_addable_with_int>));

  struct addable_with_int {
    auto operator+(int) const -> addable_with_int { return {}; }
  };

  ASSERT_EQ(
      is_inner_product_flavor,
      (arene::base::is_invocable_v<  //
          decltype(transform_reduce_test::function_under_test()),
          int*,
          int*,
          int*,
          addable_with_int>)
  );

  struct commutably_addable_with_int {
    commutably_addable_with_int(int) {}  // NOLINT(hicpp-explicit-conversions)
    operator int() const { return 0; }   // NOLINT(hicpp-explicit-conversions)
  };

  ASSERT_TRUE((arene::base::is_invocable_v<  //
               decltype(transform_reduce_test::function_under_test()),
               int*,
               int*,
               int*,
               commutably_addable_with_int>));
}

}  // namespace
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TRANSFORM_REDUCE_HPP_
