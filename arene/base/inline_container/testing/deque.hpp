// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_DEQUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_DEQUE_HPP_

#include <gtest/gtest.h>

#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/inline_container/deque.hpp"
#include "arene/base/inline_container/testing/customization.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_copyable.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_movable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "testlibs/utilities/iterator_types.hpp"
#include "testlibs/utilities/throws_on.hpp"

namespace arene {
namespace base {
namespace testing {

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

/// @brief Create a test deque containing a range of test values
/// @tparam T The deque type to create an instance of
/// @param begin The first index of test value to put in the deque
/// @param end The after-end index of test value to put in the deque
/// @return An inline_deque<T, Capacity> containing [test_value(begin), test_value(end))
template <typename T>
constexpr auto test_deque(std::size_t begin, std::size_t end) noexcept(
    std::is_nothrow_copy_constructible<typename T::value_type>::value
) -> T {
  T deque;
  for (std::size_t i = begin; i < end; i++) {
    deque.push_back(test_value<typename T::value_type>(i));
  }
  return deque;
}

/// @brief A type trait to check if a type is list-initializable using a brace-enclosed list of values with a given type
/// @tparam T Type to check
/// @tparam ElementType Type to use for the elements of the brace-enclosed initializer list
/// @{
template <typename T, typename ElementType, typename = constraints<>>
constexpr bool is_list_initializable_v{false};

template <typename T, typename ElementType>
constexpr bool is_list_initializable_v<
    T,
    ElementType,
    constraints<decltype(T{std::declval<ElementType&>(), std::declval<ElementType&>()})>>{true};
/// @}

/////////////////////////
// Parameterized tests //
/////////////////////////

/// @brief Test fixture for all type-parameterized @c inline_deque tests
/// @tparam Deque The type parameter currently being used for tests; filled in by Google Test as @c TypeParam
template <typename Deque>
class InlineDequeTest : public ::testing::Test {
  using T = typename Deque::value_type;

 public:
  /// @brief Whether the type parameter is constexpr compatible for this test
  static constexpr bool constexpr_compatible =
      std::is_trivially_default_constructible<T>::value && std::is_trivially_copyable<T>::value;
  static_assert(
      !constexpr_compatible || has_constexpr_test_value<T>,
      "Constexpr-compatible types must have constexpr specializations of test_value_array or test_value"
  );

 protected:
  /// @brief Return the @c idx 'th test value of the current @c T , parameterized by test suite users
  /// @param idx The index of the test value to get
  /// @return The @c idx 'th test value of the current @c T
  static constexpr auto test_value(std::size_t idx) noexcept(noexcept(::arene::base::testing::test_value<T>(idx)))
      -> decltype(::arene::base::testing::test_value<T>(idx)) {
    return ::arene::base::testing::test_value<T>(idx);
  }

  /// @brief Return an empty @c Deque, allowing to call as this->construct,
  /// which becomes useful with the ...STATIC_ASSERT... macros
  static constexpr auto construct() noexcept(noexcept(Deque())) -> Deque { return Deque(); }

  /// @brief Return a @c Deque containing the test values @c [begin,end) of @c T
  /// @param begin The index of the first test value to put into the test deque
  /// @param end The index after that of the last test value to put into the test deque
  /// @return A test deque holding the test values determined by the indices
  static constexpr auto test_deque(std::size_t begin, std::size_t end) noexcept(
      noexcept(::arene::base::testing::test_deque<Deque>(begin, end))
  ) -> Deque {
    return ::arene::base::testing::test_deque<Deque>(begin, end);
  }

  /// @brief Return a @c Deque containing the test values @c [0,size) of @c T
  /// @param size The number of test values to put into the test deque
  /// @return A test deque holding the test values determined by the size
  static constexpr auto test_deque(std::size_t size) noexcept(noexcept(test_deque(0, size))) -> Deque {
    return test_deque(0, size);
  }

  /// @brief Return a @c Deque containing the test values @c [0,capacity/2) of @c T
  /// @return A test deque holding the test values determined by the capacity
  static constexpr auto half_test_deque() noexcept(noexcept(test_deque(Deque::capacity))) -> Deque {
    return test_deque(Deque::capacity / 2U);
  }

  /// @brief Return a @c Deque containing the test values @c [0,capacity) of @c T
  /// @return A test deque holding the test values determined by the capacity
  static constexpr auto full_test_deque() noexcept(noexcept(test_deque(Deque::capacity))) -> Deque {
    return test_deque(Deque::capacity);
  }

  /// @brief Return a @c Deque containing the test values <c>[capacity/2, 3*capacity/2)<c> of @c T
  /// @return A test deque holding the test values, wrapped around so they start in the middle
  /// @note This is useful for testing that functions aren't accidentally assuming the data starts at buffer index 0
  static constexpr auto wrapped_test_deque() noexcept(noexcept(test_deque(Deque::capacity))) -> Deque {
    Deque deque = test_deque(Deque::capacity);
    for (std::size_t i{}; i < Deque::capacity / 2U; ++i) {
      deque.pop_front();
      deque.push_back(::arene::base::testing::test_value<T>(Deque::capacity + i));
    }
    return deque;
  }

  /// @brief Return a @c std::initializer_list containing user-parameterized <c>test_value</c>s
  /// @tparam Indices A pack of indices, one for each test value in the initializer list
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing one test value per index
  template <std::size_t... Indices>
  static constexpr auto test_initializer_list(std::index_sequence<Indices...>) noexcept -> std::initializer_list<T> {
    return ::arene::base::testing::test_initializer_list<T, Indices...>();
  }

  /// @brief Return a @c std::initializer_list containing user-parameterized <c>test_value</c>s
  /// @tparam Size The number of values in the @c std::initializer_list
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing one test value per index
  template <std::size_t Size>
  static constexpr auto test_initializer_list() noexcept -> std::initializer_list<T> {
    return test_initializer_list(std::make_index_sequence<Size>{});
  }

  /// @brief Return a @c std::initializer_list containing @c Deque::capacity user-parameterized <c>test_value</c>s
  /// @return A copy of a statically-allocated @c std::initializer_list<T> containing @c Deque::capacity entries
  static constexpr auto full_test_initializer_list() noexcept -> std::initializer_list<T> {
    return test_initializer_list<Deque::capacity>();
  }
};

/// @brief The death tests use the same fixture as the non-death tests, but as a distinct type to improve log output
/// @tparam Deque The @c inline_deque type to test
template <typename Deque>
class InlineDequeDeathTest : public InlineDequeTest<Deque> {};

/// @brief Type trait to see if a particular @c TypeParam is double-ended or not; used to exclude some tests
/// @tparam TypeParam The queue/deque type to check
/// @note This implementation works kind of by coincidence, because currently the only queue with wrapping allowed
/// happens to be single-ended. There's no inherent relationship between wrapping being allowed and being double-ended.
template <typename TypeParam>
constexpr bool is_double_ended = !TypeParam::wrapping_allowed;

// Declare the test suite(s) where test cases will be added.
TYPED_TEST_SUITE_P(InlineDequeTest);
TYPED_TEST_SUITE_P(InlineDequeDeathTest);

/// @test A default-constructed `inline_deque` has size equal to zero
TYPED_TEST_P(InlineDequeTest, CanConstruct) {
  TypeParam const deque;
  EXPECT_EQ(deque.size(), 0);

  auto maybe_deque = TypeParam::try_construct();
  ASSERT_TRUE(maybe_deque.has_value());
  EXPECT_EQ(maybe_deque->size(), 0);
}

/// @test The capacity of a default-constructed `inline_deque` is equal to the supplied template parameter
TYPED_TEST_P(InlineDequeTest, CapacityIsAsSpecified) {
  COND_STATIC_ASSERT_EQ(TestFixture::construct().capacity(), TypeParam::capacity);
}

/// @test The `capacity` member function of `inline_deque` is declared `noexcept`
TYPED_TEST_P(InlineDequeTest, CapacityIsNoexcept) { STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().capacity())); }

/// @test The `size` member function of `inline_deque` is declared `noexcept`
TYPED_TEST_P(InlineDequeTest, SizeIsNoexcept) { STATIC_ASSERT_TRUE(noexcept(TestFixture::construct().size())); }

/// @test Given a default-constructed `inline_deque` with plenty of capacity,
/// an element can be inserted via `push_front`, which causes a size increase
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, PushFrontOneElement, is_double_ended<TypeParam>) {
  TypeParam deque;
  deque.push_front(TestFixture::test_value(0));
  EXPECT_EQ(deque.size(), 1);
}

/// @test Given a default-constructed `inline_deque` with plenty of capacity,
/// an element can be inserted via `push_back`, which causes a size increase
TYPED_TEST_P(InlineDequeTest, PushBackOneElement) {
  TypeParam deque;
  deque.push_back(TestFixture::test_value(0));
  EXPECT_EQ(deque.size(), 1);
}

