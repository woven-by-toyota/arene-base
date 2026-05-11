// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/constraints.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/cstddef"
#include "stdlib/include/iterator"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

template <typename T, typename = arene::base::constraints<>>
extern constexpr bool has_difference_type = false;

template <typename T>
extern constexpr bool has_difference_type<T, arene::base::constraints<typename T::difference_type>> = true;

template <typename T, typename = arene::base::constraints<>>
extern constexpr bool has_value_type = false;

template <typename T>
extern constexpr bool has_value_type<T, arene::base::constraints<typename T::value_type>> = true;

template <typename T, typename = arene::base::constraints<>>
extern constexpr bool has_reference = false;

template <typename T>
extern constexpr bool has_reference<T, arene::base::constraints<typename T::reference>> = true;

template <typename T, typename = arene::base::constraints<>>
extern constexpr bool has_pointer = false;

template <typename T>
extern constexpr bool has_pointer<T, arene::base::constraints<typename T::pointer>> = true;

template <typename T, typename = arene::base::constraints<>>
extern constexpr bool has_iterator_category = false;

template <typename T>
extern constexpr bool has_iterator_category<T, arene::base::constraints<typename T::iterator_category>> = true;

using pointer_type = void*;
using integral_type = int;
using floating_point_type = double;
enum enum_type {};
union union_type {};
using array_type = int[];  // NOLINT(hicpp-avoid-c-arrays)
struct class_type {};
using member_pointer_type = int class_type::*;

using object_types = ::testing::Types<
    std::nullptr_t,
    pointer_type,
    integral_type,
    floating_point_type,
    enum_type,
    union_type,
    array_type,
    class_type,
    member_pointer_type>;

template <class T>
struct IteratorTraits : ::testing::Test {};

TYPED_TEST_SUITE(IteratorTraits, object_types, );

/// @test Check that all five typedefs are defined for pointer types
TYPED_TEST(IteratorTraits, TraitsForPointer) {
  static_assert(has_difference_type<std::iterator_traits<TypeParam*>>, "Must have member");
  static_assert(has_value_type<std::iterator_traits<TypeParam*>>, "Must have member");
  static_assert(has_reference<std::iterator_traits<TypeParam*>>, "Must have member");
  static_assert(has_pointer<std::iterator_traits<TypeParam*>>, "Must have member");
  static_assert(has_iterator_category<std::iterator_traits<TypeParam*>>, "Must have member");
  ::testing::StaticAssertTypeEq<std::ptrdiff_t, typename std::iterator_traits<TypeParam*>::difference_type>();
  ::testing::StaticAssertTypeEq<TypeParam, typename std::iterator_traits<TypeParam*>::value_type>();
  ::testing::StaticAssertTypeEq<TypeParam*, typename std::iterator_traits<TypeParam*>::pointer>();
  ::testing::StaticAssertTypeEq<TypeParam&, typename std::iterator_traits<TypeParam*>::reference>();
  ::testing::
      StaticAssertTypeEq<std::random_access_iterator_tag, typename std::iterator_traits<TypeParam*>::iterator_category>(
      );
}

/// @test Check that all five typedefs are defined for pointer to const types
TYPED_TEST(IteratorTraits, TraitsForPointerConst) {
  static_assert(has_difference_type<std::iterator_traits<TypeParam const*>>, "Must have member");
  static_assert(has_value_type<std::iterator_traits<TypeParam const*>>, "Must have member");
  static_assert(has_reference<std::iterator_traits<TypeParam const*>>, "Must have member");
  static_assert(has_pointer<std::iterator_traits<TypeParam const*>>, "Must have member");
  static_assert(has_iterator_category<std::iterator_traits<TypeParam const*>>, "Must have member");
  ::testing::StaticAssertTypeEq<std::ptrdiff_t, typename std::iterator_traits<TypeParam const*>::difference_type>();
  ::testing::StaticAssertTypeEq<TypeParam, typename std::iterator_traits<TypeParam const*>::value_type>();
  ::testing::StaticAssertTypeEq<TypeParam const*, typename std::iterator_traits<TypeParam const*>::pointer>();
  ::testing::StaticAssertTypeEq<TypeParam const&, typename std::iterator_traits<TypeParam const*>::reference>();
  ::testing::StaticAssertTypeEq<
      std::random_access_iterator_tag,
      typename std::iterator_traits<TypeParam const*>::iterator_category>();
}

/// @test Check that all five typedefs are defined for pointer to volatile types
TYPED_TEST(IteratorTraits, TraitsForPointerVolatile) {
  ::testing::StaticAssertTypeEq<std::ptrdiff_t, typename std::iterator_traits<TypeParam volatile*>::difference_type>();
  ::testing::StaticAssertTypeEq<TypeParam volatile, typename std::iterator_traits<TypeParam volatile*>::value_type>();
  ::testing::StaticAssertTypeEq<TypeParam volatile*, typename std::iterator_traits<TypeParam volatile*>::pointer>();
  ::testing::StaticAssertTypeEq<TypeParam volatile&, typename std::iterator_traits<TypeParam volatile*>::reference>();
  ::testing::StaticAssertTypeEq<
      std::random_access_iterator_tag,
      typename std::iterator_traits<TypeParam volatile*>::iterator_category>();
}

