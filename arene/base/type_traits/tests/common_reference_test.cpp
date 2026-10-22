// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/common_reference.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/common_type.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/duration.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/cartesian_product.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using ::arene::base::common_reference;
using ::arene::base::common_reference_t;
using ::arene::base::has_common_reference_with_v;

using ::arene::base::type_list;
namespace type_lists = ::arene::base::type_lists;

/// @brief Helper alias to extract the member typedef `type` from a common reference type.
template <typename CommonReference>
using use_member_type = typename CommonReference::type;

/// @brief Helper alias to check if a type has a member typedef `type`.
/// @tparam CommonReference The type to check for the presence of a member typedef `type`.
/// @return `true` if `CommonReference` has a member typedef `type`, `false` otherwise.
template <typename CommonReference>
constexpr bool has_member_type = ::arene::base::substitution_succeeds<use_member_type, CommonReference>;

/// @brief Generates a type list containing all reference qualifications of a given type.
/// @tparam T The type for which to generate all reference qualifications.
template <typename T>
using all_ref_qualifications_of_t = ::testing::Types<T, T&, T const&, T&&, T const&&>;

template <typename Expected>
struct expected {};

template <typename... Types>
struct test_case {
  using input_types = type_list<Types...>;
};

template <typename Expected, typename... Types>
struct test_case<expected<Expected>, Types...> : test_case<Types...> {
  using expected = Expected;
};

template <typename InputTypes>
using common_reference_for_types = type_lists::apply_all_t<InputTypes, common_reference>;

template <typename InputTypes>
using common_reference_for_types_t = typename common_reference_for_types<InputTypes>::type;

template <typename InputTypes>
using common_reference_t_for_types_t = type_lists::apply_all_t<InputTypes, common_reference_t>;

template <typename Types>
class CommonReferenceTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(CommonReferenceTest);

/// @test Given a set of types `Ts...` which share a common reference, when `common_reference<Ts...>` is invoked,
/// then it has a member typedef `type` which is exactly the expected common reference.
TYPED_TEST_P(CommonReferenceTest, HasExpectedCommonReference) {
  ::testing::
      StaticAssertTypeEq<typename TypeParam::expected, common_reference_for_types_t<typename TypeParam::input_types>>();
}

/// @test Given a set of types `Ts...` and for which `common_reference<Ts...>` has a member typedef `type`, then
/// `common_reference_t<Ts...>` is exactly `common_reference<Ts...>::type`.
TYPED_TEST_P(CommonReferenceTest, CommonReferenceTIsType) {
  ::testing::StaticAssertTypeEq<
      common_reference_t_for_types_t<typename TypeParam::input_types>,
      common_reference_for_types_t<typename TypeParam::input_types>>();
}

REGISTER_TYPED_TEST_SUITE_P(CommonReferenceTest, HasExpectedCommonReference, CommonReferenceTIsType);

template <typename Types>
class CommonReferenceNoCommonReferenceTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(CommonReferenceNoCommonReferenceTest);

/// @test Given a set of types `Ts...` which do not share a common reference, when `common_reference<Ts...>` is invoked,
/// then it has no member typedef `type`.
TYPED_TEST_P(CommonReferenceNoCommonReferenceTest, HasNoMemberType) {
  STATIC_ASSERT_FALSE(has_member_type<common_reference_for_types<typename TypeParam::input_types>>);
}

REGISTER_TYPED_TEST_SUITE_P(CommonReferenceNoCommonReferenceTest, HasNoMemberType);

/// @brief A user-defined type for testing common-reference cases.
struct a_udt {};

using zero_type_no_common_reference_types = ::testing::Types<test_case<>>;

using one_type_common_reference_types = ::testing::Types<
    test_case<expected<int>, int>,
    test_case<expected<int&>, int&>,
    test_case<expected<int const&>, int const&>,
    test_case<expected<int&&>, int&&>,
    test_case<expected<int const&&>, int const&&>,
    test_case<expected<void>, void>,
    test_case<expected<int (&)()>, int (&)()>,
    test_case<expected<a_udt>, a_udt>,
    test_case<expected<a_udt&>, a_udt&>,
    test_case<expected<a_udt const&>, a_udt const&>,
    test_case<expected<a_udt&&>, a_udt&&>,
    test_case<expected<a_udt const&&>, a_udt const&&>>;

/// @test Given an empty parameter pack, when `common_reference<>` is invoked, then it is well formed and has no member
/// typedef `type`.
INSTANTIATE_TYPED_TEST_SUITE_P(
    ZeroTypeNoCommonReference,
    CommonReferenceNoCommonReferenceTest,
    zero_type_no_common_reference_types,
);

