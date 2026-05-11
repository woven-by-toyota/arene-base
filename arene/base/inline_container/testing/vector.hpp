// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_VECTOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_VECTOR_HPP_

#include <gmock/gmock.h>

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/inline_container/testing/customization.hpp"
#include "arene/base/inline_container/testing/default_constructible_properties.hpp"
#include "arene/base/inline_container/testing/unique_test_value.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/optional/optional_resetter.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/move_iterator.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/reference_wrapper.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/utility/swap.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace arene {
namespace base {
namespace testing {

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

template <class T>
struct equal_to_value {
  constexpr explicit equal_to_value(T const& input)
      : value{input} {}
  constexpr auto operator()(T const& arg) -> bool { return arg == value; }

  T const& value;
};

/// @brief Check if all element of @c range are equal to @c value
/// @tparam Range The type of the range to check
/// @tparam T The type of the value to compare against
/// @param range The range to check
/// @param value The value to compare against
///
/// This helper is an alternative to GTest matchers @c Each(Eq(value)), which require the value to be copy
/// constructible.
template <class Range, class T>
constexpr auto all_elements_are(Range const& range, T const& value) -> bool {
  return arene::base::all_of(range.begin(), range.end(), equal_to_value<T>{value});
}

/// @brief Check if all element of @c range are equal to the elements in @c values
/// @tparam Range1 The type of the range to check
/// @tparam Range2 The type of the value to compare against
/// @param range The range to check
/// @param values The values to compare against
///
/// This helper is an alternative to GTest matcher @c ElementsAreArray, which require the values to be copy
/// constructible.
template <class Range1, class Range2>
constexpr auto elements_are_array(Range1 const& range, Range2 const& values) -> bool {
  return arene::base::equal(range.begin(), range.end(), values.begin());
}

/// @brief Create a test vector containing a range of test values
/// @tparam T The type to contain in the test vector
/// @tparam Capacity The capacity of inline_vector to populate (note: this is not the size / number of actual entries)
/// @param begin The first index of test value to put in the vector
/// @param end The after-end index of test value to put in the vector
/// @return An inline_vector<T, Capacity> containing [test_value(begin), test_value(end))
template <typename T, std::size_t Capacity>
constexpr auto test_vector(std::size_t begin, std::size_t end) noexcept(std::is_nothrow_copy_constructible<T>::value)
    -> ::arene::base::inline_vector<T, Capacity> {
  ::arene::base::inline_vector<T, Capacity> vec;
  for (std::size_t ii = begin; ii < end; ++ii) {
    vec.push_back(test_value<T>(ii));
  }
  return vec;
}

/// @brief Push back multiple values into a vector
/// @tparam Vector The type of the vector
/// @tparam Values The type of the values
/// @param vec The vector to push back into
/// @param values the values to push back
template <class Vector, class... Values>
constexpr auto vector_push_back(Vector& vec, Values&&... values) -> void {
  arene::base::consume_values({0, (vec.push_back(std::forward<Values>(values)), 0)...});
}

/// @brief sorts the container and then returns it
/// @tparam Container The type of a container
/// @param container A container to be sorted
/// @return @c container with sorted elements
///
/// @note @c std::sort is not constexpr in C++14
/// @note use of @c arene::base::sort introduces coverage misses
///
template <typename Container>
constexpr auto sort_helper(Container container) noexcept -> Container {
  // TODO
  // The use of 'arene::base::sort' introduces coverage misses due to newly
  // introduced instatiations that aren't fully exercised.
  bool swapped{true};
  while (swapped) {
    swapped = false;
    for (std::size_t ii = 0UL; ii + 1UL < container.size(); ++ii) {
      if (container[ii + 1] < container[ii]) {
        arene::base::swap(container[ii], container[ii + 1]);
        swapped = true;
      }
    }
  }
  return container;
}

/// @brief True if a type is copy constructible and copy assignable, false otherwise
/// @param T the type to check
template <class T>
constexpr auto is_copyable_v = std::is_copy_assignable<T>::value && std::is_copy_constructible<T>::value;

/////////////////////////
// Parameterized tests //
/////////////////////////

/// @brief Test fixture for all type-parameterized @c inline_vector tests
/// @tparam T The type parameter currently being used for tests; filled in by Google Test as @c TypeParam
template <typename Vector>
class InlineVectorTest : public ::testing::Test {
 public:
  using T = typename Vector::value_type;

  /// @brief Whether or not the current type parameter is constexpr compatible for this test
  static constexpr bool constexpr_compatible =
      std::is_trivially_default_constructible<T>::value && std::is_trivially_move_assignable<T>::value;
  static_assert(
      !constexpr_compatible || has_constexpr_test_value<T>,
      "Constexpr-compatible types must have constexpr specializations of test_value_array or test_value"
  );

  /// @brief The base capacity to use for most @c inline_vector instantiations in the parameterized tests
  static constexpr std::size_t capacity = Vector::capacity;
  static_assert(capacity > 0UL, "Parameterized capacity can not be 0 (0-capacity vectors are always tested)");

 protected:
  static void SetUpTestSuite() { ::arene::base::testing::assert_default_constructible_properties<Vector>(); }

  /// @brief An @c inline_vector with @c capacity using any given type; useful in tests making vectors of wrapper types
  /// @tparam ValueType The element type of this particular vector
  template <typename ValueType>
  using standard_capacity_vector = inline_vector<ValueType, capacity>;
  /// @brief An @c inline_vector with @c value_type=T using any given capacity; useful in tests doing assignment
  /// @tparam OtherCapacity The capacity of this particular vector
  template <std::size_t OtherCapacity>
  using vector_with_capacity = inline_vector<T, OtherCapacity>;

  /// @brief A helper template to get the type resulting from a @c try_construct call
  /// @tparam Vec An @c inline_vector type which we will try to construct
  /// @tparam Args The arguments with which we will try to construct @c Vec
  template <typename Vec, typename... Args>
  using try_construct_result = decltype(Vec::try_construct(std::declval<Args>()...));

  /// @brief A type with a @c noexcept(false) conversion operator to @c T (doesn't actually throw, just declares it)
  struct throws_when_converted_to_t {
    /// @brief Implicit conversion operator to @c T which declares that it might throw, though it doesn't actually
    /// @return The 0th test value of @c T
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator T() const noexcept(false) { return ::arene::base::testing::test_value<T>(0); }
  };

  /// @brief A bidirectional iterator of @c T which declares that it could throw, used in some @c noexcept tests
  struct throwing_bidirectional_move_iterator_of_t {
    /// @brief The category of this iterator
    using iterator_category = std::bidirectional_iterator_tag;
    /// @brief The type used for differences in this iterator
    using difference_type = std::ptrdiff_t;
    /// @brief The type returned by dereferencing this iterator
    using reference = T&&;
    /// @brief The value type pointed to by this iterator
    using value_type = T;
    /// @brief This iterator as a pointer (disabled)
    using pointer = void;

    /// @brief Dereference operator; never defined
    auto operator*() const -> reference;

    /// @brief Pre-increment operator; never defined
    auto operator++() -> throwing_bidirectional_move_iterator_of_t&;
    /// @brief Post-increment operator; never defined
    auto operator++(int) -> throwing_bidirectional_move_iterator_of_t;
    /// @brief Pre-decrement operator; never defined
    auto operator--() -> throwing_bidirectional_move_iterator_of_t&;
    /// @brief Post-decrement operator; never defined
    auto operator--(int) -> throwing_bidirectional_move_iterator_of_t;
    /// @brief Equality operator; never defined
    auto operator==(throwing_bidirectional_move_iterator_of_t const&) -> bool;
    /// @brief Inequality operator; never defined
    auto operator!=(throwing_bidirectional_move_iterator_of_t const&) -> bool;
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
    auto operator*() const noexcept(OperatorsNoexcept) -> reference;

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

  /// @brief Construct a @c Vector from the passed parameters; allows calling as this->construct, which can be shorter
  /// @tparam U The types of the arguments to the constructor
  /// @param params The parameters to the constructor
  template <typename... U>
  static constexpr auto construct(U&&... params) noexcept(noexcept(Vector(std::forward<U>(params)...))) -> Vector {
    return Vector(std::forward<U>(params)...);
  }

  /// @brief Return the @c idx 'th test value of the current @c T , parameterized by test suite users
  /// @param idx The index of the test value to get
  /// @return The @c idx 'th test value of the current @c T
  static constexpr auto test_value(std::size_t idx) noexcept(noexcept(::arene::base::testing::test_value<T>(idx)))
      -> decltype(::arene::base::testing::test_value<T>(idx)) {
    return ::arene::base::testing::test_value<T>(idx);
  }

  /// @brief Return the @c N 'th test value of the current @c T , parameterized by test suite users
  /// @tparam N The index of the test value to get
  /// @return The @c N 'th test value of the current @c T
  /// @note This requires <tt> test_value(0), ..., test_value(N) </tt> are unique values
  template <std::size_t N>
  static constexpr auto unique_test_value() noexcept(noexcept(::arene::base::testing::unique_test_value<T, N>()))
      -> decltype(::arene::base::testing::unique_test_value<T, N>()) {
    return ::arene::base::testing::unique_test_value<T, N>();
  }

  /// @brief Return a @c Vector containing the test values @c [begin,end) of @c T , parameterized by test suite users
  /// @param begin The index of the first test value to put into the test vector
  /// @param end The index after that of the last test value to put into the test vector
  /// @return A test Vector holding the test values determined by the indices
  static constexpr auto test_vector(std::size_t begin, std::size_t end) noexcept(
      noexcept(::arene::base::testing::test_vector<T, capacity>(begin, end))
  ) -> Vector {
    return ::arene::base::testing::test_vector<T, capacity>(begin, end);
  }

  /// @brief Return a @c Vector containing the test values @c [0,size) of @c T , parameterized by test suite users
  /// @param size The number of test values to put into the test vector
  /// @return A test vector holding the test values determined by the size
  static constexpr auto test_vector(std::size_t size) noexcept(noexcept(test_vector(0, size))) -> Vector {
    return test_vector(0, size);
  }

  /// @brief Return a @c Vector containing the test values @c [0,capacity) of @c T , parameterized by test suite users
  /// @return A test vector holding the test values determined by the capacity
  static constexpr auto full_test_vector() noexcept(noexcept(test_vector(capacity))) -> Vector {
    return test_vector(capacity);
  }

  /// @brief Return a @c std::initializer_list containing user-parameterized @c test_value s
  /// @tparam Indices A pack of indices, one for each test value in the initializer list
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing one test value per index
  template <std::size_t... Indices>
  static constexpr auto test_initializer_list(std::index_sequence<Indices...>) noexcept -> std::initializer_list<T> {
    return ::arene::base::testing::test_initializer_list<T, Indices...>();
  }

  /// @brief Return a @c std::initializer_list containing user-parameterized @c test_value s
  /// @tparam Size The number of values in the @c std::initializer_list
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing one test value per index
  template <std::size_t Size>
  static constexpr auto test_initializer_list() noexcept -> std::initializer_list<T> {
    return test_initializer_list(std::make_index_sequence<Size>{});
  }

  /// @brief Return a @c std::initializer_list containing @c capacity user-parameterized @c test_value s
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing @c capacity entries
  static constexpr auto full_test_initializer_list() noexcept -> std::initializer_list<T> {
    return test_initializer_list<capacity>();
  }
};

/// @brief The Death Tests use the same fixture as the "normal" tests. Inherits so that it will be a distinct type.
/// @tparam T The type parameter of a given test instantiation
template <typename T>
class InlineVectorDeathTest : public InlineVectorTest<T> {};

// Declare the three test suites for Google Test; test cases will all be added to one of these.
TYPED_TEST_SUITE_P(InlineVectorTest);
TYPED_TEST_SUITE_P(InlineVectorDeathTest);

/// @test A default-constructed `inline_vector` is empty
TYPED_TEST_P(InlineVectorTest, CanConstructAnEmptyVector) { ASSERT_TRUE(TestFixture::construct().empty()); }

/// @test The capacity of a default-constructed `inline_vector` is equal to the supplied template parameter
TYPED_TEST_P(InlineVectorTest, CapacityIsAsSpecified) {
  COND_STATIC_ASSERT_EQ(TestFixture::construct().capacity(), TypeParam::capacity);
}

/// @test The `capacity` member function of `inline_vector` is declared `noexcept`
TYPED_TEST_P(InlineVectorTest, CapacityIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().capacity()));
}

/// @test The size of a default-constructed `inline_vector` is zero
TYPED_TEST_P(InlineVectorTest, InitialSizeIsZero) { EXPECT_EQ(TestFixture::construct().size(), 0); }

/// @test The size of an `inline_vector` initialized from a list of values is correct in a `constexpr` context
TYPED_TEST_P(InlineVectorTest, SizeIsConstexpr) {
  COND_STATIC_ASSERT_EQ(TestFixture::full_test_vector().size(), TypeParam::capacity);
}

/// @test The `size` member function of `inline_vector` is declared `noexcept`
TYPED_TEST_P(InlineVectorTest, SizeIsNoexcept) { STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().size())); }

/// @test Using `push_back` to add an item to a default-constructed `inline_vector` increases the size
TYPED_TEST_P(InlineVectorTest, CanPushBackAValue) {
  TypeParam vec;
  vec.push_back(TestFixture::test_value(0));
  EXPECT_EQ(vec.size(), 1);
}

/// @test After using `push_back` to add an item to a default-constructed `inline_vector`, that item  can be retrieved
/// via the `back` member function
TYPED_TEST_P(InlineVectorTest, AfterPushBackCanGetLastElement) {
  TypeParam vec;
  vec.push_back(TestFixture::test_value(0));
  EXPECT_EQ(vec.back(), TestFixture::test_value(0));
}

/// @test After using `push_back` to add an item to a default-constructed `inline_vector`, the vector is no longer empty
TYPED_TEST_P(InlineVectorTest, AfterPushBackVectorIsNotEmpty) {
  TypeParam vec;
  vec.push_back(TestFixture::test_value(0));
  ASSERT_FALSE(vec.empty());
}

/// @test After using `push_back` twice to add two items to a default-constructed `inline_vector`, the size is
/// incremented to 2, and the second item can be retrieved via the `back` member function
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, SecondPushBackChangesLastValue, TypeParam::capacity >= 2) {
  TypeParam vec;
  vec.push_back(TestFixture::test_value(0));
  vec.push_back(TestFixture::test_value(1));
  EXPECT_EQ(vec.size(), 2);
  EXPECT_EQ(vec.back(), TestFixture::test_value(1));
}

/// @test `push_back` of an rvalue is `noexcept` if and only if the value type can be constructed without throwing from
/// an rvalue
TYPED_TEST_P(InlineVectorTest, PushBackIsConditionallyNoexceptRvalue) {
  constexpr bool move_noexcept = std::is_nothrow_move_constructible<typename TypeParam::value_type>{};
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().push_back(std::declval<typename TypeParam::value_type&&>())),
      move_noexcept
  );
}

/// @test `push_back` of an lvalue is `noexcept` if and only if the value type can be constructed without throwing from
/// an lvalue
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    PushBackIsConditionallyNoexceptLvalue,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  constexpr bool copy_noexcept = std::is_nothrow_copy_constructible<typename TypeParam::value_type>{};
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().push_back(std::declval<typename TypeParam::value_type const&>())),
      copy_noexcept
  );
}

/// @test A default-constructed `inline_vector` is empty in a constexpr context
TYPED_TEST_P(InlineVectorTest, EmptyIsConstexpr) { COND_STATIC_ASSERT_TRUE(TestFixture::construct().empty()); }

/// @test The `empty` member function of `inline_vector` is `noexcept`
TYPED_TEST_P(InlineVectorTest, EmptyIsNoexcept) { STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().empty())); }

/// @test The `at` member function throws a `std::out_of_range` exception when invoked with an index of 0 on
/// a default-constructed `inline_vector`
TYPED_TEST_P(InlineVectorTest, RetrievingAValueWithAtThrowsOnEmpty) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  ASSERT_THROW(TestFixture::construct().at(0), std::out_of_range);
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal
}

/// @test The `at` member function returns the value stored when invoked with an index of 0 on  a
/// `inline_vector` into which one element has been stored via `push_back`
TYPED_TEST_P(InlineVectorTest, RetrievingAValueWithAtReturnsValueAfterPushBack) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct();
  vec.push_back(TestFixture::test_value(0));
  EXPECT_EQ(vec[0], TestFixture::test_value(0));
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal
}

