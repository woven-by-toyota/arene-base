// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
///
/// @file non_owning_ptr_test.cpp
/// @brief Unit tests for arene/base/pointer/non_owning_ptr.hpp
///
// Copyright 2023, Toyota Motor Corporation
///
///

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/pointer.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/hash.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/utility/swap.hpp"

namespace {

using ::arene::base::make_non_owning_ptr;
using ::arene::base::non_owning_ptr;

/// @test `non_owning_ptr` is a trivial type
TEST(NonOwningPtr, IsTrivial) {
  static_assert(std::is_trivially_copyable<non_owning_ptr<int>>::value, "Expected non_owning_ptr to be trivial.");

  static_assert(std::is_trivially_copyable<non_owning_ptr<int const>>::value, "Expected non_owning_ptr to be trivial.");
  static_assert(std::is_trivially_destructible<non_owning_ptr<int>>::value, "Expected non_owning_ptr to be trivial.");

  static_assert(
      std::is_trivially_destructible<non_owning_ptr<int const>>::value,
      "Expected non_owning_ptr to be trivial."
  );
}

/// @test `pointer` is an alias for a raw pointer to the element type
TEST(NonOwningPtr, PointerMemberTypeIsPointerToT) {
  static_assert(std::is_same<non_owning_ptr<int>::pointer, int*>::value, "Expected the pointer member-type to be T*.");

  static_assert(
      std::is_same<non_owning_ptr<int const>::pointer, int const*>::value,
      "Expected the pointer member-type to be T*."
  );
}

/// @test `element_type` is an alias for the element type
TEST(NonOwningPtr, ElementTypeMemberTypeIsT) {
  static_assert(
      std::is_same<non_owning_ptr<int>::element_type, int>::value,
      "Expected the element_type member-type to be T."
  );

  static_assert(
      std::is_same<non_owning_ptr<int const>::element_type, int const>::value,
      "Expected the element_type member-type to be T*."
  );
}

/// @test `reference` is an alias for a reference to the element type
TEST(NonOwningPtr, ReferenceTypeMemberTypeIsTRef) {
  static_assert(
      std::is_same<non_owning_ptr<int>::reference, int&>::value,
      "Expected the reference member-type to be T&."
  );

  static_assert(
      std::is_same<non_owning_ptr<int const>::reference, int const&>::value,
      "Expected the reference member-type to be T&."
  );
}

/// @test The size of `non_owning_ptr` is the size of a raw pointer
TEST(NonOwningPtr, SizeofIsSizeofPointer) {
  static_assert(
      sizeof(non_owning_ptr<int>) == sizeof(non_owning_ptr<int>::pointer),
      "Expected non_owning_ptr to be the same size as a pointer."
  );
}

/// @test A default-constructed `non_owning_ptr` is NULL
TEST(NonOwningPtr, IsExplicitDefaultConstructableAsNullptr) { ASSERT_EQ(non_owning_ptr<int>().get(), nullptr); }

/// @test A `non_owning_ptr` constructed from `nullptr` is NULL
TEST(NonOwningPtr, IsConstructableFromNullptr) { ASSERT_EQ(non_owning_ptr<int>(nullptr).get(), nullptr); }

/// @test A `non_owning_ptr` constructed from a raw pointer holds the value
TEST(NonOwningPtr, IsConstructableFromPointerToT) {
  int value1 = 10;  // NOLINT(readability-magic-numbers)
  ASSERT_EQ(non_owning_ptr<int>(&value1).get(), &value1);

  int const value2 = 11;
  ASSERT_EQ(non_owning_ptr<int const>(&value2).get(), &value2);
}

/// @test A `non_owning_ptr` constructed from something convertible to the stored pointer holds the converted value
TEST(NonOwningPtr, IsConstructableFromPointerConvertibleToTPointer) {
  // Simple conversion from non-const to const pointer
  int value1 = 10;  // NOLINT(readability-magic-numbers)
  ASSERT_EQ(non_owning_ptr<int const>(&value1).get(), &value1);

  // Conversion from derived class pointer to base class pointer
  class foo {};
  class bar : public foo {};

  bar a_bar;
  ASSERT_EQ(non_owning_ptr<foo>(&a_bar).get(), &a_bar);
}

/// @test A `non_owning_ptr` constructed from a `non_owning_ptr` holding a pointer convertible to the stored pointer
/// holds the converted value
TEST(NonOwningPtr, IsConstructableFromNonOwningPtrConvertibleToTPointer) {
  // Simple conversion from non-const to const pointer
  int value1 = 10;  // NOLINT(readability-magic-numbers)
  ASSERT_EQ(non_owning_ptr<int const>(non_owning_ptr<int>(&value1)).get(), &value1);

  // Conversion from derived class pointer to base class pointer
  class foo {};
  class bar : public foo {};

  bar a_bar;
  ASSERT_EQ(non_owning_ptr<foo>(non_owning_ptr<bar>(&a_bar)).get(), &a_bar);
}

/// @test `non_owning_ptr` is copy constructible and assignable
TEST(NonOwningPtr, IsCopyConstructableAndAssignable) {
  int const value = 10;
  non_owning_ptr<int const> const a_ptr(&value);
  non_owning_ptr<int const> const b_ptr(a_ptr);
  ASSERT_EQ(a_ptr.get(), b_ptr.get());

  non_owning_ptr<int const> c_ptr{};
  c_ptr = a_ptr;
  ASSERT_EQ(a_ptr.get(), c_ptr.get());
}

/// @test A `non_owning_ptr` assigned from a `non_owning_ptr` holding a pointer convertible to the stored pointer holds
/// the converted value
TEST(NonOwningPtr, IsAssignableFromNonOwningPtrConvertibleToTPointer) {
  // Simple conversion from non-const to const pointer
  int value1 = 10;  // NOLINT(readability-magic-numbers)
  non_owning_ptr<int> const value1_ptr(&value1);
  non_owning_ptr<int const> converted_value1_ptr{};
  converted_value1_ptr = value1_ptr;
  ASSERT_EQ(converted_value1_ptr.get(), &value1);

  // Conversion from derived class pointer to base class pointer
  class foo {};
  class bar : public foo {};

  bar a_bar;
  non_owning_ptr<foo> const converted_foo_ptr = non_owning_ptr<bar>(&a_bar);
  ASSERT_EQ(converted_foo_ptr.get(), &a_bar);
}

/// @test A `non_owning_ptr` assigned from a raw pointer holds the new value
TEST(NonOwningPtr, IsAssignableFromPointerToT) {
  int const value = 10;
  non_owning_ptr<int const> a_ptr{};
  a_ptr = &value;
  ASSERT_EQ(a_ptr.get(), &value);
}

/// @test A `non_owning_ptr` assigned from a raw pointer convertible to the stored pointer holds the converted value
TEST(NonOwningPtr, IsAssignableFromPointerConvertibleToT) {
  // Simple conversion from non-const to const pointer
  int value1 = 10;  // NOLINT(readability-magic-numbers)
  non_owning_ptr<int const> converted_value1_ptr{};
  converted_value1_ptr = &value1;
  ASSERT_EQ(converted_value1_ptr.get(), &value1);

  // Conversion from derived class pointer to base class pointer
  class foo {};
  class bar : public foo {};

  bar a_bar;
  non_owning_ptr<foo> converted_foo_ptr{};
  converted_foo_ptr = &a_bar;
  ASSERT_EQ(converted_foo_ptr.get(), &a_bar);
}

/// @test `get` returns `nullptr` if called on a NULL `non_owning_ptr`
TEST(NonOwningPtr, GetCanBeCalledOnNullptr) { ASSERT_EQ(non_owning_ptr<int>(nullptr).get(), nullptr); }

/// @test `get` returns the stored pointer
TEST(NonOwningPtr, GetReturnsHeldPtr) {
  non_owning_ptr<int const> value_ptr{};

  int const value1 = 10;
  value_ptr = &value1;
  ASSERT_EQ(value_ptr.get(), &value1);

  int const value2 = 11;
  value_ptr = &value2;
  ASSERT_EQ(value_ptr.get(), &value2);

  int const value3 = 12;
  value_ptr = &value3;
  ASSERT_EQ(value_ptr.get(), &value3);
}

/// @test Comparison of `non_owning_ptr` to `nullPtr` has the same result as comparison of the stored value
TEST(NonOwningPtr, ComparisonToNullptrIsPointerComparison) {
  int const value = 10;
  int const* const value_raw = &value;
  non_owning_ptr<int const> const value_ptr(value_raw);
  int const* const null_raw = nullptr;
  non_owning_ptr<int const> const null_ptr(null_raw);

  ASSERT_EQ(value_ptr == nullptr, value_raw == nullptr);
  ASSERT_EQ(nullptr == value_ptr, nullptr == value_raw);
  ASSERT_EQ(null_ptr == nullptr, null_raw == nullptr);

  ASSERT_EQ(value_ptr != nullptr, value_raw != nullptr);
  ASSERT_EQ(nullptr != value_ptr, nullptr != value_raw);
  ASSERT_EQ(null_ptr != nullptr, null_raw != nullptr);
}

template <typename T>
// NOLINTNEXTLINE(readability-identifier-naming)
class NonOwningPtrComparisonTest : public ::testing::Test {
 public:
  using value_type = std::remove_reference_t<decltype(std::get<0>(std::declval<T>()))>;
  using lhs_ptr_type = std::remove_reference_t<decltype(std::get<1>(std::declval<T>()))>;
  using rhs_ptr_type = std::remove_reference_t<decltype(std::get<2>(std::declval<T>()))>;
};

class base_number {
 public:
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  base_number(int val)
      : val_(val) {}
  auto get() const -> int { return val_; }

