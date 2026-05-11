// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_EXTERNAL_VECTOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_EXTERNAL_VECTOR_HPP_

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/byte.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/inline_container/external_vector.hpp"
#include "arene/base/inline_container/testing/customization.hpp"
#include "arene/base/iterator.hpp"
#include "arene/base/optional.hpp"
#include "arene/base/span.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/utility/swap.hpp"

namespace arene {
namespace base {
namespace testing {

//////////////////////////////////////////////////////////////////////
// Customization points for generating values of user-defined types //
//////////////////////////////////////////////////////////////////////

/// @brief Create a test external vector containing a range of test values
/// @tparam T The type to contain in the test vector
/// @param storage The storage space to use for the values stored in the vector
/// @param begin The first index of test value to put in the vector
/// @param end The after-end index of test value to put in the vector
/// @return An external_vector<T> containing [test_value(begin), test_value(end))
template <typename T>
constexpr auto test_external_vector(
    arene::base::span<arene::base::byte> storage,
    std::size_t begin,
    std::size_t end
) noexcept(std::is_nothrow_copy_constructible<T>::value) -> ::arene::base::external_vector<T> {
  ::arene::base::external_vector<T> vec(storage);
  for (std::size_t ii = begin; ii < end; ++ii) {
    vec.push_back(test_value<T>(ii));
  }
  return vec;
}

/////////////////////////
// Parameterized tests //
/////////////////////////

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

/// @brief Test fixture for all type-parameterized @c external_vector tests
/// @tparam T The type parameter currently being used for tests; filled in by Google Test as @c TypeParam
template <typename T>
class ExternalVectorTest : public ::testing::Test {
 protected:
  /// @brief The base capacity to use for most @c external_vector instantiations in the parameterized tests
  static constexpr std::size_t capacity = 25;  // TODO Allow users to specify (max?) capacity

  /// @brief The type of the storage array for this type with the specified capacity
  /// @tparam Capacity The capacity of the expected container
  template <std::size_t Capacity>
  using storage_type = arene::base::array<arene::base::byte, Capacity * sizeof(T)>;

  /// @brief The type of the storage array for this type with the default capacity
  using standard_storage_type = storage_type<capacity>;

  /// @brief An instance of the default storage type to use as backing for a default capacity vector
  // NOLINTNEXTLINE(readability-identifier-naming)
  alignas(T) standard_storage_type standard_storage{};

  /// @brief A standard vector with the correct type, used in most test cases
  using vector = external_vector<T>;

  /// @brief A helper template to get the type resulting from a @c try_construct call
  /// @tparam Vector An @c external_vector type which we will try to construct
  /// @tparam Args The arguments with which we will try to construct @c Vector
  template <typename Vector, typename... Args>
  using try_construct_result = decltype(Vector::try_construct(std::declval<Args>()...));

  /// @brief A type with a @c noexcept(false) conversion operator to @c T (doesn't actually throw, just declares it)
  struct throws_when_converted_to_t {
    /// @brief Implicit conversion operator to @c T which declares that it might throw, though it doesn't actually
    /// @return The 0th test value of @c T
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator T() const noexcept(false) { return ::arene::base::testing::test_value<T>(0); }
  };

  /// @brief A bidirectional iterator of @c T which declares that it could throw, used in some @c noexcept tests
  struct throwing_bidirectional_iterator_of_t {
    /// @brief The category of this iterator
    using iterator_category = std::bidirectional_iterator_tag;
    /// @brief The type used for differences in this iterator
    using difference_type = std::ptrdiff_t;
    /// @brief The type returned by dereferencing this iterator
    using reference = T&;
    /// @brief The value type pointed to by this iterator
    using value_type = T;
    /// @brief This iterator as a pointer (disabled)
    using pointer = void;

    /// @brief Dereference operator; never defined
    auto operator*() -> reference;

    /// @brief Pre-increment operator; never defined
    auto operator++() -> throwing_bidirectional_iterator_of_t&;
    /// @brief Post-increment operator; never defined
    auto operator++(int) -> throwing_bidirectional_iterator_of_t;
    /// @brief Pre-decrement operator; never defined
    auto operator--() -> throwing_bidirectional_iterator_of_t&;
    /// @brief Post-decrement operator; never defined
    auto operator--(int) -> throwing_bidirectional_iterator_of_t;
    /// @brief Equality operator; never defined
    auto operator==(throwing_bidirectional_iterator_of_t const&) -> bool;
    /// @brief Inequality operator; never defined
    auto operator!=(throwing_bidirectional_iterator_of_t const&) -> bool;
  };

  /// @brief An input iterator pointing to any type whose operators have adjustable @c noexcept specifications
  /// @tparam U The value type of the iterator
  /// @tparam OperatorsNoexcept The @c noexcept specifications of all operators are equal to this
  template <typename U, bool OperatorsNoexcept>
  struct input_iterator_with_configurable_noexcept {
    /// @brief The category of this iterator
    using iterator_category = std::input_iterator_tag;
    /// @brief The type used for differences in this iterator
    using difference_type = std::ptrdiff_t;
    /// @brief The type returned by dereferencing this iterator
    using reference = U&;
    /// @brief The value type pointed to by this iterator
    using value_type = U;
    /// @brief This iterator as a pointer (disabled)
    using pointer = void;

    /// @brief Dereference operator; never defined
    auto operator*() noexcept(OperatorsNoexcept) -> reference;

    /// @brief Pre-increment operator; never defined
    auto operator++() noexcept(OperatorsNoexcept) -> input_iterator_with_configurable_noexcept&;
    /// @brief Post-increment operator; never defined
    auto operator++(int) noexcept(OperatorsNoexcept) -> input_iterator_with_configurable_noexcept;
    /// @brief Equality operator; never defined
    auto operator==(input_iterator_with_configurable_noexcept const&) noexcept(OperatorsNoexcept) -> bool;
    /// @brief Inequality operator; never defined
    auto operator!=(input_iterator_with_configurable_noexcept const&) noexcept(OperatorsNoexcept) -> bool;
  };

  /// @brief An input iterator pointing to any type which declares that it could throw, used in some @c noexcept tests
  /// @tparam U The value type of the iterator
  template <typename U>
  using throwing_input_iterator = input_iterator_with_configurable_noexcept<U, false>;
  /// @brief An input iterator pointing to any type which declares that it can not throw, used in some @c noexcept tests
  /// @tparam U The value type of the iterator
  template <typename U>
  using non_throwing_input_iterator = input_iterator_with_configurable_noexcept<U, true>;

  /// @brief Construct a @c vector from the passed parameters; allows calling as this->construct, which can be shorter
  /// @tparam U The types of the arguments to the constructor
  /// @param params The parameters to the constructor
  template <typename... U>
  static constexpr auto construct(U&&... params) noexcept(noexcept(vector(std::forward<U>(params)...))) -> vector {
    return vector(std::forward<U>(params)...);
  }

  /// @brief Return the @c idx 'th test value of the current @c T , parameterized by test suite users
  /// @param idx The index of the test value to get
  /// @return The @c idx 'th test value of the current @c T
  static constexpr auto test_value(std::size_t idx) noexcept(std::is_nothrow_move_constructible<T>::value) -> T {
    return ::arene::base::testing::test_value<T>(idx);
  }

  /// @brief Return a @c vector containing the test values @c [begin,end) of @c T , parameterized by test suite users
  /// @param storage The storage space to use for the values stored in the vector
  /// @param begin The index of the first test value to put into the test vector
  /// @param end The index after that of the last test value to put into the test vector
  /// @return A test vector holding the test values determined by the indices
  static constexpr auto test_external_vector(
      arene::base::span<arene::base::byte> storage,
      std::size_t begin,
      std::size_t end
  ) noexcept(std::is_nothrow_move_constructible<T>::value) -> vector {
    return ::arene::base::testing::test_external_vector<T>(storage, begin, end);
  }

  /// @brief Return a @c vector containing the test values @c [0,size) of @c T , parameterized by test suite users
  /// @param storage The storage space to use for the values stored in the vector
  /// @param size The number of test values to put into the test vector
  /// @return A test vector holding the test values determined by the size
  static constexpr auto test_external_vector(arene::base::span<arene::base::byte> storage, std::size_t size) noexcept(
      std::is_nothrow_move_constructible<T>::value
  ) -> vector {
    return test_external_vector(storage, 0, size);
  }

  /// @brief Return a @c vector containing the test values @c [0,capacity) of @c T , parameterized by test suite users
  /// @param storage The storage space to use for the values stored in the vector
  /// @return A test vector holding the test values determined by the capacity
  static constexpr auto test_external_vector(arene::base::span<arene::base::byte> storage
  ) noexcept(std::is_nothrow_move_constructible<T>::value) -> vector {
    return test_external_vector(storage, capacity);
  }
};

// An empty definition of this static variable is required until C++17.
template <typename T>
constexpr std::size_t ExternalVectorTest<T>::capacity;

/// @brief The Death Tests use the same fixture as the "normal" tests. Inherits so that it will be a distinct type.
/// @tparam T The type parameter of a given test instantiation
template <typename T>
class ExternalVectorDeathTest : public ExternalVectorTest<T> {};

// Declare the three test suites for Google Test; test cases will all be added to one of these.
TYPED_TEST_SUITE_P(ExternalVectorTest);
TYPED_TEST_SUITE_P(ExternalVectorDeathTest);

/// @test A default-constructed `external_vector` is empty
TYPED_TEST_P(ExternalVectorTest, CanConstructAnEmptyVector) { ASSERT_TRUE(TestFixture::construct().empty()); }

/// @test The capacity of a default-constructed `external_vector` is equal to the supplied template parameter
TYPED_TEST_P(ExternalVectorTest, CapacityIsAsSpecified) {
  EXPECT_EQ(TestFixture::construct(TestFixture::standard_storage).capacity(), TestFixture::capacity);

  constexpr std::size_t capacity2 = 19;
  alignas(TypeParam) typename TestFixture::template storage_type<capacity2> storage2{};
  ASSERT_TRUE(external_vector<TypeParam>{storage2}.capacity() == capacity2);
}

/// @test The `capacity` member function of `external_vector` is declared `noexcept`
TYPED_TEST_P(ExternalVectorTest, CapacityIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().capacity()));
  STATIC_ASSERT_TRUE(noexcept(TestFixture::construct(TestFixture::standard_storage).capacity()));
}

/// @test The size of a default-constructed `external_vector` is zero
TYPED_TEST_P(ExternalVectorTest, InitialSizeIsZero) {
  ASSERT_EQ(TestFixture::construct().size(), 0);
  ASSERT_EQ(TestFixture::construct(TestFixture::standard_storage).size(), 0);
}

/// @test With no storage, @c data returns a null pointer
TYPED_TEST_P(ExternalVectorTest, NoStorageDataIsNull) { ASSERT_EQ(TestFixture::construct().data(), nullptr); }

/// @test The size of an `external_vector` initialized from a list of values is correct
TYPED_TEST_P(ExternalVectorTest, SizeIsCorrect) {
  ASSERT_EQ(
      (typename TestFixture::vector{
          TestFixture::standard_storage,
          {TestFixture::test_value(0), TestFixture::test_value(1)}
      }.size()),
      2
  );
}

/// @test The `size` member function of `external_vector` is declared `noexcept`
TYPED_TEST_P(ExternalVectorTest, SizeIsNoexcept) { STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().size())); }

/// @test Using `push_back` to add an item to a default-constructed `external_vector` increases the size
TYPED_TEST_P(ExternalVectorTest, CanPushBackAValue) {
  typename TestFixture::vector vec{TestFixture::standard_storage};
  vec.push_back(TestFixture::test_value(0));
  EXPECT_EQ(vec.size(), 1);
}

/// @test After using `push_back` to add an item to a default-constructed `external_vector`, that item  can be retrieved
/// via the `back` member function
TYPED_TEST_P(ExternalVectorTest, AfterPushBackCanGetLastElement) {
  typename TestFixture::vector vec{TestFixture::standard_storage};
  TypeParam const value = TestFixture::test_value(0);
  vec.push_back(value);
  EXPECT_EQ(vec.back(), value);
}

/// @test After using `push_back` to add an item to a default-constructed `external_vector`, the vector is no longer
/// empty
TYPED_TEST_P(ExternalVectorTest, AfterPushBackVectorIsNotEmpty) {
  typename TestFixture::vector vec{TestFixture::standard_storage};
  TypeParam const value = TestFixture::test_value(0);
  vec.push_back(value);
  ASSERT_FALSE(vec.empty());
}

/// @test After using `push_back` twice to add two items to a default-constructed `external_vector`, the size is
/// incremented to 2, and the second item can be retrieved via the `back` member function
TYPED_TEST_P(ExternalVectorTest, SecondPushBackChangesLastValue) {
  typename TestFixture::vector vec{TestFixture::standard_storage};
  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);
  EXPECT_EQ(vec.size(), 2);
  EXPECT_EQ(vec.back(), value2);
}

/// @test `push_back` is `noexcept` if and only if the value type can be constructed without throwing
TYPED_TEST_P(ExternalVectorTest, PushBackIsConditionallyNoexcept) {
  constexpr bool copy_noexcept = std::is_nothrow_copy_constructible<TypeParam>{};
  constexpr bool move_noexcept = std::is_nothrow_move_constructible<TypeParam>{};
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct(std::declval<arene::base::span<arene::base::byte>>())
                   .push_back(std::declval<TypeParam const&>())),
      copy_noexcept
  );
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct(std::declval<arene::base::span<arene::base::byte>>())
                   .push_back(std::declval<TypeParam&&>())),
      move_noexcept
  );
}

/// @test A default-constructed `external_vector` is empty
TYPED_TEST_P(ExternalVectorTest, EmptybyDefault) { ASSERT_TRUE(TestFixture::construct().empty()); }

/// @test A `external_vector` constructed with just storage is empty
TYPED_TEST_P(ExternalVectorTest, EmptyIfJustStorage) {
  ASSERT_TRUE(TestFixture::construct(TestFixture::standard_storage).empty());
}

/// @test The `empty` member function of `external_vector` is `noexcept`
TYPED_TEST_P(ExternalVectorTest, EmptyIsNoexcept) { STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().empty())); }