/// @test The return type of the `at` member function on a non-`const` `inline_vector` is a non-`const` reference to
/// `value_type`
TYPED_TEST_P(InlineVectorTest, AtReturnsReference) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().at(0)), typename TypeParam::value_type&>();
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal
}

/// @test The return type of the `at` member function on a `const` `inline_vector` is a  `const` reference to
/// `value_type`
TYPED_TEST_P(InlineVectorTest, AtOnConstVectorReturnsConstReference) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().at(0)), typename TypeParam::value_type const&>();
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal
}

/// @test The `at` member function returns the corresponding value stored when invoked with an index of 0 or
/// 1 on an `inline_vector` into which two elements have been stored via `push_back`
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanAccessMultipleValuesWithAt, TypeParam::capacity >= 2) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct();
  vec.push_back(TestFixture::test_value(0));
  vec.push_back(TestFixture::test_value(1));

  EXPECT_EQ(vec.at(0), TestFixture::test_value(0));
  EXPECT_EQ(vec.at(1), TestFixture::test_value(1));
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal
}

/// @test The `at` member function throws when invoked with an index that is equal to or larger than the size of the
/// vector on an `inline_vector` into which two elements have been stored via `push_back`
TYPED_TEST_P(InlineVectorTest, AtIndexSizeOrLargerThrows) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct();
  vec.push_back(TestFixture::test_value(0));

  ASSERT_THROW(vec.at(vec.size()), std::out_of_range);
  ASSERT_THROW(vec.at(vec.size() * 2), std::out_of_range);
  ASSERT_THROW(vec.at(std::numeric_limits<std::size_t>::max()), std::out_of_range);
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal
}

/// @test The `at` member function returns the corresponding value stored when invoked via a `const` reference to a
/// vector with an index of 0 or 1 on an `inline_vector` into which two elements have been stored via `push_back`.
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanAccessMultipleValuesWithAtWithConstVec, TypeParam::capacity >= 2) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct();
  vec.push_back(TestFixture::test_value(0));
  vec.push_back(TestFixture::test_value(1));

  auto const& const_vec = vec;
  EXPECT_EQ(const_vec.at(0), TestFixture::test_value(0));
  EXPECT_EQ(const_vec.at(1), TestFixture::test_value(1));
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal
}

/// @test The `at` member function throws when invoked via a `const` reference to a vector with an index that is equal
/// to or larger than the size of an `inline_vector` into which two elements have been stored via `push_back`
TYPED_TEST_P(InlineVectorTest, AtIndexSizeOrLargerThrowsWithConstVec) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // conditional removal of at() tests based on exception state.
  auto vec = TestFixture::construct();
  vec.push_back(TestFixture::test_value(0));

  auto const& const_vec = vec;

  ASSERT_THROW(const_vec.at(vec.size()), std::out_of_range);
  ASSERT_THROW(const_vec.at(vec.size() * 2), std::out_of_range);
  ASSERT_THROW(const_vec.at(std::numeric_limits<std::size_t>::max()), std::out_of_range);
#else
  GTEST_SKIP() << "`at` tests skipped because exceptions are disabled";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) conditional removal of at() tests based on exception state.
}

/// @test The `max_size` member function of `inline_vector` returns the capacity specified as a template parameter.
TYPED_TEST_P(InlineVectorTest, MaxSizeEqualsCapacity) {
  COND_STATIC_ASSERT_EQ(TestFixture::construct().max_size(), TypeParam::capacity);
  COND_STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().max_size()));
}

/// @test Invoking `push_back` on a vector that is at capacity terminates the program with a precondition violation.
TYPED_TEST_P(InlineVectorDeathTest, PushBackBeyondMaxSizeIsPreconditionViolation) {
  auto vec = TestFixture::construct();
  for (std::size_t i = 0; i < vec.max_size(); ++i) {
    vec.push_back(TestFixture::test_value(0));
  }
  ASSERT_DEATH(vec.push_back(TestFixture::test_value(0)), "Precondition violation");
}

/// @test The size of a default-constructed vector with a capacity of zero is zero, when obtained via the
/// `size` member function, and via comparing the iterators returned from `begin` and `end`
TYPED_TEST_P(InlineVectorTest, ZeroSizeVectorIsEmpty) {
  inline_vector<typename TypeParam::value_type, 0> const zvec;
  inline_vector<typename TypeParam::value_type const, 0> const czvec;
  ASSERT_EQ(zvec.begin(), zvec.end());
  ASSERT_EQ(czvec.begin(), czvec.end());
  ASSERT_EQ(zvec.size(), 0);
  ASSERT_EQ(czvec.size(), 0);
}

/// @test Invoking `push_back` on a zero-capacity vector terminates the program with a precondition violation
TYPED_TEST_P(InlineVectorDeathTest, PushBackOnZeroSizeVectorIsPreconditionViolation) {
  inline_vector<typename TypeParam::value_type, 0> vec{};
  ASSERT_DEATH(vec.push_back(TestFixture::test_value(0)), "Precondition violation");
}

/// @test Given a default-constructed `inline_vector`, after one element has been added via `push_back`, `back` returns
/// a reference to that element. After a second element has been added via `push_back`, `back` now returns a reference
/// to the second element.
TYPED_TEST_P(InlineVectorTest, BackReturnsReference) {
  auto vec = TestFixture::construct();

  vec.push_back(TestFixture::test_value(0));
  typename TypeParam::value_type& val1 = vec.back();
  EXPECT_EQ(std::addressof(val1), std::addressof(vec[0]));

  if (TypeParam::capacity >= 2UL) {
    vec.push_back(TestFixture::test_value(1));
    typename TypeParam::value_type& val2 = vec.back();
    EXPECT_EQ(std::addressof(val2), std::addressof(vec[1]));
  }
}

/// @test Given a default-constructed `inline_vector`, after one element has been added via `push_back`, `back` called
/// via a `const` reference to the vector returns a `const` reference to that element. After a second element has been
/// added via `push_back`, `back` called via a `const` reference to the vector now returns a `const` reference to the
/// second element.
TYPED_TEST_P(InlineVectorTest, ConstBackReturnsConstReference) {
  auto vec = TestFixture::construct();
  auto const& const_vec = vec;

  vec.push_back(TestFixture::test_value(0));
  auto& val1 = const_vec.back();
  ::testing::StaticAssertTypeEq<decltype(val1), typename TypeParam::value_type const&>();
  EXPECT_EQ(std::addressof(val1), &const_vec[0]);

  if (TypeParam::capacity >= 2UL) {
    vec.push_back(TestFixture::test_value(1));
    auto& val2 = const_vec.back();
    ::testing::StaticAssertTypeEq<decltype(val2), typename TypeParam::value_type const&>();
    EXPECT_EQ(std::addressof(val2), &const_vec[1]);
  }
}

/// @test Invoking `back` cannot throw
TYPED_TEST_P(InlineVectorTest, BackIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().back()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().back()));
}

/// @test In a `constexpr` context, an `inline_vector` can be default-constructed, and a new value inserted via
/// `push_back`, such that the `size` of the container is now 1, and the value can be retrieved via `at(0)`
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanPushBack, InlineVectorTest<TypeParam>::constexpr_compatible) {
  constexpr auto vec = TestFixture::test_vector(1);
  STATIC_ASSERT_EQ(vec.size(), 1);
  STATIC_ASSERT_EQ(vec[0], TestFixture::test_value(0));
}

/// @test An `inline_vector` can be constructed from a `std::initializer_list` without throwing exceptions, and the size
/// and values stored in the vector are the same as the size and values of the initializer list.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanConstructFromInitializerList,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const values = TestFixture::full_test_initializer_list();

  TypeParam const vec{values};
  STATIC_ASSERT_EQ(noexcept(TypeParam{values}), std::is_nothrow_copy_constructible<typename TypeParam::value_type>{});

  EXPECT_THAT(vec, ::testing::ElementsAreArray(values));
}

/// @test An `inline_vector` can be constructed from a `std::initializer_list` in a `constexpr` context, and the
/// elements are those from the initializer list
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanConstructFromInitializerListConstexpr,
    InlineVectorTest<TypeParam>::constexpr_compatible&&
        std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  constexpr auto values = TestFixture::full_test_initializer_list();

  constexpr TypeParam vec{values};

  EXPECT_THAT(vec, ::testing::ElementsAreArray(values));
}

template <typename Vec>
constexpr auto constexpr_non_const_back(Vec&& vec) -> typename Vec::value_type {
  static_assert(std::is_rvalue_reference<Vec&&>::value, "Only expected to be called with an r-value reference.");
  return std::forward<Vec>(vec).back();
}

/// @test Given a `constexpr` `inline_vector` constructed by pushing back values, `back` can be used in a `constexpr`
/// context to retrieve the last value in the vector, which must therefore equal the last value pushed back
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanUseBack, InlineVectorTest<TypeParam>::constexpr_compatible) {
  constexpr auto vec = TestFixture::full_test_vector();
  STATIC_ASSERT_EQ(vec.back(), TestFixture::test_value(TypeParam::capacity - 1UL));
  STATIC_ASSERT_EQ(
      constexpr_non_const_back(TestFixture::full_test_vector()),
      TestFixture::test_value(TypeParam::capacity - 1UL)
  );
}

/// @test Given an `inline_vector` initialized by pushing back values, the `front` member function can be used to
/// retrieve the first value, which must be equal to the first value of the vector as retrieved via `at(0)`
TYPED_TEST_P(InlineVectorTest, CanGetFront) {
  auto vec = TestFixture::full_test_vector();
  EXPECT_EQ(std::addressof(vec.front()), std::addressof(vec[0]));
  ::testing::StaticAssertTypeEq<decltype(vec.front()), typename TypeParam::value_type&>();
  STATIC_ASSERT_TRUE(noexcept(vec.front()));
  auto const& const_vec = vec;
  EXPECT_EQ(&const_vec.front(), &const_vec[0]);
  ::testing::StaticAssertTypeEq<decltype(const_vec.front()), typename TypeParam::value_type const&>();
  STATIC_ASSERT_TRUE(noexcept(const_vec.front()));
}

template <typename Vec>
constexpr auto constexpr_non_const_front(Vec&& vec) -> typename Vec::value_type {
  static_assert(std::is_rvalue_reference<Vec&&>::value, "Only expected to be called with an r-value reference.");
  return std::forward<Vec>(vec).front();
}

/// @test Given an instance of `inline_vector` by pushing back values in a `constexpr` context, the
/// `front` member function can be used in a `constexpr` context to retrieve the first value, which must be equal to the
/// first value of the pushed back
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, Front, InlineVectorTest<TypeParam>::constexpr_compatible) {
  constexpr auto vec = TestFixture::full_test_vector();
  STATIC_ASSERT_EQ(vec.front(), TestFixture::test_value(0));
  STATIC_ASSERT_EQ(constexpr_non_const_front(TestFixture::full_test_vector()), TestFixture::test_value(0));
}

/// @test The `begin` and `end` member functions of an `inline_vector` must return an `iterator`, and they must be equal
/// for a default-constructed vector
TYPED_TEST_P(InlineVectorTest, CanGetIterator) {
  auto vec = TestFixture::construct();
  ::testing::StaticAssertTypeEq<decltype(vec.begin()), typename TypeParam::iterator>();
  ::testing::StaticAssertTypeEq<decltype(vec.end()), typename TypeParam::iterator>();
  STATIC_ASSERT_TRUE(noexcept(vec.begin()));
  STATIC_ASSERT_TRUE(noexcept(vec.end()));
  EXPECT_EQ(vec.begin(), vec.end());
}

/// @test Given an `inline_vector` constructed by pushing back values, the iterators returned from `begin` and `end`
/// can be passed as an iterator pair to `arene::base::equal` to compare the values in the vector to those in the
/// initializer list, which must return @c true, to indicate that the values are the same.
TYPED_TEST_P(InlineVectorTest, CanIterateOverValues) {
  auto const values = TestFixture::full_test_initializer_list();
  auto const vec = TestFixture::full_test_vector();
  ASSERT_TRUE(arene::base::equal(values.begin(), values.end(), vec.begin(), vec.end()));
}

/// @test Given a `const` `inline_vector` constructed by pushing back values, the iterators returned from `begin` and
/// `end` can be passed as an iterator pair to `arene::base::equal` to compare the values in the vector to those in the
/// initializer list, which must return @c true, to indicate that the values are the same.
TYPED_TEST_P(InlineVectorTest, CanIterateOverConstValues) {
  auto const values = TestFixture::full_test_initializer_list();
  auto const vec = TestFixture::full_test_vector();
  ASSERT_TRUE(arene::base::equal(values.begin(), values.end(), vec.begin(), vec.end()));
}

/// @test The `iterator` type of an `inline_vector` must have the required type aliases for a random-access iterator.
TYPED_TEST_P(InlineVectorTest, IteratorTypedefs) {
  using value_type = typename TypeParam::value_type;
  ::testing::StaticAssertTypeEq<typename TypeParam::iterator::value_type, value_type>();
  ::testing::StaticAssertTypeEq<typename TypeParam::iterator::reference, value_type&>();
  ::testing::StaticAssertTypeEq<typename TypeParam::iterator::pointer, value_type*>();
  ::testing::StaticAssertTypeEq<typename TypeParam::iterator::difference_type, std::ptrdiff_t>();
  ::testing::StaticAssertTypeEq<typename TypeParam::iterator::iterator_category, std::random_access_iterator_tag>();

  ::testing::StaticAssertTypeEq<typename TypeParam::const_iterator::value_type, value_type>();
  ::testing::StaticAssertTypeEq<typename TypeParam::const_iterator::reference, value_type const&>();
  ::testing::StaticAssertTypeEq<typename TypeParam::const_iterator::pointer, value_type const*>();
  ::testing::StaticAssertTypeEq<typename TypeParam::const_iterator::difference_type, std::ptrdiff_t>();
  ::testing::StaticAssertTypeEq<typename TypeParam::const_iterator::iterator_category, std::random_access_iterator_tag>(
  );
}

/// @test `inline_vector` must have the required type aliases for a sequence container
TYPED_TEST_P(InlineVectorTest, Typedefs) {
  using value_type = typename TypeParam::value_type;
  ::testing::StaticAssertTypeEq<typename TypeParam::value_type, value_type>();
  ::testing::StaticAssertTypeEq<typename TypeParam::pointer, value_type*>();
  ::testing::StaticAssertTypeEq<typename TypeParam::const_pointer, value_type const*>();
  ::testing::StaticAssertTypeEq<typename TypeParam::reference, value_type&>();
  ::testing::StaticAssertTypeEq<typename TypeParam::const_reference, value_type const&>();
  ::testing::StaticAssertTypeEq<typename TypeParam::size_type, std::size_t>();
  ::testing::StaticAssertTypeEq<typename TypeParam::difference_type, std::ptrdiff_t>();
  ::testing::StaticAssertTypeEq<
      typename TypeParam::reverse_iterator,
      ::arene::base::reverse_iterator<typename TypeParam::iterator>>();
  ::testing::StaticAssertTypeEq<
      typename TypeParam::const_reverse_iterator,
      ::arene::base::reverse_iterator<typename TypeParam::const_iterator>>();
}

/// @brief Check that iteration works when taking a vector with the given type
/// @tparam Vec The type to take the vector as; intended to be a value type either with const or without
/// @param values The values expected to be included in the vector
/// @param vec The vector to check
/// @return @c true if @c values and @c vec have the same contents, @c false if not
template <typename Vec>
constexpr auto constexpr_iterate(std::initializer_list<typename Vec::value_type> const values, Vec vec) -> bool {
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

/// @test It is possible to iterate over the values in a mutable `inline_vector` using a ranged-`for` loop in a
/// `constexpr` context.
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, Iteration, InlineVectorTest<TypeParam>::constexpr_compatible) {
  STATIC_ASSERT_TRUE(
      constexpr_iterate<TypeParam>(TestFixture::full_test_initializer_list(), TestFixture::full_test_vector())
  );
}

/// @test It is possible to iterate over the values in a const `inline_vector` using a ranged-`for` loop in a
/// `constexpr` context.
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, ConstIteration, InlineVectorTest<TypeParam>::constexpr_compatible) {
  STATIC_ASSERT_TRUE(
      constexpr_iterate<TypeParam const>(TestFixture::full_test_initializer_list(), TestFixture::full_test_vector())
  );
}