 private:
  int val_;
};

class derived_number : public base_number {
  using base_number::base_number;
};

using comparison_types = ::testing::Types<
    std::tuple<int const, non_owning_ptr<int const>, non_owning_ptr<int const>>,
    std::tuple<int const, non_owning_ptr<int const>, int const*>,
    std::tuple<int const, int const*, non_owning_ptr<int const>>,
    std::tuple<derived_number, non_owning_ptr<base_number const>, derived_number const*>,
    std::tuple<derived_number, derived_number const*, non_owning_ptr<base_number const>>>;

TYPED_TEST_SUITE(NonOwningPtrComparisonTest, comparison_types, );

/// @test Equality comparison of `non_owning_ptr` against something else yields the result of comparing the stored
/// pointer against the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonOwningPtrComparisonTest, EqualityComparisonIsPointerComparison) {
  typename TestFixture::value_type const a_value{10};
  typename TestFixture::lhs_ptr_type const a_ptr{&a_value};
  typename TestFixture::rhs_ptr_type const b_ptr{&a_value};

  ASSERT_EQ(a_ptr, b_ptr);
}

/// @test Inequality comparison of `non_owning_ptr` against something else yields the result of comparing the stored
/// pointer against the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonOwningPtrComparisonTest, NotEqualityComparisonIsPointerComparison) {
  typename TestFixture::value_type const a_value{10};
  typename TestFixture::lhs_ptr_type const a_ptr{&a_value};
  typename TestFixture::value_type const b_value{11};
  typename TestFixture::rhs_ptr_type const b_ptr{&b_value};

  ASSERT_NE(a_ptr, b_ptr);
}