/// @test The `at` member function throws a `std::out_of_range` exception when invoked with an index of 0 on
/// a default-constructed `external_vector`
TYPED_TEST_P(ExternalVectorTest, RetrievingAValueWithAtThrowsOnEmpty) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  ASSERT_THROW(TestFixture::construct().at(0), std::out_of_range);
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The `at` member function returns the value stored when invoked with an index of 0 on  a
/// `external_vector` into which one element has been stored via `push_back`
TYPED_TEST_P(ExternalVectorTest, RetrievingAValueWithAtReturnsValueAfterPushBack) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  TypeParam const value = TestFixture::test_value(0);
  vec.push_back(value);
  EXPECT_EQ(vec.at(0), value);
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The return type of the `at` member function on a non-`const` `external_vector` is a non-`const` reference to
/// `value_type`
TYPED_TEST_P(ExternalVectorTest, AtReturnsReference) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  ::testing::StaticAssertTypeEq<decltype(std::declval<typename TestFixture::vector&>().at(0)), TypeParam&>();
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The return type of the `at` member function on a `const` `external_vector` is a  `const` reference to
/// `value_type`
TYPED_TEST_P(ExternalVectorTest, AtOnConstVectorReturnsConstReference) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  ::testing::StaticAssertTypeEq<decltype(std::declval<typename TestFixture::vector const&>().at(0)), TypeParam const&>(
  );
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The `at` member function returns the corresponding value stored when invoked with an index of 0 or
/// 1 on an `external_vector` into which two elements have been stored via `push_back`
TYPED_TEST_P(ExternalVectorTest, CanAccessMultipleValuesWithAt) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);

  EXPECT_EQ(vec.at(0), value1);
  EXPECT_EQ(vec.at(1), value2);
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The `at` member function throws when invoked with an index that is equal to or larger than the size of the
/// vector on an `external_vector` into which two elements have been stored via `push_back`
TYPED_TEST_P(ExternalVectorTest, AtIndexSizeOrLargerThrows) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);

  ASSERT_THROW(vec.at(vec.size()), std::out_of_range);
  ASSERT_THROW(vec.at(vec.size() * 2), std::out_of_range);
  ASSERT_THROW(vec.at(std::numeric_limits<std::size_t>::max()), std::out_of_range);
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The `at` member function returns the corresponding value stored when invoked via a `const` reference to a
/// vector with an index of 0 or 1 on an `external_vector` into which two elements have been stored via `push_back`.
TYPED_TEST_P(ExternalVectorTest, CanAccessMultipleValuesWithAtWithConstVec) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);

  auto const& const_vec = vec;
  EXPECT_EQ(const_vec.at(0), value1);
  EXPECT_EQ(const_vec.at(1), value2);
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The `at` member function throws when invoked via a `const` reference to a vector with an index that is equal
/// to or larger than the size of an `external_vector` into which two elements have been stored via `push_back`
TYPED_TEST_P(ExternalVectorTest, AtIndexSizeOrLargerThrowsWithConstVec) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);

  auto const& const_vec = vec;

  ASSERT_THROW(const_vec.at(vec.size()), std::out_of_range);
  ASSERT_THROW(const_vec.at(vec.size() * 2), std::out_of_range);
  ASSERT_THROW(const_vec.at(std::numeric_limits<std::size_t>::max()), std::out_of_range);
#else
  GTEST_SKIP() << "`at` tests are skipped when exceptions are disabled.";
#endif
}

/// @test The `max_size` member function of `external_vector` returns the capacity of the storage
TYPED_TEST_P(ExternalVectorTest, MaxSizeEqualsCapacity) {
  ASSERT_EQ(TestFixture::construct(TestFixture::standard_storage).max_size(), TestFixture::capacity);
  ASSERT_EQ(TestFixture::construct().max_size(), 0);
  ASSERT_TRUE(noexcept(TestFixture::construct().max_size()));

  constexpr std::size_t capacity2 = 19;
  alignas(TypeParam) typename TestFixture::template storage_type<capacity2> storage2{};
  ASSERT_EQ(typename TestFixture::vector{storage2}.max_size(), capacity2);
}

/// @test Invoking `push_back` on a vector that is at capacity terminates the program with a precondition violation.
TYPED_TEST_P(ExternalVectorDeathTest, PushBackBeyondMaxSizeIsPreconditionViolation) {
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  for (std::size_t i = 0; i < vec.max_size(); ++i) {
    vec.push_back(TestFixture::test_value(0));
  }
  ASSERT_DEATH(vec.push_back(TestFixture::test_value(0)), "Precondition violation");
}

/// @test The size of a default-constructed vector with a capacity of zero is zero, when obtained via the
/// `size` member function, and via comparing the iterators returned from `begin` and `end`
TYPED_TEST_P(ExternalVectorTest, ZeroSizeVectorIsEmpty) {
  external_vector<TypeParam> const zvec{};
  external_vector<TypeParam const> const czvec{};
  ASSERT_EQ(zvec.begin(), zvec.end());
  ASSERT_EQ(czvec.begin(), czvec.end());
  ASSERT_EQ(zvec.size(), 0);
  ASSERT_EQ(czvec.size(), 0);
}

/// @test Invoking `push_back` on a zero-capacity vector terminates the program with a precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, PushBackOnZeroSizeVectorIsPreconditionViolation) {
  external_vector<TypeParam> vec{};
  ASSERT_DEATH(vec.push_back(TestFixture::test_value(0)), "Precondition violation");
}

/// @test Given a default-constructed `external_vector`, after one element has been added via `push_back`, `back`
/// returns a reference to that element. After a second element has been added via `push_back`, `back` now returns a
/// reference to the second element.
TYPED_TEST_P(ExternalVectorTest, BackReturnsReference) {
  auto vec = TestFixture::construct(TestFixture::standard_storage);

  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  TypeParam& val1 = vec.back();

  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);
  TypeParam& val2 = vec.back();

  EXPECT_EQ(std::addressof(val1), std::addressof(vec[0]));
  EXPECT_EQ(std::addressof(val2), std::addressof(vec[1]));
}

/// @test Given a default-constructed `external_vector`, after one element has been added via `push_back`, `back` called
/// via a `const` reference to the vector returns a `const` reference to that element. After a second element has been
/// added via `push_back`, `back` called via a `const` reference to the vector now returns a `const` reference to the
/// second element.
TYPED_TEST_P(ExternalVectorTest, ConstBackReturnsConstReference) {
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  auto const& const_vec = vec;

  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  auto& val1 = const_vec.back();
  ::testing::StaticAssertTypeEq<decltype(val1), TypeParam const&>();

  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);
  auto& val2 = const_vec.back();
  ::testing::StaticAssertTypeEq<decltype(val2), TypeParam const&>();

  EXPECT_EQ(std::addressof(val1), &const_vec[0]);
  EXPECT_EQ(std::addressof(val2), &const_vec[1]);
}

/// @test Invoking `back` cannot throw
TYPED_TEST_P(ExternalVectorTest, BackIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<typename TestFixture::vector&>().back()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<typename TestFixture::vector const&>().back()));
}

/// @test An `external_vector` can be constructed from a `std::initializer_list` without throwing exceptions, and the
/// size and values stored in the vector are the same as the size and values of the initializer list.
TYPED_TEST_P(ExternalVectorTest, CanConstructFromInitializerList) {
  std::initializer_list<TypeParam> const values{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3)
  };

  typename TestFixture::vector const vec{TestFixture::standard_storage, values};
  STATIC_ASSERT_EQ(
      noexcept(typename TestFixture::vector{TestFixture::standard_storage, values}),
      std::is_nothrow_copy_constructible<TypeParam>{}
  );

  EXPECT_THAT(vec, ::testing::ElementsAreArray(values));
}

/// @test Given an `external_vector` initialized from an initializer list, the `front` member function can be used to
/// retrieve the first value, which must be equal to the first value of the vector as retrieved via `at(0)`
TYPED_TEST_P(ExternalVectorTest, CanGetFront) {
  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, 4);
  EXPECT_EQ(std::addressof(vec.front()), std::addressof(vec[0]));
  ::testing::StaticAssertTypeEq<decltype(vec.front()), TypeParam&>();
  STATIC_ASSERT_TRUE(noexcept(vec.front()));
  auto const& const_vec = vec;
  EXPECT_EQ(&const_vec.front(), &const_vec[0]);
  ::testing::StaticAssertTypeEq<decltype(const_vec.front()), TypeParam const&>();
  STATIC_ASSERT_TRUE(noexcept(const_vec.front()));
}

/// @test The `begin` and `end` member functions of an `external_vector` must return an `iterator`, and they must be
/// equal for a default-constructed vector
TYPED_TEST_P(ExternalVectorTest, CanGetIterator) {
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  ::testing::StaticAssertTypeEq<decltype(vec.begin()), typename TestFixture::vector::iterator>();
  ::testing::StaticAssertTypeEq<decltype(vec.end()), typename TestFixture::vector::iterator>();
  STATIC_ASSERT_TRUE(noexcept(vec.begin()));
  STATIC_ASSERT_TRUE(noexcept(vec.end()));
  EXPECT_EQ(vec.begin(), vec.end());
}

/// @test Given an `external_vector` constructed from an initializer list, the iterators returned from `begin` and `end`
/// can be passed as an iterator pair to `std::mismatch` to compare the values in the vector to those in the initializer
/// list, and the return value of the call to `std::mismatch` must be the end iterators of the vector and initializer
/// list, to indicate that the values are the same.
TYPED_TEST_P(ExternalVectorTest, CanIterateOverValues) {
  std::initializer_list<TypeParam> const values{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3)
  };
  typename TestFixture::vector vec{
      TestFixture::standard_storage,
      values
  };  // NOLINT(misc-const-correctness) Explicitly testing lvalues
  auto const result = std::mismatch(values.begin(), values.end(), vec.begin(), vec.end());
  EXPECT_EQ(result.first, values.end());
  EXPECT_EQ(result.second, vec.end());
}

/// @test Given a `const` `external_vector` constructed from an initializer list, the iterators returned from `begin`
/// and `end` can be passed as an iterator pair to `std::mismatch` to compare the values in the vector to those in the
/// initializer list, and the return value of the call to `std::mismatch` must be the end iterators of the vector and
/// initializer list, to indicate that the values are the same.
TYPED_TEST_P(ExternalVectorTest, CanIterateOverConstValues) {
  std::initializer_list<TypeParam> const values{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3)
  };
  typename TestFixture::vector const vec{TestFixture::standard_storage, values};
  auto const result = std::mismatch(values.begin(), values.end(), vec.begin(), vec.end());
  EXPECT_EQ(result.first, values.end());
  EXPECT_EQ(result.second, vec.end());
}

/// @test The `iterator` type of an `external_vector` must have the required type aliases for a random-access iterator.
TYPED_TEST_P(ExternalVectorTest, IteratorTypedefs) {
  using vec = typename TestFixture::vector;
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename vec::iterator>::value_type, TypeParam>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename vec::iterator>::reference, TypeParam&>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename vec::iterator>::pointer, TypeParam*>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename vec::iterator>::difference_type, std::ptrdiff_t>(
  );
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<typename vec::iterator>::iterator_category,
      std::random_access_iterator_tag>();

  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename vec::const_iterator>::value_type, TypeParam>();
  ::testing::
      StaticAssertTypeEq<typename std::iterator_traits<typename vec::const_iterator>::reference, TypeParam const&>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename vec::const_iterator>::pointer, TypeParam const*>(
  );
  ::testing::
      StaticAssertTypeEq<typename std::iterator_traits<typename vec::const_iterator>::difference_type, std::ptrdiff_t>(
      );
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<typename vec::const_iterator>::iterator_category,
      std::random_access_iterator_tag>();
}

/// @test `external_vector` must have the required type aliases for a sequence container
TYPED_TEST_P(ExternalVectorTest, Typedefs) {
  using vec = typename TestFixture::vector;
  ::testing::StaticAssertTypeEq<typename vec::value_type, TypeParam>();
  ::testing::StaticAssertTypeEq<typename vec::pointer, TypeParam*>();
  ::testing::StaticAssertTypeEq<typename vec::const_pointer, TypeParam const*>();
  ::testing::StaticAssertTypeEq<typename vec::reference, TypeParam&>();
  ::testing::StaticAssertTypeEq<typename vec::const_reference, TypeParam const&>();
  ::testing::StaticAssertTypeEq<typename vec::size_type, std::size_t>();
  ::testing::StaticAssertTypeEq<typename vec::difference_type, std::ptrdiff_t>();
  ::testing::
      StaticAssertTypeEq<typename vec::reverse_iterator, ::arene::base::reverse_iterator<typename vec::iterator>>();
  ::testing::StaticAssertTypeEq<
      typename vec::const_reverse_iterator,
      ::arene::base::reverse_iterator<typename vec::const_iterator>>();
}

template <typename Vec, typename... Args>
constexpr auto constexpr_iterate(Args&&... args) -> bool {
  std::initializer_list<typename Vec::value_type> const values{std::forward<Args>(args)...};
  Vec vec{std::forward<Args>(args)...};

  // This test is specifically testing a range-based for loop, so we do that even though the iteration gets weird
  auto val_it = values.begin();
  for (auto const& vec_val : vec) {
    if (val_it == values.end()) {
      return false;  // The ranges are unequal if this happens while there are still values in `vec`
    }
    if (vec_val != *val_it) {
      return false;
    }
    ++val_it;
  }

  return val_it == values.end();  // If this reached the end and all elements were equal, the ranges are equal
}

/// @test Given a non-empty `external_vector`, invoking `erase` with the iterator returned from `begin` removes the
/// first element, moving the remaining elements to one lower index and decreasing the size by one, and returns an
/// iterator to the new first element.
TYPED_TEST_P(ExternalVectorTest, CanEraseAtBeginning) {
  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam const value2 = TestFixture::test_value(1);
  TypeParam const value3 = TestFixture::test_value(2);
  TypeParam const value4 = TestFixture::test_value(3);

  typename TestFixture::vector vec{TestFixture::standard_storage, {value1, value2, value3, value4}};

  STATIC_ASSERT_TRUE(std::is_same<
                     decltype(vec.erase(std::declval<typename TestFixture::vector::const_iterator>())),
                     typename TestFixture::vector::iterator>::value);
  auto const pos = vec.erase(vec.begin());
  EXPECT_EQ(pos, vec.begin());
  EXPECT_EQ(vec.size(), 3);
  EXPECT_EQ(vec[0], value2);
  EXPECT_EQ(vec[1], value3);
  EXPECT_EQ(vec[2], value4);
}

/// @test Given a non-empty `external_vector`, invoking `erase` with an iterator referring to an element that is neither
/// the first or last element removes that element, moves the following elements to one lower index, reduces the size by
/// one, and returns an iterator to the element that replaced the erased element.
TYPED_TEST_P(ExternalVectorTest, CanEraseInMiddle) {
  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam const value2 = TestFixture::test_value(1);
  TypeParam const value3 = TestFixture::test_value(2);
  TypeParam const value4 = TestFixture::test_value(3);

  typename TestFixture::vector vec{TestFixture::standard_storage, {value1, value2, value3, value4}};

  STATIC_ASSERT_TRUE(std::is_same<
                     decltype(vec.erase(std::declval<typename TestFixture::vector::const_iterator>())),
                     typename TestFixture::vector::iterator>::value);
  auto pos = vec.erase(vec.begin() + 1);
  EXPECT_EQ(pos, vec.begin() + 1);
  EXPECT_EQ(vec.size(), 3);
  EXPECT_EQ(vec[0], value1);
  EXPECT_EQ(vec[1], value3);
  EXPECT_EQ(vec[2], value4);
}