/// @test Given a type `T`, when `common_reference<T>` is invoked, then it has a member typedef `type` which is exactly
/// `T`.
INSTANTIATE_TYPED_TEST_SUITE_P(OneTypeCommonReference, CommonReferenceTest, one_type_common_reference_types, );

using two_type_unrelated_types =
    type_lists::cartesian_product_as_t<test_case, all_ref_qualifications_of_t<int>, all_ref_qualifications_of_t<a_udt>>;

/// @brief A virtual base class for testing inheritance relationships in common reference scenarios.
class virtual_base {
 public:
  virtual ~virtual_base() = default;

 protected:
  virtual_base() = default;
  virtual_base(virtual_base const&) = default;
  virtual_base(virtual_base&&) = default;
  auto operator=(virtual_base const&) -> virtual_base& = default;
  auto operator=(virtual_base&&) -> virtual_base& = default;
};

/// @brief A class derived from a virtual base class for testing inheritance relationships in common reference
/// scenarios.
class derived_from_virtual_base : public virtual_base {};

/// @brief A class derived from a class that is itself derived from a virtual base class, for testing inheritance
/// relationships in common reference scenarios.
class derived_from_derived : public derived_from_virtual_base {};

/// @brief A class separately derived from a virtual base class for testing sibling inheritance relationships in common
/// reference scenarios.
class separately_derived_from_virtual_base : public virtual_base {};

using two_type_extended_sibling_lvalue_types = ::testing::Types<
    test_case<derived_from_derived&, separately_derived_from_virtual_base&>,
    test_case<separately_derived_from_virtual_base&, derived_from_derived&>>;

using two_type_extended_sibling_rvalue_types = ::testing::Types<
    test_case<derived_from_derived&&, separately_derived_from_virtual_base&&>,
    test_case<separately_derived_from_virtual_base&&, derived_from_derived&&>>;

using two_type_extended_sibling_mixed_reference_types = ::testing::Types<
    test_case<derived_from_derived&, separately_derived_from_virtual_base&&>,
    test_case<separately_derived_from_virtual_base&, derived_from_derived&&>,
    test_case<derived_from_derived&&, separately_derived_from_virtual_base&>,
    test_case<separately_derived_from_virtual_base&&, derived_from_derived&>>;

using two_type_extended_sibling_pointer_types = ::testing::Types<
    test_case<derived_from_derived*, separately_derived_from_virtual_base*>,
    test_case<separately_derived_from_virtual_base*, derived_from_derived*>>;

using two_type_extended_sibling_types = type_lists::concat_t<
    two_type_extended_sibling_lvalue_types,
    two_type_extended_sibling_rvalue_types,
    two_type_extended_sibling_mixed_reference_types,
    two_type_extended_sibling_pointer_types>;

using two_type_no_common_reference_types = type_lists::concat_t<
    two_type_unrelated_types,
    two_type_extended_sibling_types,
    ::testing::Types<test_case<void, int>, test_case<int, void>>>;

/// @test Given two types `T0` and `T1` which do not share a common reference, when `common_reference<T0, T1>` is
/// invoked, then it is well formed and does not define a member typedef `type`.
INSTANTIATE_TYPED_TEST_SUITE_P(
    TwoTypeNoCommonReference,
    CommonReferenceNoCommonReferenceTest,
    two_type_no_common_reference_types,
);

using two_type_lvalue_reference_qualification_types = ::testing::Types<
    test_case<expected<int&>, int&, int&>,
    test_case<expected<int const&>, int&, int const&>,
    test_case<expected<int volatile&>, int&, int volatile&>,
    test_case<expected<int const volatile&>, int&, int const volatile&>,
    test_case<expected<int const&>, int const&, int&>,
    test_case<expected<int const&>, int const&, int const&>,
    test_case<expected<int const volatile&>, int const&, int volatile&>,
    test_case<expected<int const volatile&>, int const&, int const volatile&>,
    test_case<expected<int volatile&>, int volatile&, int&>,
    test_case<expected<int const volatile&>, int volatile&, int const&>,
    test_case<expected<int volatile&>, int volatile&, int volatile&>,
    test_case<expected<int const volatile&>, int volatile&, int const volatile&>,
    test_case<expected<int const volatile&>, int const volatile&, int&>,
    test_case<expected<int const volatile&>, int const volatile&, int const&>,
    test_case<expected<int const volatile&>, int const volatile&, int volatile&>,
    test_case<expected<int const volatile&>, int const volatile&, int const volatile&>>;

