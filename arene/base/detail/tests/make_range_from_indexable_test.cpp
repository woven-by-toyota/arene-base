// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/detail/make_range_from_indexable.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/ranges/is_borrowed_range.hpp"
#include "arene/base/ranges/size.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::detail::indexable_iterator;
using arene::base::detail::make_range_from_indexable;

/// @brief a minimal indexable used to exercise @c indexable_iterator
/// @tparam NoexceptMove whether the move constructor is @c noexcept
///
/// It is a non-owning view over an external array. It exposes element access through @c operator() and its
/// element count through a @c size() member, matching the default policies of @c indexable_iterator. The move
/// constructor's @c noexcept-ness is parameterized so tests can observe how it propagates through
/// @c make_range_from_indexable.
template <bool NoexceptMove = true>
// NOLINTNEXTLINE(hicpp-special-member-functions): a view needs no destructor or move assignment
class basic_int_view {
 public:
  /// @brief default constructor, yields an empty view
  ///
  constexpr basic_int_view() noexcept = default;

  /// @brief construct a view over @c size elements starting at @c data
  /// @param data pointer to the first element
  /// @param size the number of elements
  constexpr explicit basic_int_view(int const* const data, std::size_t const size) noexcept
      : data_{data},
        size_{size} {}

  /// @brief copy constructor
  constexpr basic_int_view(basic_int_view const&) noexcept = default;

  /// @brief copy assignment
  constexpr auto operator=(basic_int_view const&) noexcept -> basic_int_view& = default;

  /// @brief move constructor, @c noexcept only when @c NoexceptMove is @c true
  /// @param other the view to move from
  // NOLINTNEXTLINE(performance-noexcept-move-constructor,hicpp-noexcept-move): noexcept intentionally parameterized
  constexpr basic_int_view(basic_int_view&& other) noexcept(NoexceptMove)
      : data_{other.data_},
        size_{other.size_} {}

  /// @brief read the element at @c index
  /// @param index the index of the element to read
  /// @return a reference to the element at @c index
  constexpr auto operator()(std::ptrdiff_t const index) const noexcept -> int const& {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): a pointer view reads through its stored pointer
    return data_[index];
  }

  /// @brief the number of elements
  /// @return the element count
  constexpr auto size() const noexcept -> std::size_t { return size_; }

 private:
  /// @brief pointer to the first viewed element
  int const* data_{};

  /// @brief the number of viewed elements
  std::size_t size_{};
};

/// @brief an @c basic_int_view whose move constructor is @c noexcept
using int_view = basic_int_view<true>;

/// @brief an @c basic_int_view whose move constructor may throw
using throwing_move_view = basic_int_view<false>;

/// @test the elements are iterated one by one with the default policies
CONSTEXPR_TEST(IndexableIteratorTest, YieldsEachElement) {
  constexpr arene::base::array<int, 4> data{10, 20, 30, 40};
  auto const view = make_range_from_indexable(int_view{data.data(), data.size()});

  CONSTEXPR_ASSERT((view.end() - view.begin()) == 4);

  auto index = std::size_t{0};
  // NOLINTNEXTLINE(modernize-loop-convert): explicit iterator loop deliberately exercises the iterator interface
  for (auto iter = view.begin(); iter != view.end(); ++iter) {
    CONSTEXPR_ASSERT(*iter == data[index]);
    ++index;
  }
  CONSTEXPR_ASSERT(index == 4);
}

/// @test the range-for form yields each element
CONSTEXPR_TEST(IndexableIteratorTest, RangeForYieldsEachElement) {
  constexpr arene::base::array<int, 4> data{1, 2, 3, 4};
  auto const view = make_range_from_indexable(int_view{data.data(), data.size()});

  auto index = std::size_t{0};
  for (auto element : view) {
    CONSTEXPR_ASSERT(element == data[index]);
    ++index;
  }
  CONSTEXPR_ASSERT(index == 4);
}