/// @test Invoking `erase` with an iterator range referring to elements in the same `external_vector`, removes those
/// elements, moves the following elements to take their place, and returns an iterator with the same offset from the
/// beginning of the vector as the first erased element
TYPED_TEST_P(ExternalVectorTest, CanEraseRange) {
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  constexpr std::size_t count = 10;
  constexpr std::size_t erase_start = 2;
  constexpr std::size_t erase_end = 5;
  std::vector<TypeParam> expected;
  for (std::size_t idx = 0; idx < count; ++idx) {
    vec.push_back(TestFixture::test_value(idx));
    if (idx < erase_start || idx >= erase_end) {
      expected.push_back(TestFixture::test_value(idx));
    }
  }

  ::testing::StaticAssertTypeEq<
      decltype(vec.erase(
          std::declval<typename TestFixture::vector::const_iterator>(),
          std::declval<typename TestFixture::vector::const_iterator>()
      )),
      typename TestFixture::vector::iterator>();
  auto const pos = vec.erase(vec.begin() + erase_start, vec.begin() + erase_end);
  EXPECT_EQ(pos, vec.begin() + erase_start);
  EXPECT_THAT(vec, ::testing::ElementsAreArray(expected));
}

/// @test Constructing with just a size creates a vector with that many default-constructed elements
CONDITIONAL_TYPED_TEST_P(ExternalVectorTest, CanConstructWithSize, std::is_default_constructible<TypeParam>{}) {
  constexpr std::size_t count = 3;
  auto value_vec = TestFixture::construct(TestFixture::standard_storage, count);

  STATIC_ASSERT_EQ(
      noexcept(typename TestFixture::vector(TestFixture::standard_storage, count)),
      std::is_nothrow_default_constructible<TypeParam>{}
  );
  EXPECT_EQ(value_vec.size(), count);
  EXPECT_THAT(value_vec, ::testing::Each(::testing::Eq(TypeParam{})));
}

/// @test Constructing an `external_vector` with a count and a value creates the specified number of elements
/// copy-constructed from the supplied value, and sets the `size` of the vector to that count.
TYPED_TEST_P(ExternalVectorTest, CanConstructWithSizeAndValue) {
  constexpr std::size_t count = 14;
  TypeParam const value = TestFixture::test_value(0);
  auto const value_vec = TestFixture::construct(TestFixture::standard_storage, count, value);

  STATIC_ASSERT_EQ(
      noexcept(typename TestFixture::vector(TestFixture::standard_storage, count, value)),
      std::is_nothrow_copy_constructible<TypeParam>{}
  );
  EXPECT_EQ(value_vec.size(), count);
  EXPECT_THAT(value_vec, ::testing::Each(::testing::Eq(value)));
}

/// @test The `external_vector` copy constructor copies the stored elements and size from the original vector.
TYPED_TEST_P(ExternalVectorTest, CopyingVectorCopiesElements) {
  auto const vec = TestFixture::test_external_vector(TestFixture::standard_storage, 6);
  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  typename TestFixture::vector const copy(storage2, vec);

  EXPECT_THAT(copy, ::testing::ElementsAreArray(vec));
}

/// @test The `external_vector` move constructor moves the stored elements from the original vector
TYPED_TEST_P(ExternalVectorTest, MovingVectorMovesElements) {
  std::initializer_list<TypeParam> const values = {
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3),
      TestFixture::test_value(4)
  };
  typename TestFixture::vector vec(TestFixture::standard_storage, values);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  typename TestFixture::vector const moved(std::move(vec));

  EXPECT_THAT(moved, ::testing::ElementsAreArray(values));
}

/// @test The `external_vector` move constructor with new storage moves the stored elements from the original vector
TYPED_TEST_P(ExternalVectorTest, MovingVectorWithNewStorageMovesElements) {
  std::initializer_list<TypeParam> const values = {
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3),
      TestFixture::test_value(4)
  };
  typename TestFixture::vector vec(TestFixture::standard_storage, values);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  typename TestFixture::vector const moved(storage2, std::move(vec));

  EXPECT_THAT(moved, ::testing::ElementsAreArray(values));
}

/// @test The `external_vector` copy-assignment operator uses copy assignment for elements from the source where there
/// is already a corresponding element in the destination, and copy construction for elements that do not have a
/// corresponding element in the destination
TYPED_TEST_P(ExternalVectorTest, CopyAssignOverSmallerVectorCopiesElements) {
  constexpr std::size_t count = 14;
  constexpr std::size_t smaller_count = 10;

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, count);
  auto copy = TestFixture::test_external_vector(storage2, smaller_count);
  copy = vec;

  ASSERT_EQ(vec.size(), count);
  ASSERT_EQ(copy.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(vec[i], copy[i]);
    EXPECT_EQ(TestFixture::test_value(i), copy[i]);
  }
}

/// @test The `external_vector` copy-assignment operator destroys the excess elements in the destination when the source
/// vector has fewer elements than the destination vector
TYPED_TEST_P(ExternalVectorTest, CopyAssignOverLargerVectorDestroysExcess) {
  constexpr std::size_t count = 14;
  constexpr std::size_t larger_count = 18;

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, count);
  auto copy = TestFixture::test_external_vector(storage2, larger_count);
  copy = vec;

  ASSERT_EQ(vec.size(), count);
  ASSERT_EQ(copy.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(vec[i], copy[i]);
    EXPECT_EQ(TestFixture::test_value(i), copy[i]);
  }
}

/// @test Copy-assignment for `external_vector` is `noexcept` if and only if the stored element type is both
/// nothrow-copy-constructible and nothrow-copy-assignable
TYPED_TEST_P(ExternalVectorTest, CopyAssignmentIsConditionallyNoexcept) {
  using vec = ::arene::base::external_vector<TypeParam>;

  constexpr bool should_be_copyable = std::is_copy_constructible<TypeParam>{} && std::is_copy_assignable<TypeParam>{};
  STATIC_ASSERT_FALSE(std::is_copy_constructible<vec>{});
  STATIC_ASSERT_EQ(std::is_copy_assignable<vec>{}, should_be_copyable);

  constexpr bool should_be_noexcept =
      std::is_nothrow_copy_constructible<TypeParam>{} && std::is_nothrow_copy_assignable<TypeParam>{};
  STATIC_ASSERT_FALSE(std::is_nothrow_copy_constructible<vec>{});
  STATIC_ASSERT_EQ(std::is_nothrow_copy_assignable<vec>{}, should_be_noexcept);
}

/// @test The `external_vector` move-assignment operator uses move assignment for elements from the source where there
/// is already a corresponding element in the destination, and move construction for elements that do not have a
/// corresponding element in the destination
TYPED_TEST_P(ExternalVectorTest, MoveAssignOverSmallerVectorMovesElements) {
  constexpr std::size_t count = 14;
  constexpr std::size_t smaller_count = 10;

  auto first = TestFixture::construct(TestFixture::standard_storage);
  std::vector<TypeParam> expected;
  for (std::size_t idx = 0; idx < count; ++idx) {
    first.push_back(TestFixture::test_value(idx));
    expected.push_back(TestFixture::test_value(idx));
  }

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto second = TestFixture::test_external_vector(storage2, smaller_count);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  second = std::move(first);

  EXPECT_EQ(second.size(), count);
  EXPECT_THAT(second, ::testing::ElementsAreArray(expected));
}

/// @test The `external_vector` move-assignment operator destroys the excess elements in the destination when the source
/// vector has fewer elements than the destination vector
TYPED_TEST_P(ExternalVectorTest, MoveAssignOverLargerVectorDestroysExcess) {
  constexpr std::size_t count = 14;
  constexpr std::size_t larger_count = 18;

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto first = TestFixture::test_external_vector(TestFixture::standard_storage, count);
  auto second = TestFixture::test_external_vector(storage2, larger_count);
  second = std::move(first);

  ASSERT_EQ(second.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(TestFixture::test_value(i), second[i]);
  }
}

/// @test Move-assignment for `external_vector` is `noexcept` if and only if the value type is
/// nothrow-move-constructible and nothrow-move-assignable
TYPED_TEST_P(ExternalVectorTest, MoveAssignmentIsConditionallyNoexcept) {
  using vec = ::arene::base::external_vector<TypeParam>;

  constexpr bool should_be_movable = std::is_move_assignable<TypeParam>{} && std::is_move_constructible<TypeParam>{};
  STATIC_ASSERT_TRUE(std::is_move_constructible<vec>{});
  STATIC_ASSERT_EQ(std::is_move_assignable<vec>{}, should_be_movable);

  constexpr bool should_be_noexcept =
      std::is_nothrow_move_assignable<TypeParam>{} && std::is_nothrow_move_constructible<TypeParam>{};
  STATIC_ASSERT_TRUE(std::is_nothrow_move_constructible<vec>{});
  STATIC_ASSERT_EQ(std::is_nothrow_move_assignable<vec>{}, should_be_noexcept);
}

// A Matcher that checks if the elements in an argument (by index) match the result of calling a function with the index
// NOLINTNEXTLINE(modernize-use-trailing-return-type) The declaration happens in a macro and we can't control it
MATCHER_P(elements_match_function, function, "") {
  for (std::size_t ii = 0; ii < arg.size(); ++ii) {
    if (arg[ii] != function(ii)) {
      return false;
    }
  }

  return true;
}

/// @test Invoking `pop_back` on a non-empty `external_vector` destroys the last element and reduces the size by one.
TYPED_TEST_P(ExternalVectorTest, CanPopBack) {
  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, 5);

  vec.push_back(TestFixture::test_value(5));
  vec.pop_back();

  EXPECT_THAT(vec, elements_match_function(TestFixture::test_value));
}

/// @test `pop_back` is declared `noexcept`
TYPED_TEST_P(ExternalVectorTest, PopBackIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<typename TestFixture::vector>().pop_back()));
}

/// @test `erase` is `noexcept` if and only if the element type has `noexcept` move operations
TYPED_TEST_P(ExternalVectorTest, EraseIsNoexceptIfTypeHasNoexceptMove) {
  // NOLINTBEGIN(misc-const-correctness) can't see through static_assert usage
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  // NOLINTEND(misc-const-correctness)

  STATIC_ASSERT_TRUE(noexcept(vec.erase(vec.begin(), vec.end())));
  STATIC_ASSERT_TRUE(noexcept(vec.erase(vec.begin())));
}

/// @test Given an empty `external_vector`, and the iterator returned from `begin`, `insert` can add an element to the
/// vector, setting the size to one.
TYPED_TEST_P(ExternalVectorTest, CanInsertInEmptyVector) {
  TypeParam const value = TestFixture::test_value(0);
  auto vec1 = TestFixture::construct(TestFixture::standard_storage);
  auto const& const_vec1 = vec1;
  auto const itr = vec1.insert(const_vec1.begin(), value);
  EXPECT_EQ(itr, vec1.begin());
  EXPECT_EQ(vec1.size(), 1);
  EXPECT_EQ(vec1[0], value);
}

/// @test Given an `external_vector` with existing elements, and the iterator returned from `end`, `insert` can add an
/// element to the end of the vector, increasing the size by one
TYPED_TEST_P(ExternalVectorTest, CanInsertAtEndOfExistingVector) {
  TypeParam const value0 = TestFixture::test_value(0);
  TypeParam const value1 = TestFixture::test_value(1);
  typename TestFixture::vector vec1{TestFixture::standard_storage, {value0}};
  auto const& const_vec1 = vec1;
  auto const pos = vec1.insert(const_vec1.end(), value1);
  EXPECT_EQ(pos, vec1.begin() + 1);
  EXPECT_EQ(vec1.size(), 2);
  EXPECT_EQ(vec1[0], value0);
  EXPECT_EQ(vec1[1], value1);
}

/// @test Given an `external_vector` with existing elements, and an iterator to an existing element in the vector,
/// `insert` can add an element at the iterator position, increasing the vector size by one. The existing element at the
/// location referenced by the iterator, and the subsequent elements, are moved to make room for the new element.
TYPED_TEST_P(ExternalVectorTest, CanInsertInMiddleOfExistingVector) {
  TypeParam const value0 = TestFixture::test_value(0);
  TypeParam const value1 = TestFixture::test_value(1);
  TypeParam const value2 = TestFixture::test_value(2);
  TypeParam const value3 = TestFixture::test_value(3);
  TypeParam const value4 = TestFixture::test_value(4);
  typename TestFixture::vector vec1{TestFixture::standard_storage, {value0, value1, value2, value4}};
  auto const& const_vec1 = vec1;
  auto const pos = vec1.insert(const_vec1.begin() + 1, value3);
  EXPECT_EQ(pos, vec1.begin() + 1);
  EXPECT_EQ(vec1.size(), 5);
  EXPECT_EQ(vec1[0], value0);
  EXPECT_EQ(vec1[1], value3);
  EXPECT_EQ(vec1[2], value1);
  EXPECT_EQ(vec1[3], value2);
  EXPECT_EQ(vec1[4], value4);
}

/// @test Invoking `insert` on an `external_vector` with `size` equal to the capacity terminates the program with a
/// precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, InsertWhenAtCapacityIsPreconditionViolation) {
  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam const value2 = TestFixture::test_value(1);
  auto vec1 = TestFixture::construct(TestFixture::standard_storage, TestFixture::capacity, value1);
  EXPECT_EQ(vec1.size(), TestFixture::capacity);
  ASSERT_DEATH(vec1.insert(vec1.end(), value2), "Precondition violation");
  ASSERT_DEATH(vec1.insert(vec1.begin(), value2), "Precondition violation");
  EXPECT_EQ(vec1.size(), TestFixture::capacity);
  for (auto& value : vec1) {
    EXPECT_EQ(value, value1);
  }
}

/// @test Invoking the `external_vector` constructor with a count larger than the capacity terminates the program with a
/// precondition violation
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorDeathTest,
    ConstructingWithOutOfRangeSizeIsPreconditionViolation,
    std::is_default_constructible<TypeParam>{}
) {
  ASSERT_DEATH(
      TestFixture::construct(TestFixture::standard_storage, TestFixture::capacity + 1),
      "Precondition violation"
  );
}

/// @test Invoking the `external_vector` constructor with a count and value where the count is larger than the capacity
/// terminates the program with a precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, ConstructingWithOutOfRangeSizeAndSourceIsPreconditionViolation) {
  TypeParam const source = TestFixture::test_value(0);
  ASSERT_DEATH(
      TestFixture::construct(TestFixture::standard_storage, TestFixture::capacity + 1, source),
      "Precondition violation"
  );
}

/// @test Invoking the `external_vector` constructor with an initializer list with more elements than the capacity
/// terminates the program with a precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, ConstructingWithTooManyInitializersIsPreconditionViolation) {
  constexpr std::size_t small_capacity = 4;
  using vec = external_vector<TypeParam>;
  TypeParam const value = TestFixture::test_value(0);

  alignas(TypeParam) typename TestFixture::template storage_type<small_capacity> storage{};
  ASSERT_DEATH((vec{storage, {value, value, value, value, value}}), "Precondition violation");
}

template <typename Vec>
constexpr auto constexpr_resize(Vec vec, std::size_t size) -> Vec {
  vec.resize(size);
  return vec;
}

/// @test `resize` is `noexcept` if and only if the default constructor of the element type is `noexcept`
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorTest,
    ResizeIsConditionallyNoexcept,
    std::is_default_constructible<TypeParam>{}
) {
  STATIC_ASSERT_EQ(noexcept(TestFixture::construct().resize(0)), std::is_nothrow_default_constructible<TypeParam>{});
}