using two_type_derived_lvalue_reference_types = ::testing::Types<
    test_case<expected<virtual_base&>, virtual_base&, derived_from_derived&>,
    test_case<expected<virtual_base const&>, virtual_base&, derived_from_derived const&>,
    test_case<expected<virtual_base volatile&>, virtual_base&, derived_from_derived volatile&>,
    test_case<expected<virtual_base const volatile&>, virtual_base&, derived_from_derived const volatile&>,
    test_case<expected<virtual_base const&>, virtual_base const&, derived_from_derived&>,
    test_case<expected<virtual_base const&>, virtual_base const&, derived_from_derived const&>,
    test_case<expected<virtual_base const volatile&>, virtual_base const&, derived_from_derived volatile&>,
    test_case<expected<virtual_base const volatile&>, virtual_base const&, derived_from_derived const volatile&>,
    test_case<expected<virtual_base volatile&>, virtual_base volatile&, derived_from_derived&>,
    test_case<expected<virtual_base const volatile&>, virtual_base volatile&, derived_from_derived const&>,
    test_case<expected<virtual_base volatile&>, virtual_base volatile&, derived_from_derived volatile&>,
    test_case<expected<virtual_base const volatile&>, virtual_base volatile&, derived_from_derived const volatile&>,
    test_case<expected<virtual_base const volatile&>, virtual_base const volatile&, derived_from_derived&>,
    test_case<expected<virtual_base const volatile&>, virtual_base const volatile&, derived_from_derived const&>,
    test_case<expected<virtual_base const volatile&>, virtual_base const volatile&, derived_from_derived volatile&>,
    test_case<
        expected<virtual_base const volatile&>,
        virtual_base const volatile&,
        derived_from_derived const volatile&>,
    test_case<expected<virtual_base&>, derived_from_derived&, virtual_base&>,
    test_case<expected<virtual_base const&>, derived_from_derived&, virtual_base const&>,
    test_case<expected<virtual_base volatile&>, derived_from_derived&, virtual_base volatile&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived&, virtual_base const volatile&>,
    test_case<expected<virtual_base const&>, derived_from_derived const&, virtual_base&>,
    test_case<expected<virtual_base const&>, derived_from_derived const&, virtual_base const&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived const&, virtual_base volatile&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived const&, virtual_base const volatile&>,
    test_case<expected<virtual_base volatile&>, derived_from_derived volatile&, virtual_base&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived volatile&, virtual_base const&>,
    test_case<expected<virtual_base volatile&>, derived_from_derived volatile&, virtual_base volatile&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived volatile&, virtual_base const volatile&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived const volatile&, virtual_base&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived const volatile&, virtual_base const&>,
    test_case<expected<virtual_base const volatile&>, derived_from_derived const volatile&, virtual_base volatile&>,
    test_case<
        expected<virtual_base const volatile&>,
        derived_from_derived const volatile&,
        virtual_base const volatile&>>;

using two_type_lvalue_reference_types =
    type_lists::concat_t<two_type_lvalue_reference_qualification_types, two_type_derived_lvalue_reference_types>;

/// @test Given two lvalue reference types, when `common_reference<T0, T1>` is invoked, then it has a member typedef
/// `type` which is exactly the simple reference type for `T0` and `T1`.
INSTANTIATE_TYPED_TEST_SUITE_P(TwoTypeLValueReference, CommonReferenceTest, two_type_lvalue_reference_types, );

using two_type_rvalue_reference_qualification_types = ::testing::Types<
    test_case<expected<int&&>, int&&, int&&>,
    test_case<expected<int const&&>, int&&, int const&&>,
    test_case<expected<int volatile&&>, int&&, int volatile&&>,
    test_case<expected<int const volatile&&>, int&&, int const volatile&&>,
    test_case<expected<int const&&>, int const&&, int&&>,
    test_case<expected<int const&&>, int const&&, int const&&>,
    test_case<expected<int const volatile&&>, int const&&, int volatile&&>,
    test_case<expected<int const volatile&&>, int const&&, int const volatile&&>,
    test_case<expected<int volatile&&>, int volatile&&, int&&>,
    test_case<expected<int const volatile&&>, int volatile&&, int const&&>,
    test_case<expected<int volatile&&>, int volatile&&, int volatile&&>,
    test_case<expected<int const volatile&&>, int volatile&&, int const volatile&&>,
    test_case<expected<int const volatile&&>, int const volatile&&, int&&>,
    test_case<expected<int const volatile&&>, int const volatile&&, int const&&>,
    test_case<expected<int const volatile&&>, int const volatile&&, int volatile&&>,
    test_case<expected<int const volatile&&>, int const volatile&&, int const volatile&&>>;