/// @test Emplacing an element into the front of an `inline_deque` returns a reference to the emplaced element
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, EmplaceFrontReturnsReference, is_double_ended<TypeParam>) {
  TypeParam deque;
  auto& front_ref = deque.emplace_front(TestFixture::test_value(0));
  EXPECT_EQ(std::addressof(front_ref), std::addressof(deque.front()));
}

/// @test Emplacing an element into the back of an `inline_deque` returns a reference to the emplaced element
TYPED_TEST_P(InlineDequeTest, EmplaceBackReturnsReference) {
  TypeParam deque;
  auto& back_ref = deque.emplace_back(TestFixture::test_value(0));
  EXPECT_EQ(std::addressof(back_ref), std::addressof(deque.back()));
}

/// @test Given a default-constructed `inline_deque` with plenty of capacity,
/// an element can be inserted via `push_front` and then inspected
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, FrontPushedElementIsCorrect, is_double_ended<TypeParam>) {
  TypeParam deque;
  deque.push_front(TestFixture::test_value(0));
  ASSERT_EQ(deque.size(), 1);
  EXPECT_EQ(deque.front(), TestFixture::test_value(0));
  EXPECT_EQ(deque.back(), TestFixture::test_value(0));

  TypeParam const& as_const(deque);
  EXPECT_EQ(as_const.front(), TestFixture::test_value(0));
  EXPECT_EQ(as_const.back(), TestFixture::test_value(0));
}

/// @test Given a default-constructed `inline_deque` with plenty of capacity,
/// an element can be inserted via `push_back` and then inspected
TYPED_TEST_P(InlineDequeTest, BackPushedElementIsCorrect) {
  TypeParam deque;
  deque.push_back(TestFixture::test_value(0));
  ASSERT_EQ(deque.size(), 1);
  EXPECT_EQ(deque.back(), TestFixture::test_value(0));
  EXPECT_EQ(deque.front(), TestFixture::test_value(0));

  TypeParam const& as_const(deque);
  EXPECT_EQ(as_const.back(), TestFixture::test_value(0));
  EXPECT_EQ(as_const.front(), TestFixture::test_value(0));
}

/// @test Given a default-constructed `inline_deque`, elements can be inserted
/// via `push_front` within its capacity, making the size be equal to capacity
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, PushFrontWithinCapacity, is_double_ended<TypeParam>) {
  TypeParam deque;
  for (typename TypeParam::size_type i = 0; i < TypeParam::capacity(); i++) {
    deque.push_front(TestFixture::test_value(i));
  }
  EXPECT_EQ(deque.size(), TypeParam::capacity());
}

/// @test Given a default-constructed `inline_deque`, elements can be inserted
/// via `push_back` within its capacity, making the size be equal to capacity
TYPED_TEST_P(InlineDequeTest, PushBackWithinCapacity) {
  TypeParam deque;
  for (typename TypeParam::size_type i = 0; i < TypeParam::capacity(); i++) {
    deque.push_back(TestFixture::test_value(i));
  }
  EXPECT_EQ(deque.size(), TypeParam::capacity());
}

/// @test Inserting any element beyond an `inline_deque`'s capacity terminates
/// the application with a precondition violation or wraps, depending on its parameters
CONDITIONAL_TYPED_TEST_P(InlineDequeDeathTest, PushFrontMoreThanCapacity, is_double_ended<TypeParam>) {
  TypeParam deque;
  for (typename TypeParam::size_type i = 0; i < TypeParam::capacity(); i++) {
    deque.push_front(TestFixture::test_value(i));
  }

  ASSERT_DEATH(deque.push_front(TestFixture::test_value(TypeParam::capacity())), "Precondition violation");
}

/// @test For deques with a single-ended public interface, the unexposed functions in the other direction still work.
/// This test is needed for coverage purposes.
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, PushFrontDetailCoverage, !is_double_ended<TypeParam>) {
  using deque_impl = arene::base::inline_deque_detail::inline_deque_impl<  //
      typename TypeParam::value_type,                                      //
      TypeParam::capacity(),                                               //
      TypeParam::wrapping_allowed                                          //
      >;

  deque_impl deque;
  for (typename TypeParam::size_type i = 0; i < TypeParam::capacity(); i++) {
    deque.push_front(TestFixture::test_value(i));
  }

  if (deque.wrapping_allowed()) {
    EXPECT_EQ(deque.front(), TestFixture::test_value(TypeParam::capacity() - 1U));
    EXPECT_EQ(deque.back(), TestFixture::test_value(0));
    deque.push_front(TestFixture::test_value(TypeParam::capacity()));
    EXPECT_EQ(deque.front(), TestFixture::test_value(TypeParam::capacity()));
    EXPECT_EQ(deque.back(), TestFixture::test_value(1));
  } else {
    ASSERT_DEATH(deque.push_front(TestFixture::test_value(TypeParam::capacity())), "Precondition violation");
  }
}

/// @test Inserting any element beyond an `inline_deque`'s capacity terminates
/// the application with a precondition violation or wraps, depending on its parameters
TYPED_TEST_P(InlineDequeDeathTest, PushBackMoreThanCapacity) {
  TypeParam deque;
  for (typename TypeParam::size_type i = 0; i < TypeParam::capacity(); i++) {
    deque.push_back(TestFixture::test_value(i));
  }

  if (deque.wrapping_allowed()) {
    EXPECT_EQ(deque.back(), TestFixture::test_value(TypeParam::capacity() - 1U));
    EXPECT_EQ(deque.front(), TestFixture::test_value(0));
    deque.push_back(TestFixture::test_value(TypeParam::capacity()));
    EXPECT_EQ(deque.back(), TestFixture::test_value(TypeParam::capacity()));
    EXPECT_EQ(deque.front(), TestFixture::test_value(1));
  } else {
    ASSERT_DEATH(deque.push_back(TestFixture::test_value(TypeParam::capacity())), "Precondition violation");
  }
}

/// @test The constraints and exception specifications for the `insert` functions are correct
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, InsertConstraints, !TypeParam::wrapping_allowed) {
  using value_type = typename TypeParam::value_type;

  auto inserter = [](auto& deque,
                     auto&& value) noexcept(noexcept(deque.insert(deque.begin(), std::forward<decltype(value)>(value)))
                  ) -> decltype(deque.insert(deque.begin(), std::forward<decltype(value)>(value))) { return {}; };

  constexpr bool shift_ok = std::is_move_assignable<value_type>::value && std::is_move_constructible<value_type>::value;
  constexpr bool copy_insert_ok = shift_ok && std::is_copy_assignable<value_type>::value;
  constexpr bool move_insert_ok = shift_ok;

  STATIC_ASSERT_EQ((arene::base::is_invocable_v<decltype(inserter), TypeParam&, value_type const&>), copy_insert_ok);
  STATIC_ASSERT_EQ((arene::base::is_invocable_v<decltype(inserter), TypeParam&, value_type&&>), move_insert_ok);

  constexpr bool shift_noexcept =
      std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value;
  constexpr bool copy_insert_noexcept =
      copy_insert_ok && shift_noexcept && std::is_nothrow_copy_constructible<value_type>::value;
  constexpr bool move_insert_noexcept = move_insert_ok && shift_noexcept;

  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<decltype(inserter), TypeParam&, value_type const&>),
      copy_insert_noexcept
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<decltype(inserter), TypeParam&, value_type&&>),
      move_insert_noexcept
  );
}

/// @test The copy-based one-element version of `insert` inserts a copy of the value at the requested position
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    CopyInsertOne,
    !TypeParam::wrapping_allowed && std::is_copy_constructible<typename TypeParam::value_type>::value &&
        std::is_copy_assignable<typename TypeParam::value_type>::value
) {
  using index_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::half_test_deque();
  for (typename TypeParam::size_type i = 0U; i < TypeParam::capacity() / 2U; ++i) {
    // We need a new iterator every time because insert() invalidates all iterators. Each prior index has two entries.
    auto const pos = next(deque.cbegin(), static_cast<index_type>(2U * i));

    typename TypeParam::value_type const new_value = TestFixture::test_value((TypeParam::capacity() / 2U) + i);
    auto new_iter = deque.insert(pos, new_value);

    // The new iterator should have the same distance from `begin()` as `pos`, even though `pos` is now invalid.
    EXPECT_EQ(distance(deque.begin(), new_iter), static_cast<index_type>(2U * i));
  }

  for (typename TypeParam::size_type i = 0U; i < TypeParam::capacity() / 2U; ++i) {
    EXPECT_EQ(deque[static_cast<index_type>(2U * i)], TestFixture::test_value(TypeParam::capacity() / 2U + i));
    EXPECT_EQ(deque[static_cast<index_type>((2U * i) + 1U)], TestFixture::test_value(i));
  }
}