/// @test Check that all five typedefs are defined for pointer to const volatile types
TYPED_TEST(IteratorTraits, TraitsForPointerConstVolatile) {
  ::testing::
      StaticAssertTypeEq<std::ptrdiff_t, typename std::iterator_traits<TypeParam const volatile*>::difference_type>();
  ::testing::
      StaticAssertTypeEq<TypeParam volatile, typename std::iterator_traits<TypeParam const volatile*>::value_type>();
  ::testing::
      StaticAssertTypeEq<TypeParam const volatile*, typename std::iterator_traits<TypeParam const volatile*>::pointer>(
      );
  ::testing::StaticAssertTypeEq<
      TypeParam const volatile&,
      typename std::iterator_traits<TypeParam const volatile*>::reference>();
  ::testing::StaticAssertTypeEq<
      std::random_access_iterator_tag,
      typename std::iterator_traits<TypeParam const volatile*>::iterator_category>();
}

/// @test Check that all five typedefs are defined for pointer to function
/// @note This is no longer the case from C++20
TEST(IteratorTraits, TraitsForPointerToFunction) {
  using function_type = void();

  ::testing::StaticAssertTypeEq<std::ptrdiff_t, typename std::iterator_traits<function_type*>::difference_type>();
  ::testing::StaticAssertTypeEq<function_type, typename std::iterator_traits<function_type*>::value_type>();
  ::testing::StaticAssertTypeEq<function_type*, typename std::iterator_traits<function_type*>::pointer>();
  ::testing::StaticAssertTypeEq<function_type&, typename std::iterator_traits<function_type*>::reference>();
  ::testing::StaticAssertTypeEq<
      std::random_access_iterator_tag,
      typename std::iterator_traits<function_type*>::iterator_category>();
}

/// @brief a user defined iterator type
///
/// This specialization simply uses distinct types that do not satisfy
/// semantic requirements for an iterator.
struct dummy_iterator {
  using difference_type = std::integral_constant<int, 0>;
  using value_type = std::integral_constant<int, 1>;
  using pointer = std::integral_constant<int, 2>;
  using reference = std::integral_constant<int, 3>;
  using iterator_category = std::integral_constant<int, 4>;
};

/// @test Check that all five typedefs are defined for a user specified specialization
TEST(IteratorTraits, TraitsForDummyIterator) {
  static_assert(has_difference_type<std::iterator_traits<dummy_iterator>>, "Must have member");
  static_assert(has_value_type<std::iterator_traits<dummy_iterator>>, "Must have member");
  static_assert(has_reference<std::iterator_traits<dummy_iterator>>, "Must have member");
  static_assert(has_pointer<std::iterator_traits<dummy_iterator>>, "Must have member");
  static_assert(has_iterator_category<std::iterator_traits<dummy_iterator>>, "Must have member");
  ::testing::StaticAssertTypeEq<
      std::integral_constant<int, 0>,
      typename std::iterator_traits<dummy_iterator>::difference_type>();
  ::testing::
      StaticAssertTypeEq<std::integral_constant<int, 1>, typename std::iterator_traits<dummy_iterator>::value_type>();
  ::testing::StaticAssertTypeEq<std::integral_constant<int, 2>, typename std::iterator_traits<dummy_iterator>::pointer>(
  );
  ::testing::
      StaticAssertTypeEq<std::integral_constant<int, 3>, typename std::iterator_traits<dummy_iterator>::reference>();
  ::testing::StaticAssertTypeEq<
      std::integral_constant<int, 4>,
      typename std::iterator_traits<dummy_iterator>::iterator_category>();
}

template <class T>
struct IteratorTraitsNonIteratorTests : ::testing::Test {};

using non_iterator_types = arene::base::type_lists::concat_unique_t<
    ::testing::integral_types,
    ::testing::floating_point_types,
    ::testing::class_types,
    ::testing::reference_types,
    ::testing::null_pointer_types,
    ::testing::array_types,
    ::testing::void_types>;

TYPED_TEST_SUITE(IteratorTraitsNonIteratorTests, non_iterator_types, );

/// @test Check that iterator_traits does not define member type aliases if the type is not an iterator
TYPED_TEST(IteratorTraitsNonIteratorTests, NoMembersForNonIterators) {
  static_assert(!has_difference_type<std::iterator_traits<TypeParam>>, "Must not have member");
  static_assert(!has_value_type<std::iterator_traits<TypeParam>>, "Must not have member");
  static_assert(!has_reference<std::iterator_traits<TypeParam>>, "Must not have member");
  static_assert(!has_pointer<std::iterator_traits<TypeParam>>, "Must not have member");
  static_assert(!has_iterator_category<std::iterator_traits<TypeParam>>, "Must not have member");
}

}  // namespace