using two_type_derived_rvalue_reference_types = ::testing::Types<
    test_case<expected<virtual_base&&>, virtual_base&&, derived_from_derived&&>,
    test_case<expected<virtual_base const&&>, virtual_base&&, derived_from_derived const&&>,
    test_case<expected<virtual_base volatile&&>, virtual_base&&, derived_from_derived volatile&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base&&, derived_from_derived const volatile&&>,
    test_case<expected<virtual_base const&&>, virtual_base const&&, derived_from_derived&&>,
    test_case<expected<virtual_base const&&>, virtual_base const&&, derived_from_derived const&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base const&&, derived_from_derived volatile&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base const&&, derived_from_derived const volatile&&>,
    test_case<expected<virtual_base volatile&&>, virtual_base volatile&&, derived_from_derived&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base volatile&&, derived_from_derived const&&>,
    test_case<expected<virtual_base volatile&&>, virtual_base volatile&&, derived_from_derived volatile&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base volatile&&, derived_from_derived const volatile&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base const volatile&&, derived_from_derived&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base const volatile&&, derived_from_derived const&&>,
    test_case<expected<virtual_base const volatile&&>, virtual_base const volatile&&, derived_from_derived volatile&&>,
    test_case<
        expected<virtual_base const volatile&&>,
        virtual_base const volatile&&,
        derived_from_derived const volatile&&>,
    test_case<expected<virtual_base&&>, derived_from_derived&&, virtual_base&&>,
    test_case<expected<virtual_base const&&>, derived_from_derived&&, virtual_base const&&>,
    test_case<expected<virtual_base volatile&&>, derived_from_derived&&, virtual_base volatile&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived&&, virtual_base const volatile&&>,
    test_case<expected<virtual_base const&&>, derived_from_derived const&&, virtual_base&&>,
    test_case<expected<virtual_base const&&>, derived_from_derived const&&, virtual_base const&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived const&&, virtual_base volatile&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived const&&, virtual_base const volatile&&>,
    test_case<expected<virtual_base volatile&&>, derived_from_derived volatile&&, virtual_base&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived volatile&&, virtual_base const&&>,
    test_case<expected<virtual_base volatile&&>, derived_from_derived volatile&&, virtual_base volatile&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived volatile&&, virtual_base const volatile&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived const volatile&&, virtual_base&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived const volatile&&, virtual_base const&&>,
    test_case<expected<virtual_base const volatile&&>, derived_from_derived const volatile&&, virtual_base volatile&&>,
    test_case<
        expected<virtual_base const volatile&&>,
        derived_from_derived const volatile&&,
        virtual_base const volatile&&>>;

using two_type_rvalue_reference_types =
    type_lists::concat_t<two_type_rvalue_reference_qualification_types, two_type_derived_rvalue_reference_types>;

/// @test Given two rvalue reference types, when `common_reference<T0, T1>` is invoked, then it has a member typedef
/// `type` which is exactly the simple reference type for `T0` and `T1`.
INSTANTIATE_TYPED_TEST_SUITE_P(TwoTypeRValueReference, CommonReferenceTest, two_type_rvalue_reference_types, );

using two_type_mixed_reference_qualification_types = ::testing::Types<
    test_case<expected<int const&>, int&, int&&>,
    test_case<expected<int const&>, int&, int const&&>,
    test_case<expected<int const&>, int const&, int&&>,
    test_case<expected<int const&>, int const&, int const&&>,
    test_case<expected<int const&>, int&&, int&>,
    test_case<expected<int const&>, int const&&, int&>,
    test_case<expected<int const&>, int&&, int const&>,
    test_case<expected<int const&>, int const&&, int const&>,
    test_case<expected<int>, int&, int volatile&&>,
    test_case<expected<int>, int volatile&&, int&>>;

using two_type_mixed_derived_reference_types = ::testing::Types<
    test_case<expected<virtual_base const&>, virtual_base&, derived_from_derived&&>,
    test_case<expected<virtual_base const&>, virtual_base&, derived_from_derived const&&>,
    test_case<expected<virtual_base const&>, virtual_base const&, derived_from_derived&&>,
    test_case<expected<virtual_base const&>, virtual_base const&, derived_from_derived const&&>,
    test_case<expected<virtual_base const&>, derived_from_derived&, virtual_base&&>,
    test_case<expected<virtual_base const&>, derived_from_derived&, virtual_base const&&>,
    test_case<expected<virtual_base const&>, derived_from_derived const&, virtual_base&&>,
    test_case<expected<virtual_base const&>, derived_from_derived const&, virtual_base const&&>,
    test_case<expected<virtual_base const&>, derived_from_derived&&, virtual_base&>,
    test_case<expected<virtual_base const&>, derived_from_derived&&, virtual_base const&>,
    test_case<expected<virtual_base const&>, derived_from_derived const&&, virtual_base&>,
    test_case<expected<virtual_base const&>, derived_from_derived const&&, virtual_base const&>,
    test_case<expected<virtual_base const&>, virtual_base&&, derived_from_derived&>,
    test_case<expected<virtual_base const&>, virtual_base&&, derived_from_derived const&>,
    test_case<expected<virtual_base const&>, virtual_base const&&, derived_from_derived&>,
    test_case<expected<virtual_base const&>, virtual_base const&&, derived_from_derived const&>>;