/// @test The move-based one-element version of `insert` inserts the value at the requested position
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    MoveInsertOne,
    !TypeParam::wrapping_allowed && std::is_move_constructible<typename TypeParam::value_type>::value &&
        std::is_move_assignable<typename TypeParam::value_type>::value
) {
  using index_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::half_test_deque();
  for (typename TypeParam::size_type i = 0U; i < TypeParam::capacity() / 2U; ++i) {
    // We need a new iterator every time because insert() invalidates all iterators. Each prior index has two entries.
    auto const pos = next(deque.cbegin(), static_cast<index_type>(2U * i));

    deque.insert(pos, TestFixture::test_value((TypeParam::capacity() / 2U) + i));
  }

  for (typename TypeParam::size_type i = 0U; i < TypeParam::capacity() / 2U; ++i) {
    EXPECT_EQ(deque[static_cast<index_type>(2U * i)], TestFixture::test_value(TypeParam::capacity() / 2U + i));
    EXPECT_EQ(deque[static_cast<index_type>((2U * i) + 1U)], TestFixture::test_value(i));
  }
}

/// @test The copy-based one-element version of `insert` can insert a value at the end
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    CopyInsertOneAtEnd,
    !TypeParam::wrapping_allowed && std::is_copy_constructible<typename TypeParam::value_type>::value &&
        std::is_copy_assignable<typename TypeParam::value_type>::value
) {
  typename TypeParam::value_type const value = TestFixture::test_value(TypeParam::capacity() / 2U);

  TypeParam deque = TestFixture::half_test_deque();
  typename TypeParam::size_type expected_size = deque.size();

  if (deque.size() == deque.capacity()) {
    deque.pop_front();
  } else {
    ++expected_size;
  }
  deque.insert(deque.cend(), value);
  EXPECT_EQ(deque.size(), expected_size);
  EXPECT_EQ(deque.back(), value);
}

/// @test The move-based one-element version of `insert` can insert a value at the end
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    MoveInsertOneAtEnd,
    !TypeParam::wrapping_allowed && std::is_move_constructible<typename TypeParam::value_type>::value &&
        std::is_move_assignable<typename TypeParam::value_type>::value
) {
  typename TypeParam::value_type const value = TestFixture::test_value(TypeParam::capacity() / 2U);
  typename TypeParam::value_type value_again = TestFixture::test_value(TypeParam::capacity() / 2U);

  TypeParam deque = TestFixture::half_test_deque();
  typename TypeParam::size_type expected_size = deque.size();

  if (deque.size() == deque.capacity()) {
    deque.pop_front();
  } else {
    ++expected_size;
  }
  deque.insert(deque.cend(), std::move(value_again));
  EXPECT_EQ(deque.size(), expected_size);
  EXPECT_EQ(deque.back(), value);
}

/// @test Calling the one-element version of `insert` when a non-wrapping deque is full terminates the application
CONDITIONAL_TYPED_TEST_P(
    InlineDequeDeathTest,
    NonWrappingInsertOneWhenFull,
    !TypeParam::wrapping_allowed && std::is_move_constructible<typename TypeParam::value_type>::value &&
        std::is_move_assignable<typename TypeParam::value_type>::value
) {
  TypeParam deque = TestFixture::full_test_deque();

  auto pos = next(deque.begin());
  for (typename TypeParam::size_type i{0U}; i <= deque.capacity(); ++i) {
    ASSERT_DEATH(deque.insert(pos, TestFixture::test_value(TypeParam::capacity() + i)), "Precondition violation");
    ++pos;
  }
}

/// @test `inline_deque` has circular or ring-buffer behavior, such that when
/// previously inserted elements via `push_front` are removed via `pop_back`,
/// then space in the fixed-capacity storage of the deque is made available
/// for new elements to be inserted
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, GoCircularFromFront, is_double_ended<TypeParam>) {
  TypeParam deque;
  for (std::size_t i = 0; i < 2 * TypeParam::capacity(); i++) {
    deque.push_front(TestFixture::test_value(i));
    EXPECT_EQ(deque.back(), TestFixture::test_value(i));
    deque.pop_back();
  }
}

/// @test `inline_deque` has circular or ring-buffer behavior, such that when
/// previously inserted elements via `push_back` are removed via `pop_front`,
/// then space in the fixed-capacity storage of the deque is made available
/// for new elements to be inserted
TYPED_TEST_P(InlineDequeTest, GoCircularFromBack) {
  TypeParam deque;
  for (std::size_t i = 0; i < 2 * TypeParam::capacity(); i++) {
    deque.push_back(TestFixture::test_value(i));
    EXPECT_EQ(deque.front(), TestFixture::test_value(i));
    deque.pop_front();
  }
}

/// @test basic `inline_deque` functionality, inserting elements via
/// `push_front` observing them with `back`, verifying that it doesn't remove
/// elements, removing with `pop_back`, and checking the correct dequeing order
/// (namely, FIFO) is preserved with the operations
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    BasicFunctionFromFront,
    is_double_ended<TypeParam>&& TypeParam::capacity >= 2
) {
  TypeParam deque;
  // Push an element and check it.
  deque.push_front(TestFixture::test_value(0));
  EXPECT_EQ(deque.back(), TestFixture::test_value(0));
  // Check that the element is still there after observing it.
  EXPECT_EQ(deque.back(), TestFixture::test_value(0));
  // Push a second element.
  deque.push_front(TestFixture::test_value(1));
  EXPECT_EQ(deque.size(), 2);
  // Check that the first pushed element is still "at the back".
  EXPECT_EQ(deque.back(), TestFixture::test_value(0));
  deque.pop_back();
  EXPECT_EQ(deque.back(), TestFixture::test_value(1));
  EXPECT_EQ(deque.size(), 1);
}

/// @test basic `inline_deque` functionality, inserting elements via
/// `push_back` observing them with `front`, verifying that it doesn't remove
/// elements, removing with `pop_front`, and checking the correct dequeing order
/// (namely, FIFO) is preserved with the operations
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, BasicFunctionFromBack, TypeParam::capacity >= 2) {
  TypeParam deque;
  // Push an element and check it.
  deque.push_back(TestFixture::test_value(0));
  EXPECT_EQ(deque.front(), TestFixture::test_value(0));
  // Check that the element is still there after observing it.
  EXPECT_EQ(deque.front(), TestFixture::test_value(0));
  // Push a second element.
  deque.push_back(TestFixture::test_value(1));
  EXPECT_EQ(deque.size(), 2);
  // Check that the first pushed element is still "at the back".
  EXPECT_EQ(deque.front(), TestFixture::test_value(0));
  deque.pop_front();
  EXPECT_EQ(deque.front(), TestFixture::test_value(1));
  EXPECT_EQ(deque.size(), 1);
}

/// @test `inline_deque`'s `back` can also be used to modify the least-recently
/// inserted element inplace
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    CanObserveAndModifyFromFront,
    is_double_ended<TypeParam>&& TypeParam::capacity >= 2
) {
  TypeParam deque;
  deque.push_front(TestFixture::test_value(0));
  deque.push_front(TestFixture::test_value(1));
  EXPECT_EQ(deque.back(), TestFixture::test_value(0));
  deque.back() = TestFixture::test_value(2);
  EXPECT_EQ(deque.back(), TestFixture::test_value(2));
  EXPECT_EQ(deque.size(), 2);
  deque.pop_back();
  EXPECT_EQ(deque.back(), TestFixture::test_value(1));
}

/// @test `inline_deque`'s `front` can also be used to modify the least-recently
/// inserted element inplace
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CanObserveAndModifyFromBack, TypeParam::capacity >= 2) {
  TypeParam deque;
  deque.push_back(TestFixture::test_value(0));
  deque.push_back(TestFixture::test_value(1));
  EXPECT_EQ(deque.front(), TestFixture::test_value(0));
  deque.front() = TestFixture::test_value(2);
  EXPECT_EQ(deque.front(), TestFixture::test_value(2));
  EXPECT_EQ(deque.size(), 2);
  deque.pop_front();
  EXPECT_EQ(deque.front(), TestFixture::test_value(1));
}

/// @test Pushing lvalue data onto the front of an `inline_deque` copies it
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    PushFrontCopiesLvalues,
    is_double_ended<TypeParam>&& std::is_copy_constructible<TypeParam>::value
) {
  auto deque = TestFixture::construct();
  auto const value = TestFixture::test_value(0);
  deque.push_front(value);
  EXPECT_EQ(deque.front(), value);
  EXPECT_NE(std::addressof(deque.front()), std::addressof(value));
}

