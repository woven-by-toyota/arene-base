// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "stdlib/include/iterator"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

template <typename Iter>
class convertible_iterator : public testing::demoted_iterator<Iter, std::input_iterator_tag> {
  using base_type = testing::demoted_iterator<Iter, std::input_iterator_tag>;

 public:
  convertible_iterator() = default;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  convertible_iterator(Iter iterator)
      : base_type(iterator) {}

  auto operator++() -> convertible_iterator& {
    ++static_cast<base_type&>(*this);
    return *this;
  }
  auto operator++(int) -> convertible_iterator {
    convertible_iterator temp(*this);
    ++*this;
    return temp;
  }
};

struct local {
  std::uint32_t value;
};

auto operator==(local const& lhs, local const& rhs) -> bool { return lhs.value == rhs.value; }

class basic_input_iterator {
  std::uint32_t const* underlying_pointer_;

 public:
  explicit basic_input_iterator(std::uint32_t const* ptr) noexcept
      : underlying_pointer_(ptr) {}

  using iterator_category = std::input_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = local;
  using value_type = local;
  using pointer = void;

  auto operator++() noexcept -> basic_input_iterator& {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    ++underlying_pointer_;
    return *this;
  }
  auto operator*() const noexcept -> local { return local{*underlying_pointer_}; }

  struct post_increment_result {
    std::uint32_t value;

    auto operator*() noexcept -> local { return local{value}; }
  };
  auto operator++(int) noexcept -> post_increment_result {
    post_increment_result res{*underlying_pointer_};
    ++*this;
    return res;
  }

  auto operator==(basic_input_iterator const& other) const noexcept -> bool {
    return underlying_pointer_ == other.underlying_pointer_;
  }
  auto operator!=(basic_input_iterator const& other) const noexcept -> bool {
    return underlying_pointer_ != other.underlying_pointer_;
  }
};

// Mutable values so we can have mutable iterators that refer to them, even though the values won't change
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
arene::base::array<std::int32_t, 5> int_values{1, 2, 3, 4, 5};
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
arene::base::array<local, 5> local_values{{{100}, {200}, {300}, {400}, {500}}};
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
arene::base::array<float, 5> float_values{1.1F, 2.2F, 3.3F, 4.4F, 5.5F};
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
arene::base::array<std::uint32_t, 5> uint_values{10, 20, 30, 40, 50};

template <typename Iter>
Iter const initial_value_v = nullptr;

template <>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::int32_t* const initial_value_v<std::int32_t*> = &int_values[2];

template <>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
float* const initial_value_v<float*> = &float_values[2];

template <>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
local* const initial_value_v<local*> = &local_values[2];

template <>
local const* const initial_value_v<local const*> = &local_values[2];

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
basic_input_iterator const initial_value_v<basic_input_iterator> = basic_input_iterator{&uint_values[2]};

template <>
arene::base::array<local, 5>::iterator const initial_value_v<arene::base::array<local, 5>::iterator> =
    local_values.begin() + 2;

template <>
arene::base::array<local, 5>::const_iterator const initial_value_v<arene::base::array<local, 5>::const_iterator> =
    local_values.cbegin() + 2;

template <typename Iter, typename Tag>
testing::demoted_iterator<Iter, Tag> const initial_value_v<testing::demoted_iterator<Iter, Tag>> =
    testing::demoted_iterator<Iter, Tag>{initial_value_v<Iter>};

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <typename Iter>
std::move_iterator<Iter> const initial_value_v<std::move_iterator<Iter>> =
    std::move_iterator<Iter>{initial_value_v<Iter>};

using iterator_types = ::testing::Types<
    std::int32_t*,
    float*,
    local*,
    local const*,
    arene::base::array<local, 5>::iterator,
    arene::base::array<local, 5>::const_iterator,
    basic_input_iterator,
    ::testing::demoted_iterator<std::int32_t*, std::bidirectional_iterator_tag>,
    ::testing::demoted_iterator<std::int32_t*, std::forward_iterator_tag>,
    std::move_iterator<std::int32_t*>,
    std::move_iterator<local*>>;