/// @test Given a non-empty `inline_vector`, invoking `erase` with the iterator returned from `begin` removes the first
/// element, moving the remaining elements to one lower index and decreasing the size by one, and returns an iterator to
/// the new first element.
TYPED_TEST_P(InlineVectorTest, CanEraseAtBeginning) {
  auto vec = TestFixture::full_test_vector();

  STATIC_ASSERT_TRUE(std::is_same<
                     decltype(vec.erase(std::declval<typename TypeParam::const_iterator>())),
                     typename TypeParam::iterator>::value);
  auto const pos = vec.erase(vec.begin());
  EXPECT_EQ(pos, vec.begin());
  EXPECT_EQ(vec.size(), TypeParam::capacity - 1UL);
  if (TypeParam::capacity >= 2) {
    EXPECT_EQ(vec[0], TestFixture::test_value(1));
  }
  if (TypeParam::capacity >= 3) {
    EXPECT_EQ(vec[1], TestFixture::test_value(2));
  }
  if (TypeParam::capacity >= 4) {
    EXPECT_EQ(vec[2], TestFixture::test_value(3));
  }
}

/// @test Given a non-empty `inline_vector`, invoking `erase` with an iterator referring to an element that is neither
/// the first or last element removes that element, moves the following elements to one lower index, reduces the size by
/// one, and returns an iterator to the element that replaced the erased element.
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanEraseInMiddle, TypeParam::capacity >= 2) {
  auto vec = TestFixture::full_test_vector();

  STATIC_ASSERT_TRUE(std::is_same<
                     decltype(vec.erase(std::declval<typename TypeParam::const_iterator>())),
                     typename TypeParam::iterator>::value);
  auto pos = vec.erase(vec.begin() + 1);
  EXPECT_EQ(pos, vec.begin() + 1);
  EXPECT_EQ(vec.size(), TypeParam::capacity - 1UL);
  EXPECT_EQ(vec[0], TestFixture::test_value(0));
  if (TypeParam::capacity >= 3) {
    EXPECT_EQ(vec[1], TestFixture::test_value(2));
  }
  if (TypeParam::capacity >= 4) {
    EXPECT_EQ(vec[2], TestFixture::test_value(3));
  }
}

template <typename Vec, typename... Args>
constexpr auto constexpr_erase(std::size_t index, Args&&... args) -> Vec {
  Vec vec{};
  vector_push_back(vec, std::forward<Args>(args)...);
  vec.erase(vec.begin() + static_cast<std::ptrdiff_t>(index));
  return vec;
}

/// @test In a `constexpr` context, given a non-empty `inline_vector`, invoking `erase` with an iterator referring to an
/// element that is neither the first or last element removes that element, moves the following elements to one lower
/// index, reduces the size by one.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanErase,
    InlineVectorTest<TypeParam>::constexpr_compatible&& TypeParam::capacity >= 4
) {
  constexpr auto vec = constexpr_erase<TypeParam>(
      2,
      TestFixture::test_value(0),
      TestFixture::test_value(1),
      TestFixture::test_value(2),
      TestFixture::test_value(3)
  );
  STATIC_ASSERT_EQ(vec.size(), 3);
  STATIC_ASSERT_EQ(vec[0], TestFixture::test_value(0));
  STATIC_ASSERT_EQ(vec[1], TestFixture::test_value(1));
  STATIC_ASSERT_EQ(vec[2], TestFixture::test_value(3));
}

/// @test Invoking `erase` with an iterator range referring to elements in the same `inline_vector`, removes those
/// elements, moves the following elements to take their place, and returns an iterator with the same offset from the
/// beginning of the vector as the first erased element
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanEraseRange, TypeParam::capacity >= 5) {
  auto vec = TestFixture::construct();
  constexpr std::size_t erase_start = 2;
  constexpr std::size_t erase_end = 5;
  arene::base::array<arene::base::optional<typename TypeParam::value_type>, TypeParam::capacity> expected;
  std::size_t expected_pos{0};
  for (std::size_t idx = 0; idx < TypeParam::capacity; ++idx) {
    vec.push_back(TestFixture::test_value(idx));
    if (idx < erase_start || idx >= erase_end) {
      expected[expected_pos].emplace(TestFixture::test_value(idx));
      ++expected_pos;
    }
  }

  ::testing::StaticAssertTypeEq<
      decltype(vec.erase(
          std::declval<typename TypeParam::const_iterator>(),
          std::declval<typename TypeParam::const_iterator>()
      )),
      typename TypeParam::iterator>();
  auto const pos = vec.erase(vec.begin() + erase_start, vec.begin() + erase_end);
  EXPECT_EQ(pos, vec.begin() + erase_start);
  ASSERT_EQ(vec.size(), expected_pos);
  EXPECT_TRUE(elements_are_array(vec, expected));
}

/// @test Constructing with just a size creates a vector with that many default-constructed elements
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanConstructWithSize,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  auto value_vec = TestFixture::construct(TypeParam::capacity);

  STATIC_ASSERT_EQ(noexcept(TypeParam(0)), std::is_nothrow_default_constructible<typename TypeParam::value_type>{});
  EXPECT_EQ(value_vec.size(), TypeParam::capacity);
  EXPECT_TRUE(all_elements_are(value_vec, typename TypeParam::value_type{}));
}

/// @test Constructing an `inline_vector` with a count and a value creates the specified number of elements
/// copy-constructed from the supplied value, and sets the `size` of the vector to that count.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanConstructWithSizeAndValue,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const value = TestFixture::test_value(0);
  auto const value_vec = TestFixture::construct(TypeParam::capacity, value);

  STATIC_ASSERT_EQ(
      noexcept(TypeParam(TypeParam::capacity, value)),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>{}
  );
  EXPECT_EQ(value_vec.size(), TypeParam::capacity);
  EXPECT_THAT(value_vec, ::testing::Each(::testing::Eq(value)));
}

/// @test The `inline_vector` copy constructor copies the stored elements and size from the original vector.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CopyingVectorCopiesElements,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const vec = TestFixture::full_test_vector();
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  auto const copy(vec);

  EXPECT_THAT(copy, ::testing::ElementsAreArray(vec));
}

/// @test The `inline_vector` move constructor moves the stored elements from the original vector
TYPED_TEST_P(InlineVectorTest, MovingVectorMovesElements) {
  auto const values = TestFixture::full_test_initializer_list();
  auto vec = TestFixture::full_test_vector();

  // NOLINTNEXTLINE(hicpp-move-const-arg)
  TypeParam const moved(std::move(vec));
  EXPECT_TRUE(elements_are_array(moved, values));
}

/// @test The `inline_vector` copy-assignment operator uses copy assignment for elements from the source where there is
/// already a corresponding element in the destination, and copy construction for elements that do not have a
/// corresponding element in the destination
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CopyAssignOverSmallerVectorCopiesElements, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t count = TypeParam::capacity;
  constexpr std::size_t smaller_count = TypeParam::capacity - 1UL;

  auto vec = TestFixture::test_vector(count);
  auto copy = TestFixture::test_vector(smaller_count);
  copy = vec;

  ASSERT_EQ(vec.size(), count);
  ASSERT_EQ(copy.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(vec[i], copy[i]);
    EXPECT_EQ(TestFixture::test_value(i), copy[i]);
  }
}

/// @test The `inline_vector` copy-assignment operator destroys the excess elements in the destination when the source
/// vector has fewer elements than the destination vector
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CopyAssignOverLargerVectorDestroysExcess, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t count = TypeParam::capacity - 1UL;
  constexpr std::size_t larger_count = TypeParam::capacity;

  auto vec = TestFixture::test_vector(count);
  auto copy = TestFixture::test_vector(larger_count);
  copy = vec;

  ASSERT_EQ(vec.size(), count);
  ASSERT_EQ(copy.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(vec[i], copy[i]);
    EXPECT_EQ(TestFixture::test_value(i), copy[i]);
  }
}

/// @test Copy-assignment for `inline_vector` is `noexcept` if and only if the stored element type is both
/// nothrow-copy-constructible and nothrow-copy-assignable
TYPED_TEST_P(InlineVectorTest, CopyingIsConditionallyDefined) {
  using short_vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity>;
  using vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity + 1UL>;

  constexpr bool should_be_copyable = std::is_copy_constructible<typename TypeParam::value_type>{} &&
                                      std::is_copy_assignable<typename TypeParam::value_type>{};
  STATIC_ASSERT_EQ(
      std::is_copy_constructible<vec>::value,
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_EQ(std::is_copy_assignable<vec>{}, should_be_copyable);
  STATIC_ASSERT_EQ((std::is_assignable<short_vec&, vec const&>{}), should_be_copyable);
  STATIC_ASSERT_EQ((std::is_assignable<vec&, short_vec const&>{}), should_be_copyable);

  constexpr bool assign_noexcept = std::is_nothrow_copy_constructible<typename TypeParam::value_type>{} &&
                                   std::is_nothrow_copy_assignable<typename TypeParam::value_type>{};
  STATIC_ASSERT_EQ(
      std::is_nothrow_copy_constructible<vec>::value,
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_EQ(std::is_nothrow_copy_assignable<vec>{}, assign_noexcept);
  STATIC_ASSERT_EQ((std::is_nothrow_assignable<short_vec&, vec const&>{}), assign_noexcept);
  STATIC_ASSERT_EQ((std::is_nothrow_assignable<vec&, short_vec const&>{}), assign_noexcept);
}

/// @test The `inline_vector` move-assignment operator uses move assignment for elements from the source where there is
/// already a corresponding element in the destination, and move construction for elements that do not have a
/// corresponding element in the destination
TYPED_TEST_P(InlineVectorTest, MoveAssignOverSmallerVectorMovesElements) {
  constexpr std::size_t count = TypeParam::capacity;
  constexpr std::size_t smaller_count = TypeParam::capacity - 1UL;

  auto first = TestFixture::construct();
  arene::base::array<arene::base::optional<typename TypeParam::value_type>, TypeParam::capacity> expected;
  std::size_t expected_pos{0};
  for (std::size_t idx = 0; idx < count; ++idx) {
    first.push_back(TestFixture::test_value(idx));
    expected[expected_pos].emplace(TestFixture::test_value(idx));
    ++expected_pos;
  }

  auto second = TestFixture::test_vector(smaller_count);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  second = std::move(first);

  EXPECT_EQ(second.size(), count);
  EXPECT_TRUE(elements_are_array(second, expected));
}

/// @test The `inline_vector` move-assignment operator destroys the excess elements in the destination when the source
/// vector has fewer elements than the destination vector
TYPED_TEST_P(InlineVectorTest, MoveAssignOverLargerVectorDestroysExcess) {
  constexpr std::size_t count = TypeParam::capacity - 1UL;
  constexpr std::size_t larger_count = TypeParam::capacity;

  auto first = TestFixture::test_vector(count);
  auto second = TestFixture::test_vector(larger_count);
  second = std::move(first);

  ASSERT_EQ(second.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(TestFixture::test_value(i), second[i]);
  }
}

/// @test Move-assignment for `inline_vector` is `noexcept` if and only if the value type is nothrow-move-constructible
/// and nothrow-move-assignable
TYPED_TEST_P(InlineVectorTest, MovingIsConditionallyDefined) {
  using short_vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity>;
  using vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity + 1UL>;

  constexpr bool should_be_movable = std::is_move_assignable<typename TypeParam::value_type>{} &&
                                     std::is_move_constructible<typename TypeParam::value_type>{};
  STATIC_ASSERT_EQ(std::is_move_constructible<vec>{}, should_be_movable);
  STATIC_ASSERT_EQ(std::is_move_assignable<vec>{}, should_be_movable);
  STATIC_ASSERT_EQ((std::is_assignable<short_vec&, vec>{}), should_be_movable);
  STATIC_ASSERT_EQ((std::is_assignable<vec&, short_vec>{}), should_be_movable);

  constexpr bool assign_should_be_noexcept = std::is_nothrow_move_assignable<typename TypeParam::value_type>{} &&
                                             std::is_nothrow_move_constructible<typename TypeParam::value_type>{};
  STATIC_ASSERT_EQ(
      std::is_nothrow_move_constructible<vec>::value,
      std::is_nothrow_move_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_EQ(std::is_nothrow_move_assignable<vec>{}, assign_should_be_noexcept);
  STATIC_ASSERT_EQ((std::is_nothrow_assignable<short_vec&, vec>{}), assign_should_be_noexcept);
  STATIC_ASSERT_EQ((std::is_nothrow_assignable<vec&, short_vec>{}), assign_should_be_noexcept);
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

/// @test Invoking `pop_back` on a non-empty `inline_vector` destroys the last element and reduces the size by one.
TYPED_TEST_P(InlineVectorTest, CanPopBack) {
  auto vec = TestFixture::test_vector(TypeParam::capacity - 1UL);

  vec.push_back(TestFixture::test_value(TypeParam::capacity - 1UL));
  vec.pop_back();

  EXPECT_EQ(vec.size(), TypeParam::capacity - 1UL);
  EXPECT_THAT(vec, elements_match_function(TestFixture::test_value));
}

/// @test `pop_back` is declared `noexcept`
TYPED_TEST_P(InlineVectorTest, PopBackIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam>().pop_back()));
}

template <typename Vec>
constexpr auto constexpr_pop_back(Vec&& vec) -> Vec {
  static_assert(std::is_rvalue_reference<Vec&&>::value, "Only expected to be called with an r-value reference.");
  std::forward<Vec>(vec).pop_back();
  return std::forward<Vec>(vec);
}

/// @test In a `constexpr` context, `pop_back` removes the last element of a non-empty vector
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, PopBackIsUsable, InlineVectorTest<TypeParam>::constexpr_compatible) {
  constexpr auto vec = constexpr_pop_back<TypeParam>(TestFixture::full_test_vector());
  STATIC_ASSERT_EQ(vec.size(), TypeParam::capacity - 1UL);

  if (TypeParam::capacity >= 2) {
    EXPECT_EQ(vec[0], TestFixture::test_value(0));
  }
  if (TypeParam::capacity >= 3) {
    EXPECT_EQ(vec[1], TestFixture::test_value(1));
  }
  if (TypeParam::capacity >= 4) {
    EXPECT_EQ(vec[2], TestFixture::test_value(2));
  }
}

/// @test `erase` is `noexcept` if and only if the element type has `noexcept` move assignment
TYPED_TEST_P(InlineVectorTest, EraseIsNoexceptIfTypeHasNoexceptMove) {
  auto vec = TestFixture::construct();

  STATIC_ASSERT_EQ(
      noexcept(vec.erase(vec.begin(), vec.end())),
      std::is_nothrow_move_assignable<typename TypeParam::value_type>{}
  );
  STATIC_ASSERT_EQ(noexcept(vec.erase(vec.begin())), std::is_nothrow_move_assignable<typename TypeParam::value_type>{});
}

/// @test Given an empty `inline_vector`, and the iterator returned from `begin`, `insert` can add an element to the
/// vector, setting the size to one.
TYPED_TEST_P(InlineVectorTest, CanInsertInEmptyVector) {
  auto vec1 = TestFixture::construct();
  auto const& const_vec1 = vec1;
  auto const itr = vec1.insert(const_vec1.begin(), TestFixture::test_value(0));
  EXPECT_EQ(itr, vec1.begin());
  EXPECT_EQ(vec1.size(), 1);
  EXPECT_EQ(vec1[0], TestFixture::test_value(0));
}

/// @test Given an `inline_vector` with existing elements, and the iterator returned from `end`, `insert` can add an
/// element to the end of the vector, increasing the size by one
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanInsertAtEndOfExistingVector, TypeParam::capacity >= 2) {
  auto vec1 = TestFixture::test_vector(1);
  auto const& const_vec1 = vec1;
  auto const pos = vec1.insert(const_vec1.end(), TestFixture::test_value(1));
  EXPECT_EQ(pos, vec1.begin() + 1);
  EXPECT_EQ(vec1.size(), 2);
  EXPECT_EQ(vec1[0], TestFixture::test_value(0));
  EXPECT_EQ(vec1[1], TestFixture::test_value(1));
}