/// @test `resize` with a value to copy is `noexcept` if and only if the copy constructor of the element type is
/// `noexcept`
TYPED_TEST_P(ExternalVectorTest, ResizeWithValueIsConditionallyNoexcept) {
  STATIC_ASSERT_EQ(
      noexcept(std::declval<typename TestFixture::vector&>().resize(0, TestFixture::test_value(0))),
      std::is_nothrow_copy_constructible<TypeParam>{}
  );
}

/// @test Invoking the `resize` member function with a size and a value on a vector with more elements than the
/// specified size sets the size of the vector to the specified count and does not construct new elements
TYPED_TEST_P(ExternalVectorTest, ResizingFromLargeToSmallLeavesElements) {
  constexpr std::size_t initial_count = 5;
  TypeParam const initial_value = TestFixture::test_value(0);
  typename TestFixture::vector vec(TestFixture::standard_storage, initial_count, initial_value);
  constexpr std::size_t count = 3;
  TypeParam const value = TestFixture::test_value(1);
  vec.resize(count, value);
  ASSERT_EQ(vec.size(), count);

  for (std::size_t idx = 0; idx < count; ++idx) {
    EXPECT_EQ(vec[idx], initial_value);
  }
}

/// @test Invoking the `resize` member function with a size and a value on a vector with fewer elements than the
/// specified size sets the size of the vector to the specified count, and constructs a number of new elements
/// equal to the difference between the specified size and the previous size by copy-construction from the specified
/// value
TYPED_TEST_P(ExternalVectorTest, ResizingFromSmallToLargeConstructsElementsAsCopiesOfSpecified) {
  constexpr std::size_t initial_count = 5;
  TypeParam const initial_value = TestFixture::test_value(0);
  typename TestFixture::vector vec(TestFixture::standard_storage, initial_count, initial_value);
  constexpr std::size_t count = 17;
  TypeParam const value = TestFixture::test_value(1);
  vec.resize(count, value);
  ASSERT_EQ(vec.size(), count);

  for (std::size_t idx = 0; idx < initial_count; ++idx) {
    EXPECT_EQ(vec[idx], initial_value);
  }
  for (std::size_t idx = initial_count; idx < count; ++idx) {
    EXPECT_EQ(vec[idx], value);
  }
}

/// @test An `external_vector` can be constructed from an iterator range, creating a vector holding copies of the
/// elements from the source range, with a size equal to the number of elements in the source range
TYPED_TEST_P(ExternalVectorTest, CanConstructFromIteratorRange) {
  auto const source = TestFixture::test_external_vector(TestFixture::standard_storage, 4);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  typename TestFixture::vector vec(storage2, source.begin(), source.end());

  EXPECT_EQ(vec.size(), source.size());
  for (std::size_t i = 0; i < vec.size(); ++i) {
    EXPECT_EQ(vec[i], source[i]);
  }
}

/// @test Construction of an `external_vector` from an iterator range is `noexcept` if operations on the iterator are
/// `noexcept` and the element type of the vector is nothrow-constructible from the reference type of the iterator
TYPED_TEST_P(ExternalVectorTest, IteratorRangeConstructionIsConditionallyNoexcept) {
  using source_it = typename std::vector<TypeParam>::const_iterator;
  using source_val = typename source_it::reference;

  constexpr bool it_operations_noexcept = noexcept(++std::declval<source_it>())&& noexcept(*std::declval<source_it>());
  constexpr bool element_construction_noexcept =
      std::is_nothrow_constructible<typename TestFixture::vector::value_type, source_val>{};

  STATIC_ASSERT_TRUE((
      std::is_constructible<typename TestFixture::vector, arene::base::span<arene::base::byte>, source_it, source_it>{}
  ));
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<
          typename TestFixture::vector,
          arene::base::span<arene::base::byte>,
          source_it,
          source_it>{}),
      it_operations_noexcept && element_construction_noexcept
  );

  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using convertible_array = ::arene::base::array<throws_when_converted, TestFixture::capacity>;
  using throwing_bidirectional_iterator = typename TestFixture::throwing_bidirectional_iterator_of_t;

  STATIC_ASSERT_TRUE(std::is_constructible<
                     typename TestFixture::vector,
                     arene::base::span<arene::base::byte>,
                     typename convertible_array::const_iterator,
                     typename convertible_array::const_iterator>::value);
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<
                      typename TestFixture::vector,
                      arene::base::span<arene::base::byte>,
                      typename convertible_array::const_iterator,
                      typename convertible_array::const_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<
                     typename TestFixture::vector,
                     arene::base::span<arene::base::byte>,
                     throwing_bidirectional_iterator,
                     throwing_bidirectional_iterator>::value);
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<
                      typename TestFixture::vector,
                      arene::base::span<arene::base::byte>,
                      throwing_bidirectional_iterator,
                      throwing_bidirectional_iterator>::value);
}

/// @test Construction of an `external_vector` from an input iterator range is `noexcept` if operations on the iterator
/// are `noexcept` and the element type of the vector is nothrow-constructible from the reference type of the iterator
TYPED_TEST_P(ExternalVectorTest, InputIteratorRangeConstructionIsConditionallyNoexcept) {
  using stream_it = std::istream_iterator<TypeParam>;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<TypeParam>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type = typename TestFixture::template non_throwing_input_iterator<TypeParam>;

  constexpr bool it_operations_noexcept = noexcept(++std::declval<stream_it>())&& noexcept(*std::declval<stream_it>());
  constexpr bool element_construction_noexcept =
      std::is_nothrow_constructible<typename TestFixture::vector::value_type, typename stream_it::reference>{};

  STATIC_ASSERT_TRUE((
      std::is_constructible<typename TestFixture::vector, arene::base::span<arene::base::byte>, stream_it, stream_it>{}
  ));
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<
          typename TestFixture::vector,
          arene::base::span<arene::base::byte>,
          stream_it,
          stream_it>{}),
      it_operations_noexcept && element_construction_noexcept
  );

  STATIC_ASSERT_TRUE(std::is_constructible<
                     typename TestFixture::vector,
                     arene::base::span<arene::base::byte>,
                     throwing_it_throwing_type,
                     throwing_it_throwing_type>::value);
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<
                      typename TestFixture::vector,
                      arene::base::span<arene::base::byte>,
                      throwing_it_throwing_type,
                      throwing_it_throwing_type>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<
                     typename TestFixture::vector,
                     arene::base::span<arene::base::byte>,
                     throwing_it_raw_type,
                     throwing_it_raw_type>::value);
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<
                      typename TestFixture::vector,
                      arene::base::span<arene::base::byte>,
                      throwing_it_raw_type,
                      throwing_it_raw_type>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<
                     typename TestFixture::vector,
                     arene::base::span<arene::base::byte>,
                     nothrow_it_raw_type,
                     nothrow_it_raw_type>::value);
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<
          typename TestFixture::vector,
          arene::base::span<arene::base::byte>,
          nothrow_it_raw_type,
          nothrow_it_raw_type>::value),
      std::is_nothrow_copy_constructible<TypeParam>::value
  );
  STATIC_ASSERT_TRUE(std::is_constructible<
                     typename TestFixture::vector,
                     arene::base::span<arene::base::byte>,
                     nothrow_it_throwing_type,
                     nothrow_it_throwing_type>::value);
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<
                      typename TestFixture::vector,
                      arene::base::span<arene::base::byte>,
                      nothrow_it_throwing_type,
                      nothrow_it_throwing_type>::value);
}

/// @test Given an `external_vector` holding existing elements, assigning to that vector with an initializer list
/// holding more elements copy assigns over the existing elements, and then copy-constructs new elements to increase the
/// size of the vector to that of the initializer list
TYPED_TEST_P(ExternalVectorTest, CopyAssignOverSmallerVectorFromInitListCopiesElements) {
  constexpr std::size_t smaller_count = 10;

  std::initializer_list<TypeParam> const value{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3),
      TestFixture::test_value(4),
      TestFixture::test_value(5),
      TestFixture::test_value(6),
      TestFixture::test_value(7),
      TestFixture::test_value(8),
      TestFixture::test_value(9),
      TestFixture::test_value(10),
      TestFixture::test_value(11),
      TestFixture::test_value(12),
      TestFixture::test_value(13)
  };

  auto copy = TestFixture::test_external_vector(TestFixture::standard_storage, smaller_count);
  copy = value;

  EXPECT_THAT(copy, ::testing::ElementsAreArray(value));
}

/// @test Given an `external_vector` holding existing elements, assigning to that vector with an initializer list
/// holding fewer elements copy assigns over the existing elements up to the size of the initializer list, and then
/// destroys a number of elements equal to the difference between the old size of the vector and the size of the
/// initializer list
TYPED_TEST_P(ExternalVectorTest, CopyAssignOverLargerVectorFromInitListDestroysExcess) {
  constexpr std::size_t larger_count = 18;

  std::initializer_list<TypeParam> const value{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3),
      TestFixture::test_value(4),
      TestFixture::test_value(5),
      TestFixture::test_value(6),
      TestFixture::test_value(7),
      TestFixture::test_value(8),
      TestFixture::test_value(9),
      TestFixture::test_value(10),
      TestFixture::test_value(11),
      TestFixture::test_value(12),
      TestFixture::test_value(13)
  };

  alignas(TypeParam) typename TestFixture::template storage_type<larger_count> storage{};
  auto copy = TestFixture::test_external_vector(storage, larger_count);
  copy = value;

  EXPECT_THAT(copy, ::testing::ElementsAreArray(value));
}

/// @test Invoking the `assign` member function with an iterator range sets the vector to have a size equal to the
/// number of elements in the range, and each element to be a copy of the corresponding element in the range
TYPED_TEST_P(ExternalVectorTest, CanAssignFromIteratorRange) {
  std::vector<TypeParam> const source{
      TestFixture::test_value(3),
      TestFixture::test_value(4),
      TestFixture::test_value(5),
      TestFixture::test_value(6)
  };

  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, 3);
  vec.assign(source.begin(), source.end());

  EXPECT_THAT(vec, ::testing::ElementsAreArray(source));
}

/// @test Assignment of an `external_vector` from an iterator range is `noexcept` if operations on the iterator are
/// `noexcept` and the element type of the vector is nothrow-constructible from the reference type of the iterator
TYPED_TEST_P(ExternalVectorTest, IteratorRangeAssignmentIsConditionallyNoexcept) {
  using array_it = typename array<TypeParam, TestFixture::capacity>::const_iterator;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using convertible_array_it = typename array<throws_when_converted, TestFixture::capacity>::const_iterator;
  using throwing_bidirectional_iterator = typename TestFixture::throwing_bidirectional_iterator_of_t;

  constexpr bool it_operations_noexcept = noexcept(++std::declval<array_it>())&& noexcept(*std::declval<array_it>());
  constexpr bool element_construction_noexcept =
      std::is_nothrow_constructible<typename TestFixture::vector::value_type, typename array_it::reference>{} &&
      std::is_nothrow_assignable<typename TestFixture::vector::value_type&, typename array_it::reference>{};

  STATIC_ASSERT_EQ(
      noexcept(std::declval<typename TestFixture::vector&>().assign(std::declval<array_it>(), std::declval<array_it>())
      ),
      it_operations_noexcept && element_construction_noexcept
  );
  STATIC_ASSERT_FALSE(noexcept(std::declval<typename TestFixture::vector&>().assign(
      std::declval<convertible_array_it>(),
      std::declval<convertible_array_it>()
  )));
  STATIC_ASSERT_FALSE(noexcept(std::declval<typename TestFixture::vector&>().assign(
      std::declval<throwing_bidirectional_iterator>(),
      std::declval<throwing_bidirectional_iterator>()
  )));
}

/// @test Invoking the `assign` member function with an initializer list sets the vector to have a size equal to the
/// number of elements in the initializer list, and each element to be a copy of the corresponding element in the
/// initializer list
TYPED_TEST_P(ExternalVectorTest, CanAssignFromInitListUsingAssignFunction) {
  constexpr std::size_t smaller_count = 10;

  std::initializer_list<TypeParam> const value{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3),
      TestFixture::test_value(4),
      TestFixture::test_value(5),
      TestFixture::test_value(6),
      TestFixture::test_value(7),
      TestFixture::test_value(8),
      TestFixture::test_value(9),
      TestFixture::test_value(10),
      TestFixture::test_value(11),
      TestFixture::test_value(12),
      TestFixture::test_value(13)
  };

  auto copy = TestFixture::test_external_vector(TestFixture::standard_storage, smaller_count);
  copy.assign(value);

  EXPECT_THAT(copy, ::testing::ElementsAreArray(value));
}

/// @test Assignment of an `external_vector` from an initializer list is `noexcept` if the element type of the vector is
/// nothrow-copy-constructible
TYPED_TEST_P(ExternalVectorTest, AssignFromInitListUsingAssignFunctionIsConditionallyNoexcept) {
  constexpr bool is_nothrow_copyable =
      std::is_nothrow_copy_constructible<TypeParam>{} && std::is_nothrow_copy_assignable<TypeParam>{};

  std::initializer_list<TypeParam> const values{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3),
      TestFixture::test_value(4),
      TestFixture::test_value(5),
      TestFixture::test_value(6),
      TestFixture::test_value(7),
      TestFixture::test_value(8),
      TestFixture::test_value(9),
      TestFixture::test_value(10),
      TestFixture::test_value(11),
      TestFixture::test_value(12),
      TestFixture::test_value(13)
  };
  STATIC_ASSERT_EQ(noexcept(typename TestFixture::vector{}.assign(values)), is_nothrow_copyable);
}

/// @test Invoking the `assign` member function with an iterator range specified using input iterators sets the vector
/// to have a size equal to the number of elements in the range, and each element to be a copy of the corresponding
/// element in the range
TYPED_TEST_P(ExternalVectorTest, CanAssignFromInputIteratorRange) {
  std::vector<TypeParam> const source{
      TestFixture::test_value(3),
      TestFixture::test_value(4),
      TestFixture::test_value(5),
      TestFixture::test_value(6)
  };

  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, 3);
  vec.assign(source.begin(), source.end());

  EXPECT_THAT(vec, ::testing::ElementsAreArray(source));
}

/// @test Assignment of an `external_vector` from an input iterator range is `noexcept` if operations on the iterator
/// are `noexcept` and the element type of the vector is nothrow-constructible from the value type of the iterator
TYPED_TEST_P(ExternalVectorTest, InputIteratorRangeAssignmentIsConditionallyNoexcept) {
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<TypeParam>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type = typename TestFixture::template non_throwing_input_iterator<TypeParam>;

  STATIC_ASSERT_FALSE(noexcept(typename TestFixture::vector{}.assign(
      std::declval<std::istream_iterator<TypeParam>>(),
      std::declval<std::istream_iterator<TypeParam>>()
  )));
  STATIC_ASSERT_FALSE(noexcept(typename TestFixture::vector{}.assign(
      std::declval<throwing_it_throwing_type>(),
      std::declval<throwing_it_throwing_type>()
  )));
  STATIC_ASSERT_FALSE(noexcept(
      typename TestFixture::vector{}.assign(std::declval<throwing_it_raw_type>(), std::declval<throwing_it_raw_type>())
  ));
  STATIC_ASSERT_EQ(
      noexcept(typename TestFixture::vector{}
                   .assign(std::declval<nothrow_it_raw_type>(), std::declval<nothrow_it_raw_type>())),
      std::is_nothrow_copy_assignable<TypeParam>::value
  );
  STATIC_ASSERT_FALSE(noexcept(typename TestFixture::vector{}.assign(
      std::declval<nothrow_it_throwing_type>(),
      std::declval<nothrow_it_throwing_type>()
  )));
}