using two_type_mixed_reference_types =
    type_lists::concat_t<two_type_mixed_reference_qualification_types, two_type_mixed_derived_reference_types>;

/// @test Given an lvalue reference type and an rvalue reference type, when `common_reference<T0, T1>` is invoked, then
/// it has a member typedef `type` which is exactly the simple reference type for `T0` and `T1`.
INSTANTIATE_TYPED_TEST_SUITE_P(TwoTypeMixedReference, CommonReferenceTest, two_type_mixed_reference_types, );

/// @brief A user-defined type for testing implicit conversions to and from a user-defined type.
struct implicit_convertible_to_udt {
  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr operator a_udt() const;
};

/// @brief A user-defined type for testing implicit conversions from a user-defined type.
struct implicitly_convertible_from_udt {
  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr implicitly_convertible_from_udt(a_udt);
};

/// @brief A user-defined type for testing implicit conversions to and from a user-defined type.
struct implicitly_convertible_to_and_from_udt {
  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr implicitly_convertible_to_and_from_udt(a_udt);

  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr operator a_udt();
};

using two_type_implicit_conversion_types = ::testing::Types<
    test_case<expected<a_udt const>, implicit_convertible_to_udt const, a_udt const>,
    test_case<expected<a_udt const>, a_udt const, implicit_convertible_to_udt const>,
    test_case<expected<implicitly_convertible_from_udt const>, a_udt const, implicitly_convertible_from_udt const>,
    test_case<expected<implicitly_convertible_from_udt const>, implicitly_convertible_from_udt const, a_udt const>>;

using two_type_pointer_types = ::testing::Types<
    test_case<expected<virtual_base*>, derived_from_derived*, virtual_base*>,
    test_case<expected<virtual_base*>, virtual_base*, derived_from_derived*>,
    test_case<expected<virtual_base*>, separately_derived_from_virtual_base*, virtual_base*>,
    test_case<expected<virtual_base*>, virtual_base*, separately_derived_from_virtual_base*>>;

using two_type_conditional_reference_types =
    type_lists::concat_t<two_type_implicit_conversion_types, two_type_pointer_types>;

/// @test Given two non-reference types `T0` and `T1`, if `COND-RES(T0,T1)` is well-formed, when
/// `common_reference<T0, T1>` is invoked, then it has a member typedef `type` which is exactly the type of that
/// expression.
INSTANTIATE_TYPED_TEST_SUITE_P(
    TwoTypeConditionalReference,
    CommonReferenceTest,
    two_type_conditional_reference_types,
);

constexpr std::intmax_t first_period_numerator = 2;
constexpr std::intmax_t first_period_denominator = 3;
constexpr std::intmax_t second_period_numerator = 5;
constexpr std::intmax_t second_period_denominator = 7;

using first_period = std::ratio<first_period_numerator, first_period_denominator>;
using second_period = std::ratio<second_period_numerator, second_period_denominator>;
using first_duration = std::chrono::duration<std::int64_t, first_period>;
using second_duration = std::chrono::duration<std::int64_t, second_period>;

using function_type = int();
using array_type = int[3];  // NOLINT(hicpp-avoid-c-arrays) We're deliberately testing an array type here.
using abominable_function_type = int() const;

using two_type_common_type_types = ::testing::Types<
    test_case<expected<std::common_type_t<first_duration, second_duration>>, first_duration, second_duration>,
    test_case<expected<std::common_type_t<first_duration, second_duration>>, second_duration, first_duration>,
    test_case<expected<std::common_type_t<function_type, function_type>>, function_type, function_type>,
    test_case<expected<std::common_type_t<array_type, array_type>>, array_type, array_type>>;

using special_two_type_types =
    ::testing::Types<test_case<expected<void>, void, void>, test_case<expected<int (&)()>, int (&)(), int (&)()>>;