/// @test Less-than comparison of `non_owning_ptr` against something else yields the result of comparing the stored
/// pointer against the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonOwningPtrComparisonTest, LTComparisonIsPointerComparison) {
  arene::base::array<typename TestFixture::value_type, 2> const values{10, 11};
  typename TestFixture::lhs_ptr_type const a_ptr{values.data()};
  typename TestFixture::rhs_ptr_type const b_ptr{&values[1]};

  ASSERT_LT(a_ptr, b_ptr);
}

/// @test Less-than-or-equal comparison of `non_owning_ptr` against something else yields the result of comparing the
/// stored pointer against the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonOwningPtrComparisonTest, LEComparisonIsPointerComparison) {
  {
    arene::base::array<typename TestFixture::value_type, 2> const values{10, 11};
    typename TestFixture::lhs_ptr_type const a_ptr{values.data()};
    typename TestFixture::rhs_ptr_type const b_ptr{&values[1]};

    ASSERT_LE(a_ptr, b_ptr);
  }
  {
    typename TestFixture::value_type const a_value{10};
    typename TestFixture::lhs_ptr_type const a_ptr{&a_value};
    typename TestFixture::rhs_ptr_type const b_ptr{&a_value};
    ASSERT_LE(a_ptr, b_ptr);
  }
}