/// @test Pushing lvalue data onto the back of an `inline_deque` copies it
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, PushBackCopiesLvalues, std::is_copy_constructible<TypeParam>::value) {
  auto deque = TestFixture::construct();
  auto const value = TestFixture::test_value(0);
  deque.push_back(value);
  EXPECT_EQ(deque.back(), value);
  EXPECT_NE(std::addressof(deque.back()), std::addressof(value));
}

/// @test when the `inline_deque` is empty, `back` terminates the application with a precondition violation
TYPED_TEST_P(InlineDequeDeathTest, BackWhenEmpty) {
  TypeParam deque;
  ASSERT_DEATH(deque.back(), "Precondition violation");
}

/// @test when the `inline_deque` is empty, the `const` overload of `back` terminates the application with
/// a precondition violation
TYPED_TEST_P(InlineDequeDeathTest, ConstBackWhenEmpty) {
  TypeParam const deque;
  ASSERT_DEATH(deque.back(), "Precondition violation");
}

/// @test when the `inline_deque` is empty, `pop_front` terminates the application with a precondition violation
TYPED_TEST_P(InlineDequeDeathTest, PopFrontWhenEmpty) {
  TypeParam deque;
  ASSERT_DEATH(deque.pop_front(), "Precondition violation");
}

/// @test when the `inline_deque` is empty, `pop_back` terminates the application with a precondition violation
CONDITIONAL_TYPED_TEST_P(InlineDequeDeathTest, PopBackWhenEmpty, is_double_ended<TypeParam>) {
  TypeParam deque;
  ASSERT_DEATH(deque.pop_back(), "Precondition violation");
}

/// @test when the `inline_deque` is empty, `front` terminates the application with a precondition violation
TYPED_TEST_P(InlineDequeDeathTest, FrontWhenEmpty) {
  TypeParam deque;
  ASSERT_DEATH(deque.front(), "Precondition violation");
}

/// @test when the `inline_deque` is empty, the `const` overload of `front` terminates the application with
/// a precondition violation
TYPED_TEST_P(InlineDequeDeathTest, ConstFrontWhenEmpty) {
  TypeParam const deque;
  ASSERT_DEATH(deque.front(), "Precondition violation");
}

/// @test `inline_deque` is not copy-constructible if the data type is not copy-constructible
TYPED_TEST_P(InlineDequeTest, NotCopyConstructibleIfDataTypeNotCopyConstructible) {
  constexpr bool copy_ok = std::is_copy_constructible<typename TypeParam::value_type>::value;
  static_assert(
      (std::is_constructible<TypeParam, TypeParam const&>::value) == copy_ok,
      "inline_deque<T> should be copy-constructible if and only if T is"
  );
}

/// @test `inline_deque` is not move-constructible if the data type is not move-constructible
TYPED_TEST_P(InlineDequeTest, NotMoveConstructibleIfDataTypeNotMoveConstructible) {
  constexpr bool move_ok = std::is_move_constructible<typename TypeParam::value_type>::value;
  static_assert(
      (std::is_constructible<TypeParam, TypeParam&&>::value) == move_ok,
      "inline_deque<T> should be move-constructible if and only if T is"
  );
}

/// @test `inline_deque` is not copy-assignable if the data type is not copy-constructible
TYPED_TEST_P(InlineDequeTest, NotCopyAssignableIfDataTypeNotCopyConstructibleAndCopyAssignable) {
  static_assert(
      (std::is_assignable<TypeParam, TypeParam const&>::value) == arene::base::is_copyable_v<TypeParam>,
      "inline_deque<T> should be copy-assignable if and only if T is copy-constructible and -assignable"
  );
}

/// @test `inline_deque` is not move-assignable if the data type is not move-constructible
TYPED_TEST_P(InlineDequeTest, NotMoveAssignableIfDataTypeNotMoveConstructibleAndMoveAssignable) {
  static_assert(
      (std::is_assignable<TypeParam, TypeParam&&>::value) == arene::base::is_movable_v<TypeParam>,
      "inline_deque<T> should be move-assignable if and only if T is move-constructible and -assignable"
  );
}

/// @test `inline_deque` can be used at compile-time, asserting properties statically
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CanConstructAtCompileTime, InlineDequeTest<TypeParam>::constexpr_compatible) {
  constexpr auto deque = TestFixture::full_test_deque();
  STATIC_ASSERT_EQ(deque.size(), deque.capacity());
  STATIC_ASSERT_EQ(deque.front(), TestFixture::test_value(0));
  STATIC_ASSERT_EQ(deque.back(), TestFixture::test_value(deque.size() - 1U));
}

/// @test `inline_deque` range construction is enabled or disabled appropriately based on the arguments
TYPED_TEST_P(InlineDequeTest, RangeConstructionSfinae) {
  using random_access_it = typename TypeParam::value_type const*;
  using bidirectional_it = ::testing::demoted_iterator<random_access_it, std::bidirectional_iterator_tag>;
  using forward_it = ::testing::demoted_iterator<random_access_it, std::forward_iterator_tag>;
  using input_it = ::testing::demoted_iterator<random_access_it, std::input_iterator_tag>;

  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, random_access_it, random_access_it>::value),
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, bidirectional_it, bidirectional_it>::value),
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, forward_it, forward_it>::value),
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );
  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, input_it, input_it>::value);
}

/// @test `inline_deque` range construction has the correct noexcept specification
TYPED_TEST_P(InlineDequeTest, RangeConstructionNoexcept) {
  using ::testing::throws_on;
  using value_type = typename TypeParam::value_type;

  using normal_it = ::testing::noexcept_configurable_random_access_iterator<throws_on::nothing, value_type>;
  using rvalue_it = testing::rvalue_iterator<value_type>;
  using throwing_it = ::testing::noexcept_configurable_random_access_iterator<throws_on::minus_self, value_type>;

  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<TypeParam, normal_it, normal_it>::value),
      std::is_nothrow_copy_constructible<value_type>::value
  );
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<TypeParam, rvalue_it, rvalue_it>::value),
      std::is_nothrow_move_constructible<value_type>::value
  );
  STATIC_ASSERT_FALSE(std::is_nothrow_constructible<TypeParam, throwing_it, throwing_it>::value);
}

/// @test `inline_deque` can be constructed from a range of `T` using iterators
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CopyConstructFromRange, std::is_copy_constructible<TypeParam>::value) {
  auto const init_list = TestFixture::full_test_initializer_list();
  TypeParam const deque(init_list.begin(), init_list.end());

  ASSERT_EQ(deque.size(), init_list.size());
  EXPECT_TRUE(::arene::base::equal(deque.begin(), deque.end(), init_list.begin()));

  auto const try_deque = TypeParam::try_construct(init_list.begin(), init_list.end());
  ASSERT_TRUE(try_deque.has_value());
  EXPECT_TRUE(::arene::base::equal(try_deque->begin(), try_deque->end(), init_list.begin()));
}

/// @test `inline_deque` can be constructed from a range of `T` using iterators
TYPED_TEST_P(InlineDequeTest, MoveConstructFromRange) {
  TypeParam const deque(
      arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(0),
      arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(TypeParam::capacity())
  );

  // Make an initializer list here just to make it easier to compare the contents
  auto const init_list = TestFixture::full_test_initializer_list();
  ASSERT_EQ(deque.size(), init_list.size());
  EXPECT_TRUE(::arene::base::equal(deque.begin(), deque.end(), init_list.begin()));

  auto const try_deque = TypeParam::try_construct(
      arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(0),
      arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(TypeParam::capacity())
  );
  ASSERT_TRUE(try_deque.has_value());
  EXPECT_TRUE(::arene::base::equal(try_deque->begin(), try_deque->end(), init_list.begin()));
}

/// @test `inline_deque` initializer list construction is enabled or disabled appropriately based on the value type
TYPED_TEST_P(InlineDequeTest, InitializerListConstructionSfinae) {
  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, std::initializer_list<typename TypeParam::value_type>>::value),
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );

  STATIC_ASSERT_EQ(
      (is_list_initializable_v<TypeParam, typename TypeParam::value_type>),
      std::is_copy_constructible<typename TypeParam::value_type>::value
  );
}

/// @test `inline_deque` initializer list construction has the correct noexcept specification
TYPED_TEST_P(InlineDequeTest, InitializerListConstructionNoexcept) {
  STATIC_ASSERT_EQ(
      (std::is_nothrow_constructible<TypeParam, std::initializer_list<typename TypeParam::value_type>>::value),
      std::is_nothrow_copy_constructible<typename TypeParam::value_type>::value
  );
}