/// @test Given two non-reference types `T0` and `T1` for which `std::common_type_t<T0, T1>` is well formed and for
/// which no other specialization of `common_reference` is defined, when `common_reference<T0, T1>` is invoked, then it
/// has a member typedef `type` which is exactly `std::common_type_t<T0, T1>`.
INSTANTIATE_TYPED_TEST_SUITE_P(TwoTypeCommonType, CommonReferenceTest, two_type_common_type_types, );

/// @test Given two types whose common reference is `void` or a function reference, when `common_reference<T0, T1>` is
/// invoked, then it has a member typedef `type` which is the expected common reference.
INSTANTIATE_TYPED_TEST_SUITE_P(TwoTypeSpecial, CommonReferenceTest, special_two_type_types, );

template <typename T>
using common_reference_for_type_t = common_reference_t<T, T>;

/// @test Given an abominable function type, when `common_reference_t<T,T>` is substituted, then the public API does not
/// produce a hard compilation error.
TEST(CommonReference, HandlesAbominableFunctionSubstitution) {
  (void)::arene::base::substitution_succeeds<common_reference_for_type_t, abominable_function_type>;
}

/// @brief A type with a basic common reference specialization.
struct basic_common_reference_type0 {};

/// @brief Another type with a basic common reference specialization.
struct basic_common_reference_type1 {};

/// @brief Result type returned by the test basic common reference specializations.
/// @tparam T0 The type produced by applying the first qualifier template.
/// @tparam T1 The type produced by applying the second qualifier template.
template <typename T0, typename T1>
struct basic_common_reference_result {
  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr basic_common_reference_result(T0);
  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr basic_common_reference_result(T1);
};

/// @brief A base type used to verify common-reference selection priority.
struct basic_common_reference_priority_base {};

/// @brief A derived type used to verify common-reference selection priority.
struct basic_common_reference_priority_derived : basic_common_reference_priority_base {};

/// @brief A result type used to distinguish a basic common reference from a simple common reference.
struct basic_common_reference_priority_result {
  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr basic_common_reference_priority_result(basic_common_reference_priority_base);
  // NOLINTNEXTLINE(hicpp-explicit-conversions) explicitly testing the behavior of implicit conversions
  constexpr basic_common_reference_priority_result(basic_common_reference_priority_derived);
};

/// @brief A type used to test a basic common reference that is not convertible from either source type.
struct non_convertible_basic_common_reference_type0 {};

/// @brief Another type used to test a basic common reference that is not convertible from either source type.
struct non_convertible_basic_common_reference_type1 {};

/// @brief A result type that is intentionally not constructible from the source types.
struct non_convertible_basic_common_reference_result {};

}  // namespace

template <template <class> class T0Qual, template <class> class T1Qual>
class arene::base::
    basic_common_reference<::basic_common_reference_type0, ::basic_common_reference_type1, T0Qual, T1Qual> {
 public:
  /// @brief Result containing the types produced by the two qualifier templates.
  /// @details This records the cv-qualification applied by @c T0Qual and @c T1Qual for the test.
  using type =
      ::basic_common_reference_result<T0Qual<::basic_common_reference_type0>, T1Qual<::basic_common_reference_type1>>;
};

template <template <class> class T0Qual, template <class> class T1Qual>
class arene::base::
    basic_common_reference<::basic_common_reference_type1, ::basic_common_reference_type0, T1Qual, T0Qual> {
 public:
  /// @brief Result containing the types produced by the two qualifier templates in reverse specialization order.
  /// @details This keeps the reverse specialization symmetric with the forward specialization for the test.
  using type =
      ::basic_common_reference_result<T0Qual<::basic_common_reference_type0>, T1Qual<::basic_common_reference_type1>>;
};

template <template <class> class T0Qual, template <class> class T1Qual>
class arene::base::basic_common_reference<
    ::non_convertible_basic_common_reference_type0,
    ::non_convertible_basic_common_reference_type1,
    T0Qual,
    T1Qual> {
 public:
  using type = ::non_convertible_basic_common_reference_result;
};

template <template <class> class T0Qual, template <class> class T1Qual>
class arene::base::basic_common_reference<
    ::non_convertible_basic_common_reference_type1,
    ::non_convertible_basic_common_reference_type0,
    T0Qual,
    T1Qual> {
 public:
  using type = ::non_convertible_basic_common_reference_result;
};

template <template <class> class T0Qual, template <class> class T1Qual>
class arene::base::basic_common_reference<
    ::basic_common_reference_priority_base,
    ::basic_common_reference_priority_derived,
    T0Qual,
    T1Qual> {
 public:
  using type = ::basic_common_reference_priority_result;
};