/// @test the iterator supports the random-access operations
CONSTEXPR_TEST(IndexableIteratorTest, IteratorIsRandomAccess) {
  constexpr arene::base::array<int, 4> data{10, 20, 30, 40};
  auto const view = make_range_from_indexable(int_view{data.data(), data.size()});

  auto const first = view.begin();

  CONSTEXPR_ASSERT(first[2] == 30);
  CONSTEXPR_ASSERT(*(first + 3) == 40);
  CONSTEXPR_ASSERT(*(3 + first) == 40);

  auto const last = view.end();
  CONSTEXPR_ASSERT((last - first) == 4);
  CONSTEXPR_ASSERT(first < last);
  CONSTEXPR_ASSERT(first <= view.begin());
  CONSTEXPR_ASSERT((last - 4) == first);
}

/// @test the iterator can be decremented
CONSTEXPR_TEST(IndexableIteratorTest, IteratorDecrements) {
  constexpr arene::base::array<int, 4> data{10, 20, 30, 40};
  auto const view = make_range_from_indexable(int_view{data.data(), data.size()});

  auto iter = view.end();
  --iter;
  CONSTEXPR_ASSERT(*iter == 40);

  auto const previous = iter--;
  CONSTEXPR_ASSERT(*previous == 40);
  CONSTEXPR_ASSERT(*iter == 30);
}

/// @test default-constructed iterators are singular and compare equal
CONSTEXPR_TEST(IndexableIteratorTest, DefaultConstructedIteratorsAreEqual) {
  auto const first = indexable_iterator<int_view>{};
  auto const second = indexable_iterator<int_view>{};

  CONSTEXPR_ASSERT(first == second);
  CONSTEXPR_ASSERT((first - second) == 0);
}

/// @test the iterator value_type strips cv and reference qualifiers
TEST(IndexableIteratorTest, ValueTypeIsStripped) {
  using iterator_type = indexable_iterator<int_view>;

  ::testing::StaticAssertTypeEq<iterator_type::value_type, int>();
  ::testing::StaticAssertTypeEq<iterator_type::reference, int const&>();
}

/// @test a reference-yielding indexable produces a random-access iterator with a real pointer type
TEST(IndexableIteratorTest, ReferenceYieldsRandomAccessIterator) {
  using iterator_type = indexable_iterator<int_view>;

  ::testing::StaticAssertTypeEq<iterator_type::iterator_category, std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<iterator_type::pointer, int const*>();
}

/// @brief an indexable type whose element access returns a prvalue rather than a reference
///
/// A @c ForwardIterator requires @c reference to be a real reference, so an iterator over this type can only be an
/// @c InputIterator.
class prvalue_view {
 public:
  /// @brief default constructor, yields an empty view
  constexpr prvalue_view() noexcept = default;

  /// @brief read the element at @c index
  /// @param index the index of the element to read
  /// @return the index cast to @c int, by value
  constexpr auto operator()(std::ptrdiff_t const index) const noexcept -> int { return static_cast<int>(index); }

  /// @brief the number of elements
  /// @return zero, unconditionally
  static constexpr auto size() noexcept -> std::size_t { return 0U; }
};

/// @test a prvalue-yielding indexable produces an input iterator with a void pointer type but still defines random
/// access members
TEST(IndexableIteratorTest, PrvalueYieldsInputIterator) {
  using iterator_type = indexable_iterator<prvalue_view>;

  ::testing::StaticAssertTypeEq<iterator_type::reference, int>();
  ::testing::StaticAssertTypeEq<iterator_type::iterator_category, std::input_iterator_tag>();
  ::testing::StaticAssertTypeEq<iterator_type::pointer, void>();

  ::testing::StaticAssertTypeEq<iterator_type::reference, decltype(std::declval<iterator_type>()[1])>();
}

/// @brief a type that is not indexable via the default policies
///
/// It provides neither @c operator() nor a @c size() member, so it must not satisfy the constraints of
/// @c make_range_from_indexable.
class not_indexable {};

/// @brief an indexable type that is not copy-constructible
///
/// It is indexable like @c int_view but deletes its copy constructor, so it must not satisfy the copy-constructibility
/// constraint of @c make_range_from_indexable (each iterator stores a copy of the indexable).
// NOLINTNEXTLINE(hicpp-special-member-functions): copy is intentionally deleted to test the constraint
class move_only_view {
 public:
  /// @brief default constructor, yields an empty view
  constexpr move_only_view() noexcept = default;