/// @test Greater-than comparison of `non_owning_ptr` against something else yields the result of comparing the
/// stored pointer against the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonOwningPtrComparisonTest, GTComparisonIsPointerComparison) {
  arene::base::array<typename TestFixture::value_type, 2> const values{10, 11};
  typename TestFixture::lhs_ptr_type const a_ptr{values.data()};
  typename TestFixture::rhs_ptr_type const b_ptr{&values[1]};

  ASSERT_GT(b_ptr, a_ptr);
}

/// @test Greater-than-or-equal comparison of `non_owning_ptr` against something else yields the result of comparing the
/// stored pointer against the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonOwningPtrComparisonTest, GEComparisonIsPointerComparison) {
  {
    arene::base::array<typename TestFixture::value_type, 2> const values{10, 11};
    typename TestFixture::lhs_ptr_type const a_ptr{values.data()};
    typename TestFixture::rhs_ptr_type const b_ptr{&values[1]};

    ASSERT_GE(b_ptr, a_ptr);
  }
  {
    typename TestFixture::value_type const a_value{10};
    typename TestFixture::lhs_ptr_type const a_ptr{&a_value};
    typename TestFixture::rhs_ptr_type const b_ptr{&a_value};
    ASSERT_GE(a_ptr, b_ptr);
  }
}

/// @test `swap` exchanges the stored pointers of `non_owning_ptr`
TEST(NonOwningPtr, IsSwappable) {
  int const value1 = 10;
  int const value2 = 12;
  non_owning_ptr<int const> a_ptr(&value1);
  non_owning_ptr<int const> b_ptr(&value2);

  ::arene::base::swap(a_ptr, b_ptr);
  ASSERT_EQ(a_ptr, &value2);
  ASSERT_EQ(b_ptr, &value1);
}

/// @test Invoking `reset` with no arguments sets the pointer to NULL
TEST(NonOwningPtr, ResetWithNoArgSetsNullptr) {
  int const value = 10;
  non_owning_ptr<int const> value_ptr(&value);
  value_ptr.reset();
  ASSERT_EQ(value_ptr, nullptr);
}

/// @test Invoking `reset` with an argument sets the pointer to argument value
TEST(NonOwningPtr, ResetWithPointerToTSetsValue) {
  int const value1 = 10;
  non_owning_ptr<int const> value_ptr(&value1);
  int const value2 = 12;
  value_ptr.reset(&value2);
  ASSERT_EQ(value_ptr, &value2);
}

/// @test Invoking `reset` with an argument that is convertible to the stored pointer type sets the pointer to the
/// converted value
TEST(NonOwningPtr, ResetWithPointerConvertibleToTSetsValue) {
  // Simple conversion from non-const to const pointer
  int const value1 = 10;
  non_owning_ptr<int const> converted_int_ptr(&value1);
  int value2 = 12;  // NOLINT(readability-magic-numbers)
  converted_int_ptr.reset(&value2);
  ASSERT_EQ(converted_int_ptr, &value2);

  // Conversion from derived class pointer to base class pointer
  class foo {};
  class bar : public foo {};

  foo a_foo;
  non_owning_ptr<foo> converted_foo_ptr(&a_foo);
  bar a_bar;
  converted_foo_ptr.reset(&a_bar);
  ASSERT_EQ(converted_foo_ptr, &a_bar);
}

/// @test Deferencing a `non_owning_ptr` returns a reference to the pointed-to object
TEST(NonOwningPtr, DereferenceOperatorReturnsReferenceToPointedObject) {
  int const value = 10;
  non_owning_ptr<int const> const value_ptr(&value);
  ASSERT_EQ(*value_ptr, value);
}