/// @test Given an `inline_vector` with existing elements, and an iterator to an existing element in the vector,
/// `insert` can add an element at the iterator position, increasing the vector size by one. The existing element at the
/// location referenced by the iterator, and the subsequent elements, are moved to make room for the new element.
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanInsertInMiddleOfExistingVector, TypeParam::capacity >= 3) {
  auto vec1 = TestFixture::test_vector(2);
  auto const& const_vec1 = vec1;
  auto const pos = vec1.insert(const_vec1.begin() + 1, TestFixture::test_value(2));
  EXPECT_EQ(pos, vec1.begin() + 1);
  EXPECT_EQ(vec1.size(), 3);
  EXPECT_EQ(vec1[0], TestFixture::test_value(0));
  EXPECT_EQ(vec1[1], TestFixture::test_value(2));
  EXPECT_EQ(vec1[2], TestFixture::test_value(1));
}

/// @test Invoking `insert` on an `inline_vector` with `size` equal to the capacity terminates the program with a
/// precondition violation
CONDITIONAL_TYPED_TEST_P(
    InlineVectorDeathTest,
    InsertWhenAtCapacityIsPreconditionViolation,
    std::is_default_constructible<typename TypeParam::value_type>::value
) {
  auto vec1 = TestFixture::construct(TypeParam::capacity);
  EXPECT_EQ(vec1.size(), TypeParam::capacity);
  ASSERT_DEATH(vec1.insert(vec1.end(), TestFixture::test_value(0)), "Precondition violation");
  ASSERT_DEATH(vec1.insert(vec1.begin(), TestFixture::test_value(0)), "Precondition violation");
  EXPECT_EQ(vec1.size(), TypeParam::capacity);
  EXPECT_TRUE(all_elements_are(vec1, typename TypeParam::value_type{}));
}

/// @test Invoking the `inline_vector` constructor with a count larger than the capacity terminates the program with a
/// precondition violation
CONDITIONAL_TYPED_TEST_P(
    InlineVectorDeathTest,
    ConstructingWithOutOfRangeSizeIsPreconditionViolation,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  ASSERT_DEATH(TestFixture::construct(TypeParam::capacity + 1), "Precondition violation");
}

/// @test Invoking the `inline_vector` constructor with a count and value where the count is larger than the capacity
/// terminates the program with a precondition violation
CONDITIONAL_TYPED_TEST_P(
    InlineVectorDeathTest,
    ConstructingWithOutOfRangeSizeAndSourceIsPreconditionViolation,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const source = TestFixture::test_value(0);
  ASSERT_DEATH(TestFixture::construct(TypeParam::capacity + 1, source), "Precondition violation");
}

/// @test Invoking the `inline_vector` constructor with an initializer list with more elements than the capacity
/// terminates the program with a precondition violation
CONDITIONAL_TYPED_TEST_P(
    InlineVectorDeathTest,
    ConstructingWithTooManyInitializersIsPreconditionViolation,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  ASSERT_DEATH(
      TestFixture::construct(TestFixture::template test_initializer_list<TypeParam::capacity + 1UL>()),
      "Precondition violation"
  );
}

template <typename Vec>
constexpr auto constexpr_resize(Vec vec, std::size_t size) -> Vec {
  vec.resize(size);
  return vec;
}

/// @test In a constexpr context, invoking `resize` with a count of zero on an `inline_vector` with a capacity of zero
/// does nothing
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ResizeZeroCapacityVecToZeroIsOk,
    InlineVectorTest<TypeParam>::constexpr_compatible
) {
  constexpr typename TestFixture::template vector_with_capacity<0> vec{};
  constexpr typename TestFixture::template vector_with_capacity<0> vec2 = constexpr_resize(vec, 0);
  // NOLINTNEXTLINE(readability-container-size-empty)
  STATIC_ASSERT_EQ(vec2.size(), 0);
}

/// @test In a `constexpr` context, invoking `resize` with a count of zero on an `inline_vector` with a non-zero
/// capacity and size sets the size to zero
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ResizeNonZeroCapacityVecToZeroIsOk,
    InlineVectorTest<TypeParam>::constexpr_compatible&& std::is_default_constructible<typename TypeParam::value_type>{}
) {
  constexpr auto vec = TestFixture::construct(TypeParam::capacity);
  constexpr auto vec2 = constexpr_resize(vec, 0);
  // NOLINTNEXTLINE(readability-container-size-empty)
  STATIC_ASSERT_EQ(vec2.size(), 0);
}

/// @test In a `constexpr` context, invoking `resize` with a non-zero count on an `inline_vector` with a non-zero
/// capacity and different size sets the size to the specified size
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ResizeNonZeroCapacityVecToOtherIsOk,
    InlineVectorTest<TypeParam>::constexpr_compatible&& std::is_default_constructible<typename TypeParam::value_type>{}
) {
  constexpr std::size_t count1 = TypeParam::capacity - 1UL;
  constexpr std::size_t count2 = TypeParam::capacity;
  constexpr auto vec = TestFixture::construct(count1);
  STATIC_ASSERT_EQ(vec.size(), count1);
  constexpr auto vec2 = constexpr_resize(vec, count2);
  STATIC_ASSERT_EQ(vec2.size(), count2);
}

/// @test `resize` is `noexcept` if and only if the default constructor of the element type is `noexcept`
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ResizeIsConditionallyNoexcept,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().resize(0)),
      std::is_nothrow_default_constructible<typename TypeParam::value_type>{}
  );
}

/// @test `resize` with a value to copy is `noexcept` if and only if the copy constructor of the element type is
/// `noexcept`
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ResizeWithValueIsConditionallyNoexcept,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  STATIC_ASSERT_EQ(
      noexcept(std::declval<TypeParam&>().resize(0, std::declval<typename TypeParam::value_type>())),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>{}
  );
}

/// @test Invoking the `resize` member function with a size and a value on a vector with more elements than the
/// specified size sets the size of the vector to the specified count and does not construct new elements
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ResizingFromLargeToSmallLeavesElements,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  constexpr std::size_t initial_count = TypeParam::capacity;
  auto const initial_value = TestFixture::test_value(0);
  TypeParam vec(initial_count, initial_value);
  constexpr std::size_t count = TypeParam::capacity - 1UL;
  auto const value = TestFixture::test_value(1);
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
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ResizingFromSmallToLargeConstructsElementsAsCopiesOfSpecified,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  constexpr std::size_t initial_count = TypeParam::capacity - 1UL;
  auto const initial_value = TestFixture::test_value(0);
  TypeParam vec(initial_count, initial_value);
  constexpr std::size_t count = TypeParam::capacity;
  auto const value = TestFixture::test_value(1);
  vec.resize(count, value);
  ASSERT_EQ(vec.size(), count);

  for (std::size_t idx = 0; idx < initial_count; ++idx) {
    EXPECT_EQ(vec[idx], initial_value);
  }
  for (std::size_t idx = initial_count; idx < count; ++idx) {
    EXPECT_EQ(vec[idx], value);
  }
}

/// @test In a `constexpr` context, invoking the `inline_vector` constructor with a count and a value constructs a
/// vector holding the specified number of elements copied from the source value
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanInitializeWithSizeAndValue,
    InlineVectorTest<TypeParam>::constexpr_compatible&&
        std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  constexpr auto source = TestFixture::test_value(0);
  constexpr auto vec = TestFixture::construct(TypeParam::capacity, source);

  STATIC_ASSERT_EQ(vec.size(), TypeParam::capacity);
  STATIC_ASSERT_TRUE(all_elements_are(vec, source));
}

template <typename Vec, typename T>
constexpr auto constexpr_insert(Vec&& vec, std::ptrdiff_t offset, T&& value) -> Vec {
  static_assert(std::is_rvalue_reference<Vec&&>::value, "Only expected to be called with an r-value reference.");
  std::forward<Vec>(vec).insert(std::forward<Vec>(vec).begin() + offset, std::forward<T>(value));
  return std::forward<Vec>(vec);
}

/// @test In a `constexpr` context, invoking `insert` with an iterator referring to an existing element, moves the
/// referred to element and subsequent elements to make room for the new element, inserts the new element, and increases
/// the size by one.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanInsert,
    InlineVectorTest<TypeParam>::constexpr_compatible&& TypeParam::capacity >= 3
) {
  constexpr auto vec = constexpr_insert<TypeParam>(TestFixture::test_vector(2), 1, TestFixture::test_value(2));

  STATIC_ASSERT_EQ(vec.size(), 3);
  STATIC_ASSERT_EQ(vec[0], TestFixture::test_value(0));
  STATIC_ASSERT_EQ(vec[1], TestFixture::test_value(2));
  STATIC_ASSERT_EQ(vec[2], TestFixture::test_value(1));
}

/// @test An `inline_vector` can be constructed from an iterator range, creating a vector by moving the elements
/// from the source range, with a size equal to the number of elements in the source range
TYPED_TEST_P(InlineVectorTest, CanConstructFromIteratorRange) {
  auto const expected = TestFixture::full_test_vector();
  auto source = TestFixture::full_test_vector();

  TypeParam vec(std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));

  EXPECT_EQ(vec.size(), expected.size());
  EXPECT_TRUE(elements_are_array(vec, expected));
}

/// @test Construction of an `inline_vector` from an iterator range is `noexcept` if operations on the iterator are
/// `noexcept` and the element type of the vector is nothrow-constructible from the reference type of the iterator
TYPED_TEST_P(InlineVectorTest, IteratorRangeConstructionIsConditionallyNoexcept) {
  using source_it =
      std::move_iterator<typename arene::base::array<typename TypeParam::value_type, TypeParam::capacity>::iterator>;
  using source_val = typename source_it::reference;

  constexpr bool it_operations_noexcept = noexcept(++std::declval<source_it>())&& noexcept(*std::declval<source_it>());
  constexpr bool element_construction_noexcept =
      std::is_nothrow_constructible<typename TypeParam::value_type, source_val>{};

  STATIC_ASSERT_TRUE((std::is_constructible<TypeParam, source_it, source_it>{}));
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<TypeParam, source_it, source_it>{}),
      it_operations_noexcept && element_construction_noexcept
  );

  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using convertible_array = ::arene::base::array<throws_when_converted, TypeParam::capacity>;
  using throwing_bidirectional_iterator = typename TestFixture::throwing_bidirectional_move_iterator_of_t;

  STATIC_ASSERT_TRUE(std::is_constructible<
                     TypeParam,
                     typename convertible_array::const_iterator,
                     typename convertible_array::const_iterator>::value);
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<
                      TypeParam,
                      typename convertible_array::const_iterator,
                      typename convertible_array::const_iterator>::value);
  STATIC_ASSERT_TRUE(
      std::is_constructible<TypeParam, throwing_bidirectional_iterator, throwing_bidirectional_iterator>::value
  );
  STATIC_ASSERT_FALSE(
      std::is_nothrow_constructible<TypeParam, throwing_bidirectional_iterator, throwing_bidirectional_iterator>::value
  );
}

/// @test Construction of an `inline_vector` from an input iterator range is `noexcept` if operations on the iterator
/// are `noexcept` and the element type of the vector is nothrow-constructible from the reference type of the iterator
TYPED_TEST_P(InlineVectorTest, InputIteratorRangeConstructionIsConditionallyNoexcept) {
  using input_it = ::testing::demoted_iterator<typename TypeParam::value_type*, std::input_iterator_tag>;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<typename TypeParam::value_type>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type =
      typename TestFixture::template non_throwing_input_iterator<typename TypeParam::value_type>;

  constexpr bool it_operations_noexcept = noexcept(++std::declval<input_it>())&& noexcept(*std::declval<input_it>());
  constexpr bool element_construction_noexcept =
      std::is_nothrow_constructible<typename TypeParam::value_type, typename input_it::reference>{};

  STATIC_ASSERT_TRUE((std::is_constructible<TypeParam, input_it, input_it>{}));
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<TypeParam, input_it, input_it>{}),
      it_operations_noexcept && element_construction_noexcept
  );

  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, throwing_it_throwing_type, throwing_it_throwing_type>::value);
  STATIC_ASSERT_FALSE(
      std::is_nothrow_constructible<TypeParam, throwing_it_throwing_type, throwing_it_throwing_type>::value
  );
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, throwing_it_raw_type, throwing_it_raw_type>::value);
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<TypeParam, throwing_it_raw_type, throwing_it_raw_type>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, nothrow_it_raw_type, nothrow_it_raw_type>::value);
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<TypeParam, nothrow_it_raw_type, nothrow_it_raw_type>::value),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_TRUE(std::is_constructible<TypeParam, nothrow_it_throwing_type, nothrow_it_throwing_type>::value);
  STATIC_ASSERT_FALSE(
      std::is_nothrow_constructible<TypeParam, nothrow_it_throwing_type, nothrow_it_throwing_type>::value
  );
}

/// @test In a `constexpr` context, an `inline_vector` can be constructed from an iterator range, creating a vector
/// holding copies of the elements from the source range, with a size equal to the number of elements in the source
/// range
// Note: std::move_iterator is not constexpr in C++14, so this is limited to copy constructible types.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanConstructFromIteratorRangeConstexpr,
    InlineVectorTest<TypeParam>::constexpr_compatible&&
        std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  constexpr auto source = TestFixture::full_test_initializer_list();
  constexpr TypeParam vec(source.begin(), source.end());

  STATIC_ASSERT_EQ(vec.size(), source.size());
  STATIC_ASSERT_TRUE(equal(vec.begin(), vec.end(), source.begin(), source.end()));
}

/// @test Given an `inline_vector` holding existing elements, assigning to that vector with an initializer list holding
/// more elements copy assigns over the existing elements, and then copy-constructs new elements to increase the size of
/// the vector to that of the initializer list
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CopyAssignOverSmallerVectorFromInitListCopiesElements, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t smaller_count = TypeParam::capacity - 1UL;

  auto const value = TestFixture::full_test_initializer_list();

  auto copy = TestFixture::test_vector(smaller_count);
  copy = value;

  EXPECT_THAT(copy, ::testing::ElementsAreArray(value));
}

/// @test Given an `inline_vector` holding existing elements, assigning to that vector with an initializer list holding
/// fewer elements copy assigns over the existing elements up to the size of the initializer list, and then destroys a
/// number of elements equal to the difference between the old size of the vector and the size of the initializer list
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CopyAssignOverLargerVectorFromInitListDestroysExcess, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t larger_count = TypeParam::capacity;

  auto const value = TestFixture::template test_initializer_list<TypeParam::capacity - 1UL>();

  auto copy = TestFixture::test_vector(larger_count);
  copy = value;

  EXPECT_THAT(copy, ::testing::ElementsAreArray(value));
}

/// @test Invoking the `assign` member function with an iterator range sets the vector to have a size equal to the
/// number of elements in the range, and each element to be a copy of the corresponding element in the range
TYPED_TEST_P(InlineVectorTest, CanAssignFromIteratorRange) {
  auto source = TestFixture::full_test_vector();

  auto vec = TestFixture::test_vector(0);
  vec.assign(std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));

  EXPECT_TRUE(elements_are_array(vec, TestFixture::full_test_initializer_list()));
}

/// @test Assignment of an `inline_vector` from an iterator range is `noexcept` if operations on the iterator are
/// `noexcept` and the element type of the vector is nothrow-constructible and nothrow-assignable from the reference
/// type of the iterator
TYPED_TEST_P(InlineVectorTest, IteratorRangeAssignmentIsConditionallyNoexcept) {
  using array_it = std::move_iterator<typename array<typename TypeParam::value_type, TypeParam::capacity>::iterator>;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using convertible_array_it = std::move_iterator<typename array<throws_when_converted, TypeParam::capacity>::iterator>;
  using throwing_bidirectional_iterator = typename TestFixture::throwing_bidirectional_move_iterator_of_t;

  constexpr bool it_operations_noexcept = noexcept(++std::declval<array_it>())&& noexcept(*std::declval<array_it>());
  constexpr bool element_construction_noexcept =
      std::is_nothrow_constructible<typename TypeParam::value_type, typename array_it::reference>{} &&
      std::is_nothrow_assignable<typename TypeParam::value_type&, typename array_it::reference>{};

  STATIC_ASSERT_EQ(
      noexcept(TypeParam{}.assign(std::declval<array_it>(), std::declval<array_it>())),
      it_operations_noexcept && element_construction_noexcept
  );
  STATIC_ASSERT_FALSE(
      noexcept(TypeParam{}.assign(std::declval<convertible_array_it>(), std::declval<convertible_array_it>()))
  );
  STATIC_ASSERT_FALSE(noexcept(TypeParam{}.assign(
      std::declval<throwing_bidirectional_iterator>(),
      std::declval<throwing_bidirectional_iterator>()
  )));
}