template <typename T>
class MoveIteratorTypedTests : public testing::Test {};

TYPED_TEST_SUITE(MoveIteratorTypedTests, iterator_types, );

/// @test Ensure that @c move_iterator has the desired member type aliases
TYPED_TEST(MoveIteratorTypedTests, MoveIteratorHasRightTypeAliases) {
  using move_iter = std::move_iterator<TypeParam>;

  ::testing::StaticAssertTypeEq<
      typename move_iter::iterator_category,
      typename std::iterator_traits<TypeParam>::iterator_category>();
  ::testing::StaticAssertTypeEq<typename move_iter::value_type, typename std::iterator_traits<TypeParam>::value_type>();
  ::testing::StaticAssertTypeEq<
      typename move_iter::difference_type,
      typename std::iterator_traits<TypeParam>::difference_type>();
  ::testing::StaticAssertTypeEq<typename move_iter::pointer, TypeParam>();
  ::testing::StaticAssertTypeEq<
      typename move_iter::reference,
      std::conditional_t<
          std::is_reference_v<typename std::iterator_traits<TypeParam>::reference>,
          std::remove_reference_t<typename std::iterator_traits<TypeParam>::reference>&&,
          typename std::iterator_traits<TypeParam>::reference>>();
  ::testing::StaticAssertTypeEq<typename move_iter::iterator_type, TypeParam>();
}

/// @test Ensure that @c move_iterator is default-constructible if and only if the underlying iterator is
/// default-constructible
TYPED_TEST(MoveIteratorTypedTests, MoveIteratorIsDefaultConstructible) {
  ASSERT_EQ(std::is_default_constructible_v<std::move_iterator<TypeParam>>, std::is_default_constructible_v<TypeParam>);
  ASSERT_EQ(std::is_nothrow_default_constructible_v<std::move_iterator<TypeParam>>, std::is_nothrow_default_constructible_v<TypeParam>);
}

/// @test Ensure that @c move_iterator is constructible from the underlying iterator
TYPED_TEST(MoveIteratorTypedTests, MoveIteratorIsConstructibleFromUnderlyingIterator) {
  ASSERT_TRUE(std::is_constructible_v<std::move_iterator<TypeParam>, TypeParam>);
  ASSERT_EQ(
      (std::is_nothrow_constructible_v<std::move_iterator<TypeParam>, TypeParam>),
      (std::is_nothrow_copy_constructible_v<TypeParam>)
  );
}

/// @test Ensure that @c move_iterator is not implicitly constructible from the underlying iterator
TYPED_TEST(MoveIteratorTypedTests, MoveIteratorIsNotImplicitlyConstructibleFromUnderlyingIterator) {
  ASSERT_FALSE(std::is_convertible_v<TypeParam, std::move_iterator<TypeParam>>);
}

/// @test Ensure that initially @c base returns the stored iterator
TYPED_TEST(MoveIteratorTypedTests, BaseReturnsInitialStoredIterator) {
  ASSERT_EQ(std::move_iterator<TypeParam>(initial_value_v<TypeParam>).base(), initial_value_v<TypeParam>);
}

/// @test Ensure that @c base returns the incremented iterator
TYPED_TEST(MoveIteratorTypedTests, BaseReturnsNewStoredIteratorAfterIncrement) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  ++move_iter;
  ++underlying_iter;
  ASSERT_EQ(move_iter.base(), underlying_iter);
}

/// @test Ensure that @c base returns the incremented iterator after post-increment
TYPED_TEST(MoveIteratorTypedTests, BaseReturnsNewStoredIteratorAfterPostIncrement) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  move_iter++;
  underlying_iter++;
  ASSERT_EQ(move_iter.base(), underlying_iter);
}

template <typename Value>
auto same_address(Value const& lhs, Value const& rhs) -> bool {
  return &lhs == &rhs;
}

/// @test Ensure that deref returns rvalue ref to deref of underlying
CONDITIONAL_TYPED_TEST(MoveIteratorTypedTests, DereferencingReturnsRvalueRef, arene::base::is_forward_iterator_v<TypeParam>) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  ASSERT_TRUE(same_address(*underlying_iter, *move_iter));
}

