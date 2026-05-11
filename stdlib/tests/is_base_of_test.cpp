// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief Helper type to specify a base-derived relationship
/// @tparam Base base type
/// @tparam Derived derived type
///
template <class Base, class Derived>
struct test_case {
  using base_type = Base;
  using derived_type = Derived;
};

/// @brief Obtain a list of base types from a list of test case types
///
/// @{

template <class TestCases>
struct base_types_of {};
template <class... Base, class... Derived>
struct base_types_of<::testing::Types<test_case<Base, Derived>...>> {
  using type = ::testing::Types<Base...>;
};
template <class TestCases>
using base_types_of_t = typename base_types_of<TestCases>::type;

/// @}

/// @brief Obtain a list of derived types from a list of test case types
///
/// @{

template <class TestCases>
struct derived_types_of {};
template <class... Base, class... Derived>
struct derived_types_of<::testing::Types<test_case<Base, Derived>...>> {
  using type = ::testing::Types<Derived...>;
};
template <class TestCases>
using derived_types_of_t = typename derived_types_of<TestCases>::type;

/// @}

/// @brief Check if all values are equal to a given value
/// @tparam B value to check against
/// @tparam N number of values to check
/// @param values values to compare with @c value
/// @return @c true if B == values[i] for i in [0, N). Otherwise @c false.
///
template <bool B, std::size_t N>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto all_values_equal_to(bool const (&values)[N]) -> bool {
  // NOLINTNEXTLINE(readability-use-anyofallof)
  for (bool const val : values) {
    if (B != val) {
      return false;
    }
  }

  return true;
}

/// @brief Check if all types are unambiguously publicly derived from @c BaseCharacteristic
/// @tparam BaseCharacteristic desired unambiguous public base type
/// @tparam Traits types to check
/// @return @c true if
///   is_unambiguously_publicly_derived_from_v<Traits[i], BaseCharacteristic> is
///   @c true for i in [0, N). Otherwise @c false.
///
template <class BaseCharacteristic, class... Traits>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto all_are_unambiguously_publicly_derived_from() -> bool {
  return all_values_equal_to<true>({is_unambiguously_publicly_derived_from_v<Traits, BaseCharacteristic>...});
}

/// @brief Check that @c std::is_base_of matches an expected value
/// @tparam Expected expected result of @c std::is_base_of_v
/// @tparam Base0 base type passed to @c std::is_base_of
/// @tparam BaseN base type passed to @c std::is_base_of
/// @tparam Derived0 derived type passed to @c std::is_base_of
/// @tparam DerivedN derived type passed to @c std::is_base_of
///
/// Statically assert that @c std::is_base_of matches @c Expected for the
/// cartesian product of all @c Base and @c Derived types.
///
template <bool Expected, class Base0, class... BaseN, class Derived0, class... DerivedN>
constexpr auto
static_assert_cartesian_product_is_base_of_is(::testing::Types<Base0, BaseN...>, ::testing::Types<Derived0, DerivedN...>) noexcept
    -> void {
  static_assert(
      all_are_unambiguously_publicly_derived_from<
          std::bool_constant<Expected>,
          std::is_base_of<Base0, Derived0>,
          std::is_base_of<BaseN, Derived0>...,
          std::is_base_of<Base0, DerivedN>...>(),
      "'std::is_base<Base, Derived>' does not unambiguously publicly derive from expected base characteristic."
  );

  static_assert(
      all_values_equal_to<Expected>(
          {std::is_base_of_v<Base0, Derived0>,
           std::is_base_of_v<BaseN, Derived0>...,
           std::is_base_of_v<Base0, DerivedN>...}
      ),
      "'std::is_base_v<Base, Derived>' is not equal to expected base characteristic value."
  );
}