/// @test Invoking the `assign` member function with an initializer list sets the vector to have a size equal to the
/// number of elements in the initializer list, and each element to be a copy of the corresponding element in the
/// initializer list
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanAssignFromInitListUsingAssignFunction, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t smaller_count = TypeParam::capacity - 1UL;

  auto const value = TestFixture::full_test_initializer_list();

  auto copy = TestFixture::test_vector(smaller_count);
  copy.assign(value);

  EXPECT_THAT(copy, ::testing::ElementsAreArray(value));
}

/// @test Assignment of an `inline_vector` from an initializer list is `noexcept` if the element type of the vector is
/// nothrow-copy-constructible
TYPED_TEST_P(InlineVectorTest, AssignFromInitListUsingAssignFunctionIsConditionallyNoexcept) {
  constexpr bool is_nothrow_copyable = std::is_nothrow_copy_constructible<typename TypeParam::value_type>{} &&
                                       std::is_nothrow_copy_assignable<typename TypeParam::value_type>{};

  STATIC_ASSERT_TRUE(std::is_assignable<TypeParam, std::initializer_list<typename TypeParam::value_type>>::value);
  STATIC_ASSERT_EQ(
      (std::is_nothrow_assignable<TypeParam, std::initializer_list<typename TypeParam::value_type>>::value),
      is_nothrow_copyable
  );

  auto const values = TestFixture::full_test_initializer_list();
  STATIC_ASSERT_EQ(noexcept(TypeParam{}.assign(values)), is_nothrow_copyable);
}

/// @test Invoking the `assign` member function with an iterator range specified using input iterators sets the vector
/// to have a size equal to the number of elements in the range, and each element to be a copy of the corresponding
/// element in the range
TYPED_TEST_P(InlineVectorTest, CanAssignFromInputIteratorRange) {
  auto source = TestFixture::full_test_vector();

  auto vec = TestFixture::test_vector(0);
  using move_it = std::move_iterator<typename decltype(source)::iterator>;
  using input_it = ::testing::demoted_iterator<move_it, std::input_iterator_tag>;

  auto begin = input_it{std::make_move_iterator(source.begin())};
  auto end = input_it{std::make_move_iterator(source.end())};
  vec.assign(begin, end);

  EXPECT_TRUE(elements_are_array(vec, TestFixture::full_test_initializer_list()));
}

/// @test Assignment of an `inline_vector` from an input iterator range is `noexcept` if operations on the iterator are
/// `noexcept` and the element type of the vector is nothrow-constructible from the value type of the iterator
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, InputIteratorRangeAssignmentIsConditionallyNoexcept, is_copyable_v<typename TypeParam::value_type>) {
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<typename TypeParam::value_type>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type =
      typename TestFixture::template non_throwing_input_iterator<typename TypeParam::value_type>;

  STATIC_ASSERT_FALSE(noexcept(TypeParam{}.assign(
      std::declval<::testing::demoted_iterator<throwing_it_throwing_type, std::input_iterator_tag>>(),
      std::declval<::testing::demoted_iterator<throwing_it_throwing_type, std::input_iterator_tag>>()
  )));
  STATIC_ASSERT_FALSE(
      noexcept(TypeParam{}.assign(std::declval<throwing_it_throwing_type>(), std::declval<throwing_it_throwing_type>()))
  );
  STATIC_ASSERT_FALSE(
      noexcept(TypeParam{}.assign(std::declval<throwing_it_raw_type>(), std::declval<throwing_it_raw_type>()))
  );
  STATIC_ASSERT_EQ(
      noexcept(TypeParam{}.assign(std::declval<nothrow_it_raw_type>(), std::declval<nothrow_it_raw_type>())),
      std::is_nothrow_copy_assignable<typename TypeParam::value_type>::value &&
          std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_FALSE(
      noexcept(TypeParam{}.assign(std::declval<nothrow_it_throwing_type>(), std::declval<nothrow_it_throwing_type>()))
  );
}

/// @test Invoking the `assign` member function with a count and a value sets the size of the vector to the specified
/// count, where each element is a copy of the supplied value
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanAssignFromSizeAndValue, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t count = TypeParam::capacity;
  constexpr std::size_t smaller_count = TypeParam::capacity - 1UL;

  auto copy = TestFixture::test_vector(smaller_count);
  auto const value = TestFixture::test_value(0);
  copy.assign(count, value);

  EXPECT_EQ(copy.size(), count);
  for (std::size_t i = 0; i < count; ++i) {
    EXPECT_EQ(copy[i], value);
  }
}

/// @test The `assign` member function taking a size and a value is not `noexcept` if the value type is
/// not nothrow-copy-constructible
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, AssignFromSizeAndValueIsConditionallyNoexcept, is_copyable_v<typename TypeParam::value_type>) {
  constexpr bool is_nothrow_copyable = std::is_nothrow_copy_constructible<typename TypeParam::value_type>{} &&
                                       std::is_nothrow_copy_assignable<typename TypeParam::value_type>{};
  STATIC_ASSERT_EQ(noexcept(TypeParam{}.assign(0, TestFixture::test_value(0))), is_nothrow_copyable);
}

/// @test Invoking the `assign` member function with a count and a value where the count is larger than the capacity of
/// the vector terminates the program with a precondition violation
CONDITIONAL_TYPED_TEST_P(InlineVectorDeathTest, AssignIsPreconditionViolationIfSizeOverCapacity, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t smaller_count = TypeParam::capacity - 1UL;

  auto const initial_value = TestFixture::test_value(0);
  TypeParam copy(smaller_count, initial_value);
  auto const value = TestFixture::test_value(1);
  ASSERT_DEATH(copy.assign(TypeParam::capacity + 1, value), "Precondition violation");

  EXPECT_EQ(copy.size(), smaller_count);
  for (std::size_t i = 0; i < smaller_count; ++i) {
    EXPECT_EQ(copy[i], initial_value);
  }
}

/// @test The index operator returns a reference to the element type of the vector
TYPED_TEST_P(InlineVectorTest, IndexOperatorReturnsReference) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>()[0]), typename TypeParam::value_type&>();
}

/// @test The index operator for a `const` vector returns a `const` reference to the element type of the vector
TYPED_TEST_P(InlineVectorTest, IndexOperatorOnConstVectorReturnsConstReference) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>()[0]), typename TypeParam::value_type const&>();
}

/// @test Given an `inline_vector` with multiple elements, the index operator can be used with appropriate index values
/// to access each element, returning a reference to the element at that index
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanAccessMultipleValuesWithIndexOperator, TypeParam::capacity >= 2) {
  auto vec = TestFixture::test_vector(2);
  auto const& const_vec = vec;

  EXPECT_EQ(vec[0], TestFixture::test_value(0));
  EXPECT_EQ(vec[1], TestFixture::test_value(1));
  // NOLINTNEXTLINE(readability-container-data-pointer)
  EXPECT_EQ(&const_vec[0], std::addressof(vec[0]));
  EXPECT_EQ(&const_vec[1], std::addressof(vec[1]));
}

/// @test The index operator is `noexcept`
TYPED_TEST_P(InlineVectorTest, IndexOperatorIsNoexcept) { STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>()[0])); }

/// @test Passing an index that is equal to or larger than the size of the vector terminates the program with a
/// precondition violation
TYPED_TEST_P(InlineVectorDeathTest, IndexOperatorOutOfRange) {
  ASSERT_DEATH(TestFixture::construct()[0], "index < this->size()");
}

/// @test The index operator is `noexcept` for a `const` vector
TYPED_TEST_P(InlineVectorTest, IndexOperatorOnConstVectorIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>()[0]));
}

/// @test The index operator can be used to access the elements of an `inline_vector` in a `constexpr` context.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    IndexOperator,
    InlineVectorTest<TypeParam>::constexpr_compatible&& TypeParam::capacity >= 2
) {
  constexpr auto vec = TestFixture::test_vector(2);
  STATIC_ASSERT_EQ(vec[0], TestFixture::test_value(0));
  STATIC_ASSERT_EQ(vec[1], TestFixture::test_value(1));
}

/// @test The `data` member function is noexcept and returns a pointer to the first element of a non-empty vector
TYPED_TEST_P(InlineVectorTest, Data) {
  auto vec = TestFixture::full_test_vector();
  auto const& const_vec = vec;

  // NOLINTNEXTLINE(readability-container-data-pointer)
  EXPECT_EQ(vec.data(), std::addressof(vec[0]));
  // NOLINTNEXTLINE(readability-container-data-pointer)
  EXPECT_EQ(const_vec.data(), std::addressof(vec[0]));

  STATIC_ASSERT_TRUE(noexcept(vec.data()));
  STATIC_ASSERT_TRUE(noexcept(const_vec.data()));

  // This is just to check that data() works in constexpr.
  COND_STATIC_ASSERT_NE(TestFixture::full_test_vector().data(), nullptr);
}

/// @test Given an `inline_vector` which is not empty, invoking the `clear` method makes the vector empty
TYPED_TEST_P(InlineVectorTest, AfterClearVectorIsEmpty) {
  auto vec = TestFixture::full_test_vector();

  EXPECT_EQ(vec.size(), TypeParam::capacity);
  EXPECT_FALSE(vec.empty());
  vec.clear();
  EXPECT_EQ(vec.size(), 0);
  EXPECT_TRUE(vec.empty());

  STATIC_ASSERT_TRUE(noexcept(vec.clear()));
}

template <typename Vec>
constexpr auto constexpr_clear(Vec vec) -> Vec {
  vec.clear();
  return vec;
}

/// @test In a `constexpr` context, given an `inline_vector` which is not empty, invoking the `clear` method makes the
/// vector empty
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, ClearWorks, InlineVectorTest<TypeParam>::constexpr_compatible) {
  constexpr auto vec = TestFixture::full_test_vector();

  constexpr auto value2 = constexpr_clear(vec);
  STATIC_ASSERT_TRUE(value2.empty());
}

/// @test Two instances of `inline_vector` can be compared for equality using `operator==` and `operator!=`. Two vectors
/// are the same if and only if they have the same size, and each element from one vector compares equal to the element
/// with the same index in the other vector.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCompareForEquality,
    InlineVectorTest<TypeParam>::constexpr_compatible&& TypeParam::capacity >= 4
) {
  constexpr TypeParam vec{
      TestFixture::template unique_test_value<0>(),
      TestFixture::template unique_test_value<1>(),
      TestFixture::template unique_test_value<2>()
  };
  constexpr TypeParam different_contents{
      TestFixture::template unique_test_value<3>(),
      TestFixture::template unique_test_value<4>(),
      TestFixture::template unique_test_value<5>()
  };
  constexpr auto longer = TestFixture::test_vector(4);
  constexpr TypeParam copy{vec};

  STATIC_ASSERT_TRUE(vec != different_contents);
  STATIC_ASSERT_TRUE(vec != longer);
  STATIC_ASSERT_FALSE(vec != copy);
  STATIC_ASSERT_FALSE(vec == different_contents);
  STATIC_ASSERT_FALSE(vec == longer);
  STATIC_ASSERT_TRUE(vec == copy);
}

template <class Vec>
constexpr auto erase_front(Vec&& vec, std::size_t count) {
  static_assert(std::is_rvalue_reference<Vec&&>::value, "Only expected to be called with an r-value reference.");
  using difference_type = typename std::iterator_traits<typename Vec::iterator>::difference_type;
  std::forward<Vec>(vec).erase(
      std::forward<Vec>(vec).begin(),
      std::forward<Vec>(vec).begin() + static_cast<difference_type>(count)
  );
  return std::forward<Vec>(vec);
}

/// A type trait to tell if the given type parameter is fully comparable or not for the purposes of these tests
/// @tparam T The type to check
template <typename T>
constexpr bool is_fully_comparable_v = is_less_than_comparable_v<T> && is_less_than_or_equal_comparable_v<T> &&
                                       is_greater_than_comparable_v<T> && is_greater_than_or_equal_comparable_v<T>;

/// @test Two instances of `inline_vector` can be compared for ordering using `operator<` and `operator>`. One vector
/// compares less than another if the first has a smaller size than the second, and all elements are equal to the
/// corresponding element in the second vector with the same index or they have the same size, and there is an element
/// such that all elements with a lower index compare equal to the corresponding element in the second vector, and that
/// element compares less than the corresponding element of the second vector.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCompareForOrdering,
    InlineVectorTest<TypeParam>::constexpr_compatible&& is_fully_comparable_v<typename TypeParam::value_type>&&
            TypeParam::capacity >= 4
) {
  constexpr auto sorted_test_values = sort_helper(TypeParam{
      TestFixture::template unique_test_value<0>(),
      TestFixture::template unique_test_value<1>(),
      TestFixture::template unique_test_value<2>(),
      TestFixture::template unique_test_value<3>()
  });

  constexpr auto low_value = TypeParam(sorted_test_values.begin(), sorted_test_values.begin() + 2UL);
  constexpr auto high_value = TypeParam(sorted_test_values.begin() + 2UL, sorted_test_values.begin() + 4UL);
  constexpr auto long_value = TypeParam(sorted_test_values.begin(), sorted_test_values.begin() + 3UL);
  constexpr auto low_value_copy = low_value;

  STATIC_ASSERT_TRUE(low_value < high_value);
  STATIC_ASSERT_TRUE(low_value < long_value);
  STATIC_ASSERT_FALSE(low_value < low_value_copy);
  STATIC_ASSERT_FALSE(high_value < low_value);
  STATIC_ASSERT_TRUE(high_value > low_value);
  STATIC_ASSERT_TRUE(long_value > low_value);
  STATIC_ASSERT_FALSE(low_value_copy > low_value);
  STATIC_ASSERT_FALSE(low_value > high_value);

  STATIC_ASSERT_TRUE(low_value <= high_value);
  STATIC_ASSERT_TRUE(low_value <= long_value);
  STATIC_ASSERT_TRUE(low_value <= low_value_copy);
  STATIC_ASSERT_FALSE(high_value <= low_value);
  STATIC_ASSERT_TRUE(high_value >= low_value);
  STATIC_ASSERT_TRUE(long_value >= low_value);
  STATIC_ASSERT_TRUE(low_value_copy >= low_value);
  STATIC_ASSERT_FALSE(low_value >= high_value);
}

template <typename ValueType>
class move_only_wrapper {
  using value_type = ValueType;
  value_type value_;

 public:
  explicit move_only_wrapper(value_type value)
      : value_(std::move(value)) {}
  move_only_wrapper(move_only_wrapper const&) = delete;
  move_only_wrapper(move_only_wrapper&&) = default;
  auto operator=(move_only_wrapper const&) -> move_only_wrapper& = delete;
  auto operator=(move_only_wrapper&&) -> move_only_wrapper& = default;
  ~move_only_wrapper() = default;

  auto get() -> value_type& { return value_; }
};

/// @test The `emplace_back` member function can be used to add an element to an `inline_vector`. The new element is
/// constructed with the arguments supplied to `emplace_back`
TYPED_TEST_P(InlineVectorTest, CanEmplaceBack) {
  using value_type = typename TypeParam::value_type;

  struct with_constructor_args {
    std::reference_wrapper<value_type const> value;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    value_type& reference;
    move_only_wrapper<value_type> wrapper;

    with_constructor_args(value_type const& val, value_type& ref, move_only_wrapper<value_type> wrap_arg)
        : value(val),
          reference(ref),
          wrapper(std::move(wrap_arg)) {}
  };

  typename TestFixture::template standard_capacity_vector<with_constructor_args> vec;

  value_type const value1 = TestFixture::test_value(0);
  value_type value2 = TestFixture::test_value(1);
  auto& result = vec.emplace_back(value1, value2, move_only_wrapper<value_type>(TestFixture::test_value(2)));

  ::testing::StaticAssertTypeEq<decltype(result), with_constructor_args&>();
  EXPECT_EQ(vec.size(), 1);
  ASSERT_FALSE(vec.empty());
  EXPECT_EQ(std::addressof(result), std::addressof(vec[0]));

  EXPECT_EQ(result.value.get(), value1);
  EXPECT_EQ(std::addressof(result.reference), std::addressof(value2));
  EXPECT_EQ(result.wrapper.get(), TestFixture::test_value(2));
}

template <typename Container, typename... Args>
constexpr auto constexpr_emplace_back(Args&&... args) -> Container {
  Container res;
  res.emplace_back(std::forward<Args>(args)...);
  return res;
}