/// @test Ensure that deref returns same value as deref of underlying
TYPED_TEST(MoveIteratorTypedTests, DereferencingReturnsValue) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: checking for unmodified identical values");
  ASSERT_EQ(*underlying_iter, *move_iter);
  ARENE_IGNORE_END();
}

/// @test Ensure that arrow operator returns underlying iterator
TYPED_TEST(MoveIteratorTypedTests, ArrowReturnsUnderlying) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  ASSERT_EQ(underlying_iter, move_iter.operator->());
}

/// @test Ensure that @c base returns the decremented iterator after pre-decrement
CONDITIONAL_TYPED_TEST(MoveIteratorTypedTests, BaseReturnsNewStoredIteratorAfterDecrement, arene::base::is_bidirectional_iterator_v<TypeParam>) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  --move_iter;
  --underlying_iter;
  ASSERT_EQ(move_iter.base(), underlying_iter);
}

/// @test Ensure that @c base returns the decremented iterator after post-decrement
CONDITIONAL_TYPED_TEST(MoveIteratorTypedTests, BaseReturnsNewStoredIteratorAfterPostDecrement, arene::base::is_bidirectional_iterator_v<TypeParam>) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  move_iter--;
  underlying_iter--;
  ASSERT_EQ(move_iter.base(), underlying_iter);
}

/// @test Ensure that @c move_iterator is constructible from another move iterator if the underlying iterator is
/// constructible from the other underlying iterator
TYPED_TEST(MoveIteratorTypedTests, MoveIteratorIsConstructibleFromOtherMoveIterator) {
  ASSERT_TRUE(std::is_constructible_v<
              std::move_iterator<convertible_iterator<TypeParam>>,
              std::move_iterator<TypeParam>>);
  ASSERT_TRUE(std::
                  is_convertible_v<std::move_iterator<TypeParam>, std::move_iterator<convertible_iterator<TypeParam>>>);
  auto source = std::move_iterator<TypeParam>{initial_value_v<TypeParam>};
  auto dest = std::move_iterator<convertible_iterator<TypeParam>>{source};

  ASSERT_EQ(dest.base(), (convertible_iterator<TypeParam>{initial_value_v<TypeParam>}));
}

/// @test Ensure that @c move_iterator is assignable from another move iterator if the underlying iterator is
/// constructible from the other underlying iterator
TYPED_TEST(MoveIteratorTypedTests, MoveIteratorIsAssignableFromOtherMoveIterator) {
  ASSERT_TRUE(std::
                  is_assignable_v<std::move_iterator<convertible_iterator<TypeParam>>&, std::move_iterator<TypeParam>>);
  auto source = std::move_iterator<TypeParam>{initial_value_v<TypeParam>};
  auto dest = std::move_iterator<convertible_iterator<TypeParam>>{source};
  ++source;
  dest = source;

  ASSERT_EQ(dest.base(), (convertible_iterator<TypeParam>{source.base()}));
}

/// @test Ensure that @c base returns the new iterator after random access operations
CONDITIONAL_TYPED_TEST(MoveIteratorTypedTests, BaseReturnsNewStoredIteratorAfterRandomAccess, arene::base::is_random_access_iterator_v<TypeParam>) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  move_iter += 2;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  underlying_iter += 2;
  ASSERT_EQ(move_iter.base(), underlying_iter);

  move_iter -= 3;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  underlying_iter -= 3;
  ASSERT_EQ(move_iter.base(), underlying_iter);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ((move_iter - 1).base(), underlying_iter - 1);
  ASSERT_EQ(move_iter.base(), underlying_iter);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ((move_iter + 2).base(), underlying_iter + 2);
  ASSERT_EQ(move_iter.base(), underlying_iter);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ((2 + move_iter).base(), underlying_iter + 2);
  ASSERT_EQ(move_iter.base(), underlying_iter);
}