/// @test `inline_deque` can be constructed from an initializer list of `T`
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, ConstructFromInitializerList, std::is_copy_constructible<TypeParam>::value) {
  auto const init_list = TestFixture::full_test_initializer_list();
  TypeParam const deque(init_list);

  ASSERT_EQ(deque.size(), init_list.size());
  EXPECT_TRUE(::arene::base::equal(deque.begin(), deque.end(), init_list.begin()));

  auto const try_deque = TypeParam::try_construct(init_list);
  ASSERT_TRUE(try_deque.has_value());
  EXPECT_TRUE(::arene::base::equal(try_deque->begin(), try_deque->end(), init_list.begin()));
}

/// @test `inline_deque` can be constructed from an empty initializer list
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, ConstructFromEmptyList, std::is_copy_constructible<TypeParam>::value) {
  auto const init_list = TestFixture::template test_initializer_list<0U>();

  TypeParam const deque_from_ilist(init_list);
  EXPECT_EQ(deque_from_ilist.size(), 0U);

  TypeParam const deque_from_range(init_list.begin(), init_list.end());
  EXPECT_EQ(deque_from_range.size(), 0U);

  auto const try_deque_from_ilist = TypeParam::try_construct(init_list);
  ASSERT_TRUE(try_deque_from_ilist.has_value());
  EXPECT_EQ(try_deque_from_ilist->size(), 0U);

  auto const try_deque_from_range = TypeParam::try_construct(init_list.begin(), init_list.end());
  ASSERT_TRUE(try_deque_from_range.has_value());
  EXPECT_EQ(try_deque_from_range->size(), 0U);
}

/// @test `inline_deque` crashes when copy constructed from a random-access range in the wrong order
CONDITIONAL_TYPED_TEST_P(
    InlineDequeDeathTest,
    CopyConstructFromBackwardRange,
    std::is_copy_constructible<TypeParam>::value
) {
  auto const init_list = TestFixture::full_test_initializer_list();
  ASSERT_DEATH(TypeParam(init_list.end(), init_list.begin()), "Precondition");
}

/// @test `try_construct` returns empty when copy constructing from a random-access range in the wrong order
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    TryCopyConstructFromBackwardRange,
    std::is_copy_constructible<TypeParam>::value
) {
  auto const init_list = TestFixture::full_test_initializer_list();
  EXPECT_FALSE(TypeParam::try_construct(init_list.end(), init_list.begin()).has_value());
}

/// @test `inline_deque` crashes when copy constructed from a range that's too large to fit within the capacity
CONDITIONAL_TYPED_TEST_P(
    InlineDequeDeathTest,
    CopyConstructFromOversizedRange,
    std::is_copy_constructible<TypeParam>::value
) {
  auto const init_list = TestFixture::template test_initializer_list<TypeParam::capacity() + 1U>();
  ASSERT_DEATH(TypeParam(init_list.begin(), init_list.end()), "Precondition");
}

/// @test `try_construct` returns empty when copy constructing from a range that's too large to fit within the capacity
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    TryCopyConstructFromOversizedRange,
    std::is_copy_constructible<TypeParam>::value
) {
  auto const init_list = TestFixture::template test_initializer_list<TypeParam::capacity() + 1U>();
  EXPECT_FALSE(TypeParam::try_construct(init_list.begin(), init_list.end()).has_value());
}

/// @test `inline_deque` crashes when move constructed from a range with the iterators in the wrong order
TYPED_TEST_P(InlineDequeDeathTest, MoveConstructFromBackwardRange) {
  ASSERT_DEATH(
      TypeParam(
          arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(TypeParam::capacity()),
          arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(0)
      ),
      "Precondition"
  );
}

/// @test `try_construct` returns empty when move constructing from a range with the iterators in the wrong order
TYPED_TEST_P(InlineDequeTest, TryMoveConstructFromBackwardRange) {
  EXPECT_FALSE(TypeParam::try_construct(
                   arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(TypeParam::capacity()),
                   arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(0)
  )
                   .has_value());
}

/// @test `inline_deque` crashes when move constructed from a range that's too large to fit within the capacity
TYPED_TEST_P(InlineDequeDeathTest, MoveConstructFromOversizedRange) {
  ASSERT_DEATH(
      TypeParam(
          arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(0),
          arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(TypeParam::capacity() + 1U)
      ),
      "Precondition"
  );
}

/// @test `try_construct` returns empty when move constructing from a range that's too large to fit within the capacity
TYPED_TEST_P(InlineDequeTest, TryMoveConstructFromOversizedRange) {
  EXPECT_FALSE(TypeParam::try_construct(
                   arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(0),
                   arene::base::testing::rvalue_iterator<typename TypeParam::value_type>(TypeParam::capacity() + 1U)
  )
                   .has_value());
}

/// @test `inline_deque` crashes when copy constructed from an initializer list that's too large to fit in the capacity
CONDITIONAL_TYPED_TEST_P(
    InlineDequeDeathTest,
    ConstructFromOversizedInitializerList,
    std::is_copy_constructible<TypeParam>::value
) {
  auto const init_list = TestFixture::template test_initializer_list<TypeParam::capacity() + 1U>();
  ASSERT_DEATH(static_cast<void>(TypeParam(init_list)), "Precondition");
}

/// @test `try_construct` returns empty when constructing from an initializer list that's too large for the capacity
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    TryConstructFromOversizedInitializerList,
    std::is_copy_constructible<TypeParam>::value
) {
  auto const init_list = TestFixture::template test_initializer_list<TypeParam::capacity() + 1U>();
  EXPECT_FALSE(TypeParam::try_construct(init_list).has_value());
}

/// @test `inline_deque` copy-construction works as expected
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CanCopyConstruct, std::is_copy_constructible<TypeParam>::value) {
  auto org = TestFixture::full_test_deque();
  auto dst(org);
  EXPECT_EQ(org.size(), org.capacity());
  EXPECT_EQ(org.size(), dst.size());
  for (std::size_t i = 0; i < org.capacity(); i++) {
    EXPECT_EQ(TestFixture::test_value(i), dst.front());
    EXPECT_EQ(org.front(), dst.front());
    org.pop_front();
    dst.pop_front();
  }
  EXPECT_TRUE(org.empty());
  EXPECT_TRUE(dst.empty());
}

/// @test `inline_deque` copy-construction works as expected when invoked via `try_construct`
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CanCopyTryConstruct, std::is_copy_constructible<TypeParam>::value) {
  auto org = TestFixture::full_test_deque();
  auto dst = TypeParam::try_construct(org);

  ASSERT_TRUE(dst.has_value());
  EXPECT_TRUE(::arene::base::equal(org.begin(), org.end(), dst->begin()));
}

/// @test `inline_deque` move-construction works as expected
TYPED_TEST_P(InlineDequeTest, CanMoveConstruct) {
  auto org = TestFixture::full_test_deque();
  EXPECT_EQ(org.size(), org.capacity());
  TypeParam dst(std::move(org));
  EXPECT_EQ(dst.size(), dst.capacity());
  for (std::size_t i = 0; i < dst.capacity(); i++) {
    EXPECT_EQ(TestFixture::test_value(i), dst.front());
    dst.pop_front();
  }
  EXPECT_TRUE(dst.empty());
}

/// @test `inline_deque` move-construction works as expected when invoked via `try_construct`
TYPED_TEST_P(InlineDequeTest, CanMoveTryConstruct) {
  auto org = TestFixture::full_test_deque();
  auto tmp = TestFixture::full_test_deque();
  auto dst = TypeParam::try_construct(std::move(tmp));

  ASSERT_TRUE(dst.has_value());
  EXPECT_TRUE(::arene::base::equal(org.begin(), org.end(), dst->begin()));
}

/// @test `inline_deque` copy-assignment works as expected
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CanCopyAssign, std::is_copy_assignable<TypeParam>::value) {
  auto org = TestFixture::full_test_deque();
  TypeParam dst;
  EXPECT_EQ(org.size(), org.capacity());
  EXPECT_EQ(dst.size(), 0);
  dst = org;
  EXPECT_EQ(org.size(), org.capacity());
  EXPECT_EQ(dst.size(), org.capacity());
  for (std::size_t i = 0; i < org.capacity(); i++) {
    EXPECT_EQ(TestFixture::test_value(i), dst.front());
    EXPECT_EQ(org.front(), dst.front());
    org.pop_front();
    dst.pop_front();
  }
  EXPECT_TRUE(org.empty());
  EXPECT_TRUE(dst.empty());
}

/// @test `inline_deque` self copy-assignment does nothing
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, SelfCopyAssign, std::is_copy_assignable<TypeParam>::value) {
  TypeParam deque = TestFixture::full_test_deque();

  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wself-assign-overloaded", "We're specifically testing self assignment");
  // NOLINTNEXTLINE(misc-redundant-expression)
  deque = deque;
  ASSERT_EQ(deque.size(), deque.capacity());
  ARENE_IGNORE_END();

  for (std::size_t i = 0; i < deque.size(); ++i) {
    EXPECT_EQ(deque.front(), TestFixture::test_value(i));
    deque.pop_front();
  }
}