/// @test Invoking the `assign` member function with a count and a value sets the size of the vector to the specified
/// count, where each element is a copy of the supplied value
TYPED_TEST_P(ExternalVectorTest, CanAssignFromSizeAndValue) {
  constexpr std::size_t count = 14;
  constexpr std::size_t smaller_count = 10;

  auto copy = TestFixture::test_external_vector(TestFixture::standard_storage, smaller_count);
  TypeParam const value = TestFixture::test_value(0);
  copy.assign(count, value);

  EXPECT_EQ(copy.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(copy[i], value);
  }
}

/// @test The `assign` member function taking a size and a value is not `noexcept` if the value type is
/// not nothrow-copy-constructible
TYPED_TEST_P(ExternalVectorTest, AssignFromSizeAndValueIsConditionallyNoexcept) {
  constexpr std::size_t count = 14;
  TypeParam const value = TestFixture::test_value(0);

  constexpr bool is_nothrow_copyable =
      std::is_nothrow_copy_constructible<TypeParam>{} && std::is_nothrow_copy_assignable<TypeParam>{};
  STATIC_ASSERT_EQ(noexcept(typename TestFixture::vector{}.assign(count, value)), is_nothrow_copyable);
}

/// @test Invoking the `assign` member function with a count and a value where the count is larger than the capacity of
/// the vector terminates the program with a precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, AssignIsPreconditionViolationIfSizeOverCapacity) {
  constexpr std::size_t smaller_count = 10;

  TypeParam const initial_value = TestFixture::test_value(0);
  typename TestFixture::vector copy(TestFixture::standard_storage, smaller_count, initial_value);
  TypeParam const value = TestFixture::test_value(1);
  ASSERT_DEATH(copy.assign(TestFixture::capacity + 1, value), "Precondition violation");

  EXPECT_EQ(copy.size(), smaller_count);
  for (std::size_t i = 0; i < smaller_count; ++i) {
    EXPECT_EQ(copy[i], initial_value);
  }
}

/// @test The index operator returns a reference to the element type of the vector
TYPED_TEST_P(ExternalVectorTest, IndexOperatorReturnsReference) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<typename TestFixture::vector&>()[0]), TypeParam&>();
}

/// @test The index operator for a `const` vector returns a `const` reference to the element type of the vector
TYPED_TEST_P(ExternalVectorTest, IndexOperatorOnConstVectorReturnsConstReference) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<typename TestFixture::vector const&>()[0]), TypeParam const&>();
}

/// @test Given an `external_vector` with multiple elements, the index operator can be used with appropriate index
/// values to access each element, and the returned reference is the same as that returned by `at` given the same index
TYPED_TEST_P(ExternalVectorTest, CanAccessMultipleValuesWithIndexOperator) {
  auto vec = TestFixture::construct(TestFixture::standard_storage);
  auto const& const_vec = vec;
  TypeParam const value1 = TestFixture::test_value(0);
  vec.push_back(value1);
  TypeParam const value2 = TestFixture::test_value(1);
  vec.push_back(value2);

  EXPECT_EQ(vec[0], value1);
  EXPECT_EQ(vec[1], value2);
  // NOLINTNEXTLINE(readability-container-data-pointer)
  EXPECT_EQ(&const_vec[0], std::addressof(vec[0]));
  EXPECT_EQ(&const_vec[1], std::addressof(vec[1]));
}

/// @test The index operator is `noexcept`
TYPED_TEST_P(ExternalVectorTest, IndexOperatorIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<typename TestFixture::vector&>()[0]));
}

/// @test Passing an index that is equal to or larger than the size of the vector terminates the program with a
/// precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, IndexOperatorOutOfRange) {
  ASSERT_DEATH(TestFixture::construct()[0], "index < this->size()");
}

/// @test The index operator is `noexcept` for a `const` vector
TYPED_TEST_P(ExternalVectorTest, IndexOperatorOnConstVectorIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<typename TestFixture::vector const&>()[0]));
}

/// @test The `data` member function is noexcept and returns a pointer to the first element of a non-empty vector
TYPED_TEST_P(ExternalVectorTest, Data) {
  typename TestFixture::vector vec{
      TestFixture::standard_storage,
      {TestFixture::test_value(3), TestFixture::test_value(4)}
  };
  auto const& const_vec = vec;

  // NOLINTNEXTLINE(readability-container-data-pointer)
  EXPECT_EQ(vec.data(), std::addressof(vec[0]));
  // NOLINTNEXTLINE(readability-container-data-pointer)
  EXPECT_EQ(const_vec.data(), std::addressof(vec[0]));

  STATIC_ASSERT_TRUE(noexcept(vec.data()));
  STATIC_ASSERT_TRUE(noexcept(const_vec.data()));

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto const value2 = TestFixture::test_external_vector(storage2, 3);
  // This first assertion is not static because std::addressof is not constexpr until C++17.
  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(value2.data(), std::addressof(value2[0]));
  alignas(TypeParam) typename TestFixture::standard_storage_type storage3{};
  ASSERT_NE(TestFixture::test_external_vector(storage3, 3).data(), nullptr);
}

/// @test Given an `external_vector` which is not empty, invoking the `clear` method makes the vector empty
TYPED_TEST_P(ExternalVectorTest, AfterClearVectorIsEmpty) {
  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, 2);

  EXPECT_EQ(vec.size(), 2);
  EXPECT_FALSE(vec.empty());
  vec.clear();
  EXPECT_EQ(vec.size(), 0);
  EXPECT_TRUE(vec.empty());

  STATIC_ASSERT_TRUE(noexcept(vec.clear()));
}

/// @brief Checks whether or not the given container has any duplicates, which we need to assert for test logic
/// @tparam Container The type of the container to check
/// @param container The container to check
/// @return @c true if @c container has any duplicate entries, @c false if every entry is unique
template <typename Container>
constexpr auto contains_duplicates(Container const& container) noexcept -> bool {
  // This runs at compile time and we can't assume that the type is hashable so we do the slow O(N^2) implementation
  for (std::size_t ii = 0UL; ii + 1UL < container.size(); ++ii) {
    for (std::size_t jj = ii + 1UL; jj < container.size(); ++jj) {
      if (container[ii] == container[jj]) {
        return true;
      }
    }
  }

  return false;
}

/// @brief Inefficiently bubble sorts the argument because @c std::sort is not constexpr in C++14
/// @tparam Container The type of a container
/// @param container A container to be sorted
/// @return An instance of @c Container with copies of the elements of @c container , but sorted
template <typename Container>
constexpr auto bubble_sort_helper(Container container) noexcept -> Container {
  bool swapped{true};
  while (swapped) {
    swapped = false;
    for (std::size_t ii = 0UL; ii + 1UL < container.size(); ++ii) {
      if (container[ii + 1] < container[ii]) {
        // std::swap is not constexpr until C++20
        typename Container::value_type const temp{container[ii]};
        container[ii] = container[ii + 1];
        container[ii + 1] = temp;
        swapped = true;
      }
    }
  }
  return container;
}

/// A type trait to tell if the given type parameter is fully comparable or not for the purposes of these tests
/// @tparam T The type to check
template <typename T>
constexpr bool is_fully_comparable_v = is_less_than_comparable_v<T> && is_less_than_or_equal_comparable_v<T> &&
                                       is_greater_than_comparable_v<T> && is_greater_than_or_equal_comparable_v<T>;

/// @test Two instances of `external_vector` can be compared for ordering using `operator<` and `operator>`. One vector
/// compares less than another if the first has a smaller size than the second, and all elements are equal to the
/// corresponding element in the second vector with the same index or they have the same size, and there is an element
/// such that all elements with a lower index compare equal to the corresponding element in the second vector, and that
/// element compares less than the corresponding element of the second vector.
CONDITIONAL_TYPED_TEST_P(ExternalVectorTest, CanCompareForOrdering, is_fully_comparable_v<TypeParam>) {
  // ASSERT_FALSE(contains_duplicates(TestFixture::test_external_vector(TestFixture::standard_storage, 4)));

  // auto sorted_test_values = bubble_sort_helper(TestFixture::test_external_vector(TestFixture::standard_storage, 4));
  // typename TestFixture::vector low_value(sorted_test_values.begin(), sorted_test_values.begin() + 2UL);
  // typename TestFixture::vector high_value(sorted_test_values.begin() + 2UL, sorted_test_values.begin() + 4UL);
  // typename TestFixture::vector long_value(sorted_test_values.begin(), sorted_test_values.begin() + 3UL);
  // typename TestFixture::vector low_value_copy{low_value};

  // ASSERT_TRUE(low_value < high_value);
  // ASSERT_TRUE(low_value < long_value);
  // ASSERT_FALSE(low_value < low_value_copy);
  // ASSERT_FALSE(high_value < low_value);
  // ASSERT_TRUE(high_value > low_value);
  // ASSERT_TRUE(long_value > low_value);
  // ASSERT_FALSE(low_value_copy > low_value);
  // ASSERT_FALSE(low_value > high_value);

  // ASSERT_TRUE(low_value <= high_value);
  // ASSERT_TRUE(low_value <= long_value);
  // ASSERT_TRUE(low_value <= low_value_copy);
  // ASSERT_FALSE(high_value <= low_value);
  // ASSERT_TRUE(high_value >= low_value);
  // ASSERT_TRUE(long_value >= low_value);
  // ASSERT_TRUE(low_value_copy >= low_value);
  // ASSERT_FALSE(low_value >= high_value);
}

/// @test The `emplace_back` member function can be used to add an element to an `external_vector`. The new element is
/// constructed with the arguments supplied to `emplace_back`
TYPED_TEST_P(ExternalVectorTest, CanEmplaceBack) {
  struct with_constructor_args {
    TypeParam value;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    TypeParam& reference;
    std::unique_ptr<TypeParam> pointer;

    with_constructor_args(TypeParam val, TypeParam& ref, std::unique_ptr<TypeParam> ptr)
        : value(val),
          reference(ref),
          pointer(std::move(ptr)) {}
  };

  alignas(with_constructor_args)
      arene::base::array<arene::base::byte, sizeof(with_constructor_args) * TestFixture::capacity>
          storage{};
  external_vector<with_constructor_args> vec(storage);

  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam value2 = TestFixture::test_value(1);
  TypeParam const value3 = TestFixture::test_value(2);
  auto& result = vec.emplace_back(value1, value2, std::make_unique<TypeParam>(value3));

  ::testing::StaticAssertTypeEq<decltype(result), with_constructor_args&>();
  EXPECT_EQ(vec.size(), 1);
  ASSERT_FALSE(vec.empty());
  EXPECT_EQ(std::addressof(result), std::addressof(vec[0]));

  EXPECT_EQ(result.value, value1);
  EXPECT_EQ(std::addressof(result.reference), std::addressof(value2));
  ASSERT_NE(result.pointer, nullptr);
  EXPECT_EQ(*result.pointer, value3);
}

template <typename T>
struct throws_without_nullptr {
  explicit throws_without_nullptr(T) {}
  throws_without_nullptr(T, std::nullptr_t) noexcept {}
};

/// @test `emplace_back` is `noexcept` if and only if the element type of the vector is nothrow-constructible from the
/// supplied arguments
TYPED_TEST_P(ExternalVectorTest, EmplaceBackIsConditionallyNoexcept) {
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace_back(std::declval<TypeParam&&>())),
      std::is_nothrow_move_constructible<TypeParam>{}
  );
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace_back(std::declval<TypeParam const&>())),
      std::is_nothrow_copy_constructible<TypeParam>{}
  );

  STATIC_ASSERT_FALSE(
      noexcept(external_vector<throws_without_nullptr<TypeParam>>{}.emplace_back(std::declval<TypeParam>()))
  );
  STATIC_ASSERT_TRUE(
      noexcept(external_vector<throws_without_nullptr<TypeParam>>{}.emplace_back(std::declval<TypeParam>(), nullptr))
  );
}

/// @test `emplace` with a position is `noexcept` if and only if the element type of the vector is
/// nothrow-move-constructible, nothrow-move-assignable, and nothrow-constructible from the supplied arguments
TYPED_TEST_P(ExternalVectorTest, EmplaceAtPositionIsConditionallyNoexcept) {
  using const_it = typename TestFixture::vector::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<TypeParam>{} && std::is_nothrow_move_assignable<TypeParam>{};

  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace(std::declval<const_it>(), std::declval<TypeParam&&>())),
      is_nothrow_movable && std::is_nothrow_move_constructible<TypeParam>{}
  );
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace(std::declval<const_it>(), std::declval<TypeParam const&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<TypeParam>{}
  );

  using overload_test_external_vector = external_vector<throws_without_nullptr<TypeParam>>;
  using overload_it = typename overload_test_external_vector::const_iterator;
  STATIC_ASSERT_FALSE(
      noexcept(overload_test_external_vector{}.emplace(std::declval<overload_it>(), std::declval<TypeParam>()))
  );
  STATIC_ASSERT_TRUE(
      noexcept(overload_test_external_vector{}.emplace(std::declval<overload_it>(), std::declval<TypeParam>(), nullptr))
  );
}

/// @test Emplacing using the default constructor has the right exception specification. This test is separate from the
/// ones above due to a bug in GCC8 which tries to instantiate the `emplace_back` and `emplace` functions before
/// checking their exception specifications, which fails for types that are not default-constructible.
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorTest,
    DefaultEmplaceIsConditionallyNoexcept,
    std::is_default_constructible<TypeParam>{}
) {
  using const_it = typename TestFixture::vector::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<TypeParam>{} && std::is_nothrow_move_assignable<TypeParam>{};

  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace_back()),
      std::is_nothrow_default_constructible<TypeParam>{}
  );
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace(std::declval<const_it>())),
      is_nothrow_movable && std::is_nothrow_default_constructible<TypeParam>{}
  );
}

/// @test Invoking `emplace_back` on an `external_vector<T>` which has a size equal to its capacity terminates the
/// program with a precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, EmplaceBackIsPreconditionViolationIfFull) {
  auto vec = TestFixture::test_external_vector(TestFixture::standard_storage, TestFixture::capacity);

  TypeParam const value1 = TestFixture::test_value(0);
  ASSERT_DEATH(vec.emplace_back(value1), "Precondition violation");
}