/// @test In a `constexpr` context, the `emplace_back` member function can be used to add an element to an
/// `inline_vector`. The new element is constructed with the arguments supplied to `emplace_back`
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanEmplaceBackConstexpr, InlineVectorTest<TypeParam>::constexpr_compatible) {
  using value_type = typename TypeParam::value_type;
  struct with_constructor_args {
    value_type first;
    value_type second;
    value_type third;

    with_constructor_args() = default;

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr with_constructor_args(value_type&& fst, value_type&& snd, value_type&& trd)
        : first(std::move(fst)),
          second(std::move(snd)),
          third(std::move(trd)) {}
  };

  constexpr auto vec =
      constexpr_emplace_back<typename TestFixture::template standard_capacity_vector<with_constructor_args>>(
          TestFixture::test_value(0),
          TestFixture::test_value(1),
          TestFixture::test_value(2)
      );

  STATIC_ASSERT_EQ(vec.size(), 1);
  STATIC_ASSERT_EQ(vec[0].first, TestFixture::test_value(0));
  STATIC_ASSERT_EQ(vec[0].second, TestFixture::test_value(1));
  STATIC_ASSERT_EQ(vec[0].third, TestFixture::test_value(2));
}

template <typename T>
struct throws_without_nullptr {
  explicit throws_without_nullptr(T const&) {}
  throws_without_nullptr(T const&, std::nullptr_t) noexcept {}
};

/// @test `emplace_back` is `noexcept` if and only if the element type of the vector is nothrow-constructible from the
/// supplied arguments
TYPED_TEST_P(InlineVectorTest, EmplaceBackIsConditionallyNoexceptRvalue) {
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace_back(std::declval<typename TypeParam::value_type&&>())),
      std::is_nothrow_move_constructible<typename TypeParam::value_type>{}
  );
}

/// @test `emplace_back` is `noexcept` if and only if the element type of the vector is nothrow-constructible from the
/// supplied arguments
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    EmplaceBackIsConditionallyNoexceptLvalue,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  using value_type = typename TypeParam::value_type;
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace_back(std::declval<value_type const&>())),
      std::is_nothrow_copy_constructible<value_type>{}
  );

  STATIC_ASSERT_FALSE(
      noexcept(typename TestFixture::template standard_capacity_vector<throws_without_nullptr<value_type>>{}
                   .emplace_back(std::declval<value_type const&>()))
  );
  STATIC_ASSERT_TRUE(
      noexcept(typename TestFixture::template standard_capacity_vector<throws_without_nullptr<value_type>>{}
                   .emplace_back(std::declval<value_type const&>(), nullptr))
  );
}

/// @test `emplace` with a position is `noexcept` if and only if the element type of the vector is
/// nothrow-move-constructible, nothrow-move-assignable, and nothrow-constructible from the supplied arguments
TYPED_TEST_P(InlineVectorTest, EmplaceAtPositionIsConditionallyNoexceptRvalue) {
  using value_type = typename TypeParam::value_type;
  using const_it = typename TypeParam::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<value_type>{} && std::is_nothrow_move_assignable<value_type>{};

  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace(std::declval<const_it>(), std::declval<value_type&&>())),
      is_nothrow_movable
  );

  using overload_test_vector =
      typename TestFixture::template standard_capacity_vector<throws_without_nullptr<value_type>>;
  using overload_it = typename overload_test_vector::const_iterator;
  STATIC_ASSERT_FALSE(noexcept(overload_test_vector{}.emplace(std::declval<overload_it>(), std::declval<value_type>()))
  );
  STATIC_ASSERT_TRUE(
      noexcept(overload_test_vector{}.emplace(std::declval<overload_it>(), std::declval<value_type>(), nullptr))
  );
}

/// @test Invoking `emplace` inserts a new value at the position indicated, constructing the new value with the supplied
/// arguments, and returning an iterator referencing the new value
TYPED_TEST_P(InlineVectorTest, EmplaceInsert) {
  using value_type = typename TypeParam::value_type;
  struct with_constructor_args {
    std::reference_wrapper<value_type const> value;
    value_type* raw_pointer;
    move_only_wrapper<value_type> wrapper;

    with_constructor_args(value_type const& val, value_type* rptr, move_only_wrapper<value_type> wrap_arg)
        : value(val),
          raw_pointer(rptr),
          wrapper(std::move(wrap_arg)) {}
  };

  typename TestFixture::template standard_capacity_vector<with_constructor_args> vec;
  auto const& const_vec1 = vec;
  value_type const value1 = TestFixture::test_value(0);
  value_type value2 = TestFixture::test_value(1);
  auto pos = vec.emplace(
      const_vec1.begin(),
      value1,
      std::addressof(value2),
      move_only_wrapper<value_type>(TestFixture::test_value(2))
  );
  EXPECT_EQ(pos, vec.begin());
  EXPECT_EQ(vec.size(), 1);
  EXPECT_EQ(&*pos, std::addressof(vec[0]));

  EXPECT_EQ(pos->value.get(), value1);
  ASSERT_NE(pos->raw_pointer, nullptr);
  EXPECT_EQ(pos->raw_pointer, std::addressof(value2));
  EXPECT_EQ(pos->wrapper.get(), TestFixture::test_value(2));
}

template <typename Vec, typename Arg>
constexpr auto constexpr_emplace(Vec&& vec, std::ptrdiff_t pos, Arg&& arg) -> Vec {
  static_assert(std::is_rvalue_reference<Vec&&>::value, "Only expected to be called with an r-value reference.");
  std::forward<Vec>(vec).emplace(std::forward<Vec>(vec).begin() + pos, std::forward<Arg>(arg));
  return std::forward<Vec>(vec);
}

/// @test `emplace` can be invoked in a `constexpr` context to construct an element at a specified index.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanEmplace,
    InlineVectorTest<TypeParam>::constexpr_compatible&& TypeParam::capacity >= 3
) {
  constexpr auto vec1 = TestFixture::test_vector(2);

  struct convertible_to_typeparam {
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    constexpr operator typename TypeParam::value_type() noexcept { return TestFixture::test_value(2); }
  };

  constexpr TypeParam vec2 = constexpr_emplace(TestFixture::test_vector(2), 1, convertible_to_typeparam{});

  STATIC_ASSERT_EQ(vec2.size(), vec1.size() + 1);
  STATIC_ASSERT_EQ(vec2[0], vec1[0]);
  STATIC_ASSERT_EQ(vec2[1], static_cast<typename TypeParam::value_type>(convertible_to_typeparam{}));
  STATIC_ASSERT_EQ(vec2[2], vec1[1]);
}

/// @test `emplace` with a position is `noexcept` if and only if the element type of the vector is
/// nothrow-move-constructible, nothrow-move-assignable, and nothrow-constructible from an lvalue argument
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    EmplaceAtPositionIsConditionallyNoexceptLvalue,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  using value_type = typename TypeParam::value_type;
  using const_it = typename TypeParam::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<value_type>{} && std::is_nothrow_move_assignable<value_type>{};

  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace(std::declval<const_it>(), std::declval<value_type const&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<value_type>{}
  );
}

/// @test Emplacing using the default constructor has the right exception specification. This test is separate from the
/// ones above due to a bug in GCC8 which tries to instantiate the `emplace_back` and `emplace` functions before
/// checking their exception specifications, which fails for types that are not default-constructible.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    DefaultEmplaceIsConditionallyNoexcept,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  using value_type = typename TypeParam::value_type;
  using const_it = typename TypeParam::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<value_type>{} && std::is_nothrow_move_assignable<value_type>{};

  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace_back()),
      std::is_nothrow_default_constructible<value_type>{}
  );
  STATIC_ASSERT_EQ(
      noexcept(TestFixture::construct().emplace(std::declval<const_it>())),
      is_nothrow_movable && std::is_nothrow_default_constructible<value_type>{}
  );
}

/// @test Invoking `emplace_back` on an `inline_vector<T>` which has a size equal to its capacity terminates the program
/// with a precondition violation
TYPED_TEST_P(InlineVectorDeathTest, EmplaceBackIsPreconditionViolationIfFull) {
  auto vec = TestFixture::test_vector(TypeParam::capacity);

  ASSERT_DEATH(vec.emplace_back(TestFixture::test_value(0)), "Precondition violation");
}

/// @test Invoking `swap` without a namespace qualification on two `inline_vector` instances exchanges the contents, so
/// the elements from one have been moved to the other, and vice-versa.
TYPED_TEST_P(InlineVectorTest, ADLSwapSwapsContentsForBasicTypes) {
  auto value1 = TestFixture::full_test_vector();
  auto value2 = TestFixture::test_vector(TypeParam::capacity, TypeParam::capacity + TypeParam::capacity / 2UL);

  swap(value1, value2);

  EXPECT_EQ(value1, TestFixture::test_vector(TypeParam::capacity, TypeParam::capacity + TypeParam::capacity / 2UL));
  EXPECT_EQ(value2, TestFixture::full_test_vector());
}

template <typename Vec>
constexpr auto constexpr_swap(Vec lhs, Vec rhs) -> std::pair<Vec, Vec> {
  swap(lhs, rhs);
  return {lhs, rhs};
}

/// @test In a `constexpr` context, two `inline_vector` instances can be swapped with an unqualified call to `swap`.
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanSwap, InlineVectorTest<TypeParam>::constexpr_compatible) {
  constexpr auto value1 = TestFixture::full_test_vector();
  constexpr auto value2 =
      TestFixture::test_vector(TypeParam::capacity, TypeParam::capacity + TypeParam::capacity / 2UL);

  constexpr auto vec_swapped = constexpr_swap(value1, value2);
  STATIC_ASSERT_EQ(vec_swapped.first, value2);
  STATIC_ASSERT_EQ(vec_swapped.second, value1);
}

/// @test Swapping two `inline_vector`s is `noexcept` if and only if the element
/// type is nothrow swappable AND the element type is nothrow move constructible
TYPED_TEST_P(InlineVectorTest, SwapNoexceptMatchesElementType) {
  constexpr bool element_nothrow = ::arene::base::is_nothrow_swappable_v<typename TypeParam::value_type> &&
                                   std::is_nothrow_move_constructible<typename TypeParam::value_type>::value;

  STATIC_ASSERT_EQ(::arene::base::is_nothrow_swappable_v<TypeParam>, element_nothrow);
  STATIC_ASSERT_EQ(noexcept(std::declval<TypeParam>().swap(std::declval<TypeParam&>())), element_nothrow);
}

/// @test `insert` of an rvalue is `noexcept` if the element type of the vector is nothrow-constructible from the
/// supplied value, and is nothrow-move-constructible and nothrow-move-assignable
TYPED_TEST_P(InlineVectorTest, InsertRvalueIsConditionallyNoexcept) {
  using value_type = typename TypeParam::value_type;
  using iterator = typename TypeParam::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<value_type>{} && std::is_nothrow_move_assignable<value_type>{};

  STATIC_ASSERT_EQ(
      noexcept(TypeParam{}.insert(std::declval<iterator>(), std::declval<value_type&&>())),
      is_nothrow_movable
  );
}

/// @test `insert` of an lvalue is `noexcept` if the element type of the vector is nothrow-constructible from the
/// supplied value, and is nothrow-move-constructible and nothrow-move-assignable
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, InsertLvalueIsConditionallyNoexcept, is_copyable_v<typename TypeParam::value_type>) {
  using value_type = typename TypeParam::value_type;
  using iterator = typename TypeParam::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<value_type>{} && std::is_nothrow_move_assignable<value_type>{};

  STATIC_ASSERT_EQ(
      noexcept(TypeParam{}.insert(std::declval<iterator>(), std::declval<value_type const&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<value_type>::value
  );
}

/// @test `insert` is `noexcept` if the element type of the vector is nothrow-copy-constructible, and is
/// nothrow-move-constructible and nothrow-move-assignable
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, InsertWithCountIsConditionallyNoexcept, is_copyable_v<typename TypeParam::value_type>) {
  using value_type = typename TypeParam::value_type;
  using iterator = typename TypeParam::const_iterator;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<value_type>{} && std::is_nothrow_move_assignable<value_type>{};

  STATIC_ASSERT_EQ(
      noexcept(TypeParam{}.insert(std::declval<iterator>(), 0U, std::declval<value_type&&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<value_type>{}
  );
  STATIC_ASSERT_EQ(
      noexcept(TypeParam{}.insert(std::declval<iterator>(), 0U, std::declval<value_type const&>())),
      is_nothrow_movable && std::is_nothrow_copy_constructible<value_type>{}
  );
}

/// @test Invoking `insert` with a count and a value to insert more elements than there are remaining spaces in the
/// vector before it exceeds capacity terminates the program with a precondition violation.
CONDITIONAL_TYPED_TEST_P(InlineVectorDeathTest, InsertNIsPreconditionViolationIfTooManyItems, is_copyable_v<typename TypeParam::value_type>) {
  auto value = TestFixture::test_vector(TypeParam::capacity - 1UL);
  ASSERT_DEATH(value.insert(value.begin(), 2UL, TestFixture::test_value(5)), "Precondition violation");
}

/// @test Invoking `insert` with the end iterator of the `inline_vector` and a count and value appends the specified
/// number of copies of the supplied value to the end of the vector.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    InsertNAtEndInsertsElements,
    is_copyable_v<typename TypeParam::value_type>&& TypeParam::capacity >= 7
) {
  auto const value1 = TestFixture::test_value(0);
  auto const value2 = TestFixture::test_value(1);
  auto const value3 = TestFixture::test_value(2);
  TypeParam vec{value1, value2};
  constexpr std::size_t insert_count = 5;

  vec.insert(vec.end(), insert_count, value3);
  EXPECT_EQ(vec.size(), insert_count + 2);
  EXPECT_EQ(vec[0], value1);
  EXPECT_EQ(vec[1], value2);
  for (std::size_t i = 0; i < insert_count; ++i) {
    EXPECT_EQ(vec[i + 2], value3);
  }
}

/// @test Invoking `insert` with an iterator referring to an existing element of an `inline_vector` and a count and
/// value moves the referred to element and subsequent elements to make room, and inserts the specified number of copies
/// of the supplied value prior to the existing element.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    InsertNInMiddleInsertsElements,
    is_copyable_v<typename TypeParam::value_type>&& TypeParam::capacity >= 7
) {
  auto const value1 = TestFixture::test_value(0);
  auto const value2 = TestFixture::test_value(1);
  auto const value3 = TestFixture::test_value(2);
  auto const value4 = TestFixture::test_value(3);
  auto const value_inserted = TestFixture::test_value(4);
  TypeParam vec{value1, value2, value3, value4};
  constexpr std::size_t insert_count = 3;

  vec.insert(vec.begin() + 1, insert_count, value_inserted);
  EXPECT_EQ(vec.size(), insert_count + 4);
  EXPECT_EQ(vec[0], value1);
  for (std::size_t i = 0; i < insert_count; ++i) {
    EXPECT_EQ(vec[i + 1], value_inserted);
  }
  EXPECT_EQ(vec[insert_count + 1], value2);
  EXPECT_EQ(vec[insert_count + 2], value3);
  EXPECT_EQ(vec[insert_count + 3], value4);
}

/// @test Invoking `insert` with an iterator referring to an existing element of an `inline_vector` and a source
/// iterator range denoted by a pair of iterators moves the referred to element and subsequent elements to make room,
/// and inserts the elements from the source iterator range prior to the existing element.
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, InsertFromIteratorRangeInMiddle, TypeParam::capacity >= 11) {
  auto vec1 = TestFixture::test_vector(4);
  auto vec2 = TestFixture::test_vector(4, 11);
  auto const vec2_expected = TestFixture::test_vector(4, 11);

  vec1.insert(vec1.begin() + 1, std::make_move_iterator(vec2.begin()), std::make_move_iterator(vec2.end()));
  EXPECT_EQ(vec1.size(), vec2.size() + 4);
  EXPECT_EQ(vec1[0], TestFixture::test_value(0));
  for (std::size_t idx = 0; idx < vec2.size(); ++idx) {
    EXPECT_EQ(vec1[idx + 1], vec2_expected[idx]);
  }
  EXPECT_EQ(vec1[vec2_expected.size() + 1], TestFixture::test_value(1));
  EXPECT_EQ(vec1[vec2_expected.size() + 2], TestFixture::test_value(2));
  EXPECT_EQ(vec1[vec2_expected.size() + 3], TestFixture::test_value(3));
}

