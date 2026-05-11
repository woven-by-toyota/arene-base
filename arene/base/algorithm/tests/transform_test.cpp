// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/transform.hpp"

#include <array>
#include <cstdint>
#include <iterator>
#include <set>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple.hpp"
#include "arene/base/type_list.hpp"
#include "testlibs/utilities/iterator_types.hpp"
#include "testlibs/utilities/throws_on.hpp"

namespace {

using ::arene::base::for_each_index;
using ::arene::base::transform;
using ::arene::base::type_list;

namespace type_lists = ::arene::base::type_lists;

/// Helper for casting a type_list to testing::Types
template <typename Tl>
using as_testing_types = type_lists::apply_all_t<Tl, ::testing::Types>;

struct test_struct1 {
  std::int32_t x_val;
  float y_val;
};

struct test_struct2 {
  std::uint16_t s_val;
  double t_val;
};

/// @test `transform` an array of primitive types to the same type yields the expected results.
TEST(TransformTest, ArrayPrimitiveToSameType) {
  std::array<std::int32_t, 5UL> const array{4, 5, 6, 7, 8};
  auto const func = [](std::int32_t value) { return value * 3; };
  auto const transformed = transform(func, array);
  for_each_index(transformed, [&func, &array](std::size_t idx, auto value) { ASSERT_EQ(value, func(array.at(idx))); });
}

/// @test `transform` a tuple of primitive types to the same type yields the expected results.
TEST(TransformTest, TuplePrimitiveToSameType) {
  std::tuple<std::int32_t, double, std::uint8_t> const tuple{6, 7.963, 8U};
  auto const func = [](auto value) { return value * 3; };
  auto const transformed = transform(func, tuple);
  ASSERT_EQ(std::get<0UL>(transformed), func(std::get<0UL>(tuple)));
  ASSERT_DOUBLE_EQ(std::get<1UL>(transformed), func(std::get<1UL>(tuple)));
  ASSERT_EQ(std::get<2UL>(transformed), func(std::get<2UL>(tuple)));
}

/// @test `transform` a pair of primitive types to the same type yields the expected results.
TEST(TransformTest, PairPrimitiveToSameType) {
  std::pair<std::int32_t, float> const pair{5, 6.25};
  auto const func = [](auto value) { return value * 3; };
  auto const transformed = transform(func, pair);
  ASSERT_EQ(transformed.first, func(pair.first));
  ASSERT_FLOAT_EQ(transformed.second, func(pair.second));
}

/// @test `transform` an array of structs to the same type yields the expected results.
TEST(TransformTest, ArrayStructToSameType) {
  std::array<test_struct1, 5UL> const array{
      test_struct1{4, 3.4F},
      test_struct1{5, 3.5F},
      test_struct1{6, 3.6F},
      test_struct1{7, 3.7F},
      test_struct1{8, 3.8F}
  };
  auto const func = [](test_struct1 const& ts1) { return test_struct1{ts1.x_val * 3, ts1.y_val * 2.5F}; };
  auto const transformed = transform(func, array);
  for_each_index(transformed, [&func, &array](std::size_t idx, auto& ts1) {
    auto ts1x = func(array.at(idx));
    ASSERT_EQ(ts1.x_val, ts1x.x_val);
    ASSERT_FLOAT_EQ(ts1.y_val, ts1x.y_val);
  });
}

/// @test `transform` a tuple of structs to the same type yields the expected results.
TEST(TransformTest, TupleStructToSameType) {
  std::tuple<test_struct1, test_struct1> const tuple{test_struct1{5, 3.5F}, test_struct1{6, 3.6F}};
  auto const func = [](auto& ts1) { return test_struct1{ts1.x_val * 3, ts1.y_val * 2.5F}; };
  auto const transformed = transform(func, tuple);
  auto const ts1x1 = func(std::get<0UL>(tuple));
  auto const ts1x2 = func(std::get<1UL>(tuple));
  ASSERT_EQ(std::get<0UL>(transformed).x_val, ts1x1.x_val);
  ASSERT_FLOAT_EQ(std::get<0UL>(transformed).y_val, ts1x1.y_val);
  ASSERT_EQ(std::get<1UL>(transformed).x_val, ts1x2.x_val);
  ASSERT_FLOAT_EQ(std::get<1UL>(transformed).y_val, ts1x2.y_val);
}

/// @test `transform` a pair of structs to the same type yields the expected results.
TEST(TransformTest, PairStructToSameType) {
  std::pair<test_struct1, test_struct1> const pair{test_struct1{5, 3.5F}, test_struct1{6, 4.6F}};
  auto const func = [](auto& ts1) { return test_struct1{ts1.x_val * 7, ts1.y_val * 4.5F}; };
  auto const transformed = transform(func, pair);
  auto const ts1x1 = func(pair.first);
  auto const ts1x2 = func(pair.second);
  ASSERT_EQ(transformed.first.x_val, ts1x1.x_val);
  ASSERT_FLOAT_EQ(transformed.first.y_val, ts1x1.y_val);
  ASSERT_EQ(transformed.second.x_val, ts1x2.x_val);
  ASSERT_FLOAT_EQ(transformed.second.y_val, ts1x2.y_val);
}

/// @test `transform` an array of primitive types to a different type yields the expected results.
TEST(TransformTest, ArrayPrimitiveToDifferentType) {
  std::array<std::int32_t, 5UL> const array{4, 5, 6, 7, 8};
  auto const func = [](std::int32_t val) { return static_cast<float>(val) * 17.8F; };
  auto const transformed = transform(func, array);
  for_each_index(transformed, [&func, &array](std::size_t idx, auto value) {
    ASSERT_FLOAT_EQ(value, func(array.at(idx)));
  });
}

/// @test `transform` a tuple of primitive types to a different type yields the expected results.
TEST(TransformTest, TuplePrimitiveToDifferentType) {
  std::tuple<std::int32_t, double, std::uint8_t> const tuple{6, 7.963, 8U};
  auto const func = [](auto value) { return value * 93.211; };
  auto const transformed = transform(func, tuple);
  ASSERT_DOUBLE_EQ(std::get<0UL>(transformed), func(std::get<0UL>(tuple)));
  ASSERT_DOUBLE_EQ(std::get<1UL>(transformed), func(std::get<1UL>(tuple)));
  ASSERT_DOUBLE_EQ(std::get<2UL>(transformed), func(std::get<2UL>(tuple)));
}

/// @test `transform` a pair of primitive types to a different type yields the expected results.
TEST(TransformTest, PairPrimitiveToDifferentType) {
  std::pair<std::int32_t, std::uint64_t> const pair{5, 6251UL};
  auto const func = [](auto value) { return static_cast<double>(value) * 121.88; };
  auto const transformed = transform(func, pair);
  ASSERT_DOUBLE_EQ(transformed.first, func(pair.first));
  ASSERT_DOUBLE_EQ(transformed.second, func(pair.second));
}

/// @test `transform` an array of structs to a different type yields the expected results.
TEST(TransformTest, ArrayStructToDifferentType) {
  std::array<test_struct1, 5UL> array{
      test_struct1{4, 3.4F},
      test_struct1{5, 3.5F},
      test_struct1{6, 3.6F},
      test_struct1{7, 3.7F},
      test_struct1{8, 3.8F}
  };
  auto const func = [](test_struct1 const& ts1) {
    return test_struct2{static_cast<std::uint16_t>(ts1.x_val * 2), ts1.y_val * 275.99};
  };
  auto transformed = transform(func, array);
  for_each_index(transformed, [&func, &array](std::size_t idx, auto& ts2) {
    auto ts2x = func(array.at(idx));
    ASSERT_EQ(ts2.s_val, ts2x.s_val);
    ASSERT_DOUBLE_EQ(ts2.t_val, ts2x.t_val);
  });
}

/// @test `transform` a tuple of structs to a different type yields the expected results.
TEST(TransformTest, TupleStructToDifferentType) {
  std::tuple<test_struct1, test_struct1> const tuple{test_struct1{5, 3.5F}, test_struct1{6, 3.6F}};
  auto const func = [](auto& ts1) {
    return test_struct2{static_cast<std::uint16_t>(ts1.x_val * 4), ts1.y_val * 9191.77};
  };
  auto const transformed = transform(func, tuple);
  auto const ts2x1 = func(std::get<0UL>(tuple));
  auto const ts2x2 = func(std::get<1UL>(tuple));
  ASSERT_EQ(std::get<0UL>(transformed).s_val, ts2x1.s_val);
  ASSERT_DOUBLE_EQ(std::get<0UL>(transformed).t_val, ts2x1.t_val);
  ASSERT_EQ(std::get<1UL>(transformed).s_val, ts2x2.s_val);
  ASSERT_DOUBLE_EQ(std::get<1UL>(transformed).t_val, ts2x2.t_val);
}

/// @test `transform` a pair of structs to a different type yields the expected results.
TEST(TransformTest, PairStructToDifferentType) {
  std::pair<test_struct1, test_struct1> const pair{test_struct1{5, 3.5F}, test_struct1{6, 4.6F}};
  auto const func = [](auto& ts1) {
    return test_struct2{static_cast<std::uint16_t>(ts1.x_val * 3), ts1.y_val * 6651.332};
  };
  auto const transformed = transform(func, pair);
  auto const ts2x1 = func(pair.first);
  auto const ts2x2 = func(pair.second);
  ASSERT_EQ(transformed.first.s_val, ts2x1.s_val);
  ASSERT_DOUBLE_EQ(transformed.first.t_val, ts2x1.t_val);
  ASSERT_EQ(transformed.second.s_val, ts2x2.s_val);
  ASSERT_DOUBLE_EQ(transformed.second.t_val, ts2x2.t_val);
}

constexpr std::size_t container_size = 10U;
using constexpr_containers =
    type_list<arene::base::array<int, container_size>, arene::base::array<double, container_size>>;
using non_constexpr_containers = type_list<std::vector<int>, std::set<int>, std::vector<double>, std::vector<double>>;

using all_containers = type_lists::concat_t<constexpr_containers, non_constexpr_containers>;

template <typename T>
constexpr auto multiply_by_10(T const& value) -> double {
  return static_cast<double>(value * 10);
}

template <typename T>
constexpr auto multiply_by_10_and_add(T const& lhs_value, T const& rhs_value) -> double {
  return static_cast<double>(lhs_value * 10 + rhs_value);
}

template <typename T>
class TransformStdTest : public ::testing::Test {
 public:
  using input_container = T;
  using input_value_type = typename input_container::value_type;
  using output_container = arene::base::array<double, container_size>;
  using output_value_type = typename output_container::value_type;
};

TYPED_TEST_SUITE(TransformStdTest, as_testing_types<all_containers>, );

/// @test Verify that the unary transform operation is called exactly once for each element from start to end.
TYPED_TEST(TransformStdTest, InvokesTransformOpExactlyOnceForEachElementBetweenStartAndEnd) {
  typename TestFixture::input_container const input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  typename TestFixture::output_container output;
  std::vector<typename TestFixture::input_value_type> called_elements;
  std::ignore = transform(
      input.begin(),
      input.end(),
      output.begin(),
      [&called_elements](auto const& original_element) -> typename TestFixture::output_value_type {
        called_elements.push_back(original_element);
        return {};
      }
  );
  EXPECT_THAT(called_elements, ::testing::UnorderedElementsAreArray(input));
}

/// @test Verify that the binary transform operation is called exactly once for each element from lhs_start to
/// lhs_end and each element within rhs_start.
TYPED_TEST(TransformStdTest, InvokesTransformOpExactlyOnceForEachElementBetweenStartAndEndOrWithinrhsstart) {
  typename TestFixture::input_container const lhs_input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  typename TestFixture::input_container const rhs_input{11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  typename TestFixture::output_container output;
  std::vector<typename TestFixture::input_value_type> lhs_called_elements;
  std::vector<typename TestFixture::input_value_type> rhs_called_elements;
  std::ignore = transform(
      lhs_input.begin(),
      lhs_input.end(),
      rhs_input.begin(),
      output.begin(),
      [&lhs_called_elements, &rhs_called_elements](auto const& original_element, auto const& rhs_original_element) ->
      typename TestFixture::output_value_type {
        lhs_called_elements.push_back(original_element);
        rhs_called_elements.push_back(rhs_original_element);
        return {};
      }
  );
  EXPECT_THAT(lhs_called_elements, ::testing::UnorderedElementsAreArray(lhs_input));
  EXPECT_THAT(rhs_called_elements, ::testing::UnorderedElementsAreArray(rhs_input));
}

/// @test Verify that the output sequence contains the result of applying the unary transformer on each element in
/// order.
TYPED_TEST(TransformStdTest, TheOutputSequenceContainsTheResultOfApplyingTheUnaryTransformerOnEachElementInOrder) {
  typename TestFixture::input_container const input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  typename TestFixture::output_container output;

  std::ignore =
      transform(input.begin(), input.end(), output.begin(), multiply_by_10<typename TestFixture::output_value_type>);

  std::vector<typename TestFixture::output_value_type> expected_output;
  for (auto const& element : input) {
    expected_output.push_back(multiply_by_10(element));
  }
  EXPECT_THAT(output, ::testing::ElementsAreArray(expected_output));
}

/// @test Verify that the output sequence contains the result of applying the binary transformer on each element in
/// order.
TYPED_TEST(TransformStdTest, TheOutputSequenceContainsTheResultOfApplyingTheBinaryTransformerOnEachElementInOrder) {
  typename TestFixture::input_container const lhs_input{11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  typename TestFixture::input_container const rhs_input{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  typename TestFixture::output_container output;

  std::ignore =
      transform(lhs_input.begin(), lhs_input.end(), rhs_input.begin(), output.begin(), multiply_by_10_and_add<typename TestFixture::output_value_type>);

  std::vector<typename TestFixture::output_value_type> expected_output;
  auto rhs_itr = rhs_input.begin();
  for (auto const& element : lhs_input) {
    expected_output.push_back(multiply_by_10_and_add(element, *rhs_itr));
    ++rhs_itr;
  }
  EXPECT_THAT(output, ::testing::ElementsAreArray(expected_output));
}

/// @test Verify that the return iterator points to the position after the last unary transformed element.
TYPED_TEST(TransformStdTest, ReturnsIteratorToPositionAfterLastUnaryTransformedElement) {
  typename TestFixture::input_container const input{1, 2, 3};
  typename TestFixture::output_container output;

  auto const return_itr =
      transform(input.begin(), input.end(), output.begin(), multiply_by_10<typename TestFixture::output_value_type>);
  EXPECT_EQ(return_itr, output.begin() + std::distance(input.begin(), input.end()));
}

/// @test Verify that the return iterator points to the position after the last binary transformed element.
TYPED_TEST(TransformStdTest, ReturnsIteratorToPositionAfterLastBinaryTransformedElement) {
  typename TestFixture::input_container const lhs_input{1, 2, 3};
  typename TestFixture::input_container const rhs_input{4, 5, 6};
  typename TestFixture::output_container output;

  auto const return_itr =
      transform(lhs_input.begin(), lhs_input.end(), rhs_input.begin(), output.begin(), multiply_by_10_and_add<typename TestFixture::output_value_type>);
  EXPECT_EQ(return_itr, output.begin() + std::distance(lhs_input.begin(), lhs_input.end()));
}

/// Simple wrapper to execute unary transform in a constexpr context
template <typename OutputContainer, typename InputContainer>
constexpr auto do_unary_transform_constexpr(InputContainer const& input) -> bool {
  OutputContainer output{};
  transform(input.begin(), input.end(), output.begin(), multiply_by_10<typename InputContainer::value_type>);
  // constexpr "expect near" when the values are positive
  return (*output.begin() - multiply_by_10(*input.begin())) < 0.001;
}

/// Simple wrapper to execute binary transform in a constexpr context
template <typename OutputContainer, typename InputContainer>
constexpr auto do_binary_transform_constexpr(InputContainer const& lhs_input, InputContainer const& rhs_input) -> bool {
  OutputContainer output{};
  transform(lhs_input.begin(), lhs_input.end(), rhs_input.begin(), output.begin(), multiply_by_10_and_add<typename InputContainer::value_type>);
  // constexpr "expect near" when the values are positive
  return (*output.begin() - multiply_by_10_and_add(*lhs_input.begin(), *rhs_input.begin())) < 0.001;
}

template <typename T>
using TransformStdTestConstexpr = TransformStdTest<T>;

TYPED_TEST_SUITE(TransformStdTestConstexpr, as_testing_types<constexpr_containers>, );

/// @test Verify that unary `transform` can be invoked in a constexpr context.
TYPED_TEST(TransformStdTestConstexpr, UnaryCanBeInvokedInConstexprContext) {
  STATIC_ASSERT_TRUE(do_unary_transform_constexpr<typename TestFixture::output_container>(
      typename TestFixture::input_container{1, 2, 3, 4, 5}
  ));
}

/// @test Verify that binary `transform` can be invoked in a constexpr context.
TYPED_TEST(TransformStdTestConstexpr, BinaryCanBeInvokedInConstexprContext) {
  STATIC_ASSERT_TRUE(do_binary_transform_constexpr<typename TestFixture::output_container>(
      typename TestFixture::input_container{11, 12, 13, 14, 15},
      typename TestFixture::input_container{1, 2, 3, 4, 5}
  ));
}

/// @test @c unary transform is @c noexcept(false) if iterator operations can throw
///
TEST(TransformStdUnary, NoexceptIfIteratorOpsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant1, auto specifier_constant2) {
    using iterator1 = testing::noexcept_configurable_input_iterator<specifier_constant1>;
    using iterator2 = testing::noexcept_configurable_input_iterator<specifier_constant2>;

    return noexcept(arene::base::transform(  //
        std::declval<iterator1>(),
        std::declval<iterator1>(),
        std::declval<iterator2>(),
        std::negate<>{}
    ));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal, throws_on::nothing));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference, throws_on::nothing));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::post_increment, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::nothing, throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::nothing, throws_on::post_increment));
}