/// @test Invoking `swap` without a namespace qualification on two `external_vector` instances exchanges the contents,
/// so the elements from one have been moved to the other, and vice-versa.
TYPED_TEST_P(ExternalVectorTest, ADLSwapSwapsContentsForBasicTypes) {
  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto const source1 = TestFixture::test_external_vector(TestFixture::standard_storage, 12);
  auto const source2 = TestFixture::test_external_vector(storage2, 12, 16);
  alignas(TypeParam) typename TestFixture::standard_storage_type storage3{};
  alignas(TypeParam) typename TestFixture::standard_storage_type storage4{};
  typename TestFixture::vector value1(storage3, source1);
  typename TestFixture::vector value2(storage4, source2);

  swap(value1, value2);

  EXPECT_EQ(value1, source2);
  EXPECT_EQ(value2, source1);
}

/// @test Swapping two `external_vector`s is `noexcept` if and only if the element type is nothrow swappable
TYPED_TEST_P(ExternalVectorTest, SwapNoexceptMatchesElementType) {
  using vec = typename TestFixture::vector;
  constexpr bool element_nothrow_swappable = ::arene::base::is_nothrow_swappable_v<TypeParam>;

  STATIC_ASSERT_EQ(::arene::base::is_nothrow_swappable_v<vec>, element_nothrow_swappable);
  STATIC_ASSERT_EQ(noexcept(std::declval<vec>().swap(std::declval<vec&>())), element_nothrow_swappable);
}

/// @test `insert` is `noexcept` if the element type of the vector is nothrow-constructible from the supplied value, and
/// is nothrow-move-constructible and nothrow-move-assignable
TYPED_TEST_P(ExternalVectorTest, InsertIsConditionallyNoexcept) {
  using vec = typename TestFixture::vector;
  using iterator = typename vec::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<TypeParam>{} && std::is_nothrow_move_assignable<TypeParam>{};

  STATIC_ASSERT_EQ(noexcept(vec{}.insert(std::declval<iterator>(), std::declval<TypeParam&&>())), is_nothrow_movable);
  STATIC_ASSERT_EQ(
      noexcept(vec{}.insert(std::declval<iterator>(), std::declval<TypeParam const&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<TypeParam>{}
  );
}

/// @test `insert` is `noexcept` if the element type of the vector is nothrow-copy-constructible, and is
/// nothrow-move-constructible and nothrow-move-assignable
TYPED_TEST_P(ExternalVectorTest, InsertWithCountIsConditionallyNoexcept) {
  using vec = typename TestFixture::vector;
  using iterator = typename vec::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<TypeParam>{} && std::is_nothrow_move_assignable<TypeParam>{};

  STATIC_ASSERT_EQ(
      noexcept(vec{}.insert(std::declval<iterator>(), 0U, std::declval<TypeParam&&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<TypeParam>{}
  );
  STATIC_ASSERT_EQ(
      noexcept(vec{}.insert(std::declval<iterator>(), 0U, std::declval<TypeParam const&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<TypeParam>{}
  );
}

/// @test Invoking `emplace` inserts a new value at the position indicated, constructing the new value with the supplied
/// arguments, and returning an iterator referencing the new value
TYPED_TEST_P(ExternalVectorTest, EmplaceInsert) {
  struct with_constructor_args {
    TypeParam value;
    TypeParam* raw_pointer;
    std::unique_ptr<TypeParam> unique_pointer;

    with_constructor_args(TypeParam val, TypeParam* rptr, std::unique_ptr<TypeParam> uptr)
        : value(val),
          raw_pointer(rptr),
          unique_pointer(std::move(uptr)) {}
  };

  alignas(with_constructor_args)
      arene::base::array<arene::base::byte, sizeof(with_constructor_args) * TestFixture::capacity>
          storage{};
  external_vector<with_constructor_args> vec(storage);
  auto const& const_vec1 = vec;
  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam value2 = TestFixture::test_value(1);
  TypeParam const value3 = TestFixture::test_value(2);
  auto pos = vec.emplace(const_vec1.begin(), value1, std::addressof(value2), std::make_unique<TypeParam>(value3));
  EXPECT_EQ(pos, vec.begin());
  EXPECT_EQ(vec.size(), 1);
  EXPECT_EQ(&*pos, std::addressof(vec[0]));

  EXPECT_EQ(pos->value, value1);
  ASSERT_NE(pos->raw_pointer, nullptr);
  EXPECT_EQ(pos->raw_pointer, std::addressof(value2));
  ASSERT_NE(pos->unique_pointer, nullptr);
  EXPECT_EQ(*pos->unique_pointer, value3);
}

/// @test Invoking `insert` with a count and a value to insert more elements than there are remaining spaces in the
/// vector before it exceeds capacity terminates the program with a precondition violation.
TYPED_TEST_P(ExternalVectorDeathTest, InsertNIsPreconditionViolationIfTooManyItems) {
  auto value = TestFixture::test_external_vector(TestFixture::standard_storage, 4);
  ASSERT_DEATH(
      value.insert(value.begin(), (TestFixture::capacity + 1) - value.size(), TestFixture::test_value(5)),
      "Precondition violation"
  );
  EXPECT_EQ(value.size(), 4);
}

/// @test Invoking `insert` with the end iterator of the `external_vector` and a count and value appends the specified
/// number of copies of the supplied value to the end of the vector.
TYPED_TEST_P(ExternalVectorTest, InsertNAtEndInsertsElements) {
  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam const value2 = TestFixture::test_value(1);
  TypeParam const value3 = TestFixture::test_value(2);
  typename TestFixture::vector vec{TestFixture::standard_storage, {value1, value2}};
  constexpr std::size_t count = 5;

  vec.insert(vec.end(), count, value3);
  EXPECT_EQ(vec.size(), count + 2);
  EXPECT_EQ(vec[0], value1);
  EXPECT_EQ(vec[1], value2);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(vec[i + 2], value3);
  }
}

/// @test Invoking `insert` with an iterator referring to an existing element of an `external_vector` and a count and
/// value moves the referred to element and subsequent elements to make room, and inserts the specified number of copies
/// of the supplied value prior to the existing element.
TYPED_TEST_P(ExternalVectorTest, InsertNInMiddleInsertsElements) {
  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam const value2 = TestFixture::test_value(1);
  TypeParam const value3 = TestFixture::test_value(2);
  TypeParam const value4 = TestFixture::test_value(3);
  TypeParam const value_inserted = TestFixture::test_value(4);
  typename TestFixture::vector vec{TestFixture::standard_storage, {value1, value2, value3, value4}};
  constexpr std::size_t count = 5;

  vec.insert(vec.begin() + 1, count, value_inserted);
  EXPECT_EQ(vec.size(), count + 4);
  EXPECT_EQ(vec[0], value1);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(vec[i + 1], value_inserted);
  }
  EXPECT_EQ(vec[count + 1], value2);
  EXPECT_EQ(vec[count + 2], value3);
  EXPECT_EQ(vec[count + 3], value4);
}

/// @test Invoking `insert` with an iterator referring to an existing element of an `external_vector` and a source
/// iterator range denoted by a pair of iterators moves the referred to element and subsequent elements to make room,
/// and inserts the elements from the source iterator range prior to the existing element.
TYPED_TEST_P(ExternalVectorTest, InsertFromIteratorRangeInMiddle) {
  TypeParam const value1 = TestFixture::test_value(0);
  TypeParam const value2 = TestFixture::test_value(1);
  TypeParam const value3 = TestFixture::test_value(2);
  TypeParam const value4 = TestFixture::test_value(3);
  auto vec1 = TestFixture::test_external_vector(TestFixture::standard_storage, 4);
  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto const vec2 = TestFixture::test_external_vector(storage2, 4, 11);

  vec1.insert(vec1.begin() + 1, vec2.begin(), vec2.end());
  EXPECT_EQ(vec1.size(), vec2.size() + 4);
  EXPECT_EQ(vec1[0], value1);
  for (std::size_t idx = 0; idx < vec2.size(); ++idx) {
    EXPECT_EQ(vec1[idx + 1], vec2[idx]);
  }
  EXPECT_EQ(vec1[vec2.size() + 1], value2);
  EXPECT_EQ(vec1[vec2.size() + 2], value3);
  EXPECT_EQ(vec1[vec2.size() + 3], value4);
}

/// @test `insert` is `noexcept` if the element type of the vector is nothrow-constructible from the supplied iterator's
/// reference type, and is nothrow-move-constructible and nothrow-move-assignable
TYPED_TEST_P(ExternalVectorTest, InsertFromIteratorRangeIsConditionallyNoexcept) {
  using ivec_iter = typename TestFixture::vector::const_iterator;
  using source_array = ::arene::base::array<TypeParam, TestFixture::capacity>;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throw_conv_array = ::arene::base::array<throws_when_converted, TestFixture::capacity>;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<TypeParam>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type = typename TestFixture::template non_throwing_input_iterator<TypeParam>;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<TypeParam>{} && std::is_nothrow_move_assignable<TypeParam>{};

  STATIC_ASSERT_EQ(
      noexcept(this->construct().insert(
          std::declval<ivec_iter>(),
          std::declval<typename source_array::const_iterator>(),
          std::declval<typename source_array::const_iterator>()
      )),
      is_nothrow_movable && std::is_nothrow_copy_constructible<TypeParam>{}
  );

  STATIC_ASSERT_FALSE(noexcept(this->construct().insert(
      std::declval<ivec_iter>(),
      std::declval<typename throw_conv_array::const_iterator>(),
      std::declval<typename throw_conv_array::const_iterator>()
  )));

  STATIC_ASSERT_EQ(
      noexcept(this->construct().insert(
          std::declval<ivec_iter>(),
          std::declval<nothrow_it_raw_type>(),
          std::declval<nothrow_it_raw_type>()
      )),
      is_nothrow_movable && std::is_nothrow_copy_constructible<TypeParam>{}
  );
  STATIC_ASSERT_FALSE(noexcept(this->construct().insert(
      std::declval<ivec_iter>(),
      std::declval<std::istream_iterator<TypeParam>>(),
      std::declval<std::istream_iterator<TypeParam>>()
  )));
  STATIC_ASSERT_FALSE(noexcept(this->construct().insert(
      std::declval<ivec_iter>(),
      std::declval<throwing_it_throwing_type>(),
      std::declval<throwing_it_throwing_type>()
  )));
  STATIC_ASSERT_FALSE(noexcept(this->construct().insert(
      std::declval<ivec_iter>(),
      std::declval<throwing_it_raw_type>(),
      std::declval<throwing_it_raw_type>()
  )));
  STATIC_ASSERT_FALSE(noexcept(this->construct().insert(
      std::declval<ivec_iter>(),
      std::declval<nothrow_it_throwing_type>(),
      std::declval<nothrow_it_throwing_type>()
  )));
}

/// @test `cbegin` and `cend` return `const_iterator`s to the corresponding locations of the `external_vector`.
TYPED_TEST_P(ExternalVectorTest, CanGetCBeginAndEnd) {
  auto const value1 = TestFixture::test_external_vector(TestFixture::standard_storage, 4);

  ASSERT_EQ(value1.cbegin(), value1.begin());
  ASSERT_EQ(value1.cend(), value1.end());

  ::testing::StaticAssertTypeEq<decltype(value1.cbegin()), typename TestFixture::vector::const_iterator>();
  STATIC_ASSERT_TRUE(noexcept(value1.cbegin()));
  STATIC_ASSERT_TRUE(noexcept(value1.cend()));

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto value2 = TestFixture::test_external_vector(storage2, 4);
  ::testing::StaticAssertTypeEq<decltype(value2.cbegin()), typename TestFixture::vector::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(value2.cend()), typename TestFixture::vector::const_iterator>();

  EXPECT_EQ(value2.begin(), value2.cbegin());
  EXPECT_EQ(value2.end(), value2.cend());
}

/// @test `rbegin`, `rend`, `crbegin` and `crend` return reverse iterators to the appropriate locations in the
/// `external_vector`, such that iteration _forwards_ from `rbegin` or `crbegin` iterates through the elements of the
/// `external_vector` in reverse order.
TYPED_TEST_P(ExternalVectorTest, ReverseIteration) {
  auto const value1 = TestFixture::test_external_vector(TestFixture::standard_storage, 4);

  auto itr = value1.rbegin();
  ASSERT_NE(itr, value1.rend());
  EXPECT_EQ(*itr, value1[3]);
  ++itr;
  ASSERT_NE(itr, value1.rend());
  EXPECT_EQ(*itr, value1[2]);
  ++itr;
  ASSERT_NE(itr, value1.rend());
  EXPECT_EQ(*itr, value1[1]);
  ++itr;
  ASSERT_NE(itr, value1.rend());
  EXPECT_EQ(*itr, value1[0]);
  ++itr;
  ASSERT_EQ(itr, value1.rend());

  auto citr = value1.crbegin();
  ASSERT_NE(citr, value1.crend());
  EXPECT_EQ(*citr, value1[3]);
  ++citr;
  ASSERT_NE(citr, value1.crend());
  EXPECT_EQ(*citr, value1[2]);
  ++citr;
  ASSERT_NE(citr, value1.crend());
  EXPECT_EQ(*citr, value1[1]);
  ++citr;
  ASSERT_NE(citr, value1.crend());
  EXPECT_EQ(*citr, value1[0]);
  ++citr;
  ASSERT_EQ(citr, value1.crend());
}

/// @test An `external_vector` can be assigned from an `external_vector` with a larger capacity, provided the number of
/// elements in the source vector is less than the capacity of the target vector
TYPED_TEST_P(ExternalVectorTest, CanAssignFromBiggerCapacityVector) {
  constexpr std::size_t larger_capacity = TestFixture::capacity + 2UL;
  alignas(TypeParam) typename TestFixture::template storage_type<larger_capacity> storage{};
  external_vector<TypeParam> const large_vector{
      storage,
      {TestFixture::test_value(0),
       TestFixture::test_value(1),
       TestFixture::test_value(2),
       TestFixture::test_value(3),
       TestFixture::test_value(4)}
  };

  alignas(TypeParam) typename TestFixture::template storage_type<TestFixture::capacity> storage2{};
  typename TestFixture::vector small_vector(storage2, large_vector);
  small_vector = large_vector;

  EXPECT_EQ(small_vector.size(), large_vector.size());
  for (std::size_t i = 0; i < small_vector.size(); ++i) {
    EXPECT_EQ(small_vector[i], large_vector[i]);
  }
}

/// @test `try_construct` can be used to construct an empty `external_vector`.
TYPED_TEST_P(ExternalVectorTest, CanDefaultConstructViaTryConstruct) {
  using vec = typename TestFixture::vector;
  STATIC_ASSERT_TRUE(std::is_same<decltype(vec::try_construct()), ::arene::base::optional<vec>>::value);
  STATIC_ASSERT_TRUE(noexcept(vec::try_construct()));

  ASSERT_TRUE(vec::try_construct().has_value());
  ASSERT_TRUE(vec::try_construct()->empty());
}

/// @test `try_construct` can be used to construct an `external_vector` with a specified number of elements
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorTest,
    CanConstructWithSizeViaTryConstruct,
    std::is_default_constructible<TypeParam>{}
) {
  using vec = typename TestFixture::vector;
  constexpr std::size_t size1 = 10;
  STATIC_ASSERT_TRUE(
      std::is_same<decltype(vec::try_construct(TestFixture::standard_storage, size1)), ::arene::base::optional<vec>>::
          value
  );
  STATIC_ASSERT_TRUE(noexcept(vec::try_construct(TestFixture::standard_storage, size1)));

  ASSERT_TRUE(vec::try_construct(TestFixture::standard_storage, size1).has_value());
  ASSERT_FALSE(vec::try_construct(TestFixture::standard_storage, size1)->empty());
  ASSERT_EQ(vec::try_construct(TestFixture::standard_storage, size1)->size(), size1);
}

/// @test `try_construct` can be used to attempt to construct an `external_vector` with a specified number of elements
/// that exceeds the capacity of the vector, in which case the result is an empty `optional`.
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorTest,
    ConstructWithExcessiveSizeViaTryConstructFails,
    std::is_default_constructible<TypeParam>{}
) {
  using vec = typename TestFixture::vector;
  constexpr std::size_t size1 = TestFixture::capacity + 1;
  STATIC_ASSERT_TRUE(
      std::is_same<decltype(vec::try_construct(TestFixture::standard_storage, size1)), ::arene::base::optional<vec>>::
          value
  );
  STATIC_ASSERT_TRUE(noexcept(vec::try_construct(TestFixture::standard_storage, size1)));

  ASSERT_FALSE(vec::try_construct(TestFixture::standard_storage, size1).has_value());
}

/// @test `try_construct` with a number of elements is not considered for overload resolution if the element type of the
/// `external_vector<T>` is not default-constructible.
TYPED_TEST_P(ExternalVectorTest, CannotTryConstructWithSizeForANonDefaultConstructibleType) {
  constexpr bool can_construct_with_size_only = ::arene::base::substitution_succeeds<
      TestFixture::template try_construct_result,
      typename TestFixture::vector,
      arene::base::span<arene::base::byte>,
      std::size_t>;
  STATIC_ASSERT_EQ(can_construct_with_size_only, std::is_default_constructible<TypeParam>{});
}

/// @test The `external_vector` constructor with a number of elements is not considered for overload resolution if the
/// element type of the `external_vector<T>` is not default-constructible.
TYPED_TEST_P(ExternalVectorTest, CannotConstructWithSizeForANonDefaultConstructibleType) {
  STATIC_ASSERT_EQ(
      (std::is_constructible<typename TestFixture::vector, arene::base::span<arene::base::byte>, std::size_t>::value),
      std::is_default_constructible<TypeParam>{}
  );
}

/// @test The default constructor and corresponding `try_construct` function of `external_vector<T>` is always
/// `noexcept`, even if the stored element type's default constructor can throw
TYPED_TEST_P(ExternalVectorTest, DefaultTryConstructNoexceptEvenIfDefaultConstructorNotNoexcept) {
  using ivec = typename TestFixture::vector;
  STATIC_ASSERT_TRUE(noexcept(ivec::try_construct()));
  STATIC_ASSERT_TRUE(noexcept(ivec{}));

  ASSERT_TRUE(ivec::try_construct().has_value());
  ASSERT_TRUE(ivec::try_construct()->empty());
  ASSERT_TRUE(ivec().empty());
}

/// @test The constructor with just storage and corresponding `try_construct` function of `external_vector<T>` is always
/// `noexcept`, even if the stored element type's default constructor can throw
TYPED_TEST_P(ExternalVectorTest, StorageOnlyTryConstructNoexceptEvenIfDefaultConstructorNotNoexcept) {
  using ivec = typename TestFixture::vector;
  STATIC_ASSERT_TRUE(noexcept(ivec::try_construct(TestFixture::standard_storage)));
  STATIC_ASSERT_TRUE(noexcept(ivec{TestFixture::standard_storage}));

  ASSERT_TRUE(ivec::try_construct(TestFixture::standard_storage).has_value());
  ASSERT_TRUE(ivec::try_construct(TestFixture::standard_storage)->empty());
  ASSERT_TRUE(ivec(TestFixture::standard_storage).empty());
}

/// @test The `try_construct` function of `external_vector` that takes a number of elements is not `noexcept` if the
/// default constructor of the stored element type is not `noexcept`
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorTest,
    ConstructWithSizeViaTryConstructNotNoexceptIfDefaultConstructorNotNoexcept,
    std::is_default_constructible<TypeParam>{}
) {
  using ivec = typename TestFixture::vector;
  constexpr std::size_t small_size = 10;
  STATIC_ASSERT_TRUE(std::is_same<
                     decltype(ivec::try_construct(TestFixture::standard_storage, small_size)),
                     ::arene::base::optional<ivec>>::value);
  STATIC_ASSERT_EQ(
      noexcept(ivec::try_construct(TestFixture::standard_storage, small_size)),
      std::is_default_constructible<TypeParam>{}
  );
}

/// @test The constructor of `external_vector` that takes a number of elements is not `noexcept` if the default
/// constructor of the stored element type is not `noexcept`
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorTest,
    ConstructWithSizeViaNormalConstructWithDefaultConstructorNotNoexceptIsNotNoexcept,
    std::is_default_constructible<TypeParam>{}
) {
  using ivec = typename TestFixture::vector;
  constexpr std::size_t size1 = TestFixture::capacity - 1;
  STATIC_ASSERT_EQ(
      noexcept(ivec(TestFixture::standard_storage, size1)),
      std::is_nothrow_default_constructible<TypeParam>{}
  );
}