/// @test Ensure that random access indexing works as expected
CONDITIONAL_TYPED_TEST(MoveIteratorTypedTests, RandomAccessIndexing, arene::base::is_random_access_iterator_v<TypeParam>) {
  auto underlying_iter = initial_value_v<TypeParam>;
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_TRUE(same_address(move_iter[2], underlying_iter[2]));
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_TRUE(same_address(move_iter[-1], underlying_iter[-1]));

  ASSERT_EQ(move_iter.base(), underlying_iter);

  ::testing::StaticAssertTypeEq<decltype(move_iter[2]), decltype(*move_iter)>();
}

/// @test Ensure that difference between random access iterators works as expected
CONDITIONAL_TYPED_TEST(MoveIteratorTypedTests, RandomAccessDifference, arene::base::is_random_access_iterator_v<TypeParam>) {
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  ASSERT_EQ((move_iter + 2) - move_iter, 2);
  ASSERT_EQ((move_iter - 1) - move_iter, -1);
}

/// @test Equality works based on underlying iterator equality
TYPED_TEST(MoveIteratorTypedTests, Equality) {
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);
  ASSERT_EQ(move_iter, move_iter);
  ++move_iter;
  ASSERT_EQ(move_iter, move_iter);
  auto move_iter2 = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);
  ASSERT_FALSE(move_iter == move_iter2);
  ++move_iter2;
  ASSERT_EQ(move_iter, move_iter2);
}

/// @test Inequality works based on underlying iterator equality
TYPED_TEST(MoveIteratorTypedTests, Inequality) {
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);
  ASSERT_FALSE(move_iter != move_iter);
  ++move_iter;
  ASSERT_FALSE(move_iter != move_iter);
  auto move_iter2 = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);
  ASSERT_NE(move_iter, move_iter2);
  ASSERT_NE(move_iter2, move_iter);
  ++move_iter2;
  ASSERT_FALSE(move_iter != move_iter2);
}

/// @test Ensure that ordering of random access iterators works as expected
CONDITIONAL_TYPED_TEST(MoveIteratorTypedTests, RandomAccessOrdering, arene::base::is_random_access_iterator_v<TypeParam>) {
  auto move_iter = std::move_iterator<TypeParam>(initial_value_v<TypeParam>);

  ASSERT_GT((move_iter + 2), move_iter);
  ASSERT_GE((move_iter + 2), move_iter);
  ASSERT_LT((move_iter - 2), move_iter);
  ASSERT_LE((move_iter - 2), move_iter);

  ASSERT_LE(move_iter, move_iter);
  ASSERT_GE(move_iter, move_iter);

  ASSERT_FALSE((move_iter + 2) <= move_iter);
  ASSERT_FALSE((move_iter + 2) < move_iter);
  ASSERT_FALSE(move_iter < move_iter);
  ASSERT_FALSE((move_iter - 2) >= move_iter);
  ASSERT_FALSE((move_iter - 2) > move_iter);
  ASSERT_FALSE(move_iter > move_iter);
}

/// @test Ensure that @c make_move_iterator creates an iterator holding the provided value
TYPED_TEST(MoveIteratorTypedTests, MakeMoveIterator) {
  ::testing::
      StaticAssertTypeEq<decltype(std::make_move_iterator(initial_value_v<TypeParam>)), std::move_iterator<TypeParam>>(
      );
  ASSERT_EQ(std::make_move_iterator(initial_value_v<TypeParam>).base(), initial_value_v<TypeParam>);
}

/// @test Ensure move_iterator has the same iterator category as the underlying iterator
TYPED_TEST(MoveIteratorTypedTests, SameCategory) {
  ASSERT_EQ(arene::base::is_input_iterator_v<TypeParam>, arene::base::is_input_iterator_v<std::move_iterator<TypeParam>>);
  ASSERT_EQ(arene::base::is_forward_iterator_v<TypeParam>, arene::base::is_forward_iterator_v<std::move_iterator<TypeParam>>);
  ASSERT_EQ(arene::base::is_bidirectional_iterator_v<TypeParam>, arene::base::is_bidirectional_iterator_v<std::move_iterator<TypeParam>>);
  ASSERT_EQ(arene::base::is_random_access_iterator_v<TypeParam>, arene::base::is_random_access_iterator_v<std::move_iterator<TypeParam>>);
}

}  // namespace