/// @test `inline_deque` move-assignment works as expected
TYPED_TEST_P(InlineDequeTest, CanMoveAssign) {
  auto org = TestFixture::full_test_deque();
  TypeParam dst;
  ASSERT_EQ(org.size(), org.capacity());
  ASSERT_EQ(dst.size(), 0);

  dst = std::move(org);
  ASSERT_EQ(dst.size(), org.capacity());
  for (std::size_t i = 0; i < org.capacity(); i++) {
    EXPECT_EQ(TestFixture::test_value(i), dst.front());
    dst.pop_front();
  }
  EXPECT_TRUE(dst.empty());
}

/// @test `inline_deque` self move-assignment does nothing
TYPED_TEST_P(InlineDequeTest, SelfMoveAssign) {
  TypeParam deque = TestFixture::full_test_deque();

  deque = std::move(deque);
  ASSERT_EQ(deque.size(), deque.capacity());

  for (std::size_t i = 0; i < deque.size(); ++i) {
    EXPECT_EQ(deque.front(), TestFixture::test_value(i));
    deque.pop_front();
  }
}

/// @test Copy assignment works into a partially full target
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CopyAssignIntoPartiallyFull, std::is_copy_assignable<TypeParam>::value) {
  auto org = TestFixture::full_test_deque();
  auto dst = TestFixture::full_test_deque();

  // Remove half of the elements of dst
  while (dst.size() > dst.capacity() / 2U) {
    dst.pop_front();
  }

  dst = org;
  ASSERT_EQ(dst.size(), org.capacity());
  for (std::size_t i = 0; i < org.capacity(); i++) {
    EXPECT_EQ(TestFixture::test_value(i), dst.front());
    dst.pop_front();
  }
}

/// @test Copy assignment works from a partially full input
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, CopyAssignFromPartiallyFull, std::is_copy_assignable<TypeParam>::value) {
  auto org = TestFixture::full_test_deque();
  auto dst = TestFixture::full_test_deque();

  // Remove half of the elements of org
  std::size_t removed_count{};
  while (org.size() > org.capacity() / 2U) {
    org.pop_front();
    ++removed_count;
  }

  dst = org;
  ASSERT_EQ(dst.size(), org.capacity() / 2U);
  for (std::size_t i = 0; i < org.capacity() / 2U; i++) {
    EXPECT_EQ(TestFixture::test_value(i + removed_count), dst.front());
    dst.pop_front();
  }
}

/// @test Move assignment works into a partially full target
TYPED_TEST_P(InlineDequeTest, MoveAssignIntoPartiallyFull) {
  auto org = TestFixture::full_test_deque();
  auto dst = TestFixture::full_test_deque();

  // Remove half of the elements of dst
  while (dst.size() > dst.capacity() / 2U) {
    dst.pop_front();
  }

  dst = std::move(org);
  ASSERT_EQ(dst.size(), org.capacity());
  for (std::size_t i = 0; i < org.capacity(); i++) {
    EXPECT_EQ(TestFixture::test_value(i), dst.front());
    dst.pop_front();
  }
}

/// @test Move assignment works from a partially full input
TYPED_TEST_P(InlineDequeTest, MoveAssignFromPartiallyFull) {
  auto org = TestFixture::full_test_deque();
  auto dst = TestFixture::full_test_deque();

  // Remove half of the elements of org
  std::size_t removed_count{};
  while (org.size() > org.capacity() / 2U) {
    org.pop_front();
    ++removed_count;
  }

  dst = std::move(org);
  ASSERT_EQ(dst.size(), org.capacity() / 2U);
  for (std::size_t i = 0; i < org.capacity() / 2U; i++) {
    EXPECT_EQ(TestFixture::test_value(i + removed_count), dst.front());
    dst.pop_front();
  }
}

/// @test Both const and non-const iterator types for a deque are random access, and creating them is noexcept
TYPED_TEST_P(InlineDequeTest, IteratorsAreRandomAccessAndNoexcept) {
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<typename TypeParam::iterator>);
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<typename TypeParam::const_iterator>);
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().begin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().cbegin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().rbegin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().crbegin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().end()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().cend()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().rend()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().crend()));
}

/// @test Non-const iterators are implicitly convertible to the corresponding const iterator, but not vice versa
TYPED_TEST_P(InlineDequeTest, NonConstIteratorsImplicitlyConvertibleToConst) {
  using iterator = typename TypeParam::iterator;
  using const_iterator = typename TypeParam::const_iterator;

  STATIC_ASSERT_TRUE(arene::base::is_implicitly_constructible_v<const_iterator, iterator>);
  STATIC_ASSERT_FALSE(arene::base::is_implicitly_constructible_v<iterator, const_iterator>);

  TypeParam deque = TestFixture::full_test_deque();
  EXPECT_EQ(std::addressof(*deque.begin()), std::addressof(*deque.cbegin()));
  EXPECT_EQ(deque.begin(), deque.cbegin());
}

/// @test Inspecting a deque using its iterators yields the elements from front to back
TYPED_TEST_P(InlineDequeTest, InspectWithIterators) {
  TypeParam const deque = TestFixture::full_test_deque();

  std::size_t value_idx{0U};
  for (auto const& element : deque) {
    EXPECT_EQ(element, TestFixture::test_value(value_idx));
    ++value_idx;
  }
}

/// @test Inspecting a deque using its iterators yields the elements from front to back, even when wrapping around
TYPED_TEST_P(InlineDequeTest, IteratorsWrapAround) {
  TypeParam const deque = TestFixture::wrapped_test_deque();

  // The wrapped deque contains the elements [capacity/2, 3*capacity/2).
  std::size_t value_idx{(deque.capacity() / 2U)};
  for (auto const& element : deque) {
    EXPECT_EQ(element, TestFixture::test_value(value_idx));
    ++value_idx;
  }
}

/// @test The elements of a deque can be changed using its mutable iterators
TYPED_TEST_P(InlineDequeTest, MutableIteratorsWork) {
  // As confirmed in the above test, this contains the elements [capacity/2, 3*capacity/2), arranged backwards.
  TypeParam deque = TestFixture::wrapped_test_deque();

  // Use the mutable iterators to change the elements to [0, capacity), arranged forwards
  std::size_t value_idx{};
  for (auto& element : deque) {
    element = TestFixture::test_value(value_idx);
    ++value_idx;
  }

  // Use the constant iterators to inspect that the entries have the right values in the same order
  value_idx = {};
  for (auto itr = deque.cbegin(); itr != deque.cend(); ++itr) {
    EXPECT_EQ(*itr, TestFixture::test_value(value_idx));
    ++value_idx;
  }
}

/// @test Inspecting a deque using its reverse iterators yields the elements from back to front
TYPED_TEST_P(InlineDequeTest, InspectWithReverseIterators) {
  TypeParam const deque = TestFixture::wrapped_test_deque();

  // The wrapped deque contains the elements [capacity/2, 3*capacity/2).
  std::size_t idx{deque.capacity() + (deque.capacity() / 2U) - 1U};
  for (auto itr = deque.crbegin(); itr != deque.crend(); ++itr) {
    EXPECT_EQ(*itr, TestFixture::test_value(idx));
    --idx;
  }
}

/// @test The elements of a deque can be changed using its mutable reverse iterators
TYPED_TEST_P(InlineDequeTest, MutableReverseIteratorsWork) {
  // As confirmed in the above test, this contains the elements [capacity/2, 3*capacity/2), arranged backwards.
  TypeParam deque = TestFixture::wrapped_test_deque();

  // Use the mutable reverse iterators to change the elements to [0, capacity), in reverse
  std::size_t value_idx{};
  for (auto itr = deque.rbegin(); itr != deque.rend(); ++itr) {
    *itr = TestFixture::test_value(value_idx);
    ++value_idx;
  }

  // Use the forward iterators to inspect that the entries have the right values in the opposite order
  for (auto const& element : deque) {
    --value_idx;
    EXPECT_EQ(element, TestFixture::test_value(value_idx));
  }
}

/// @test Using `pop_front()` only invalidates iterators and references to the first element
CONDITIONAL_TYPED_TEST_P(InlineDequeTest, IteratorsNotInvalidatedByPopFront, TypeParam::capacity() >= 2U) {
  // The normal TestFixture::test_deque is populated front-to-back, but we want back-to-front for this test.
  TypeParam deque = TestFixture::test_deque(2U);

  auto itr = deque.cbegin();
  for (std::size_t value_idx{}; value_idx / 3U < TypeParam::capacity(); ++value_idx) {
    EXPECT_EQ(*itr, TestFixture::test_value(value_idx));
    ++itr;
    deque.pop_front();                                         // remove the element that was just inspected
    deque.push_back(TestFixture::test_value(value_idx + 2U));  // add the element we'll inspect 2 iterations from now
  }
}