/// @test Specifying a number of elements larger than the capacity of an `external_vector` when invoking the
/// constructor terminates the program with a precondition violation
CONDITIONAL_TYPED_TEST_P(
    ExternalVectorDeathTest,
    ConstructWithExcessiveSizeViaNormalConstructIsAlwaysPreconditionViolation,
    std::is_default_constructible<TypeParam>{}
) {
  constexpr std::size_t size1 = TestFixture::capacity + 1;
  ASSERT_DEATH(
      this->construct(TestFixture::standard_storage, size1),
      "Precondition violation"
  );  // dies regardless of exception behavior
}

/// @test The overload of `try_construct` that takes a size and source value is not considered for overload resolution
/// if the element type of the `external_vector<T>` is not copy-constructible.
TYPED_TEST_P(ExternalVectorTest, NotConstructibleWithSizeAndSourceIfDataNotCopyable) {
  using ivec = typename TestFixture::vector;
  constexpr bool copy_ok = std::is_copy_constructible<TypeParam>::value;
  STATIC_ASSERT_EQ(
      (std::is_constructible<ivec, arene::base::span<arene::base::byte>, std::size_t, TypeParam>::value),
      copy_ok
  );
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<
          TestFixture::template try_construct_result,
          ivec,
          arene::base::span<arene::base::byte>,
          std::size_t,
          TypeParam>),
      copy_ok
  );
}

/// @test Invoking `try_construct` with an initializer list constructs an `external_vector` holding copies of the
/// elements from the initializer list
TYPED_TEST_P(ExternalVectorTest, CanTryConstructFromInitializerList) {
  static std::initializer_list<TypeParam> const values{
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3)
  };

  STATIC_ASSERT_EQ(
      noexcept(TestFixture::vector::try_construct(TestFixture::standard_storage, values)),
      std::is_nothrow_copy_constructible<TypeParam>{}
  );
  ASSERT_TRUE(TestFixture::vector::try_construct(TestFixture::standard_storage, values).has_value());
  ASSERT_EQ(TestFixture::vector::try_construct(TestFixture::standard_storage, values)->size(), values.size());

  auto res = TestFixture::vector::try_construct(TestFixture::standard_storage, values);
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an initializer list holding more elements than the capacity of the
/// `external_vector` returns an empty `optional`
TYPED_TEST_P(ExternalVectorTest, TryConstructFromTooLargeInitializerListReturnsEmpty) {
  static std::initializer_list<TypeParam> const values{
      TestFixture::test_value(0),  TestFixture::test_value(1),  TestFixture::test_value(2),
      TestFixture::test_value(3),  TestFixture::test_value(4),  TestFixture::test_value(5),
      TestFixture::test_value(6),  TestFixture::test_value(7),  TestFixture::test_value(8),
      TestFixture::test_value(9),  TestFixture::test_value(10), TestFixture::test_value(11),
      TestFixture::test_value(12), TestFixture::test_value(13), TestFixture::test_value(14),
      TestFixture::test_value(15), TestFixture::test_value(16), TestFixture::test_value(17),
      TestFixture::test_value(18), TestFixture::test_value(19), TestFixture::test_value(20),
      TestFixture::test_value(21), TestFixture::test_value(22), TestFixture::test_value(23),
      TestFixture::test_value(24), TestFixture::test_value(25)
  };

  ASSERT_GT(values.size(), TestFixture::capacity);
  ASSERT_FALSE(TestFixture::vector::try_construct(TestFixture::standard_storage, values).has_value());
}