/// @brief Check that @c std::is_base_of matches an expected value
/// @tparam Expected expected result of @c std::is_base_of_v
/// @tparam Base cv-unqualified base type
/// @tparam Derived cv-unqualified derived type
///
/// Statically assert that @c std::is_base_of matches @c Expected for all
/// cv-qualified combinations of @c Base @c Derived.
///
template <bool Expected, class Base, class Derived>
constexpr auto static_assert_all_cv_qualified_is_base_of_is() noexcept -> void {
  static_assert_cartesian_product_is_base_of_is<Expected>(
      ::testing::Types<Base, Base const, Base volatile, Base const volatile>{},
      ::testing::Types<Derived, Derived const, Derived volatile, Derived const volatile>{}
  );
}

/// @brief base types under test
///
/// @{

struct base {};
struct another_base {};

/// @}

/// @brief derived types under test
///
/// @{

struct single_public_base : public base {};
struct single_protected_base : protected base {};
struct single_private_base : private base {};

struct single_virtual_base : public virtual base {};
struct also_single_virtual_base : public virtual base {};

struct transitive_public_public_base : public single_public_base {};
struct transitive_public_protected_base : public single_protected_base {};
struct transitive_public_private_base : public single_private_base {};

struct transitive_protected_public_base : protected single_public_base {};
struct transitive_protected_protected_base : protected single_protected_base {};
struct transitive_protected_private_base : protected single_private_base {};

struct transitive_private_public_base : private single_public_base {};
struct transitive_private_protected_base : private single_protected_base {};
struct transitive_private_private_base : private single_private_base {};

struct multiple_public_bases
    : public base
    , public another_base {};
struct multiple_protected_bases
    : protected base
    , protected another_base {};
struct multiple_private_bases
    : private base
    , private another_base {};

struct ambiguous_base
    : public single_public_base
    , public single_protected_base {};

struct diamond_virtual_base
    : public single_virtual_base
    , also_single_virtual_base {};

/// @}

using test_case_types = ::testing::Types<  //
    test_case<base, single_public_base>,
    test_case<base, single_protected_base>,
    test_case<base, single_private_base>,
    test_case<base, single_virtual_base>,
    test_case<base, also_single_virtual_base>,
    test_case<base, transitive_public_public_base>,
    test_case<base, transitive_public_protected_base>,
    test_case<base, transitive_public_private_base>,
    test_case<base, transitive_protected_public_base>,
    test_case<base, transitive_protected_protected_base>,
    test_case<base, transitive_protected_private_base>,
    test_case<base, transitive_private_public_base>,
    test_case<base, transitive_private_protected_base>,
    test_case<base, transitive_private_private_base>,
    test_case<base, multiple_public_bases>,
    test_case<base, multiple_protected_bases>,
    test_case<base, multiple_private_bases>,
    test_case<another_base, multiple_public_bases>,
    test_case<another_base, multiple_protected_bases>,
    test_case<another_base, multiple_private_bases>,
    test_case<base, ambiguous_base>,
    test_case<single_public_base, ambiguous_base>,
    test_case<single_protected_base, ambiguous_base>,
    test_case<base, diamond_virtual_base>,
    test_case<single_virtual_base, diamond_virtual_base>,
    test_case<also_single_virtual_base, diamond_virtual_base>>;

template <class T>
struct IsBaseOf : ::testing::Test {};

TYPED_TEST_SUITE(IsBaseOf, test_case_types, );

/// @test @c is_base_of defines static member @c value to @c true if @c Base is a base class of @c Derived
TYPED_TEST(IsBaseOf, BaseClassAndDerivedClassHasValueMemberTrue) {
  static_assert_all_cv_qualified_is_base_of_is<true, typename TypeParam::base_type, typename TypeParam::derived_type>();
}