/// @test @c binary transform is @c noexcept(false) if iterator operations can throw
///
TEST(TransformStdBinary, NoexceptIfIteratorOpsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant1, auto specifier_constant2, auto specifier_constant3
                                       ) {
    using iterator1 = testing::noexcept_configurable_input_iterator<specifier_constant1>;
    using iterator2 = testing::noexcept_configurable_input_iterator<specifier_constant2>;
    using iterator3 = testing::noexcept_configurable_input_iterator<specifier_constant3>;

    return noexcept(arene::base::transform(  //
        std::declval<iterator1>(),
        std::declval<iterator1>(),
        std::declval<iterator2>(),
        std::declval<iterator3>(),
        std::plus<>{}
    ));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing, throws_on::nothing, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal, throws_on::nothing, throws_on::nothing));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference, throws_on::nothing, throws_on::nothing));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::post_increment, throws_on::nothing, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::nothing, throws_on::dereference, throws_on::nothing));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::nothing, throws_on::post_increment, throws_on::nothing));
}

/// @test @c unary transform is @c noexcept(false) if the transform op can throw
///
TEST(TransformStdUnary, NoexceptIfTransformOpCanThrow) {
  auto const is_noexcept_with_transform = [](auto unary_op) {
    return noexcept(arene::base::transform(  //
        std::declval<int*>(),
        std::declval<int*>(),
        std::declval<int*>(),
        std::declval<decltype(unary_op)>()
    ));
  };

  auto const nonthrowing_unary_op = [](auto&&) noexcept(true) -> int { return {}; };
  auto const throwing_unary_op = [](auto&&) noexcept(false) -> int { return {}; };

  ASSERT_TRUE(is_noexcept_with_transform(nonthrowing_unary_op));
  ASSERT_FALSE(is_noexcept_with_transform(throwing_unary_op));
}

