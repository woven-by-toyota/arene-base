// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <initializer_list>
#include <string>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/detail/tests/wrapped_iterator/helpers.hpp"
#include "arene/base/detail/tests/wrapped_iterator/test_template.hpp"
#include "arene/base/type_list.hpp"

namespace wrapped_iterator_tests {

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(WrappedIteratorMemberTypesTest);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(WrappedIteratorReturnTypeEquivalenceTest);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(WrappedIteratorOperationEquivalenceTest);

// NOLINTBEGIN(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic) We want to explicitly test the behavior for raw pointers to c-arrays

template <>
class test_container_storage<std::string> {
 public:
  std::string data_{"1234567890"};
};

/// A span of iterator types that require stdlib types not backported by stdlib_choice.
using stdlib_iterator_types = ::testing::Types<
    std::string::iterator,
    std::string::const_iterator,
    std::vector<int>::iterator,
    std::vector<int>::const_iterator,
    std::array<int, 10>::iterator,
    std::array<int, 10>::const_iterator>;

/// A span of iterator types including raw pointers and complex iterators, which does not include any stdlib types not
/// provided by stdlib_choice.
using nostdlib_iterator_types = ::testing::Types<
    int*,
    int const*,
    char*,
    char const*,
    std::initializer_list<int>::iterator,
    std::initializer_list<int>::const_iterator>;

using all_iterator_tests_types = ::arene::base::type_lists::concat_t<stdlib_iterator_types, nostdlib_iterator_types>;

template <typename T>
class WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest : public ::testing::Test {};

TYPED_TEST_SUITE(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, all_iterator_tests_types, );

/// @test The dereference operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, Dereference) {
  EXPECT_DEATH(std::ignore = *wrapped_iterator_with_default_passkey<TypeParam>{}, "Precondition");
}

/// @test The arrow operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, Arrow) {
  EXPECT_DEATH(std::ignore = wrapped_iterator_with_default_passkey<TypeParam>{}.operator->(), "Precondition");
}

/// @test The index operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, Index) {
  EXPECT_DEATH(std::ignore = wrapped_iterator_with_default_passkey<TypeParam>{}[1], "Precondition");
}

/// @test The pre-increment operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, PreIncrement) {
  EXPECT_DEATH(std::ignore = ++wrapped_iterator_with_default_passkey<TypeParam>{}, "Precondition");
}

/// @test The post-increment operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, PostIncrement) {
  EXPECT_DEATH(std::ignore = (wrapped_iterator_with_default_passkey<TypeParam> {} ++), "Precondition");
}

/// @test The pre-decrement operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, PreDecrement) {
  EXPECT_DEATH(std::ignore = --wrapped_iterator_with_default_passkey<TypeParam>{}, "Precondition");
}

/// @test The post-decrement operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, PostDecrement) {
  EXPECT_DEATH(std::ignore = (wrapped_iterator_with_default_passkey<TypeParam> {} --), "Precondition");
}

/// @test The addition operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, AdditionWithOffset) {
  EXPECT_DEATH(std::ignore = wrapped_iterator_with_default_passkey<TypeParam>{} + 1, "Precondition");
  EXPECT_DEATH(std::ignore = 1 + wrapped_iterator_with_default_passkey<TypeParam>{}, "Precondition");
}

/// @test The subtraction operator on a default-constructed `wrapped_iterator` terminates the program with a
/// precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, SubtractionWithOffset) {
  EXPECT_DEATH(std::ignore = wrapped_iterator_with_default_passkey<TypeParam>{} - 1, "Precondition");
}

/// @test The `+=` operator on a default-constructed `wrapped_iterator` terminates the program with a precondition
/// violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, IncrementAssign) {
  EXPECT_DEATH(std::ignore = wrapped_iterator_with_default_passkey<TypeParam>{} += 1, "Precondition");
}

/// @test The `-=` operator on a default-constructed `wrapped_iterator` terminates the program with a precondition
/// violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperatorPreconditionOnDefaultConstructedDeathTest, DecrementAssign) {
  EXPECT_DEATH(std::ignore = wrapped_iterator_with_default_passkey<TypeParam>{} -= 1, "Precondition");
}

using nostdlib_container_types = ::testing::Types<int[10], int const[10], char[10], char const[10]>;
using stdlib_container_types = ::testing::Types<std::string, std::vector<int>, std::array<int, 10>>;
using all_container_types = ::arene::base::type_lists::concat_t<stdlib_container_types, nostdlib_container_types>;

template <typename T>
using WrappedIteratorOperationEquivalenceDeathTest = WrappedIteratorOperationEquivalenceTest<T>;

TYPED_TEST_SUITE(WrappedIteratorOperationEquivalenceDeathTest, all_container_types, );

/// @test Subtracting two `wrapped_iterator` valus where either is a default-constructed value terminates the program
/// with a precondition violation
/// @tparam TypeParam The underlying iterator type
TYPED_TEST(WrappedIteratorOperationEquivalenceDeathTest, SubtractingAValidAndDefaultCtordIteratorIsPrecondition) {
  auto const valid_itr = typename TestFixture::wrapped_iterator{default_passkey{}, this->container.begin()};
  auto const default_itr = typename TestFixture::wrapped_iterator{};
  EXPECT_DEATH(std::ignore = valid_itr - default_itr, "Precondition");
  EXPECT_DEATH(std::ignore = default_itr - valid_itr, "Precondition");
}

// NOLINTEND(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic)

}  // namespace wrapped_iterator_tests
