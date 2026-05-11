// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/constraints.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

using single_types = arene::base::type_lists::concat_unique_t<
    ::testing::extend_with_all_t<::testing::scalar_types>,
    ::testing::array_types,
    ::testing::union_types,
    ::testing::class_types>;

template <typename T>
class CommonTypeSingleTest : public testing::Test {};

TYPED_TEST_SUITE(CommonTypeSingleTest, single_types, );

/// @test Verify @c common_type decays the input types.
TYPED_TEST(CommonTypeSingleTest, TypesAreDecayed) {
  ::testing::StaticAssertTypeEq<std::common_type_t<TypeParam>, std::decay_t<TypeParam>>();
  ::testing::StaticAssertTypeEq<std::common_type_t<TypeParam, TypeParam>, std::decay_t<TypeParam>>();
  ::testing::StaticAssertTypeEq<std::common_type_t<TypeParam, TypeParam, TypeParam>, std::decay_t<TypeParam>>();
  ::testing::
      StaticAssertTypeEq<std::common_type_t<TypeParam, TypeParam, TypeParam, TypeParam>, std::decay_t<TypeParam>>();
}

/// @brief A simple user-defined type
class base_type {};

/// @brief A derived user-defined type
class derived_type : public base_type {};
/// @brief A derived user-defined type
class derived_type2 : public base_type {};

/// @brief User-defined types with conversion operators
class convertible_to_int {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator std::int32_t() const { return 2; }
};

using fn_type = void(int);

/// @brief Test case structure that pairs input type list with expected common type
/// @tparam InputList Type list containing input types for common_type
/// @tparam Expected Expected result type from common_type
template <class InputList, class Expected>
struct test_case {
  using inputs = InputList;
  using expected = Expected;
};

using arene::base::type_list;

using test_cases = ::testing::Types<
    // Check integer promotion rules are applied to get the common_type.
    test_case<type_list<std::int32_t, std::int16_t>, std::int32_t>,
    test_case<type_list<std::int32_t, std::int8_t, std::int16_t>, std::int32_t>,
    test_case<type_list<std::int8_t, std::int16_t, std::uint8_t, std::uint16_t>, int>,
    test_case<type_list<std::uint8_t, std::uint16_t, std::uint32_t>, std::uint32_t>,
    test_case<type_list<std::int32_t, std::int16_t, std::int64_t>, std::int64_t>,
    test_case<type_list<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>, std::uint64_t>,
    // Mix of value and cv qualified reference types that are all decayed to the same value type.
    test_case<type_list<std::int32_t, std::int32_t&>, std::int32_t>,
    test_case<type_list<std::int32_t, std::int32_t const&>, std::int32_t>,
    test_case<type_list<std::int32_t, std::int32_t volatile&>, std::int32_t>,
    test_case<type_list<std::int32_t, std::int32_t const volatile&>, std::int32_t>,
    test_case<type_list<std::int32_t&, std::int32_t>, std::int32_t>,
    test_case<type_list<std::int32_t const&, std::int32_t>, std::int32_t>,
    test_case<type_list<std::int32_t volatile&, std::int32_t>, std::int32_t>,
    test_case<type_list<std::int32_t const volatile&, std::int32_t>, std::int32_t>,
    test_case<type_list<std::int32_t&, std::int32_t, std::int32_t const&>, std::int32_t>,
    test_case<type_list<std::int32_t const&, std::int32_t, std::int32_t const&>, std::int32_t>,
    test_case<type_list<std::int32_t volatile&, std::int32_t, std::int32_t volatile&>, std::int32_t>,
    test_case<type_list<std::int32_t const volatile&, std::int32_t, std::int32_t const volatile&>, std::int32_t>,
    // Types in the same inheritance hierarchy
    test_case<type_list<base_type, derived_type>, base_type>,
    test_case<type_list<base_type, derived_type2>, base_type>,
    test_case<type_list<base_type, derived_type, derived_type2>, base_type>,
    // Types that are implicitly convertible to another type
    test_case<type_list<convertible_to_int, std::int32_t>, std::int32_t>,
    test_case<type_list<convertible_to_int, std::int8_t, std::int32_t>, std::int32_t>,
    // Mix of array types that all are decayed to a pointer.
    // NOLINTBEGIN(hicpp-avoid-c-arrays)
    test_case<type_list<std::int32_t*, std::int32_t[]>, std::int32_t*>,
    test_case<type_list<std::int32_t*, std::int32_t (&)[]>, std::int32_t*>,
    test_case<type_list<std::int32_t*, std::int32_t (&&)[]>, std::int32_t*>,
    test_case<type_list<std::int32_t*, std::int32_t[], std::int32_t[]>, std::int32_t*>,
    test_case<type_list<std::int32_t[], std::int32_t[], std::int32_t[]>, std::int32_t*>,
    test_case<type_list<std::int32_t*, std::int32_t[2]>, std::int32_t*>,
    test_case<type_list<std::int32_t*, std::int32_t (&)[2]>, std::int32_t*>,
    test_case<type_list<std::int32_t*, std::int32_t (&&)[2]>, std::int32_t*>,
    test_case<type_list<std::int32_t*, std::int32_t (&&)[2], std::int32_t (&&)[2]>, std::int32_t*>,
    // NOLINTEND(hicpp-avoid-c-arrays)
    // Mix of function types that all are decayed to a function pointer.
    test_case<type_list<fn_type, fn_type>, fn_type*>,
    test_case<type_list<fn_type&, fn_type>, fn_type*>,
    test_case<type_list<fn_type, fn_type&>, fn_type*>,
    test_case<type_list<fn_type, fn_type*>, fn_type*>,
    test_case<type_list<fn_type*, fn_type>, fn_type*>,
    test_case<type_list<fn_type*, fn_type*>, fn_type*>,
    test_case<type_list<fn_type*, fn_type, fn_type&>, fn_type*>>;