/// @test @c binary transform is @c noexcept(false) if the transform op can throw
///
TEST(TransformStdBinary, NoexceptIfTransformOpCanThrow) {
  auto const is_noexcept_with_transform = [](auto binary_op) {
    return noexcept(arene::base::transform(  //
        std::declval<int*>(),
        std::declval<int*>(),
        std::declval<int*>(),
        std::declval<int*>(),
        std::declval<decltype(binary_op)>()
    ));
  };

  auto const nonthrowing_binary_op = [](auto&&, auto&&) noexcept(true) -> int { return {}; };
  auto const throwing_binary_op = [](auto&&, auto&&) noexcept(false) -> int { return {}; };

  ASSERT_TRUE(is_noexcept_with_transform(nonthrowing_binary_op));
  ASSERT_FALSE(is_noexcept_with_transform(throwing_binary_op));
}

/// @test @c unary transform requires iterators as arguments
///
TEST(TransformStdUnary, RequiresIterators) {
  ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::transform), int*, int*, int*, std::negate<>>));

  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::transform),
                int,
                int,  // sentinel type matches input iterator
                int*,
                std::negate<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::transform), int*, int*, int, std::negate<>>));
}

/// @test @c binary transform requires iterators as arguments
///
TEST(TransformStdBinary, RequiresIterators) {
  ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::transform), int*, int*, int*, int*, std::plus<>>));

  ASSERT_FALSE((arene::base::is_invocable_v<
                decltype(arene::base::transform),
                int,
                int,  // sentinel type matches input iterator
                int*,
                int*,
                std::plus<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::transform), int*, int*, int*, int, std::negate<>>));
}
struct custom_sentinel {
  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG(
      "-Wunneeded-internal-declaration",
      "Performing invokability tests require this function to exist even if it is never called."
  );
  friend auto operator!=(int*, custom_sentinel) -> bool { return true; }
  ARENE_IGNORE_END();
};