/// @test `insert` is `noexcept` if the element type of the vector is nothrow-constructible from the supplied iterator's
/// reference type, and is nothrow-move-constructible and nothrow-move-assignable
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, InsertFromIteratorRangeIsConditionallyNoexcept, is_copyable_v<typename TypeParam::value_type>) {
  using value_type = typename TypeParam::value_type;
  using ivec_iter = typename TypeParam::const_iterator;
  using source_array = ::arene::base::array<value_type, TypeParam::capacity>;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throw_conv_array = ::arene::base::array<throws_when_converted, TypeParam::capacity>;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<value_type>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type = typename TestFixture::template non_throwing_input_iterator<value_type>;

  constexpr bool is_nothrow_movable =
      std::is_nothrow_move_constructible<value_type>{} && std::is_nothrow_move_assignable<value_type>{};

  STATIC_ASSERT_EQ(
      noexcept(this->construct().insert(
          std::declval<ivec_iter>(),
          std::declval<typename source_array::const_iterator>(),
          std::declval<typename source_array::const_iterator>()
      )),
      is_nothrow_movable && std::is_nothrow_copy_constructible<value_type>{}
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
      is_nothrow_movable && std::is_nothrow_copy_constructible<value_type>{}
  );
  STATIC_ASSERT_FALSE(noexcept(this->construct().insert(
      std::declval<ivec_iter>(),
      std::declval<::testing::demoted_iterator<throwing_it_throwing_type, std::input_iterator_tag>>(),
      std::declval<::testing::demoted_iterator<throwing_it_throwing_type, std::input_iterator_tag>>()
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

/// @test `cbegin` and `cend` return `const_iterator`s to the corresponding locations of the `inline_vector`.
TYPED_TEST_P(InlineVectorTest, CanGetCBeginAndEnd) {
  auto const value1 = TestFixture::full_test_vector();

  ASSERT_EQ(value1.cbegin(), value1.begin());
  ASSERT_EQ(value1.cend(), value1.end());

  ::testing::StaticAssertTypeEq<decltype(value1.cbegin()), typename TypeParam::const_iterator>();
  STATIC_ASSERT_TRUE(noexcept(value1.cbegin()));
  STATIC_ASSERT_TRUE(noexcept(value1.cend()));

  auto value2 = TestFixture::full_test_vector();
  ::testing::StaticAssertTypeEq<decltype(value2.cbegin()), typename TypeParam::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(value2.cend()), typename TypeParam::const_iterator>();

  EXPECT_EQ(value2.begin(), value2.cbegin());
  EXPECT_EQ(value2.end(), value2.cend());
}

/// @test `rbegin`, `rend`, `crbegin` and `crend` return reverse iterators to the appropriate locations in the
/// `inline_vector`, such that iteration _forwards_ from `rbegin` or `crbegin` iterates through the elements of the
/// `inline_vector` in reverse order.
TYPED_TEST_P(InlineVectorTest, ReverseIteration) {
  auto const value1 = TestFixture::full_test_vector();

  auto itr = value1.rbegin();
  auto citr = value1.crbegin();
  for (std::size_t steps = 0UL; steps < value1.size(); ++steps) {
    ASSERT_NE(itr, value1.rend());
    ASSERT_NE(citr, value1.crend());

    EXPECT_EQ(*itr, value1[value1.size() - steps - 1UL]);
    EXPECT_EQ(*citr, value1[value1.size() - steps - 1UL]);

    ++itr;
    ++citr;
  }
  EXPECT_EQ(itr, value1.rend());
  EXPECT_EQ(citr, value1.crend());
}

/// @test An `inline_vector` can be constructed from an `inline_vector` with a larger capacity, provided the number of
/// elements in the source vector is less than the capacity of the target vector
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanConstructFromBiggerCapacityVector,
    InlineVectorTest<TypeParam>::constexpr_compatible&&
        std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  constexpr std::size_t larger_capacity = TypeParam::capacity + 2UL;
  constexpr typename TestFixture::template vector_with_capacity<larger_capacity> large_vector{
      TestFixture::template test_initializer_list<TypeParam::capacity - 1UL>()
  };

  constexpr TypeParam small_vector(large_vector);

  STATIC_ASSERT_EQ(small_vector.size(), large_vector.size());
  STATIC_ASSERT_TRUE(equal(small_vector.begin(), small_vector.end(), large_vector.begin(), large_vector.end()));
}

/// @test An `inline_vector` can be assigned from an `inline_vector` with a larger capacity, provided the number of
/// elements in the source vector is less than the capacity of the target vector
CONDITIONAL_TYPED_TEST_P(InlineVectorTest, CanAssignFromBiggerCapacityVector, is_copyable_v<typename TypeParam::value_type>) {
  constexpr std::size_t larger_capacity = TypeParam::capacity + 2UL;
  typename TestFixture::template vector_with_capacity<larger_capacity> const large_vector{
      TestFixture::template test_initializer_list<TypeParam::capacity - 1UL>()
  };

  TypeParam small_vector;
  small_vector = large_vector;

  EXPECT_EQ(small_vector.size(), large_vector.size());
  for (std::size_t i = 0; i < small_vector.size(); ++i) {
    EXPECT_EQ(small_vector[i], large_vector[i]);
  }
}

/// @test `try_construct` can be used to construct an empty `inline_vector`.
TYPED_TEST_P(InlineVectorTest, CanDefaultConstructViaTryConstruct) {
  STATIC_ASSERT_TRUE(std::is_same<decltype(TypeParam::try_construct()), ::arene::base::optional<TypeParam>>::value);
  STATIC_ASSERT_TRUE(noexcept(TypeParam::try_construct()));

  COND_STATIC_ASSERT_TRUE(TypeParam::try_construct().has_value());
  COND_STATIC_ASSERT_TRUE(TypeParam::try_construct()->empty());
}

/// @test `try_construct` can be used to construct an `inline_vector` with a specified number of elements
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanConstructWithSizeViaTryConstruct,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  STATIC_ASSERT_TRUE(std::is_same<decltype(TypeParam::try_construct(0)), ::arene::base::optional<TypeParam>>::value);
  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(0)),
      std::is_nothrow_default_constructible<typename TypeParam::value_type>::value
  );

  COND_STATIC_ASSERT_TRUE(TypeParam::try_construct(TypeParam::capacity).has_value());
  COND_STATIC_ASSERT_FALSE(TypeParam::try_construct(TypeParam::capacity)->empty());
  COND_STATIC_ASSERT_EQ(TypeParam::try_construct(TypeParam::capacity)->size(), TypeParam::capacity);
}

/// @test `try_construct` can be used to attempt to construct an `inline_vector` with a specified number of elements
/// that exceeds the capacity of the vector, in which case the result is an empty `optional`.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ConstructWithExcessiveSizeViaTryConstructFails,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  constexpr std::size_t size1 = TypeParam::capacity + 1UL;
  STATIC_ASSERT_TRUE(std::is_same<decltype(TypeParam::try_construct(size1)), ::arene::base::optional<TypeParam>>::value
  );
  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(size1)),
      std::is_nothrow_default_constructible<typename TypeParam::value_type>::value
  );

  COND_STATIC_ASSERT_FALSE(TypeParam::try_construct(size1).has_value());
}

/// @test `try_construct` with a number of elements is not considered for overload resolution if the element type of the
/// `inline_vector<T>` is not default-constructible.
TYPED_TEST_P(InlineVectorTest, CannotTryConstructWithSizeForANonDefaultConstructibleType) {
  constexpr bool can_construct_with_size_only =
      ::arene::base::substitution_succeeds<TestFixture::template try_construct_result, TypeParam, std::size_t>;
  STATIC_ASSERT_EQ(can_construct_with_size_only, std::is_default_constructible<typename TypeParam::value_type>{});
}

/// @test The `inline_vector` constructor with a number of elements is not considered for overload resolution if the
/// element type of the `inline_vector<T>` is not default-constructible.
TYPED_TEST_P(InlineVectorTest, CannotConstructWithSizeForANonDefaultConstructibleType) {
  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, std::size_t>::value),
      std::is_default_constructible<typename TypeParam::value_type>{}
  );
}

/// @test The default constructor and corresponding `try_construct` function of `inline_vector<T>` is always `noexcept`,
/// even if the stored element type's default constructor can throw
TYPED_TEST_P(InlineVectorTest, DefaultTryConstructNoexceptEvenIfDefaultConstructorNotNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(TypeParam::try_construct()));
  STATIC_ASSERT_TRUE(noexcept(TypeParam{}));

  COND_STATIC_ASSERT_TRUE(TypeParam::try_construct().has_value());
  COND_STATIC_ASSERT_TRUE(TypeParam::try_construct()->empty());
  COND_STATIC_ASSERT_TRUE(TypeParam().empty());
}

/// @test The `try_construct` function of `inline_vector` that takes a number of elements is not `noexcept` if the
/// default constructor of the stored element type is not `noexcept`
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ConstructWithSizeViaTryConstructNotNoexceptIfDefaultConstructorNotNoexcept,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  STATIC_ASSERT_TRUE(std::is_same<decltype(TypeParam::try_construct(0)), ::arene::base::optional<TypeParam>>::value);
  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(0)),
      std::is_nothrow_default_constructible<typename TypeParam::value_type>{}
  );
}

/// @test The constructor of `inline_vector` that takes a number of elements is not `noexcept` if the default
/// constructor of the stored element type is not `noexcept`
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    ConstructWithSizeViaNormalConstructWithDefaultConstructorNotNoexceptIsNotNoexcept,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  STATIC_ASSERT_EQ(noexcept(TypeParam(0)), std::is_nothrow_default_constructible<typename TypeParam::value_type>{});
}

/// @test Specifying a number of elements larger than the capacity of an `inline_vector` when invoking the
/// constructor terminates the program with a precondition violation
CONDITIONAL_TYPED_TEST_P(
    InlineVectorDeathTest,
    ConstructWithExcessiveSizeViaNormalConstructIsAlwaysPreconditionViolation,
    std::is_default_constructible<typename TypeParam::value_type>{}
) {
  constexpr std::size_t size1 = TypeParam::capacity + 1UL;
  ASSERT_DEATH(this->construct(size1), "Precondition violation");  // dies regardless of exception behavior
}

/// @test The overload of `try_construct` that takes a size and source value is not considered for overload resolution
/// if the element type of the `inline_vector<T>` is not copy-constructible.
TYPED_TEST_P(InlineVectorTest, NotConstructibleWithSizeAndSourceIfDataNotCopyable) {
  using value_type = typename TypeParam::value_type;
  constexpr bool copy_ok = std::is_copy_constructible<value_type>::value;
  STATIC_ASSERT_EQ((std::is_constructible<TypeParam, std::size_t, value_type>::value), copy_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::
           substitution_succeeds<TestFixture::template try_construct_result, TypeParam, std::size_t, value_type>),
      copy_ok
  );
}

/// @test The overload of `try_construct` that takes an initializer list is not considered for overload resolution if
/// the element type of the `inline_vector` is not copy-constructible.
TYPED_TEST_P(InlineVectorTest, NotConstructibleFromInitializerListIfDataTypeNotCopyConstructible) {
  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, std::initializer_list<typename TypeParam::value_type>>::value),
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_EQ(
      (arene::base::substitution_succeeds<
          TestFixture::template try_construct_result,
          TypeParam,
          std::initializer_list<typename TypeParam::value_type>>),
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );
}

/// @test Invoking `try_construct` with an initializer list constructs an `inline_vector` holding copies of the elements
/// from the initializer list
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanTryConstructFromInitializerList,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const values = TestFixture::full_test_initializer_list();

  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(values)),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>{}
  );
  ASSERT_TRUE(TypeParam::try_construct(values).has_value());
  ASSERT_EQ(TypeParam::try_construct(values)->size(), values.size());

  auto res = TypeParam::try_construct(values);
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an initializer list holding more elements than the capacity of the
/// `inline_vector` returns an empty `optional`
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    TryConstructFromTooLargeInitializerListReturnsEmpty,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const values = TestFixture::template test_initializer_list<TypeParam::capacity + 1UL>();

  ASSERT_FALSE(TypeParam::try_construct(values).has_value());
}

/// @test Invoking `try_construct` with an existing `inline_vector` returns a copy of that source vector
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCopyViaTryConstruct,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  using value_type = typename TypeParam::value_type;
  auto const values = TestFixture::full_test_vector();

  STATIC_ASSERT_TRUE(::arene::base::substitution_succeeds<
                     TestFixture::template try_construct_result,
                     TypeParam,
                     TypeParam const&>);
  STATIC_ASSERT_EQ(noexcept(TypeParam::try_construct(values)), std::is_nothrow_copy_constructible<value_type>::value);
  auto res = TypeParam::try_construct(values);
  ASSERT_TRUE(res.has_value());
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Constructing an `inline_vector` with an existing `inline_vector` copies the elements from the source vector
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCopyViaConstruct,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const values = TestFixture::full_test_vector();

  STATIC_ASSERT_EQ(noexcept(TypeParam(values)), std::is_nothrow_copy_constructible<typename TypeParam::value_type>{});
  auto res = this->construct(values);
  EXPECT_THAT(res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an existing `inline_vector` with a smaller capacity returns an `inline_vector`
/// with the same elements as the source vector
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCopyViaTryConstructFromSmaller,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  // typename TestFixture::template vector_with_capacity<TypeParam::capacity - 1> const values{
  //     TestFixture::test_vector(TypeParam::capacity - 1)
  // };

  auto const values = ::arene::base::testing::test_vector<typename TypeParam::value_type, TypeParam::capacity - 1>(
      0,
      TypeParam::capacity - 1
  );

  STATIC_ASSERT_TRUE(::arene::base::substitution_succeeds<
                     TestFixture::template try_construct_result,
                     TypeParam,
                     TypeParam const&>);
  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(values)),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value
  );
  ARENE_IGNORE_START();
  ARENE_IGNORE_GCC(
      "-Wmaybe-uninitialized",
      "GCC issues false positive here when compiling with -fno-exceptions and using GCC's stdlib"
  );
  auto res = TypeParam::try_construct(values);
  ARENE_IGNORE_END();
  ASSERT_TRUE(res.has_value());
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Constructing an `inline_vector` from an existing `inline_vector` with a smaller capacity returns an
/// `inline_vector` with the same elements as the source vector
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCopyViaConstructFromSmaller,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  static typename TestFixture::template vector_with_capacity<TypeParam::capacity - 1> const values{
      TestFixture::test_vector(TypeParam::capacity - 1)
  };

  STATIC_ASSERT_EQ(noexcept(TypeParam(values)), std::is_nothrow_copy_constructible<typename TypeParam::value_type>{});
  auto res = this->construct(values);
  EXPECT_THAT(res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an existing `inline_vector` with a larger capacity but a size that is less than
/// or equal to the capacity of the destination type returns an `inline_vector` with the same elements as the source
/// vector
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCopyViaTryConstructFromLargerWithFewerElements,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const values = ::arene::base::testing::test_vector<typename TypeParam::value_type, TypeParam::capacity + 1>(
      0,
      TypeParam::capacity
  );

  STATIC_ASSERT_TRUE(::arene::base::substitution_succeeds<
                     TestFixture::template try_construct_result,
                     TypeParam,
                     TypeParam const&>);
  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(values)),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value
  );
  auto res = TypeParam::try_construct(values);
  ASSERT_TRUE(res.has_value());
  EXPECT_THAT(*res, ::testing::ElementsAreArray(values));
}

/// @test Invoking `try_construct` with an existing `inline_vector` with a larger capacity and a size that is larger
/// than the capacity of the destination type returns an empty `optional`
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    FailToCopyViaTryConstructFromLargerWithMoreElements,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const values = ::arene::base::testing::test_vector<typename TypeParam::value_type, TypeParam::capacity + 1>(
      0,
      TypeParam::capacity + 1
  );

  ASSERT_GT(values.size(), TypeParam::capacity);
  ASSERT_FALSE(TypeParam::try_construct(values).has_value());
}

/// @test Constructing an `inline_vector` from an existing `inline_vector` with a larger capacity but a size that is
/// less than or equal to the capacity of the destination type copies elements from the source vector
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    CanCopyViaConstructFromLargerWithFewerElements,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  auto const values = ::arene::base::testing::test_vector<typename TypeParam::value_type, TypeParam::capacity + 1>(
      0,
      TypeParam::capacity
  );
  constexpr bool copy_is_noexcept = std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value;

  STATIC_ASSERT_EQ(noexcept(TypeParam(values)), copy_is_noexcept);
  auto res = this->construct(values);
  EXPECT_THAT(res, ::testing::ElementsAreArray(values));
}