/// @test @c is_base_of defines static member @c value to @c true if @c Base and @c Derived are the same class type
TYPED_TEST(IsBaseOf, SameClassForBaseAndDerivedHasValueMemberTrue) {
  static_assert_all_cv_qualified_is_base_of_is<true, typename TypeParam::base_type, typename TypeParam::base_type>();
  static_assert_all_cv_qualified_is_base_of_is<
      true,
      typename TypeParam::derived_type,
      typename TypeParam::derived_type>();
}

/// @test @c is_base_of defines static member @c value to @c false if @c Derived is a base class of @c Base
TYPED_TEST(IsBaseOf, BaseAndDerivedSwappedHasValueMemberFalse) {
  static_assert_all_cv_qualified_is_base_of_is<false, typename TypeParam::derived_type, typename TypeParam::base_type>(
  );
}

/// @brief Check that @c std::is_base_of matches an expected value
/// @tparam Expected expected result of @c std::is_base_of_v
/// @tparam BaseN base type passed to @c std::is_base_of
/// @tparam DerivedN derived type passed to @c std::is_base_of
///
/// Statically assert that @c std::is_base_of matches @c Expected for all
/// elementwise pairs of the ith @c Base type and ith @c Derived type.
///
template <bool Expected, class... BaseN, class... DerivedN>
constexpr auto
static_assert_elementwise_is_base_of_is(::testing::Types<BaseN...>, ::testing::Types<DerivedN...>) noexcept -> void {
  static_assert(
      all_are_unambiguously_publicly_derived_from<std::bool_constant<Expected>, std::is_base_of<BaseN, DerivedN>...>(),
      "'std::is_base<Base, Derived>' does not unambiguously publicly derive from expected base characteristic."
  );

  static_assert(
      all_values_equal_to<Expected>({std::is_base_of_v<BaseN, DerivedN>...}),
      "'std::is_base_v<Base, Derived>' is not equal to expected base characteristic value."
  );
}

using non_class_types = arene::base::type_lists::concat_unique_t<
    ::testing::non_referenceable_types,
    ::testing::scalar_types,
    ::testing::array_types,
    ::testing::function_types,
    ::testing::pointer_types,
    ::testing::member_pointer_types,
    ::testing::reference_types,
    ::testing::union_types,
    ::testing::enum_types>;

/// @test @c is_base_of defines static member @c value to @c false if either type is a non-class type
TEST(IsBaseOf, NonClassTypeHasValueMemberFalse) {
  static_assert_cartesian_product_is_base_of_is<false>(non_class_types{}, base_types_of_t<test_case_types>{});
  static_assert_cartesian_product_is_base_of_is<false>(non_class_types{}, derived_types_of_t<test_case_types>{});
  static_assert_cartesian_product_is_base_of_is<false>(base_types_of_t<test_case_types>{}, non_class_types{});
  static_assert_cartesian_product_is_base_of_is<false>(derived_types_of_t<test_case_types>{}, non_class_types{});
}

/// @test @c is_base_of defines static member @c value to @c false if both @c Base and @c Derived are the same non-class
/// type
TEST(IsBaseOf, SameNonClassTypeHasValueMemberFalse) {
  static_assert_elementwise_is_base_of_is<false>(non_class_types{}, non_class_types{});
}

/// @test @c is_base_of defines static member @c value to @c false if both @c
/// Base and @c Derived are class types but not related by inheritance
TEST(IsBaseOf, UnrelatedClassTypes) {
  static_assert_all_cv_qualified_is_base_of_is<false, base, another_base>();
  static_assert_all_cv_qualified_is_base_of_is<false, another_base, base>();

  static_assert_all_cv_qualified_is_base_of_is<false, ambiguous_base, diamond_virtual_base>();
  static_assert_all_cv_qualified_is_base_of_is<false, diamond_virtual_base, ambiguous_base>();

  struct has_base_as_member {
    base data;
  };

  static_assert_all_cv_qualified_is_base_of_is<false, base, has_base_as_member>();
  static_assert_all_cv_qualified_is_base_of_is<false, has_base_as_member, base>();
}

}  // namespace