struct bad_custom_sentinel {
  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG(
      "-Wunused-function",
      "Performing invokability tests require this function to exist even if it is never called."
  );
  friend auto operator==(int*, bad_custom_sentinel) -> bool { return true; }
  ARENE_IGNORE_END();
};

/// @test @c unary transform requires a valid sentinel type
///
TEST(TransformStdUnary, RequiresValidSentinel) {
  ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::transform), int*, int*, int*, std::negate<>>));

  ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::transform), int*, std::nullptr_t, int*, std::negate<>>)
  );

  ASSERT_TRUE(
      (arene::base::is_invocable_v<decltype(arene::base::transform), int*, custom_sentinel, int*, std::negate<>>)
  );

  ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(arene::base::transform), int*, bad_custom_sentinel, int*, std::negate<>>)
  );
}

/// @test @c binary transform requires a valid sentinel type
///
TEST(TransformStdBinary, RequiresValidSentinel) {
  ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::transform), int*, int*, int*, int*, std::plus<>>));

  ASSERT_TRUE(
      (arene::base::is_invocable_v<decltype(arene::base::transform), int*, std::nullptr_t, int*, int*, std::plus<>>)
  );

  ASSERT_TRUE(
      (arene::base::is_invocable_v<decltype(arene::base::transform), int*, custom_sentinel, int*, int*, std::plus<>>)
  );

  ASSERT_FALSE(
      (arene::base::
           is_invocable_v<decltype(arene::base::transform), int*, bad_custom_sentinel, int*, int*, std::plus<>>)
  );
}
}  // namespace