  /// @brief deleted copy constructor
  move_only_view(move_only_view const&) = delete;

  /// @brief deleted copy assignment
  auto operator=(move_only_view const&) -> move_only_view& = delete;

  /// @brief read the element at @c index
  /// @param index the index of the element to read
  /// @return the value zero, unconditionally
  constexpr auto operator()(std::ptrdiff_t const index) const noexcept -> int { return static_cast<int>(index); }

  /// @brief the number of elements
  /// @return zero, unconditionally
  static constexpr auto size() noexcept -> std::size_t { return 0U; }
};

/// @brief an index policy that is not move-constructible
///
/// It reads elements just like @c int_view but deletes its copy and move constructors, so it must not satisfy the
/// move-constructibility constraint of @c make_range_from_indexable (each iterator stores the policy by value).
// NOLINTNEXTLINE(hicpp-special-member-functions): copy and move are intentionally deleted to test the constraint
class non_move_constructible_policy {
 public:
  /// @brief default constructor
  non_move_constructible_policy() = default;

  /// @brief deleted copy constructor
  non_move_constructible_policy(non_move_constructible_policy const&) = delete;

  /// @brief deleted move constructor
  non_move_constructible_policy(non_move_constructible_policy&&) = delete;

  /// @brief read the element of @c indexable at @c index
  /// @param indexable the indexable value to read from
  /// @param index the index of the element to read
  /// @return the result of @c indexable(index)
  constexpr auto operator()(int_view const& indexable, std::ptrdiff_t const index) const noexcept -> int const& {
    return indexable(index);
  }
};

/// @brief an index policy that carries state affecting the value it reads
///
/// It applies a stored additive @c bias to every element it reads, so a test can observe whether the policy value
/// passed to @c make_range_from_indexable is actually stored and used rather than discarded and default-constructed.
class biased_index_policy {
 public:
  /// @brief default constructor, yields a zero bias
  constexpr biased_index_policy() noexcept = default;

  /// @brief construct a policy applying @c bias to every read element
  /// @param bias the value added to each element read
  constexpr explicit biased_index_policy(int const bias) noexcept
      : bias_{bias} {}

  /// @brief read the element of @c indexable at @c index, shifted by the stored bias
  /// @param indexable the indexable value to read from
  /// @param index the index of the element to read
  /// @return @c indexable(index) plus the stored bias
  constexpr auto operator()(int_view const& indexable, std::ptrdiff_t const index) const noexcept -> int {
    return indexable(index) + bias_;
  }

 private:
  /// @brief the value added to every element read
  int bias_{};
};

/// @brief a small indexable exposing element access through a named member function
///
/// Used to verify that a pointer-to-member-function is accepted as an index policy via @c arene::base::invoke.
class member_read_view {
 public:
  /// @brief default constructor, yields an empty view
  constexpr member_read_view() noexcept = default;

  /// @brief construct a view over @c size elements starting at @c data
  /// @param data pointer to the first element
  /// @param size the number of elements
  constexpr explicit member_read_view(int const* const data, std::size_t const size) noexcept
      : data_{data},
        size_{size} {}

  /// @brief read the element at @c index
  /// @param index the index of the element to read
  /// @return a reference to the element at @c index
  constexpr auto at(std::ptrdiff_t const index) const noexcept -> int const& {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): a pointer view reads through its stored pointer
    return data_[index];
  }

  /// @brief the number of elements
  /// @return the element count
  constexpr auto size() const noexcept -> std::size_t { return size_; }

 private:
  /// @brief pointer to the first viewed element
  int const* data_{};

  /// @brief the number of viewed elements
  std::size_t size_{};
};

/// @brief the type of the @c make_range_from_indexable function object
using make_range_from_indexable_t = decltype(make_range_from_indexable);