namespace {

using basic_common_reference_type_types = ::testing::Types<test_case<
    expected<basic_common_reference_result<basic_common_reference_type0, basic_common_reference_type1>>,
    basic_common_reference_type0,
    basic_common_reference_type1>>;

using basic_common_reference_qualification_types = ::testing::Types<
    test_case<
        expected<
            basic_common_reference_result<basic_common_reference_type0 const, basic_common_reference_type1 volatile>>,
        basic_common_reference_type0 const,
        basic_common_reference_type1 volatile>,
    test_case<
        expected<basic_common_reference_result<
            basic_common_reference_type0 const volatile,
            basic_common_reference_type1 const>>,
        basic_common_reference_type0 const volatile,
        basic_common_reference_type1 const>>;

/// @test Given two non-reference user-defined types with a `basic_common_reference` specialization, when
/// `common_reference<T0, T1>` is invoked, then it returns the `type` member provided by that specialization.
INSTANTIATE_TYPED_TEST_SUITE_P(TwoTypeBasicCommonReference, CommonReferenceTest, basic_common_reference_type_types, );

/// @test Given cv-qualified types with a `basic_common_reference` specialization, when `common_reference<T0, T1>` is
/// invoked, then the specialization receives qualifier templates that apply the cv-qualifiers of `T0` and `T1`.
INSTANTIATE_TYPED_TEST_SUITE_P(
    TwoTypeBasicCommonReferenceQualifiers,
    CommonReferenceTest,
    basic_common_reference_qualification_types,
);

/// @test Given a simple common reference and a lower-priority `basic_common_reference` specialization, when
/// `common_reference_t<T0, T1>` is evaluated, then the simple common reference is selected.
TEST(CommonReference, SimpleReferenceTakesPriorityOverBasicCommonReference) {
  ::testing::StaticAssertTypeEq<
      common_reference_t<basic_common_reference_priority_base&, basic_common_reference_priority_derived&>,
      basic_common_reference_priority_base&>();
}

using three_or_more_totally_unrelated_types =
    ::testing::Types<test_case<int, a_udt, virtual_base>, test_case<virtual_base, a_udt, int>>;

using three_or_more_common_initial_pair_types = ::testing::Types<
    test_case<int&, int const&, a_udt>,
    test_case<int const&, int&, a_udt>,
    test_case<int, std::int64_t, a_udt>,
    test_case<std::int64_t, int, a_udt>>;

using three_or_more_common_initial_triple_types = ::testing::Types<
    test_case<int&, int const&, int volatile&, a_udt>,
    test_case<int const&, int&, int const volatile&, a_udt>,
    test_case<int, std::int16_t, std::int64_t, a_udt>>;

using three_or_more_nonfunctional_sibling_inheritance_types = ::testing::Types<
    test_case<derived_from_derived&, separately_derived_from_virtual_base&, virtual_base&>,
    test_case<separately_derived_from_virtual_base&, derived_from_derived&, virtual_base&>,
    test_case<derived_from_derived&&, separately_derived_from_virtual_base&&, virtual_base&&>,
    test_case<separately_derived_from_virtual_base&&, derived_from_derived&&, virtual_base&&>,
    test_case<derived_from_derived&, separately_derived_from_virtual_base&&, virtual_base&>,
    test_case<separately_derived_from_virtual_base&, derived_from_derived&&, virtual_base&>,
    test_case<derived_from_derived*, separately_derived_from_virtual_base*, virtual_base*>,
    test_case<separately_derived_from_virtual_base*, derived_from_derived*, virtual_base*>>;

using three_or_more_no_common_reference_types = type_lists::concat_t<
    three_or_more_totally_unrelated_types,
    three_or_more_common_initial_pair_types,
    three_or_more_common_initial_triple_types,
    three_or_more_nonfunctional_sibling_inheritance_types>;

/// @test Given three or more types without a common reference, when `common_reference<Ts...>` is invoked, then it is
/// well formed and does not define a member typedef `type`.
INSTANTIATE_TYPED_TEST_SUITE_P(
    ThreeOrMoreNoCommonReference,
    CommonReferenceNoCommonReferenceTest,
    three_or_more_no_common_reference_types,
);

using three_or_more_reference_common_reference_types = ::testing::Types<
    test_case<expected<int const volatile&>, int&, int const&, int volatile&>,
    test_case<expected<int const volatile&>, int&, int const&, int volatile&, int const volatile&>,
    test_case<expected<int const volatile&&>, int&&, int const&&, int volatile&&>,
    test_case<expected<virtual_base const&>, virtual_base&, derived_from_derived&, virtual_base const&>>;

using three_or_more_value_types = ::testing::Types<
    test_case<expected<std::int64_t>, int, std::int16_t, std::int64_t>,
    test_case<expected<double>, int, float, double>,
    test_case<expected<double>, int, float, double, std::int16_t>>;

using three_or_more_mixed_value_types = ::testing::Types<
    test_case<expected<double>, int&, int const&, double>,
    test_case<expected<double>, int&, int&&, double>,
    test_case<expected<double>, int&, int const&, double, float>>;

using three_or_more_sibling_inheritance_types = ::testing::Types<
    test_case<expected<virtual_base&>, virtual_base&, derived_from_derived&, separately_derived_from_virtual_base&>,
    test_case<expected<virtual_base&>, virtual_base&, separately_derived_from_virtual_base&, derived_from_derived&>,
    test_case<expected<virtual_base*>, virtual_base*, derived_from_derived*, separately_derived_from_virtual_base*>,
    test_case<
        expected<virtual_base&>,
        virtual_base&,
        virtual_base&,
        derived_from_derived&,
        separately_derived_from_virtual_base&>>;

using three_or_more_common_reference_types = type_lists::concat_t<
    three_or_more_reference_common_reference_types,
    three_or_more_value_types,
    three_or_more_mixed_value_types,
    three_or_more_sibling_inheritance_types>;

/// @test Given three or more types with a common reference, when `common_reference<Ts...>` is invoked, then it has a
/// member typedef `type` which is exactly the expected common reference.
INSTANTIATE_TYPED_TEST_SUITE_P(ThreeOrMoreCommonReference, CommonReferenceTest, three_or_more_common_reference_types, );

template <typename Types>
class HasCommonReferenceWithVTest : public ::testing::Test {
 public:
  using t0 = type_lists::at_t<typename Types::input_types, 0U>;
  using t1 = type_lists::at_t<typename Types::input_types, 1U>;
};

using two_type_has_common_reference_with_types = type_lists::concat_t<
    two_type_lvalue_reference_types,
    two_type_rvalue_reference_types,
    two_type_mixed_reference_types,
    two_type_conditional_reference_types,
    two_type_common_type_types,
    basic_common_reference_type_types>;

template <typename Types>
class HasCommonReferenceWithVCommonReference : public HasCommonReferenceWithVTest<Types> {};

TYPED_TEST_SUITE(HasCommonReferenceWithVCommonReference, two_type_has_common_reference_with_types, );

/// @test Given two types that share a common reference and are each convertible to it, when
/// `has_common_reference_with_v<T0, T1>` is evaluated, then it is `true`.
TYPED_TEST(HasCommonReferenceWithVCommonReference, IsTrue) {
  STATIC_ASSERT_TRUE((has_common_reference_with_v<typename TestFixture::t0, typename TestFixture::t1>));
}

/// @brief Types that have a common reference but are not convertible to it. This generally could only happen where
/// @c basic_common_reference is specialized but the types themselves do not provide implicit conversions to the common
/// reference type.
using two_type_basic_common_reference_non_convertible_types = ::testing::Types<
    test_case<non_convertible_basic_common_reference_type0, non_convertible_basic_common_reference_type1>,
    test_case<non_convertible_basic_common_reference_type1, non_convertible_basic_common_reference_type0>>;

template <typename Types>
class HasCommonReferenceWithVCommonReferenceNonConvertible : public HasCommonReferenceWithVTest<Types> {};

TYPED_TEST_SUITE(
    HasCommonReferenceWithVCommonReferenceNonConvertible,
    two_type_basic_common_reference_non_convertible_types,
);

/// @test Given two types that share a specialized common reference but are not convertible to that reference, when
/// `has_common_reference_with_v<T0, T1>` is evaluated, then it is `false`.
TYPED_TEST(HasCommonReferenceWithVCommonReferenceNonConvertible, IsFalse) {
  STATIC_ASSERT_FALSE((has_common_reference_with_v<typename TestFixture::t0, typename TestFixture::t1>));
}

template <typename Types>
class HasCommonReferenceWithVNoCommonReference : public HasCommonReferenceWithVTest<Types> {};

TYPED_TEST_SUITE(HasCommonReferenceWithVNoCommonReference, two_type_no_common_reference_types, );

/// @test Given two types that do not share a common reference, when `has_common_reference_with_v<T0, T1>` is evaluated,
/// then it is `false`.
TYPED_TEST(HasCommonReferenceWithVNoCommonReference, IsFalse) {
  STATIC_ASSERT_FALSE((has_common_reference_with_v<typename TestFixture::t0, typename TestFixture::t1>));
}

}  // namespace