/// @test The Arrow operator on a `non_owning_ptr` returns the stored pointer
TEST(NonOwningPtr, ArrowOperatorReturnsPointerToT) {
  int const original_value = 11;
  std::tuple<int> original{original_value};
  non_owning_ptr<std::tuple<int>> const original_ptr{&original};
  int const swap_value = 10;
  std::tuple<int> swap_with{swap_value};
  original_ptr->swap(swap_with);

  ASSERT_EQ(std::get<0>(original), swap_value);
}

/// @test `non_owning_ptr` is explicitly convertible to `bool`
TEST(NonOwningPtr, BoolConversionIsExplicit) {
  static_assert(
      std::is_constructible<bool, non_owning_ptr<int>>::value,
      "Expected non_owning_ptr to be explicitly convertible to bool"
  );
  static_assert(
      !std::is_convertible<bool, non_owning_ptr<int>>::value,
      "Expected non_owning_ptr to not be implicitly convertible to bool"
  );
}

/// @test Converting a NULL `non_owning_ptr` to `bool` yields `false`
TEST(NonOwningPtr, BoolConversionFalseForNullptr) { ASSERT_FALSE(static_cast<bool>(non_owning_ptr<int>(nullptr))); }

/// @test Converting a non-NULL `non_owning_ptr` to `bool` yields `true`
TEST(NonOwningPtr, BoolConversionTrueForNonNullPtr) {
  int const value = 10;
  ASSERT_TRUE(static_cast<bool>(non_owning_ptr<int const>(&value)));
}

/// @test Invoking the `!` operator on a NULL `non_owning_ptr` to `bool` yields `true`
TEST(NonOwningPtr, BoolNegationOperatorTrueForNullptr) { ASSERT_TRUE(!non_owning_ptr<int>(nullptr)); }

/// @test Invoking the `!` operator on a non-NULL `non_owning_ptr` to `bool` yields `false`
TEST(NonOwningPtr, BoolNegationOperatorFalseForNonNullPtr) {
  int const value = 10;
  ASSERT_FALSE(!non_owning_ptr<int const>(&value));
}

/// @test The `std::hash` value for a `non_owning_ptr` is the hash value of the stored pointer
TEST(NonOwningPtr, StdHashIsPointerHash) {
  int const value = 10;
  non_owning_ptr<int const> const value_ptr(&value);
  ASSERT_EQ(std::hash<non_owning_ptr<int const>>{}(value_ptr), std::hash<int const*>{}(&value));
}

/// @test The return type of `make_non_owning_ptr` is deduced
TEST(MakeNonOwningPtr, ReturnTypeIsObjectPointerWithTypeDeducedFromInputPointer) {
  static_assert(
      (std::is_same<decltype(make_non_owning_ptr(std::declval<int*>())), non_owning_ptr<int>>::value),
      "Expected make_non_owning_ptr(int*) to return non_owning_ptr<int>"
  );

  static_assert(
      (std::is_same<decltype(make_non_owning_ptr(std::declval<int const*>())), non_owning_ptr<int const>>::value),
      "Expected make_non_owning_ptr(int*) to return non_owning_ptr<int>"
  );
}

/// @test The return type of `make_non_owning_ptr` is constructed from the input
TEST(MakeNonOwningPtr, ReturnsNonOwningPtrEquivalentToConstructingFromInputPtr) {
  static_assert(
      std::is_same<decltype(make_non_owning_ptr(std::declval<int*>())), non_owning_ptr<int>>::value,
      "Expected make_non_owning_ptr to be equivalent to "
      "std::remove_pointer<T>, it wasn't."
  );
  static_assert(
      std::is_same<decltype(make_non_owning_ptr(std::declval<int const*>())), non_owning_ptr<int const>>::value,
      "Expected make_non_owning_ptr to be equivalent to "
      "std::remove_pointer<T>, it wasn't."
  );

  int const value1 = 10;
  non_owning_ptr<int const> const explicit_ptr(&value1);

  auto const make_ptr = make_non_owning_ptr(&value1);
  ASSERT_EQ(make_ptr, explicit_ptr);
}

}  // namespace