/// @test Using `pop_back()` only invalidates iterators and references to the last element and `end()`
CONDITIONAL_TYPED_TEST_P(
    InlineDequeTest,
    IteratorsNotInvalidatedByPopBack,
    is_double_ended<TypeParam>&& TypeParam::capacity() >= 3U
) {
  // This test requires capacity 3 or more because a reverse iterator is invalidated by removing the element it points
  // to *or* the one after (reverse-before) it. To prevent this we need to advance the iterator twice before popping.
  //
  // The above test using a forward iterator and pop_front()/push_back() works with only 2 elements in the queue,
  // because the forward iterator is only invalidated if the element it points to is.

  TypeParam deque = TestFixture::construct();
  deque.push_front(TestFixture::test_value(0U));
  deque.push_front(TestFixture::test_value(1U));
  deque.push_front(TestFixture::test_value(2U));

  auto itr = deque.crbegin();
  ++itr;  // advance the iterator to ensure the element being popped is *two* elements behind the iterator
  for (std::size_t value_idx{1U}; value_idx / 3U < TypeParam::capacity(); ++value_idx) {
    EXPECT_EQ(*itr, TestFixture::test_value(value_idx));
    ++itr;
    deque.pop_back();                                           // remove the element that was just inspected
    deque.push_front(TestFixture::test_value(value_idx + 2U));  // add the element we'll inspect 2 iterations from now
  }
}

/// @test Using binary `operator+` on an iterator gives the same result as using `operator+=` on a copy of it
TYPED_TEST_P(InlineDequeTest, BinaryPlusEquivalentToPlusEquals) {
  using difference_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::full_test_deque();

  // Taking this difference out to size() so we can test the cases where the iterator falls off the edge as well
  for (difference_type diff{}; diff <= static_cast<difference_type>(deque.size()); ++diff) {
    auto begin_copy = deque.begin();
    begin_copy += diff;
    EXPECT_EQ(begin_copy, deque.begin() + diff);
    EXPECT_EQ(begin_copy, diff + deque.begin());

    auto cbegin_copy = deque.cbegin();
    cbegin_copy += diff;
    EXPECT_EQ(cbegin_copy, deque.cbegin() + diff);
    EXPECT_EQ(cbegin_copy, diff + deque.cbegin());
  }
}

/// @test Using binary `operator-` on an iterator and a difference gives the same result as using `operator-=` on them
TYPED_TEST_P(InlineDequeTest, BinaryMinusEquivalentToMinusEquals) {
  using difference_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::full_test_deque();

  // Taking this difference out to size() so we can test the cases where the iterator falls off the edge as well
  for (difference_type diff{}; diff <= static_cast<difference_type>(deque.size()); ++diff) {
    auto end_copy = deque.end();
    end_copy -= diff;
    EXPECT_EQ(end_copy, deque.end() - diff);

    auto cend_copy = deque.cend();
    cend_copy -= diff;
    EXPECT_EQ(cend_copy, deque.cend() - diff);
  }
}

/// @test Using `operator[]` on an iterator gives the same result as using binary `operator+` and unary `operator*`
TYPED_TEST_P(InlineDequeTest, IteratorBracketOperatorEquivalentToBinaryPlus) {
  using difference_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::full_test_deque();

  for (difference_type diff{}; diff < static_cast<difference_type>(deque.size()); ++diff) {
    EXPECT_EQ(std::addressof(deque.begin()[diff]), std::addressof(*(deque.begin() + diff)));
    EXPECT_EQ(std::addressof(deque.cbegin()[diff]), std::addressof(*(deque.cbegin() + diff)));

    EXPECT_EQ(std::addressof(deque.rbegin()[diff]), std::addressof(*(deque.rbegin() + diff)));
    EXPECT_EQ(std::addressof(deque.crbegin()[diff]), std::addressof(*(deque.crbegin() + diff)));
  }
}

/// @test Using `operator[]` on the deque gives the same result as using `operator[]` on `begin()`
TYPED_TEST_P(InlineDequeTest, ContainerBracketOperatorEquivalentToIteratorBracketOperator) {
  using difference_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::full_test_deque();
  TypeParam const& const_deque = deque;

  for (difference_type diff{}; diff < static_cast<difference_type>(deque.size()); ++diff) {
    EXPECT_EQ(std::addressof(deque[diff]), std::addressof(deque.begin()[diff]));
    EXPECT_EQ(std::addressof(const_deque[diff]), std::addressof(deque.cbegin()[diff]));
  }
}

/// @test Binary `operator-` between any two iterators returns the distance between them
TYPED_TEST_P(InlineDequeTest, AnyIteratorDifferenceReturnsDistance) {
  using difference_type = typename TypeParam::iterator::difference_type;

  // If it's possible to construct a deque that's neither full nor empty, test both the full one and the partially full
  // one; if not, then just test the full one.
  using deque_array = std::
      conditional_t<TypeParam::capacity() >= 2U, arene::base::array<TypeParam, 2U>, arene::base::array<TypeParam, 1U>>;
  deque_array deques;
  deques[0U] = TestFixture::full_test_deque();
  if (deques.size() >= 2U) {
    deques[1U] = TestFixture::half_test_deque();
  }

  for (auto& deque : deques) {
    auto const signed_size = static_cast<difference_type>(deque.size());

    auto current = deque.begin();
    auto const_current = deque.cbegin();
    for (difference_type current_pos{}; current_pos <= signed_size; ++current_pos) {
      // Check that every iterator from beginning to end has the expected difference from the current position

      auto test_iter = deque.begin();
      auto const_test_iter = deque.cbegin();
      for (difference_type diff{-current_pos}; diff < (signed_size - current_pos); ++diff) {
        EXPECT_EQ(test_iter - current, diff);
        EXPECT_EQ(const_test_iter - const_current, diff);

        ++test_iter;
        ++const_test_iter;
      }

      ++current;
      ++const_current;
    }
  }
}

/// @test Incrementing an iterator all at once is the same as repeatedly incrementing it one step
TYPED_TEST_P(InlineDequeTest, IteratorForwardMovementIsTheSameAsRepeatedIncrementing) {
  using difference_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::full_test_deque();
  auto curr = deque.begin();
  auto const_curr = deque.cbegin();
  for (difference_type diff{}; diff <= static_cast<difference_type>(deque.size()); ++diff) {
    EXPECT_EQ(deque.begin() + diff, curr);
    ++curr;

    EXPECT_EQ(deque.cbegin() + diff, const_curr);
    ++const_curr;
  }
}

/// @test Decrementing an iterator all at once is the same as repeatedly decrementing it one step
TYPED_TEST_P(InlineDequeTest, IteratorBackwardMovementIsTheSameAsRepeatedDecrementing) {
  using difference_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::full_test_deque();
  auto curr = deque.end();
  auto const_curr = deque.cend();
  for (difference_type diff{}; diff <= static_cast<difference_type>(deque.size()); ++diff) {
    EXPECT_EQ(deque.end() - diff, curr);
    --curr;

    EXPECT_EQ(deque.cend() - diff, const_curr);
    --const_curr;
  }
}

/// @test Incrementing an iterator by exactly enough to go out of bounds causes it to become the end iterator
TYPED_TEST_P(InlineDequeTest, IteratorsIncrementedOutOfBoundsBecomeEnd) {
  using difference_type = typename TypeParam::iterator::difference_type;

  TypeParam deque = TestFixture::full_test_deque();
  auto start = deque.begin();
  auto const_start = deque.cbegin();
  for (auto diff = static_cast<difference_type>(deque.size()); diff >= 0; --diff) {
    EXPECT_EQ(start + diff, deque.end());
    EXPECT_EQ(const_start + diff, deque.cend());
    ++start;
    ++const_start;
  }
}

/// @test When created on an empty deque, the begin and end pair of each iterator type equal each other
TYPED_TEST_P(InlineDequeTest, EmptyDequeBeginEqualsEnd) {
  TypeParam deque = TestFixture::construct();
  EXPECT_EQ(deque.begin(), deque.end());
  EXPECT_EQ(deque.cbegin(), deque.cend());
  EXPECT_EQ(deque.rbegin(), deque.rend());
  EXPECT_EQ(deque.crbegin(), deque.crend());
}

/// @brief Check that the first value is ordered strictly before the second
template <typename T>
auto check_ordering(T const& first, T const& second) noexcept -> void {
  EXPECT_TRUE(first < second);
  EXPECT_TRUE(first <= second);
  EXPECT_FALSE(first > second);
  EXPECT_FALSE(first >= second);

  EXPECT_FALSE(second < first);
  EXPECT_FALSE(second <= first);
  EXPECT_TRUE(second > first);
  EXPECT_TRUE(second >= first);
}