template <typename T>
class CommonTypeMultipleTypes : public testing::Test {};

TYPED_TEST_SUITE(CommonTypeMultipleTypes, test_cases, );

/// @test Verify @c common_type works as expected with different input types.
TYPED_TEST(CommonTypeMultipleTypes, CheckCommonTypeWithMultipleTypesEqualExpected) {
  using Inputs = typename TypeParam::inputs;
  using Expected = typename TypeParam::expected;

  using Result = arene::base::type_lists::apply_all_t<Inputs, std::common_type_t>;
  ::testing::StaticAssertTypeEq<Result, Expected>();
}

struct S {};
using no_member_type_types = ::testing::Types<
    type_list<>,
    type_list<int, S>,
    type_list<S, int>,
    type_list<S, int, int>,
    type_list<int, S, int>,
    type_list<int, int, S>>;
template <typename T>
class NoMemberTypeTestSuite : public testing::Test {};

TYPED_TEST_SUITE(NoMemberTypeTestSuite, no_member_type_types, );

/// @brief SFINAE helper to detect if a type has a nested 'type' member alias
/// @tparam T Type to check for nested 'type' member
// TODO Define this in one common place.
template <typename T, typename = arene::base::constraints<>>
struct has_type : std::false_type {};

/// @brief Specialization that matches when T::type exists
/// @tparam T Type that has a nested 'type' member alias
template <typename T>
struct has_type<T, arene::base::constraints<typename T::type>> : std::true_type {};

/// @brief Variable template to check if common_type has a 'type' member for given input list
/// @tparam Inputs Type list to check
template <class Inputs>
constexpr auto has_type_for_list_v = false;

/// @brief Specialization that unpacks type_list and checks if common_type has 'type' member
/// @tparam InputTypes Variadic pack of types from the type_list
template <class... InputTypes>
constexpr auto has_type_for_list_v<type_list<InputTypes...>> = has_type<std::common_type<InputTypes...>>::value;

/// @test Verify that no member alias named type exists in the result if there is no common type.
CONSTEXPR_TYPED_TEST(NoMemberTypeTestSuite, MemberAliasTypeIsNotSetIfNoCommonType) {
  EXPECT_FALSE(has_type_for_list_v<TypeParam>);
}

/// @brief Custom types for testing user specializations
class custom_type_a {};
class custom_type_b {};
class custom_common_result {};

class custom_type_specialized_no_common {};

}  // namespace

/// @brief Specialization of std::common_type for custom types
template <>
struct std::common_type<custom_type_a, custom_type_b> {
  using type = custom_common_result;
};

/// @brief Symmetric specialization (common_type should be commutative)
template <>
struct std::common_type<custom_type_b, custom_type_a> {
  using type = custom_common_result;
};

/// @brief Specialization of std::common_type for a custom types that is empty
template <>
struct std::common_type<custom_type_specialized_no_common, custom_type_specialized_no_common> {};

namespace {

/// @test Verify that user specializations of std::common_type are respected
TEST(CommonTypeUserSpecializationTest, UserSpecializationIsRespected) {
  // Test that our custom specialization works
  ::testing::StaticAssertTypeEq<std::common_type_t<custom_type_a, custom_type_b>, custom_common_result>();
  ::testing::StaticAssertTypeEq<std::common_type_t<custom_type_b, custom_type_a>, custom_common_result>();

  // Test with additional types - should still resolve to custom_common_result
  ::testing::StaticAssertTypeEq<
      std::common_type_t<  //
          custom_type_a,
          custom_type_a,
          custom_type_b>,
      custom_common_result>();

  ::testing::StaticAssertTypeEq<
      std::common_type_t<  //
          custom_type_b,
          custom_type_b,
          custom_type_a>,
      custom_common_result>();

  ::testing::StaticAssertTypeEq<
      std::common_type_t<  //
          custom_type_a,
          custom_type_b,
          custom_common_result>,
      custom_common_result>();
}

/// @test Verify that user specializations of std::common_type are respected
TEST(CommonTypeUserSpecializationTest, EmptyUserSpecializationIsRespected) {
  EXPECT_FALSE(has_type<std::common_type<custom_type_specialized_no_common, custom_type_specialized_no_common>>::value);

  // The single argument version should delegate to the two argument version.
  EXPECT_FALSE(has_type<std::common_type<custom_type_specialized_no_common>>::value);
}

}  // namespace