/// @test the constraint accepts an indexable type and rejects a non-indexable one
TEST(IndexableIteratorTest, ConstraintGatesOnIndexability) {
  EXPECT_TRUE((arene::base::is_invocable_v<make_range_from_indexable_t, int_view>));
  EXPECT_FALSE((arene::base::is_invocable_v<make_range_from_indexable_t, not_indexable>));
}

/// @test the constraint rejects an indexable type that is not copy-constructible
TEST(IndexableIteratorTest, ConstraintGatesOnCopyConstructibleIndexable) {
  EXPECT_TRUE((arene::base::is_invocable_v<make_range_from_indexable_t, int_view>));
  EXPECT_FALSE((arene::base::is_invocable_v<make_range_from_indexable_t, move_only_view>));
}

/// @test the constraint rejects a policy that is not move-constructible
TEST(IndexableIteratorTest, ConstraintGatesOnMoveConstructiblePolicy) {
  EXPECT_TRUE((arene::base::is_invocable_v<
               make_range_from_indexable_t,
               int_view,
               arene::base::detail::default_end_policy,
               arene::base::detail::call_operator_policy>));
  EXPECT_FALSE((arene::base::is_invocable_v<
                make_range_from_indexable_t,
                int_view,
                arene::base::detail::default_end_policy,
                non_move_constructible_policy>));
}

/// @test a stateful index policy passed by value is stored and used, not discarded
CONSTEXPR_TEST(IndexableIteratorTest, StatefulIndexPolicyIsUsed) {
  constexpr arene::base::array<int, 4> data{10, 20, 30, 40};
  auto const view = make_range_from_indexable(
      int_view{data.data(), data.size()},
      arene::base::detail::default_end_policy{},
      biased_index_policy{100}
  );

  auto index = std::size_t{0};
  for (auto element : view) {
    CONSTEXPR_ASSERT(element == data[index] + 100);
    ++index;
  }
  CONSTEXPR_ASSERT(index == 4);
}

/// @test a pointer-to-member-function is accepted as an index policy via @c invoke
CONSTEXPR_TEST(IndexableIteratorTest, MemberFunctionPointerIndexPolicy) {
  constexpr arene::base::array<int, 4> data{10, 20, 30, 40};
  auto const view = make_range_from_indexable(
      member_read_view{data.data(), data.size()},
      arene::base::detail::default_end_policy{},
      &member_read_view::at
  );

  auto index = std::size_t{0};
  for (auto const& element : view) {
    CONSTEXPR_ASSERT(element == data[index]);
    ++index;
  }
  CONSTEXPR_ASSERT(index == 4);
}

/// @test the returned range is a borrowed range
TEST(IndexableIteratorTest, ResultIsBorrowedRange) {
  using range_type = decltype(make_range_from_indexable(std::declval<int_view>()));

  STATIC_ASSERT_TRUE(arene::base::ranges::is_borrowed_range_v<range_type>);
}

/// @test the iterators of the returned range remain usable after the range object is destroyed
CONSTEXPR_TEST(IndexableIteratorTest, IteratorsOutliveTheRange) {
  constexpr arene::base::array<int, 4> data{10, 20, 30, 40};

  // the temporary range is destroyed at the end of each full expression, only its iterators survive
  auto const first = make_range_from_indexable(int_view{data.data(), data.size()}).begin();
  auto const last = make_range_from_indexable(int_view{data.data(), data.size()}).end();

  CONSTEXPR_ASSERT((last - first) == 4);

  auto index = std::size_t{0};
  // NOLINTNEXTLINE(modernize-loop-convert): the range object no longer exists, only its iterators
  for (auto iter = first; iter != last; ++iter) {
    CONSTEXPR_ASSERT(*iter == data[index]);
    ++index;
  }
  CONSTEXPR_ASSERT(index == 4);
}

/// @test the call is @c noexcept for a trivially-copyable indexable and not @c noexcept when moving it may throw
TEST(IndexableIteratorTest, NoexceptReflectsIteratorMoves) {
  EXPECT_TRUE(noexcept(make_range_from_indexable(std::declval<int_view>())));
  EXPECT_FALSE(noexcept(make_range_from_indexable(std::declval<throwing_move_view>())));
}

}  // namespace