/// @test Iterators to the same deque are ordered with respect to each other in the way they're supposed to be
TYPED_TEST_P(InlineDequeTest, IteratorsAreOrdered) {
  TypeParam deque = TestFixture::full_test_deque();

  auto curr = deque.begin();
  auto const_curr = deque.cbegin();
  for (std::size_t i{}; i + 1U < deque.size(); ++i) {
    ++curr;
    ++const_curr;

    check_ordering(deque.begin(), curr);
    check_ordering(deque.cbegin(), const_curr);
    check_ordering(curr, deque.end());
    check_ordering(const_curr, deque.cend());
  }
}

/// @test Dereferencing the end iterator of any iterator type causes a crash
TYPED_TEST_P(InlineDequeDeathTest, DereferencingEndIteratorIsPreconditionViolation) {
  TypeParam deque = TestFixture::full_test_deque();
  ASSERT_DEATH(*deque.end(), "Precondition");
  ASSERT_DEATH(*deque.cend(), "Precondition");
  ASSERT_DEATH(static_cast<void>(*deque.rend()), "Precondition");
  ASSERT_DEATH(static_cast<void>(*deque.crend()), "Precondition");
}

/// @test When created on an empty deque, dereferencing the begin iterator of any iterator type causes a crash
TYPED_TEST_P(InlineDequeDeathTest, DereferencingBeginOfEmptyDequeIsPreconditionViolation) {
  TypeParam deque = TestFixture::construct();
  ASSERT_DEATH(*deque.begin(), "Precondition");
  ASSERT_DEATH(*deque.cbegin(), "Precondition");
  ASSERT_DEATH(static_cast<void>(*deque.rbegin()), "Precondition");
  ASSERT_DEATH(static_cast<void>(*deque.crbegin()), "Precondition");
}

/// @test Default-constructed instances of any deque's iterators cause crashes when incremented or dereferenced
TYPED_TEST_P(InlineDequeDeathTest, DefaultConstructedIteratorCantBeUsed) {
  TypeParam deque = TestFixture::full_test_deque();

  typename TypeParam::iterator default_it;
  EXPECT_NE(default_it, deque.begin());
  EXPECT_NE(default_it, deque.end());
  ASSERT_DEATH(*default_it, "Precondition");
  ASSERT_DEATH(default_it += 0, "Precondition");
}

/// @test Default-constructed instances of any deque's const iterators cause crashes when incremented or dereferenced
TYPED_TEST_P(InlineDequeDeathTest, DefaultConstructedConstIteratorCantBeUsed) {
  TypeParam deque = TestFixture::full_test_deque();

  typename TypeParam::const_iterator default_it;
  EXPECT_NE(default_it, deque.cbegin());
  EXPECT_NE(default_it, deque.cend());
  ASSERT_DEATH(*default_it, "Precondition");
  ASSERT_DEATH(default_it += 0, "Precondition");
}

/// @test Trying to compare iterators for ordering when they come from two different deques causes a crash
TYPED_TEST_P(InlineDequeDeathTest, IteratorOrderingFromDifferentDequesIsPreconditionViolation) {
  TypeParam deque1 = TestFixture::full_test_deque();
  TypeParam deque2 = TestFixture::full_test_deque();

  ASSERT_DEATH(std::ignore = (deque1.begin() < deque2.begin()), "Precondition");
  ASSERT_DEATH(std::ignore = (deque1.begin() <= deque2.begin()), "Precondition");
  ASSERT_DEATH(std::ignore = (deque1.begin() > deque2.begin()), "Precondition");
  ASSERT_DEATH(std::ignore = (deque1.begin() >= deque2.begin()), "Precondition");
}

/// @test Trying to subtract the most negative possible difference from an iterator causes a precondition violation
TYPED_TEST_P(InlineDequeDeathTest, SubtractingIteratorByMinimumValueIsPreconditionViolation) {
  TypeParam deque = TestFixture::full_test_deque();

  // This value can not be negated, so trying to subtract by it causes a precondition violation.
  constexpr auto minimum_value = std::numeric_limits<typename TypeParam::iterator::difference_type>::lowest();

  ASSERT_DEATH(std::ignore = (deque.begin() - minimum_value), "Precondition");
  ASSERT_DEATH(std::ignore = (deque.cbegin() - minimum_value), "Precondition");
}

ARENE_IGNORE_END();

REGISTER_TYPED_TEST_SUITE_P(
    InlineDequeTest,
    CanConstruct,
    CapacityIsAsSpecified,
    CapacityIsNoexcept,
    SizeIsNoexcept,
    PushFrontOneElement,
    PushBackOneElement,
    EmplaceFrontReturnsReference,
    EmplaceBackReturnsReference,
    FrontPushedElementIsCorrect,
    BackPushedElementIsCorrect,
    PushFrontWithinCapacity,
    PushBackWithinCapacity,
    PushFrontDetailCoverage,
    InsertConstraints,
    CopyInsertOne,
    MoveInsertOne,
    CopyInsertOneAtEnd,
    MoveInsertOneAtEnd,
    GoCircularFromFront,
    GoCircularFromBack,
    BasicFunctionFromFront,
    BasicFunctionFromBack,
    CanObserveAndModifyFromFront,
    CanObserveAndModifyFromBack,
    PushFrontCopiesLvalues,
    PushBackCopiesLvalues,
    NotCopyConstructibleIfDataTypeNotCopyConstructible,
    NotMoveConstructibleIfDataTypeNotMoveConstructible,
    NotCopyAssignableIfDataTypeNotCopyConstructibleAndCopyAssignable,
    NotMoveAssignableIfDataTypeNotMoveConstructibleAndMoveAssignable,
    CanConstructAtCompileTime,
    RangeConstructionSfinae,
    RangeConstructionNoexcept,
    CopyConstructFromRange,
    MoveConstructFromRange,
    InitializerListConstructionSfinae,
    InitializerListConstructionNoexcept,
    ConstructFromInitializerList,
    ConstructFromEmptyList,
    TryCopyConstructFromBackwardRange,
    TryCopyConstructFromOversizedRange,
    TryMoveConstructFromBackwardRange,
    TryMoveConstructFromOversizedRange,
    TryConstructFromOversizedInitializerList,
    CanCopyConstruct,
    CanCopyTryConstruct,
    CanMoveConstruct,
    CanMoveTryConstruct,
    CanCopyAssign,
    SelfCopyAssign,
    CanMoveAssign,
    SelfMoveAssign,
    CopyAssignIntoPartiallyFull,
    CopyAssignFromPartiallyFull,
    MoveAssignIntoPartiallyFull,
    MoveAssignFromPartiallyFull,
    IteratorsAreRandomAccessAndNoexcept,
    NonConstIteratorsImplicitlyConvertibleToConst,
    InspectWithIterators,
    IteratorsWrapAround,
    MutableIteratorsWork,
    InspectWithReverseIterators,
    MutableReverseIteratorsWork,
    IteratorsNotInvalidatedByPopFront,
    IteratorsNotInvalidatedByPopBack,
    BinaryPlusEquivalentToPlusEquals,
    BinaryMinusEquivalentToMinusEquals,
    IteratorBracketOperatorEquivalentToBinaryPlus,
    ContainerBracketOperatorEquivalentToIteratorBracketOperator,
    AnyIteratorDifferenceReturnsDistance,
    IteratorForwardMovementIsTheSameAsRepeatedIncrementing,
    IteratorBackwardMovementIsTheSameAsRepeatedDecrementing,
    IteratorsIncrementedOutOfBoundsBecomeEnd,
    EmptyDequeBeginEqualsEnd,
    IteratorsAreOrdered
);

REGISTER_TYPED_TEST_SUITE_P(
    InlineDequeDeathTest,
    PushFrontMoreThanCapacity,
    PushBackMoreThanCapacity,
    NonWrappingInsertOneWhenFull,
    BackWhenEmpty,
    ConstBackWhenEmpty,
    PopFrontWhenEmpty,
    PopBackWhenEmpty,
    FrontWhenEmpty,
    ConstFrontWhenEmpty,
    CopyConstructFromBackwardRange,
    CopyConstructFromOversizedRange,
    MoveConstructFromBackwardRange,
    MoveConstructFromOversizedRange,
    ConstructFromOversizedInitializerList,
    DereferencingEndIteratorIsPreconditionViolation,
    DereferencingBeginOfEmptyDequeIsPreconditionViolation,
    DefaultConstructedIteratorCantBeUsed,
    DefaultConstructedConstIteratorCantBeUsed,
    IteratorOrderingFromDifferentDequesIsPreconditionViolation,
    SubtractingIteratorByMinimumValueIsPreconditionViolation
);

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_DEQUE_HPP_