/// @test Invoking `try_construct` with an existing `external_vector` returns a copy of that source vector
TYPED_TEST_P(ExternalVectorTest, CanCopyViaTryConstruct) {
  using ivec = typename TestFixture::vector;
  auto const values = TestFixture::test_external_vector(TestFixture::standard_storage);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  STATIC_ASSERT_TRUE(::arene::base::substitution_succeeds<
                     TestFixture::template try_construct_result,
                     ivec,
                     arene::base::span<arene::base::byte>,
                     ivec const&>);
  STATIC_ASSERT_EQ(
      noexcept(ivec::try_construct(storage2, values)),
      std::is_nothrow_copy_constructible<TypeParam>::value
  );
  auto res = ivec::try_construct(storage2, values);
  ASSERT_TRUE(res.has_value());
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Constructing an `external_vector` with an existing `external_vector` copies the elements from the source
/// vector
TYPED_TEST_P(ExternalVectorTest, CanCopyViaConstruct) {
  auto const values = TestFixture::test_external_vector(TestFixture::standard_storage);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  STATIC_ASSERT_EQ(
      noexcept(typename TestFixture::vector(storage2, values)),
      std::is_nothrow_copy_constructible<TypeParam>{}
  );
  auto res = this->construct(storage2, values);
  EXPECT_THAT(res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an existing `external_vector` with a smaller capacity returns an
/// `external_vector` with the same elements as the source vector
TYPED_TEST_P(ExternalVectorTest, CanCopyViaTryConstructFromSmaller) {
  using ivec = typename TestFixture::vector;
  constexpr std::size_t smaller_capacity = TestFixture::capacity - 1;
  alignas(TypeParam) alignas(arene::base::byte)
      arene::base::array<arene::base::byte, smaller_capacity * sizeof(TypeParam)>
          storage{};

  external_vector<TypeParam> const values{TestFixture::test_external_vector(storage, smaller_capacity)};

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  STATIC_ASSERT_TRUE(::arene::base::substitution_succeeds<
                     TestFixture::template try_construct_result,
                     ivec,
                     arene::base::span<arene::base::byte>,
                     ivec const&>);
  STATIC_ASSERT_EQ(
      noexcept(ivec::try_construct(storage2, values)),
      std::is_nothrow_copy_constructible<TypeParam>::value
  );
  auto res = ivec::try_construct(storage2, values);
  ASSERT_TRUE(res.has_value());
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Constructing an `external_vector` from an existing `external_vector` with a smaller capacity returns an
/// `external_vector` with the same elements as the source vector
TYPED_TEST_P(ExternalVectorTest, CanCopyViaConstructFromSmaller) {
  constexpr std::size_t smaller_capacity = TestFixture::capacity - 1;
  alignas(TypeParam) typename TestFixture::template storage_type<smaller_capacity> storage{};
  static external_vector<TypeParam> const values{TestFixture::test_external_vector(storage, smaller_capacity)};

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  STATIC_ASSERT_EQ(
      noexcept(typename TestFixture::vector(storage2, values)),
      std::is_nothrow_copy_constructible<TypeParam>{}
  );
  auto res = this->construct(storage2, values);
  EXPECT_THAT(res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an existing `external_vector` with a larger capacity but a size that is less
/// than or equal to the capacity of the destination type returns an `external_vector` with the same elements as the
/// source vector
TYPED_TEST_P(ExternalVectorTest, CanCopyViaTryConstructFromLargerWithFewerElements) {
  using ivec = typename TestFixture::vector;
  constexpr std::size_t larger_capacity = TestFixture::capacity + 1;
  alignas(TypeParam) alignas(arene::base::byte)
      arene::base::array<arene::base::byte, larger_capacity * sizeof(TypeParam)>
          storage{};
  auto const values = TestFixture::test_external_vector(storage, 0, TestFixture::capacity);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  STATIC_ASSERT_TRUE(::arene::base::substitution_succeeds<
                     TestFixture::template try_construct_result,
                     ivec,
                     arene::base::span<arene::base::byte>,
                     ivec const&>);
  STATIC_ASSERT_EQ(
      noexcept(ivec::try_construct(storage2, values)),
      std::is_nothrow_copy_constructible<TypeParam>::value
  );
  auto res = ivec::try_construct(storage2, values);
  ASSERT_TRUE(res.has_value());
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an existing `external_vector` with a larger capacity and a size that is larger
/// than the capacity of the destination type returns an empty `optional`
TYPED_TEST_P(ExternalVectorTest, FailToCopyViaTryConstructFromLargerWithMoreElements) {
  constexpr std::size_t larger_capacity = TestFixture::capacity + 1;
  alignas(TypeParam) alignas(arene::base::byte)
      arene::base::array<arene::base::byte, larger_capacity * sizeof(TypeParam)>
          storage{};
  auto const values = TestFixture::test_external_vector(storage, 0, larger_capacity);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  EXPECT_GT(values.size(), TestFixture::capacity);
  EXPECT_FALSE(TestFixture::vector::try_construct(storage2, values).has_value());
}

/// @test Constructing an `external_vector` from an existing `external_vector` with a larger capacity but a size that is
/// less than or equal to the capacity of the destination type copies elements from the source vector
TYPED_TEST_P(ExternalVectorTest, CanCopyViaConstructFromLargerWithFewerElements) {
  constexpr std::size_t larger_capacity = TestFixture::capacity + 1;
  alignas(TypeParam) typename TestFixture::template storage_type<larger_capacity> storage{};
  auto const values = TestFixture::test_external_vector(storage, 0, TestFixture::capacity);
  constexpr bool copy_is_noexcept = std::is_nothrow_copy_constructible<TypeParam>::value;

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  STATIC_ASSERT_EQ(noexcept(typename TestFixture::vector(storage2, values)), copy_is_noexcept);
  auto res = this->construct(storage2, values);
  EXPECT_THAT(res, ::testing::ElementsAreArray(values));
}

/// @test `external_vector` is not copy-constructible
TYPED_TEST_P(ExternalVectorTest, NotCopyConstructible) {
  using ivec = typename TestFixture::vector;

  STATIC_ASSERT_FALSE((std::is_constructible<ivec, ivec const&>::value));
  STATIC_ASSERT_FALSE(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, ivec, ivec const&>)
  );
}

/// @test `external_vector` is constructible with storage and an old vector if and only if the data type is
/// copy-constructible
TYPED_TEST_P(ExternalVectorTest, NotCopyConstructibleIfDataTypeNotCopyConstructible) {
  using ivec = typename TestFixture::vector;
  constexpr bool copy_ok = std::is_copy_constructible<TypeParam>::value;

  STATIC_ASSERT_EQ((std::is_constructible<ivec, span<byte>, ivec const&>::value), copy_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, ivec, span<byte>, ivec const&>),
      copy_ok
  );
}

/// @test Invoking `try_construct` with an rvalue `external_vector` constructs a new `external_vector`, and
/// move-constructs the elements from the source to the new vector.
TYPED_TEST_P(ExternalVectorTest, CanMoveViaTryConstruct) {
  auto const values = TestFixture::test_external_vector(TestFixture::standard_storage);
  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  typename TestFixture::vector values_copy(storage2, values);

  STATIC_ASSERT_TRUE(::arene::base::substitution_succeeds<
                     TestFixture::template try_construct_result,
                     typename TestFixture::vector,
                     typename TestFixture::vector&&>);
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::vector::try_construct(std::move(values_copy))),
      std::is_nothrow_move_constructible<TypeParam>::value
  );
  auto res = TestFixture::vector::try_construct(std::move(values_copy));
  ASSERT_TRUE(res.has_value());
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Constructing an `external_vector` from an rvalue `external_vector` move-constructs the elements from the
/// source to the new vector.
TYPED_TEST_P(ExternalVectorTest, CanMoveViaConstruct) {
  auto const values = TestFixture::test_external_vector(TestFixture::standard_storage);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  typename TestFixture::vector source(storage2, values);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  STATIC_ASSERT_TRUE(noexcept(typename TestFixture::vector(std::move(source))));
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  auto res = this->construct(std::move(source));
  EXPECT_THAT(res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an rvalue `external_vector` with a larger capacity and a size that is larger
/// than the capacity of the destination storage returns an empty `optional`
TYPED_TEST_P(ExternalVectorTest, FailToMoveViaTryConstructFromLargerWithMoreElements) {
  constexpr std::size_t larger_capacity = TestFixture::capacity + 1;
  alignas(TypeParam) alignas(arene::base::byte)
      arene::base::array<arene::base::byte, larger_capacity * sizeof(TypeParam)>
          storage{};
  auto values = TestFixture::test_external_vector(storage, TestFixture::capacity + 1);

  ASSERT_GT(values.size(), TestFixture::capacity);

  ASSERT_FALSE(
      // NOLINTNEXTLINE(hicpp-move-const-arg)
      TestFixture::vector::try_construct(TestFixture::standard_storage, external_vector<TypeParam>{std::move(values)})
          .has_value()
  );
}

/// @test `external_vector` is not move constructible if the element type is not move constructible
TYPED_TEST_P(ExternalVectorTest, NotMoveConstructibleIfDataTypeNotMoveConstructible) {
  using ivec = external_vector<TypeParam>;
  constexpr bool move_ok = std::is_move_constructible<TypeParam>::value;

  STATIC_ASSERT_EQ((std::is_constructible<ivec, ivec&&>::value), move_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, ivec, ivec&&>),
      move_ok
  );
}

/// @test Invoking `try_construct` with an iterator range returns an `external_vector` with the same elements as the
/// source range
TYPED_TEST_P(ExternalVectorTest, CanTryConstructFromIteratorRange) {
  auto const source = TestFixture::test_external_vector(TestFixture::standard_storage, 5);

  alignas(TypeParam) typename TestFixture::standard_storage_type storage2{};
  auto maybe_vec = TestFixture::vector::try_construct(storage2, source.begin(), source.end());
  ASSERT_TRUE(maybe_vec.has_value());

  ASSERT_EQ(maybe_vec->size(), source.size());
  for (std::size_t i = 0; i < maybe_vec->size(); ++i) {
    EXPECT_EQ((*maybe_vec)[i], source[i]);
  }
}

/// @test `try_construct` from an iterator range is `noexcept` if the element type of the vector is
/// nothrow-constructible from the iterator's reference type
TYPED_TEST_P(ExternalVectorTest, IteratorRangeTryConstructionIsConditionallyNoexcept) {
  using array = ::arene::base::array<TypeParam, TestFixture::capacity>;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using convertible_array = ::arene::base::array<throws_when_converted, TestFixture::capacity>;
  using throwing_bidirectional_iterator = typename TestFixture::throwing_bidirectional_iterator_of_t;

  STATIC_ASSERT_EQ(
      noexcept(TestFixture::vector::try_construct(
          std::declval<arene::base::span<arene::base::byte>>(),
          std::declval<typename array::const_iterator>(),
          std::declval<typename array::const_iterator>()
      )),
      std::is_nothrow_copy_constructible<TypeParam>::value
  );
  STATIC_ASSERT_FALSE(noexcept(TestFixture::vector::try_construct(
      std::declval<arene::base::span<arene::base::byte>>(),
      std::declval<typename convertible_array::const_iterator>(),
      std::declval<typename convertible_array::const_iterator>()
  )));
  STATIC_ASSERT_FALSE(noexcept(TestFixture::vector::try_construct(
      std::declval<arene::base::span<arene::base::byte>>(),
      std::declval<throwing_bidirectional_iterator>(),
      std::declval<throwing_bidirectional_iterator>()
  )));
}

/// @test `try_construct` from an input iterator range is `noexcept` if the element type of the vector is
/// nothrow-constructible from the iterator's reference type
TYPED_TEST_P(ExternalVectorTest, InputIteratorRangeTryConstructionIsConditionallyNoexcept) {
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<TypeParam>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type = typename TestFixture::template non_throwing_input_iterator<TypeParam>;

  STATIC_ASSERT_FALSE(noexcept(TestFixture::vector::try_construct(
      std::declval<arene::base::span<arene::base::byte>>(),
      std::declval<std::istream_iterator<TypeParam>>(),
      std::declval<std::istream_iterator<TypeParam>>()
  )));
  STATIC_ASSERT_FALSE(noexcept(TestFixture::vector::try_construct(
      std::declval<arene::base::span<arene::base::byte>>(),
      std::declval<throwing_it_throwing_type>(),
      std::declval<throwing_it_throwing_type>()
  )));
  STATIC_ASSERT_FALSE(noexcept(TestFixture::vector::try_construct(
      std::declval<arene::base::span<arene::base::byte>>(),
      std::declval<throwing_it_raw_type>(),
      std::declval<throwing_it_raw_type>()
  )));
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::vector::try_construct(
          std::declval<arene::base::span<arene::base::byte>>(),
          std::declval<nothrow_it_raw_type>(),
          std::declval<nothrow_it_raw_type>()
      )),
      std::is_nothrow_copy_constructible<TypeParam>::value
  );
  STATIC_ASSERT_FALSE(noexcept(TestFixture::vector::try_construct(
      std::declval<arene::base::span<arene::base::byte>>(),
      std::declval<nothrow_it_throwing_type>(),
      std::declval<nothrow_it_throwing_type>()
  )));
}

/// @test Dereferencing a default-constructed `external_vector` iterator terminates the program with a precondition
/// violation
TYPED_TEST_P(ExternalVectorDeathTest, DereferencingInvalidIteratorIsPreconditionViolation) {
  ASSERT_DEATH(std::ignore = *typename TestFixture::vector::iterator{}, "Precondition violation");
  ASSERT_DEATH(std::ignore = *typename TestFixture::vector::const_iterator{}, "Precondition violation");
}

/// @test Iterator arithmetic on a default-constructed `external_vector` iterator terminates the program with a
/// precondition violation
TYPED_TEST_P(ExternalVectorDeathTest, AddingToInvalidIteratorIsPreconditionViolation) {
  ASSERT_DEATH(std::ignore = typename TestFixture::vector::iterator{} + 1, "Precondition violation");
  ASSERT_DEATH(std::ignore = typename TestFixture::vector::const_iterator{} + 1, "Precondition violation");
}

ARENE_IGNORE_END();

REGISTER_TYPED_TEST_SUITE_P(
    ExternalVectorTest,
    ADLSwapSwapsContentsForBasicTypes,
    AfterClearVectorIsEmpty,
    AfterPushBackCanGetLastElement,
    AfterPushBackVectorIsNotEmpty,
    AssignFromInitListUsingAssignFunctionIsConditionallyNoexcept,
    AssignFromSizeAndValueIsConditionallyNoexcept,
    AtIndexSizeOrLargerThrows,
    AtIndexSizeOrLargerThrowsWithConstVec,
    AtOnConstVectorReturnsConstReference,
    AtReturnsReference,
    BackIsNoexcept,
    BackReturnsReference,
    CanAccessMultipleValuesWithAt,
    CanAccessMultipleValuesWithAtWithConstVec,
    CanAccessMultipleValuesWithIndexOperator,
    CanAssignFromBiggerCapacityVector,
    CanAssignFromInitListUsingAssignFunction,
    CanAssignFromInputIteratorRange,
    CanAssignFromIteratorRange,
    CanAssignFromSizeAndValue,
    CanCompareForOrdering,
    CanConstructAnEmptyVector,
    CanConstructFromInitializerList,
    CanConstructFromIteratorRange,
    CanConstructWithSize,
    CanConstructWithSizeAndValue,
    CanConstructWithSizeViaTryConstruct,
    CanCopyViaConstruct,
    CanCopyViaConstructFromLargerWithFewerElements,
    CanCopyViaConstructFromSmaller,
    CanCopyViaTryConstruct,
    CanCopyViaTryConstructFromLargerWithFewerElements,
    CanCopyViaTryConstructFromSmaller,
    CanDefaultConstructViaTryConstruct,
    CanEmplaceBack,
    CanEraseAtBeginning,
    CanEraseInMiddle,
    CanEraseRange,
    CanGetCBeginAndEnd,
    CanGetFront,
    CanGetIterator,
    CanInsertAtEndOfExistingVector,
    CanInsertInEmptyVector,
    CanInsertInMiddleOfExistingVector,
    CanIterateOverConstValues,
    CanIterateOverValues,
    CanMoveViaConstruct,
    CanMoveViaTryConstruct,
    CanPopBack,
    CanPushBackAValue,
    CanTryConstructFromInitializerList,
    CanTryConstructFromIteratorRange,
    CannotConstructWithSizeForANonDefaultConstructibleType,
    CannotTryConstructWithSizeForANonDefaultConstructibleType,
    CapacityIsAsSpecified,
    CapacityIsNoexcept,
    ConstBackReturnsConstReference,
    ConstructWithExcessiveSizeViaTryConstructFails,
    ConstructWithSizeViaNormalConstructWithDefaultConstructorNotNoexceptIsNotNoexcept,
    ConstructWithSizeViaTryConstructNotNoexceptIfDefaultConstructorNotNoexcept,
    CopyAssignOverLargerVectorDestroysExcess,
    CopyAssignOverLargerVectorFromInitListDestroysExcess,
    CopyAssignOverSmallerVectorCopiesElements,
    CopyAssignOverSmallerVectorFromInitListCopiesElements,
    CopyAssignmentIsConditionallyNoexcept,
    CopyingVectorCopiesElements,
    Data,
    DefaultEmplaceIsConditionallyNoexcept,
    DefaultTryConstructNoexceptEvenIfDefaultConstructorNotNoexcept,
    EmplaceAtPositionIsConditionallyNoexcept,
    EmplaceBackIsConditionallyNoexcept,
    EmplaceInsert,
    EmptyIfJustStorage,
    EmptyIsNoexcept,
    EmptybyDefault,
    EraseIsNoexceptIfTypeHasNoexceptMove,
    FailToCopyViaTryConstructFromLargerWithMoreElements,
    FailToMoveViaTryConstructFromLargerWithMoreElements,
    IndexOperatorIsNoexcept,
    IndexOperatorOnConstVectorIsNoexcept,
    IndexOperatorOnConstVectorReturnsConstReference,
    IndexOperatorReturnsReference,
    InitialSizeIsZero,
    InputIteratorRangeAssignmentIsConditionallyNoexcept,
    InputIteratorRangeConstructionIsConditionallyNoexcept,
    InputIteratorRangeTryConstructionIsConditionallyNoexcept,
    InsertFromIteratorRangeInMiddle,
    InsertFromIteratorRangeIsConditionallyNoexcept,
    InsertIsConditionallyNoexcept,
    InsertNAtEndInsertsElements,
    InsertNInMiddleInsertsElements,
    InsertWithCountIsConditionallyNoexcept,
    IteratorRangeAssignmentIsConditionallyNoexcept,
    IteratorRangeConstructionIsConditionallyNoexcept,
    IteratorRangeTryConstructionIsConditionallyNoexcept,
    IteratorTypedefs,
    MaxSizeEqualsCapacity,
    MoveAssignOverLargerVectorDestroysExcess,
    MoveAssignOverSmallerVectorMovesElements,
    MoveAssignmentIsConditionallyNoexcept,
    MovingVectorMovesElements,
    MovingVectorWithNewStorageMovesElements,
    NoStorageDataIsNull,
    NotConstructibleWithSizeAndSourceIfDataNotCopyable,
    NotCopyConstructible,
    NotCopyConstructibleIfDataTypeNotCopyConstructible,
    NotMoveConstructibleIfDataTypeNotMoveConstructible,
    PopBackIsNoexcept,
    PushBackIsConditionallyNoexcept,
    ResizeIsConditionallyNoexcept,
    ResizeWithValueIsConditionallyNoexcept,
    ResizingFromLargeToSmallLeavesElements,
    ResizingFromSmallToLargeConstructsElementsAsCopiesOfSpecified,
    RetrievingAValueWithAtReturnsValueAfterPushBack,
    RetrievingAValueWithAtThrowsOnEmpty,
    ReverseIteration,
    SecondPushBackChangesLastValue,
    SizeIsCorrect,
    SizeIsNoexcept,
    StorageOnlyTryConstructNoexceptEvenIfDefaultConstructorNotNoexcept,
    SwapNoexceptMatchesElementType,
    TryConstructFromTooLargeInitializerListReturnsEmpty,
    Typedefs,
    ZeroSizeVectorIsEmpty
);

REGISTER_TYPED_TEST_SUITE_P(
    ExternalVectorDeathTest,
    AddingToInvalidIteratorIsPreconditionViolation,
    AssignIsPreconditionViolationIfSizeOverCapacity,
    ConstructWithExcessiveSizeViaNormalConstructIsAlwaysPreconditionViolation,
    ConstructingWithOutOfRangeSizeAndSourceIsPreconditionViolation,
    ConstructingWithOutOfRangeSizeIsPreconditionViolation,
    ConstructingWithTooManyInitializersIsPreconditionViolation,
    DereferencingInvalidIteratorIsPreconditionViolation,
    EmplaceBackIsPreconditionViolationIfFull,
    IndexOperatorOutOfRange,
    InsertNIsPreconditionViolationIfTooManyItems,
    InsertWhenAtCapacityIsPreconditionViolation,
    PushBackBeyondMaxSizeIsPreconditionViolation,
    PushBackOnZeroSizeVectorIsPreconditionViolation
);

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_EXTERNAL_VECTOR_HPP_