/// @test `inline_vector` is not copy-constructible if the data type is not copy-constructible
TYPED_TEST_P(InlineVectorTest, NotCopyConstructibleIfDataTypeNotCopyConstructible) {
  using smaller_vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity - 1>;
  using larger_vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity + 1>;
  constexpr bool copy_ok = std::is_copy_constructible<typename TypeParam::value_type>::value;

  STATIC_ASSERT_EQ((std::is_constructible<TypeParam, TypeParam const&>::value), copy_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, TypeParam, TypeParam const&>),
      copy_ok
  );
  STATIC_ASSERT_EQ((std::is_constructible<TypeParam, smaller_vec const&>::value), copy_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, TypeParam, smaller_vec const&>),
      copy_ok
  );
  STATIC_ASSERT_EQ((std::is_constructible<TypeParam, larger_vec const&>::value), copy_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, TypeParam, larger_vec const&>),
      copy_ok
  );
}

/// @test Invoking `try_construct` with an rvalue `inline_vector` constructs a new `inline_vector`, and move-constructs
/// the elements from the source to the new vector.
TYPED_TEST_P(InlineVectorTest, CanMoveViaTryConstruct) {
  auto const values = TestFixture::full_test_vector();
  auto values_copy = TestFixture::full_test_vector();

  STATIC_ASSERT_TRUE(::arene::base::
                         substitution_succeeds<TestFixture::template try_construct_result, TypeParam, TypeParam&&>);
  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(std::move(values_copy))),
      std::is_nothrow_move_constructible<typename TypeParam::value_type>::value
  );
  auto res = TypeParam::try_construct(std::move(values_copy));
  ASSERT_TRUE(res.has_value());
  EXPECT_TRUE(elements_are_array(*res, values));
}

/// @test Constructing an `inline_vector` from an rvalue `inline_vector` move-constructs the elements from the source to
/// the new vector.
TYPED_TEST_P(InlineVectorTest, CanMoveViaConstruct) {
  auto const values = TestFixture::full_test_vector();
  auto source = TestFixture::full_test_vector();

  // NOLINTNEXTLINE(hicpp-move-const-arg)
  STATIC_ASSERT_EQ(
      noexcept(TypeParam(std::move(source))),
      std::is_nothrow_move_constructible<typename TypeParam::value_type>::value
  );
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  auto res = this->construct(std::move(source));
  EXPECT_TRUE(elements_are_array(res, values));
}

/// @test Invoking `try_construct` with an rvalue `inline_vector` with a larger capacity and a size that is larger
/// than the capacity of the destination type returns an empty `optional`
TYPED_TEST_P(InlineVectorTest, FailToMoveViaTryConstructFromLargerWithMoreElements) {
  auto values = ::arene::base::testing::test_vector<typename TypeParam::value_type, TypeParam::capacity + 1>(
      0,
      TypeParam::capacity + 1
  );

  ASSERT_GT(values.size(), TypeParam::capacity);

  ASSERT_FALSE(TypeParam::try_construct(std::move(values)).has_value());
}

/// @test `inline_vector` is not move constructible if the element type is not move constructible
TYPED_TEST_P(InlineVectorTest, NotMoveConstructibleIfDataTypeNotMoveConstructible) {
  using ivec = typename TestFixture::template vector_with_capacity<TypeParam::capacity>;
  using smaller_vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity - 1>;
  using larger_vec = typename TestFixture::template vector_with_capacity<TypeParam::capacity + 1>;
  constexpr bool move_ok = std::is_move_constructible<typename TypeParam::value_type>::value;

  STATIC_ASSERT_EQ((std::is_constructible<ivec, ivec&&>::value), move_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, ivec, ivec&&>),
      move_ok
  );
  STATIC_ASSERT_EQ((std::is_constructible<ivec, smaller_vec&&>::value), move_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, ivec, smaller_vec&&>),
      move_ok
  );
  STATIC_ASSERT_EQ((std::is_constructible<ivec, larger_vec&&>::value), move_ok);
  STATIC_ASSERT_EQ(
      (::arene::base::substitution_succeeds<TestFixture::template try_construct_result, ivec, larger_vec&&>),
      move_ok
  );
}

/// @test Invoking `try_construct` with an iterator range returns an `inline_vector` with the same elements as the
/// source range
TYPED_TEST_P(InlineVectorTest, CanTryConstructFromIteratorRange) {
  auto const expected = TestFixture::full_test_vector();
  auto source = TestFixture::full_test_vector();

  auto maybe_vec =
      TypeParam::try_construct(std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()));
  ASSERT_TRUE(maybe_vec.has_value());

  ASSERT_EQ(maybe_vec->size(), source.size());
  ASSERT_TRUE(elements_are_array(*maybe_vec, expected));
}

/// @test `try_construct` from an iterator range is `noexcept` if the element type of the vector is
/// nothrow-constructible from the iterator's reference type
// Note: std::move_iterator is not marked noexcept, so this test requires the type to be copy constructible.
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    IteratorRangeTryConstructionIsConditionallyNoexcept,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  using array = ::arene::base::array<typename TypeParam::value_type, TypeParam::capacity>;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using convertible_array = ::arene::base::array<throws_when_converted, TypeParam::capacity>;
  using throwing_bidirectional_iterator = typename TestFixture::throwing_bidirectional_move_iterator_of_t;

  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(
          std::declval<typename array::const_iterator>(),
          std::declval<typename array::const_iterator>()
      )),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_FALSE(noexcept(TypeParam::try_construct(
      std::declval<typename convertible_array::const_iterator>(),
      std::declval<typename convertible_array::const_iterator>()
  )));
  STATIC_ASSERT_FALSE(noexcept(TypeParam::try_construct(
      std::declval<throwing_bidirectional_iterator>(),
      std::declval<throwing_bidirectional_iterator>()
  )));
}

/// @test `try_construct` from an input iterator range is `noexcept` if the element type of the vector is
/// nothrow-constructible from the iterator's reference type
CONDITIONAL_TYPED_TEST_P(
    InlineVectorTest,
    InputIteratorRangeTryConstructionIsConditionallyNoexcept,
    std::is_copy_constructible<typename TypeParam::value_type>::value
) {
  using value_type = typename TypeParam::value_type;
  using throws_when_converted = typename TestFixture::throws_when_converted_to_t;
  using throwing_it_throwing_type = typename TestFixture::template throwing_input_iterator<throws_when_converted>;
  using throwing_it_raw_type = typename TestFixture::template throwing_input_iterator<value_type>;
  using nothrow_it_throwing_type = typename TestFixture::template non_throwing_input_iterator<throws_when_converted>;
  using nothrow_it_raw_type = typename TestFixture::template non_throwing_input_iterator<value_type>;

  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(
          std::declval<::testing::demoted_iterator<value_type*, std::input_iterator_tag>>(),
          std::declval<::testing::demoted_iterator<value_type*, std::input_iterator_tag>>()
      )),
      std::is_nothrow_copy_constructible<value_type>::value
  );
  STATIC_ASSERT_FALSE(noexcept(
      TypeParam::try_construct(std::declval<throwing_it_throwing_type>(), std::declval<throwing_it_throwing_type>())
  ));
  STATIC_ASSERT_FALSE(
      noexcept(TypeParam::try_construct(std::declval<throwing_it_raw_type>(), std::declval<throwing_it_raw_type>()))
  );
  STATIC_ASSERT_EQ(
      noexcept(TypeParam::try_construct(std::declval<nothrow_it_raw_type>(), std::declval<nothrow_it_raw_type>())),
      std::is_nothrow_copy_constructible<value_type>::value
  );
  STATIC_ASSERT_FALSE(noexcept(
      TypeParam::try_construct(std::declval<nothrow_it_throwing_type>(), std::declval<nothrow_it_throwing_type>())
  ));
}

/// @test Dereferencing a default-constructed `inline_vector` iterator terminates the program with a precondition
/// violation
TYPED_TEST_P(InlineVectorDeathTest, DereferencingInvalidIteratorIsPreconditionViolation) {
  ASSERT_DEATH(*typename TypeParam::iterator{}, "Precondition violation");
  ASSERT_DEATH(*typename TypeParam::const_iterator{}, "Precondition violation");
}

/// @test Iterator arithmetic on a default-constructed `inline_vector` iterator terminates the program with a
/// precondition violation
TYPED_TEST_P(InlineVectorDeathTest, AddingToInvalidIteratorIsPreconditionViolation) {
  ASSERT_DEATH(typename TypeParam::iterator{} + 1, "Precondition violation");
  ASSERT_DEATH(typename TypeParam::const_iterator{} + 1, "Precondition violation");
}

ARENE_IGNORE_END();

REGISTER_TYPED_TEST_SUITE_P(
    InlineVectorTest,
    CanConstructAnEmptyVector,
    CapacityIsAsSpecified,
    CapacityIsNoexcept,
    InitialSizeIsZero,
    SizeIsConstexpr,
    SizeIsNoexcept,
    CanPushBackAValue,
    AfterPushBackCanGetLastElement,
    AfterPushBackVectorIsNotEmpty,
    SecondPushBackChangesLastValue,
    PushBackIsConditionallyNoexceptRvalue,
    PushBackIsConditionallyNoexceptLvalue,
    EmptyIsConstexpr,
    EmptyIsNoexcept,
    RetrievingAValueWithAtThrowsOnEmpty,
    RetrievingAValueWithAtReturnsValueAfterPushBack,
    AtReturnsReference,
    AtOnConstVectorReturnsConstReference,
    CanAccessMultipleValuesWithAt,
    AtIndexSizeOrLargerThrows,
    CanAccessMultipleValuesWithAtWithConstVec,
    AtIndexSizeOrLargerThrowsWithConstVec,
    MaxSizeEqualsCapacity,
    ZeroSizeVectorIsEmpty,
    BackReturnsReference,
    ConstBackReturnsConstReference,
    BackIsNoexcept,
    CanPushBack,
    CanConstructFromInitializerList,
    CanConstructFromInitializerListConstexpr,
    CanUseBack,
    CanGetFront,
    Front,
    CanGetIterator,
    CanIterateOverValues,
    CanIterateOverConstValues,
    IteratorTypedefs,
    Typedefs,
    Iteration,
    ConstIteration,
    CanEraseAtBeginning,
    CanEraseInMiddle,
    CanErase,
    CanEraseRange,
    CanConstructWithSize,
    CanConstructWithSizeAndValue,
    CopyingVectorCopiesElements,
    MovingVectorMovesElements,
    CopyAssignOverSmallerVectorCopiesElements,
    CopyAssignOverLargerVectorDestroysExcess,
    CopyingIsConditionallyDefined,
    MoveAssignOverSmallerVectorMovesElements,
    MoveAssignOverLargerVectorDestroysExcess,
    MovingIsConditionallyDefined,
    CanPopBack,
    PopBackIsNoexcept,
    PopBackIsUsable,
    EraseIsNoexceptIfTypeHasNoexceptMove,
    CanInsertInEmptyVector,
    CanInsertAtEndOfExistingVector,
    CanInsertInMiddleOfExistingVector,
    ResizeZeroCapacityVecToZeroIsOk,
    ResizeNonZeroCapacityVecToZeroIsOk,
    ResizeNonZeroCapacityVecToOtherIsOk,
    ResizeIsConditionallyNoexcept,
    ResizeWithValueIsConditionallyNoexcept,
    ResizingFromLargeToSmallLeavesElements,
    ResizingFromSmallToLargeConstructsElementsAsCopiesOfSpecified,
    CanInitializeWithSizeAndValue,
    CanInsert,
    CanConstructFromIteratorRange,
    IteratorRangeConstructionIsConditionallyNoexcept,
    InputIteratorRangeConstructionIsConditionallyNoexcept,
    CanConstructFromIteratorRangeConstexpr,
    CopyAssignOverSmallerVectorFromInitListCopiesElements,
    CopyAssignOverLargerVectorFromInitListDestroysExcess,
    CanAssignFromIteratorRange,
    IteratorRangeAssignmentIsConditionallyNoexcept,
    CanAssignFromInputIteratorRange,
    InputIteratorRangeAssignmentIsConditionallyNoexcept,
    CanAssignFromInitListUsingAssignFunction,
    AssignFromInitListUsingAssignFunctionIsConditionallyNoexcept,
    CanAssignFromSizeAndValue,
    AssignFromSizeAndValueIsConditionallyNoexcept,
    IndexOperatorReturnsReference,
    IndexOperatorOnConstVectorReturnsConstReference,
    CanAccessMultipleValuesWithIndexOperator,
    IndexOperatorIsNoexcept,
    IndexOperatorOnConstVectorIsNoexcept,
    IndexOperator,
    Data,
    AfterClearVectorIsEmpty,
    ClearWorks,
    CanCompareForEquality,
    CanCompareForOrdering,
    CanEmplaceBack,
    CanEmplaceBackConstexpr,
    EmplaceBackIsConditionallyNoexceptRvalue,
    EmplaceBackIsConditionallyNoexceptLvalue,
    EmplaceInsert,
    CanEmplace,
    EmplaceAtPositionIsConditionallyNoexceptRvalue,
    EmplaceAtPositionIsConditionallyNoexceptLvalue,
    DefaultEmplaceIsConditionallyNoexcept,
    ADLSwapSwapsContentsForBasicTypes,
    CanSwap,
    SwapNoexceptMatchesElementType,
    InsertRvalueIsConditionallyNoexcept,
    InsertLvalueIsConditionallyNoexcept,
    InsertWithCountIsConditionallyNoexcept,
    InsertNAtEndInsertsElements,
    InsertNInMiddleInsertsElements,
    InsertFromIteratorRangeInMiddle,
    InsertFromIteratorRangeIsConditionallyNoexcept,
    CanGetCBeginAndEnd,
    ReverseIteration,
    CanConstructFromBiggerCapacityVector,
    CanAssignFromBiggerCapacityVector,
    CanDefaultConstructViaTryConstruct,
    CanConstructWithSizeViaTryConstruct,
    ConstructWithExcessiveSizeViaTryConstructFails,
    CannotTryConstructWithSizeForANonDefaultConstructibleType,
    CannotConstructWithSizeForANonDefaultConstructibleType,
    DefaultTryConstructNoexceptEvenIfDefaultConstructorNotNoexcept,
    ConstructWithSizeViaTryConstructNotNoexceptIfDefaultConstructorNotNoexcept,
    ConstructWithSizeViaNormalConstructWithDefaultConstructorNotNoexceptIsNotNoexcept,
    NotConstructibleWithSizeAndSourceIfDataNotCopyable,
    NotConstructibleFromInitializerListIfDataTypeNotCopyConstructible,
    CanTryConstructFromInitializerList,
    TryConstructFromTooLargeInitializerListReturnsEmpty,
    CanCopyViaTryConstruct,
    CanCopyViaConstruct,
    CanCopyViaTryConstructFromSmaller,
    CanCopyViaConstructFromSmaller,
    CanCopyViaTryConstructFromLargerWithFewerElements,
    FailToCopyViaTryConstructFromLargerWithMoreElements,
    CanCopyViaConstructFromLargerWithFewerElements,
    NotCopyConstructibleIfDataTypeNotCopyConstructible,
    CanMoveViaTryConstruct,
    CanMoveViaConstruct,
    FailToMoveViaTryConstructFromLargerWithMoreElements,
    NotMoveConstructibleIfDataTypeNotMoveConstructible,
    CanTryConstructFromIteratorRange,
    IteratorRangeTryConstructionIsConditionallyNoexcept,
    InputIteratorRangeTryConstructionIsConditionallyNoexcept
);

REGISTER_TYPED_TEST_SUITE_P(
    InlineVectorDeathTest,
    PushBackBeyondMaxSizeIsPreconditionViolation,
    PushBackOnZeroSizeVectorIsPreconditionViolation,
    InsertWhenAtCapacityIsPreconditionViolation,
    ConstructingWithOutOfRangeSizeIsPreconditionViolation,
    ConstructingWithOutOfRangeSizeAndSourceIsPreconditionViolation,
    ConstructingWithTooManyInitializersIsPreconditionViolation,
    AssignIsPreconditionViolationIfSizeOverCapacity,
    IndexOperatorOutOfRange,
    EmplaceBackIsPreconditionViolationIfFull,
    InsertNIsPreconditionViolationIfTooManyItems,
    ConstructWithExcessiveSizeViaNormalConstructIsAlwaysPreconditionViolation,
    DereferencingInvalidIteratorIsPreconditionViolation,
    AddingToInvalidIteratorIsPreconditionViolation
);

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_VECTOR_HPP_
