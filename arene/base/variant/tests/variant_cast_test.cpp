// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file variant_cast_test.cpp
/// @brief Unit tests for arene::base::variant_cast
///

#include "arene/base/variant/variant_cast.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/cartesian_product.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/contains.hpp"
#include "arene/base/type_list/drop.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/in_place.hpp"
#include "arene/base/variant/traits.hpp"
#include "arene/base/variant/variant.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

// can't be anonymous namespace because gtest limitation in abstract tests with referencing fixture names
namespace variant_cast_test {

using ::arene::base::type_list;
using ::arene::base::variant;
using ::arene::base::variant_cast;
using ::arene::base::variant_cast_detail::compatible_type_index;
using ::arene::base::variant_cast_detail::encode_type_compatibility;
using ::arene::base::variant_cast_detail::has_type_compatibility_v;
using ::arene::base::variant_cast_detail::incompatible_type_index;
using ::arene::base::variant_cast_detail::index_of_compatible_type_v;
using ::arene::base::variant_cast_detail::type_compatibility;
using ::arene::base::variant_cast_detail::type_compatibility_bits;
using ::arene::base::variant_cast_detail::type_compatibility_v;
namespace type_lists = ::arene::base::type_lists;

/// @brief A counting wrapper around std::int32_t to give it meaningful differentiation between copy/move.
using cv_i32 = ::testing::counting_wrapper<std::int32_t>;

/// @brief A stock inline string to use in the tests.
using inline_str = ::arene::base::inline_string<10>;

template <type_compatibility Compatibility>
using type_compatibility_wrapper = std::integral_constant<type_compatibility, Compatibility>;

template <typename Compatibility>
class EncodeTypeCompatibilityTest : public ::testing::Test {};

using all_compatibilities = ::testing::Types<
    type_compatibility_wrapper<type_compatibility::incompatible>,
    type_compatibility_wrapper<type_compatibility::explicit_conversion>,
    type_compatibility_wrapper<type_compatibility::implicit_conversion>,
    type_compatibility_wrapper<type_compatibility::exact_match>>;

TYPED_TEST_SUITE(EncodeTypeCompatibilityTest, all_compatibilities, );

/// @test Given a <c>arene::base::variant_cast_detail::type_compatibility</c> value and a false predicate, when
/// <c>arene::base::variant_cast_detail::encode_type_compatibility</c> is called, then it returns all zeros.
CONSTEXPR_TYPED_TEST(EncodeTypeCompatibilityTest, ForFalseReturnsAllZeros) {
  CONSTEXPR_ASSERT(encode_type_compatibility(TypeParam::value, false) == 0U);
}

/// @test Given a <c>arene::base::variant_cast_detail::type_compatibility</c> value and a true predicate, when
/// <c>arene::base::variant_cast_detail::encode_type_compatibility</c> is called, then the encoded bits are an
/// identity for the compatibility.
CONSTEXPR_TYPED_TEST(EncodeTypeCompatibilityTest, ForTrueTheEncodedBitsAreAnIdentityForTheCompatibility) {
  auto const compatibility = TypeParam::value;
  CONSTEXPR_ASSERT(
      encode_type_compatibility(compatibility, true) == static_cast<type_compatibility_bits>(compatibility)
  );
}

template <typename Compatibility>
class HasTypeCompatibilityTest : public ::testing::Test {};

using non_incompatible_compatibilities = ::testing::Types<
    type_compatibility_wrapper<type_compatibility::explicit_conversion>,
    type_compatibility_wrapper<type_compatibility::implicit_conversion>,
    type_compatibility_wrapper<type_compatibility::exact_match>>;

TYPED_TEST_SUITE(HasTypeCompatibilityTest, non_incompatible_compatibilities, );

/// @test Given a <c>arene::base::variant_cast_detail::type_compatibility_bits</c> value of zero, when
/// <c>arene::base::variant_cast_detail::has_type_compatibility_v</c> is called with any compatibility other than
/// <c>arene::base::variant_cast_detail::type_compatibility::incompatible</c>, then it returns false.
CONSTEXPR_TYPED_TEST(HasTypeCompatibilityTest, ReturnsFalseForAnyCompatibilityWhenTheBitsAreZero) {
  auto const compatibility = TypeParam::value;
  CONSTEXPR_ASSERT(!has_type_compatibility_v(0U, compatibility));
}

/// @test Given a <c>arene::base::variant_cast_detail::type_compatibility_bits</c> value with the requested
/// compatibility encoded, when <c>arene::base::variant_cast_detail::has_type_compatibility_v</c> is called, then
/// it returns true.
CONSTEXPR_TYPED_TEST(HasTypeCompatibilityTest, ReturnsTrueIfTheRequestedCompatibilityIsPresentInTheBits) {
  auto const compatibility = TypeParam::value;
  auto const bits = encode_type_compatibility(compatibility, true);
  CONSTEXPR_ASSERT(has_type_compatibility_v(bits, compatibility));

  auto const all_set_bits = type_compatibility_bits{0xFFU};
  CONSTEXPR_ASSERT(has_type_compatibility_v(all_set_bits, compatibility));
}

/// @test Given a <c>arene::base::variant_cast_detail::type_compatibility_bits</c> value of zero, when
/// <c>arene::base::variant_cast_detail::has_type_compatibility_v</c> is called with
/// <c>arene::base::variant_cast_detail::type_compatibility::incompatible</c>, then it returns true.
CONSTEXPR_TEST(
    HasTypeCompatibilityIncompatibilityTest,
    ReturnsTrueIfTheRequestedCompatibilityIsIncompatibleAndTheBitsAreZero
) {
  CONSTEXPR_ASSERT(has_type_compatibility_v(type_compatibility_bits{}, type_compatibility::incompatible));
}

/// @test Given a <c>arene::base::variant_cast_detail::type_compatibility_bits</c> value with any bits set, when
/// <c>arene::base::variant_cast_detail::has_type_compatibility_v</c> is called with
/// <c>arene::base::variant_cast_detail::type_compatibility::incompatible</c>, then it returns false.
CONSTEXPR_TEST(
    HasTypeCompatibilityIncompatibilityTest,
    ReturnsFalseIfTheRequestedCompatibilityIsIncompatibleAndTheBitsAreNotZero
) {
  for (auto const bits :
       {type_compatibility_bits{0x01U},
        type_compatibility_bits{0x02U},
        type_compatibility_bits{0x04U},
        type_compatibility_bits{0xFFU}}) {
    CONSTEXPR_ASSERT(!has_type_compatibility_v(bits, type_compatibility::incompatible));
  }
}

/// @test Given two identical types with cvref qualifiers stripped from the source, when
/// <c>arene::base::variant_cast_detail::type_compatibility</c> is evaluated, then it returns
/// a bit field where <c>arene::base::variant_cast_detail::type_compatibility::exact_match</c> is set.
TEST(TypeCompatibility, ReturnsExactMatchForSameTypeWithFromCRefQualifiersStripped) {
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<std::int32_t, std::int32_t>(), type_compatibility::exact_match)
  );
  STATIC_ASSERT_TRUE(has_type_compatibility_v(
      type_compatibility_v<std::int32_t const&, std::int32_t>(),
      type_compatibility::exact_match
  ));
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<std::int32_t&&, std::int32_t>(), type_compatibility::exact_match)
  );
  STATIC_ASSERT_TRUE(has_type_compatibility_v(type_compatibility_v<cv_i32, cv_i32>(), type_compatibility::exact_match));
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<cv_i32 const&, cv_i32>(), type_compatibility::exact_match)
  );
  STATIC_ASSERT_TRUE(has_type_compatibility_v(type_compatibility_v<cv_i32&&, cv_i32>(), type_compatibility::exact_match)
  );
}

/// @test Given a source type that is implicitly convertible to the target type, when
/// <c>arene::base::variant_cast_detail::type_compatibility</c> is evaluated, then it returns
/// a bit field where <c>arene::base::variant_cast_detail::type_compatibility::implicit_conversion</c> is set.
TEST(TypeCompatibility, ReturnsImplicitConversionForImplicitlyConstructibleType) {
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<std::int32_t, cv_i32>(), type_compatibility::implicit_conversion)
  );
  STATIC_ASSERT_TRUE(has_type_compatibility_v(
      type_compatibility_v<std::int32_t const&, cv_i32>(),
      type_compatibility::implicit_conversion
  ));
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<std::int32_t&&, cv_i32>(), type_compatibility::implicit_conversion)
  );
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<float, double>(), type_compatibility::implicit_conversion)
  );
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<float const&, double>(), type_compatibility::implicit_conversion)
  );
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<float&&, double>(), type_compatibility::implicit_conversion)
  );
}

/// @test Given a source type that is only explicitly convertible to the target type, when
/// <c>arene::base::variant_cast_detail::type_compatibility</c> is evaluated, then it returns
/// a bit field where <c>arene::base::variant_cast_detail::type_compatibility::explicit_conversion</c> is set.
TEST(TypeCompatibility, ReturnsExplicitConversionForExplicitlyConstructibleType) {
  STATIC_ASSERT_TRUE(has_type_compatibility_v(
      type_compatibility_v<::arene::base::string_view, inline_str>(),
      type_compatibility::explicit_conversion
  ));
  STATIC_ASSERT_TRUE(has_type_compatibility_v(
      type_compatibility_v<::arene::base::string_view const&, inline_str>(),
      type_compatibility::explicit_conversion
  ));
  STATIC_ASSERT_TRUE(has_type_compatibility_v(
      type_compatibility_v<::arene::base::string_view&&, inline_str>(),
      type_compatibility::explicit_conversion
  ));
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<double, float>(), type_compatibility::explicit_conversion)
  );
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<double const&, float>(), type_compatibility::explicit_conversion)
  );
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<double&&, float>(), type_compatibility::explicit_conversion)
  );
}

/// @test Given two types that are not constructible from one another, when
/// <c>arene::base::variant_cast_detail::type_compatibility</c> is evaluated, then it returns
/// a bitfield with no bits set.
TEST(TypeCompatibility, ReturnsIncompatibleForIncompatibleTypes) {
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<cv_i32, inline_str>(), type_compatibility::incompatible)
  );
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<cv_i32 const&, inline_str>(), type_compatibility::incompatible)
  );
  STATIC_ASSERT_TRUE(
      has_type_compatibility_v(type_compatibility_v<cv_i32&&, inline_str>(), type_compatibility::incompatible)
  );
}

/// @test Given an empty or fully incompatible compatibility list, when
/// <c>arene::base::variant_cast_detail::index_of_compatible_type_v</c> is evaluated, then it returns
/// <c>arene::base::variant_cast_detail::incompatible_type_index()</c>.
TEST(IndexOfCompatibleType, IfNoCompatibleTypeExistsReturnsIncompatibleTypeIndex) {
  STATIC_ASSERT_EQ(index_of_compatible_type_v({}), incompatible_type_index());
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      incompatible_type_index()
  );
}

/// @test Given a compatibility list with multiple explicit conversions and no implicit or exact matches, when
/// <c>arene::base::variant_cast_detail::index_of_compatible_type_v</c> is evaluated, then it returns
/// <c>arene::base::variant_cast_detail::incompatible_type_index()</c>.
TEST(IndexOfCompatibleType, IfMultipleExplicitlyConstructibleTypesExistReturnsIncompatibleTypeIndex) {
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::explicit_conversion, true)}
      ),
      incompatible_type_index()
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      incompatible_type_index()
  );
}

/// @test Given a compatibility list with multiple implicit conversions and no exact matches, when
/// <c>arene::base::variant_cast_detail::index_of_compatible_type_v</c> is evaluated, then it returns
/// <c>arene::base::variant_cast_detail::incompatible_type_index()</c>.
TEST(IndexOfCompatibleType, IfMultipleImplicitlyConstructibleTypesExistReturnsIncompatibleTypeIndex) {
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::implicit_conversion, true),
           encode_type_compatibility(type_compatibility::implicit_conversion, true)}
      ),
      incompatible_type_index()
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::implicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::implicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      incompatible_type_index()
  );
}

/// @test Given a compatibility list with exactly one implicit conversion among explicit or incompatible entries, when
/// <c>arene::base::variant_cast_detail::index_of_compatible_type_v</c> is evaluated, then it returns the index of
/// the implicitly convertible type.
TEST(
    IndexOfCompatibleType,
    IfOneImplicitlyConvertibleTypeExistsWithAnyNumberOfExplicitOrIncompatibleTypesReturnsIndexOfImplicitlyConvertibleType
) {
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v({encode_type_compatibility(type_compatibility::implicit_conversion, true)}),
      0U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::implicit_conversion, true),
           encode_type_compatibility(type_compatibility::explicit_conversion, true)}
      ),
      0U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::implicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      2U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::implicit_conversion, true),
           encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::explicit_conversion, true)}
      ),
      1U
  );
}

/// @test Given a compatibility list with exactly one explicit conversion among incompatible entries, when
/// <c>arene::base::variant_cast_detail::index_of_compatible_type_v</c> is evaluated, then it returns the index of
/// the explicitly convertible type.
TEST(
    IndexOfCompatibleType,
    IfOneExplicitlyConvertibleTypeExistsWithAnyNumberOfIncompatibleTypesReturnsIndexOfExplicitlyConvertibleType
) {
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v({encode_type_compatibility(type_compatibility::explicit_conversion, true)}),
      0U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      0U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::explicit_conversion, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      2U
  );
}

/// @test Given a compatibility list containing one or more exact matches, when
/// <c>arene::base::variant_cast_detail::index_of_compatible_type_v</c> is evaluated, then it returns the index of
/// the first exact match regardless of other implicit or explicit conversions.
TEST(
    IndexOfCompatibleType,
    IfAnyNumberOfExactMatchesExistTheIndexOfTheFirstIsReturnedRegardlessOfOtherImplicitOrExplicitConversions
) {
  STATIC_ASSERT_EQ(index_of_compatible_type_v({encode_type_compatibility(type_compatibility::exact_match, true)}), 0U);
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::exact_match, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      0U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v(
          {encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::incompatible, true),
           encode_type_compatibility(type_compatibility::exact_match, true),
           encode_type_compatibility(type_compatibility::incompatible, true)}
      ),
      2U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v({
          encode_type_compatibility(type_compatibility::incompatible, true),
          encode_type_compatibility(type_compatibility::implicit_conversion, true),
          encode_type_compatibility(type_compatibility::explicit_conversion, true),
          encode_type_compatibility(type_compatibility::exact_match, true),
      }),
      3U
  );
  STATIC_ASSERT_EQ(
      index_of_compatible_type_v({
          encode_type_compatibility(type_compatibility::exact_match, true),
          encode_type_compatibility(type_compatibility::implicit_conversion, true),
          encode_type_compatibility(type_compatibility::explicit_conversion, true),
          encode_type_compatibility(type_compatibility::exact_match, true),
      }),
      0U
  );
}

/// @brief A metafunction to drop one type from a type list
template <typename TL>
using drop_one_t = type_lists::drop_t<TL, 1U>;

/// @brief Static property test case for invocability and return type of variant_cast<ToVariant>(FromVariant).
/// These depend only on FromVariant and ToVariant, not on a held value.
template <typename FromTypes, typename ToTypes>
struct static_property_case {
  using from_variant = type_lists::apply_all_t<FromTypes, variant>;
  using to_variant = type_lists::apply_all_t<ToTypes, variant>;
};

/// @brief Behavioral tests: holds_alternative and value preservation.
/// Each test case operates on exactly one ActiveType within one (subset, superset) pair.
/// The typed fixture encodes <ActiveType, FromTypes, ToTypes> so that on
/// failure the test name directly tells you the exact combination that broke.
template <typename ActiveType, typename FromTypes, typename ToTypes>
class behavior_case : public static_property_case<FromTypes, ToTypes> {
 public:
  using active_type = ActiveType;
  using from_variant = typename static_property_case<FromTypes, ToTypes>::from_variant;
  using to_variant = typename static_property_case<FromTypes, ToTypes>::to_variant;
  static constexpr bool is_valid = ::arene::base::type_lists::contains_v<FromTypes, ActiveType>;
};

/// @brief Filter to remove invalid behavior cases where the active type is not present in the subset.
template <typename BehaviorCase>
using is_valid_behavior_case = std::integral_constant<bool, BehaviorCase::is_valid>;

template <typename Case>
class VariantCastStaticPropertyTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(VariantCastStaticPropertyTest);

/// @test Given source and target variant types that are compatible type spaces, when
/// <c>arene::base::variant_cast</c> is called with an lvalue source variant, then the cast expression is valid.
TYPED_TEST_P(VariantCastStaticPropertyTest, IsInvocableFromLvalue) {
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;
  using variant_cast_fn = decltype(variant_cast<to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant&>>();
}

/// @test Given source and target variant types that are compatible type spaces, when
/// <c>arene::base::variant_cast</c> is called with a const lvalue source variant, then the cast expression is valid.
TYPED_TEST_P(VariantCastStaticPropertyTest, IsInvocableFromConstLvalue) {
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;
  using variant_cast_fn = decltype(variant_cast<to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant const&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant const&>>();
}

/// @test Given source and target variant types that are compatible type spaces, when
/// <c>arene::base::variant_cast</c> is called with an rvalue source variant, then the cast expression is valid.
TYPED_TEST_P(VariantCastStaticPropertyTest, IsInvocableFromRvalue) {
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;
  using variant_cast_fn = decltype(variant_cast<to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant&&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant&&>>();
}

/// @test Given source and target variant types that are compatible type spaces, when
/// <c>arene::base::variant_cast</c> is called with a const rvalue source variant, then the cast expression is valid.
TYPED_TEST_P(VariantCastStaticPropertyTest, IsInvocableFromConstRvalue) {
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;
  using variant_cast_fn = decltype(variant_cast<to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant const&&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant const&&>>();
}

REGISTER_TYPED_TEST_SUITE_P(
    VariantCastStaticPropertyTest,
    IsInvocableFromLvalue,
    IsInvocableFromConstLvalue,
    IsInvocableFromRvalue,
    IsInvocableFromConstRvalue
);

/// @brief Superset orderings (all permutations of the full set).
using expand_superset_orders = ::testing::Types<
    type_list<std::int32_t, cv_i32, std::int64_t>,
    type_list<std::int32_t, std::int64_t, cv_i32>,
    type_list<cv_i32, std::int32_t, std::int64_t>,
    type_list<cv_i32, std::int64_t, std::int32_t>,
    type_list<std::int64_t, std::int32_t, cv_i32>,
    type_list<std::int64_t, cv_i32, std::int32_t>>;

/// @brief All the subset permutations with two arguments
using expand_two_type_subset_orders =
    type_lists::remove_duplicates_t<type_lists::apply_each_t<expand_superset_orders, drop_one_t>>;

/// @brief All the subset permutations with one argument
using expand_one_type_subset_orders =
    type_lists::remove_duplicates_t<type_lists::apply_each_t<expand_two_type_subset_orders, drop_one_t>>;

/// @brief All the subset permutations with one or two arguments
using expand_subset_orders = type_lists::concat_t<expand_one_type_subset_orders, expand_two_type_subset_orders>;

using expand_static_property_cases =
    type_lists::cartesian_product_as_t<static_property_case, expand_subset_orders, expand_superset_orders>;

INSTANTIATE_TYPED_TEST_SUITE_P(Expanding, VariantCastStaticPropertyTest, expand_static_property_cases, );

/// @brief The premutations of a subset type space that can be compressed into.
using compress_subset_orders = ::testing::Types<type_list<inline_str, double>, type_list<double, inline_str>>;
/// @brief The premutations of a superset type space that can be compressed from.
using compress_superset_orders = ::testing::Types<
    type_list<inline_str, double, ::arene::base::string_view>,
    type_list<double, inline_str, ::arene::base::string_view>,
    type_list<inline_str, ::arene::base::string_view, double>,
    type_list<double, ::arene::base::string_view, inline_str>,
    type_list<::arene::base::string_view, inline_str, double>,
    type_list<::arene::base::string_view, double, inline_str>>;

using compress_static_property_cases =
    type_lists::cartesian_product_as_t<static_property_case, compress_superset_orders, compress_subset_orders>;

INSTANTIATE_TYPED_TEST_SUITE_P(Compressing, VariantCastStaticPropertyTest, compress_static_property_cases, );

/// @brief A functor to produce a value of the active type for a given test case.
template <typename T>
struct make_active_value {
  constexpr auto operator()() const noexcept -> T { return T{42}; }
};

/// @brief Specialization of make_active_value for inline_str
template <>
struct make_active_value<inline_str> {
  constexpr auto operator()() const noexcept -> inline_str { return inline_str{"42"}; }
};

template <>
struct make_active_value<::arene::base::string_view> {
  constexpr auto operator()() const noexcept -> ::arene::base::string_view { return ::arene::base::string_view{"24"}; }
};

template <typename From, typename ToVariant>
constexpr std::size_t compatible_type_index_v = compatible_type_index<From, ToVariant>::value;

template <typename From, typename... Tos>
constexpr std::size_t compatible_type_index_v<From, variant<Tos...>> = compatible_type_index<From, Tos...>::value;

template <typename Case>
class VariantCastBehaviourTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(VariantCastBehaviourTest);

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

/// @test Given a source variant holding <c>ActiveType</c>, when <c>arene::base::variant_cast</c> is called with an
/// lvalue source variant, then the result holds <c>ActiveType</c>.
TYPED_TEST_P(VariantCastBehaviourTest, LvalueCastResultHoldsActiveAlternative) {
  using active_type = typename TypeParam::active_type;
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;

  // NOLINTNEXTLINE(misc-const-correctness) Intentionally non-const to validate lvalue (non-const) semantics.
  from_variant src{make_active_value<active_type>{}()};
  to_variant const result = variant_cast<to_variant>(src);

  constexpr auto compatible_type_idx = compatible_type_index_v<active_type, to_variant>;
  EXPECT_EQ(result.index(), compatible_type_idx);
}

/// @test Given a source variant holding <c>ActiveType</c>, when <c>arene::base::variant_cast</c> is called with an
/// lvalue source variant, then the held value is preserved.
TYPED_TEST_P(VariantCastBehaviourTest, LvalueCastResultPreservesValue) {
  using active_type = typename TypeParam::active_type;
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;

  // NOLINTNEXTLINE(misc-const-correctness) Intentionally non-const to validate lvalue (non-const) semantics.
  from_variant src{make_active_value<active_type>{}()};
  to_variant const result = variant_cast<to_variant>(src);

  constexpr auto compatible_type_idx = compatible_type_index_v<active_type, to_variant>;
  EXPECT_EQ(::arene::base::get<compatible_type_idx>(result), make_active_value<active_type>{}());
}

/// @test Given a source variant holding <c>ActiveType</c>, when <c>arene::base::variant_cast</c> is called with a
/// const lvalue source variant, then the result holds <c>ActiveType</c>.
TYPED_TEST_P(VariantCastBehaviourTest, ConstLvalueCastResultHoldsActiveAlternative) {
  using active_type = typename TypeParam::active_type;
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;

  from_variant const src{make_active_value<active_type>{}()};
  to_variant const result = variant_cast<to_variant>(src);

  constexpr auto compatible_type_idx = compatible_type_index_v<active_type, to_variant>;
  EXPECT_EQ(result.index(), compatible_type_idx);
}

/// @test Given a source variant holding <c>ActiveType</c>, when <c>arene::base::variant_cast</c> is called with a
/// const lvalue source variant, then the held value is preserved.
TYPED_TEST_P(VariantCastBehaviourTest, ConstLvalueCastResultPreservesValue) {
  using active_type = typename TypeParam::active_type;
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;

  from_variant const src{make_active_value<active_type>{}()};
  to_variant const result = variant_cast<to_variant>(src);

  constexpr auto compatible_type_idx = compatible_type_index_v<active_type, to_variant>;
  EXPECT_EQ(::arene::base::get<compatible_type_idx>(result), make_active_value<active_type>{}());
}

/// @test Given a source variant holding <c>ActiveType</c>, when <c>arene::base::variant_cast</c> is called with an
/// rvalue source variant, then the result holds <c>ActiveType</c>.
TYPED_TEST_P(VariantCastBehaviourTest, RvalueCastResultHoldsActiveAlternative) {
  using active_type = typename TypeParam::active_type;
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;

  from_variant src{make_active_value<active_type>{}()};
  to_variant const result = variant_cast<to_variant>(std::move(src));

  constexpr auto compatible_type_idx = compatible_type_index_v<active_type, to_variant>;
  EXPECT_EQ(result.index(), compatible_type_idx);
}

/// @test Given a source variant holding <c>ActiveType</c>, when <c>arene::base::variant_cast</c> is called with an
/// rvalue source variant, then the held value is preserved.
TYPED_TEST_P(VariantCastBehaviourTest, RvalueCastResultPreservesValue) {
  using active_type = typename TypeParam::active_type;
  using from_variant = typename TypeParam::from_variant;
  using to_variant = typename TypeParam::to_variant;

  from_variant src{make_active_value<active_type>{}()};
  to_variant const result = variant_cast<to_variant>(std::move(src));

  constexpr auto compatible_type_idx = compatible_type_index_v<active_type, to_variant>;
  EXPECT_EQ(::arene::base::get<compatible_type_idx>(result), make_active_value<active_type>{}());
}

REGISTER_TYPED_TEST_SUITE_P(
    VariantCastBehaviourTest,
    LvalueCastResultHoldsActiveAlternative,
    LvalueCastResultPreservesValue,
    ConstLvalueCastResultHoldsActiveAlternative,
    ConstLvalueCastResultPreservesValue,
    RvalueCastResultHoldsActiveAlternative,
    RvalueCastResultPreservesValue
);

/// @brief Active-type dimension: each type is tested individually. We have to cast this to gtest's types due to a quirk
/// in gtest's meta expansion.
using expand_active_types = type_lists::apply_all_t<type_lists::at_t<expand_superset_orders, 0U>, ::testing::Types>;

/// @brief Cartesian product of the three dimensions: active type, subset order, superset order
/// @note The product produces invalid behavior classes where the active type is not present in the subset. These are
///       filtered out.
using expanding_behaviour_cases = type_lists::filter_t<
    type_lists::
        cartesian_product_as_t<behavior_case, expand_active_types, expand_subset_orders, expand_superset_orders>,
    is_valid_behavior_case>;

INSTANTIATE_TYPED_TEST_SUITE_P(Expanding, VariantCastBehaviourTest, expanding_behaviour_cases, );

/// @brief Active-type dimension: each type is tested individually. We have to cast this to gtest's types due to a quirk
/// in gtest's meta expansion.
using compress_active_types = type_lists::apply_all_t<type_lists::at_t<compress_superset_orders, 0U>, ::testing::Types>;

/// @brief Cartesian product of the three dimensions: active type, subset order, superset order
/// @note The product produces invalid behavior classes where the active type is not present in the subset. These are
///       filtered out.
using compress_behaviour_cases = type_lists::filter_t<
    type_lists::
        cartesian_product_as_t<behavior_case, compress_active_types, compress_superset_orders, compress_subset_orders>,
    is_valid_behavior_case>;

INSTANTIATE_TYPED_TEST_SUITE_P(Compressing, VariantCastBehaviourTest, compress_behaviour_cases, );

ARENE_IGNORE_END();

class explicit_conversion_int {
 public:
  explicit explicit_conversion_int(std::int32_t value)
      : value_(value) {}
  constexpr auto operator==(explicit_conversion_int const& other) const noexcept -> bool {
    return value_ == other.value_;
  }

 private:
  std::int32_t value_;
};

using requires_explicit_conversion_variant = ::testing::Types<explicit_conversion_int>;
using requires_implicit_conversion_variant = ::testing::Types<cv_i32>;
using implicit_selected_over_explicit_variant = ::testing::Types<cv_i32, explicit_conversion_int>;

template <typename ToTypes>
class VariantCastCastRequiresConversionTest : public ::testing::Test {
 protected:
  using convertible_source_type = std::int32_t;
  using from_variant = variant<convertible_source_type>;
  using to_variant = type_lists::apply_all_t<ToTypes, variant>;
};

using convert_to_variant_types = ::testing::Types<
    requires_explicit_conversion_variant,
    requires_implicit_conversion_variant,
    implicit_selected_over_explicit_variant>;

TYPED_TEST_SUITE(VariantCastCastRequiresConversionTest, convert_to_variant_types, );

/// @test Given source and target variant types where target alternatives are only constructible from source
/// alternatives, when <c>arene::base::variant_cast</c> is called with an lvalue source variant, then the cast
/// expression is valid.
TYPED_TEST(VariantCastCastRequiresConversionTest, IsInvocableFromLvalue) {
  using variant_cast_fn = decltype(variant_cast<typename TestFixture::to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, typename TestFixture::from_variant&>));
  ::testing::StaticAssertTypeEq<
      typename TestFixture::to_variant,
      ::arene::base::invoke_result_t<variant_cast_fn, typename TestFixture::from_variant&>>();
}

/// @test Given source and target variant types where target alternatives are only constructible from source
/// alternatives, when <c>arene::base::variant_cast</c> is called with a const lvalue source variant, then the cast
/// expression is valid.
TYPED_TEST(VariantCastCastRequiresConversionTest, IsInvocableFromConstLvalue) {
  using variant_cast_fn = decltype(variant_cast<typename TestFixture::to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, typename TestFixture::from_variant const&>));
  ::testing::StaticAssertTypeEq<
      typename TestFixture::to_variant,
      ::arene::base::invoke_result_t<variant_cast_fn, typename TestFixture::from_variant const&>>();
}

/// @test Given source and target variant types where target alternatives are only constructible from source
/// alternatives, when <c>arene::base::variant_cast</c> is called with an rvalue source variant, then the cast
/// expression is valid.
TYPED_TEST(VariantCastCastRequiresConversionTest, IsInvocableFromRvalue) {
  using variant_cast_fn = decltype(variant_cast<typename TestFixture::to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, typename TestFixture::from_variant&&>));
  ::testing::StaticAssertTypeEq<
      typename TestFixture::to_variant,
      ::arene::base::invoke_result_t<variant_cast_fn, typename TestFixture::from_variant&&>>();
}

/// @test Given source and target variant types where target alternatives are only constructible from source
/// alternatives, when <c>arene::base::variant_cast</c> is called with a const rvalue source variant, then the cast
/// expression is valid.
TYPED_TEST(VariantCastCastRequiresConversionTest, IsInvocableFromConstRvalue) {
  using variant_cast_fn = decltype(variant_cast<typename TestFixture::to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, typename TestFixture::from_variant const&&>));
  ::testing::StaticAssertTypeEq<
      typename TestFixture::to_variant,
      ::arene::base::invoke_result_t<variant_cast_fn, typename TestFixture::from_variant const&&>>();
}

/// @test Given a source variant holding an alternative which must be converted from, and a target variant with a
/// compatible alternative, when <c>arene::base::variant_cast</c> is called, then the result holds the matching
/// alternative.
TYPED_TEST(VariantCastCastRequiresConversionTest, CastPreservesValueIntoConstructibleAlternative) {
  auto const src = typename TestFixture::from_variant{typename TestFixture::convertible_source_type{123}};
  auto const result = variant_cast<typename TestFixture::to_variant>(src);

  constexpr auto compatible_type_idx =
      compatible_type_index_v<typename TestFixture::convertible_source_type, typename TestFixture::to_variant>;
  EXPECT_TRUE(result.index() == compatible_type_idx);
}

class VariantCastForwardingTest : public ::testing::Test {
 protected:
  using from_variant = variant<std::int32_t, cv_i32>;
  using to_variant = variant<std::int32_t, cv_i32, std::int64_t>;
};

/// @test Given a source variant holding <c>cv_i32</c>, when <c>arene::base::variant_cast</c> is called with a
/// non-const lvalue source variant, then the held <c>cv_i32</c> value is copied into the target variant.
TEST_F(VariantCastForwardingTest, LvalueCastCopiesCvI32Value) {
  from_variant const src{cv_i32{42}};
  cv_i32::count = {};

  auto const result = variant_cast<to_variant>(src);

  EXPECT_TRUE(::arene::base::holds_alternative<cv_i32>(result));
  EXPECT_EQ(::arene::base::get<cv_i32>(result), cv_i32{42});
  EXPECT_EQ(cv_i32::count.copy_ctor, 1U);
  EXPECT_EQ(cv_i32::count.move_ctor, 0U);
}

/// @test Given a source variant holding <c>cv_i32</c>, when <c>arene::base::variant_cast</c> is called with a
/// const lvalue source variant, then the held <c>cv_i32</c> value is copied into the target variant.
TEST_F(VariantCastForwardingTest, ConstLvalueCastCopiesCvI32Value) {
  from_variant const src{cv_i32{42}};
  cv_i32::count = {};

  auto const result = variant_cast<to_variant>(src);

  EXPECT_TRUE(::arene::base::holds_alternative<cv_i32>(result));
  EXPECT_EQ(::arene::base::get<cv_i32>(result), cv_i32{42});
  EXPECT_EQ(cv_i32::count.copy_ctor, 1U);
  EXPECT_EQ(cv_i32::count.move_ctor, 0U);
}

/// @test Given a source variant holding <c>cv_i32</c>, when <c>arene::base::variant_cast</c> is called with an
/// rvalue source variant, then the held <c>cv_i32</c> value is moved into the target variant.
TEST_F(VariantCastForwardingTest, RvalueCastMovesCvI32Value) {
  from_variant src{cv_i32{42}};
  cv_i32::count = {};

  auto const result = variant_cast<to_variant>(std::move(src));

  EXPECT_TRUE(::arene::base::holds_alternative<cv_i32>(result));
  EXPECT_EQ(::arene::base::get<cv_i32>(result), cv_i32{42});
  EXPECT_EQ(cv_i32::count.copy_ctor, 0U);
  EXPECT_EQ(cv_i32::count.move_ctor, 1U);
}

class VariantCastDuplicateTypesTest : public ::testing::Test {};

/// @test Given <c>FromVariant<Us...></c> and <c>ToVariant<Ts...></c>, where <c>Ts...</c> is a superset of
/// <c>Us...</c> but <c>Us...</c> contains duplicate types and <c>Ts...</c> does not, when
/// <c>arene::base::variant_cast<ToVariant></c> is called, then the cast expression is valid and returns
/// <c>ToVariant</c> for all cvref qualifications of <c>FromVariant<Us...></c>.
TEST_F(VariantCastDuplicateTypesTest, IsInvocableFromVariantWithDuplicateTypes) {
  using from_variant = variant<std::int32_t, std::int32_t>;
  using to_variant = variant<std::int32_t, cv_i32, std::int64_t>;
  using variant_cast_fn = decltype(variant_cast<to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant&>>();
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant const&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant const&>>();
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant&&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant&&>>();
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant const&&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant const&&>>();
}

/// @test Given <c>FromVariant<Us...></c> and <c>ToVariant<Ts...></c>, where <c>Ts...</c> is a superset of
/// <c>Us...</c> but <c>Us...</c> contains duplicate types and <c>Ts...</c> does not, when
/// <c>arene::base::variant_cast<ToVariant></c> is called, then the held value is preserved.
TEST_F(VariantCastDuplicateTypesTest, HeldValueIsPreservedWhenFromVariantHasDuplicateTypes) {
  using from_variant = variant<std::int32_t, std::int32_t>;
  using to_variant = variant<std::int32_t, cv_i32, std::int64_t>;

  // We can't eliminate this two-phase process till variant supports in_place_index ctor.
  auto const expected_value = std::int32_t{42};
  auto const src = from_variant{::arene::base::in_place_index<0U>, expected_value};
  auto const result = variant_cast<to_variant>(src);
  EXPECT_EQ(::arene::base::get<std::int32_t>(result), expected_value);
}

/// @test Given <c>FromVariant<Us...></c> and <c>ToVariant<Ts...></c>, where <c>Ts...</c> is a superset of
/// <c>Us...</c> but <c>Ts...</c> contains duplicate types and <c>Us...</c> does not, when
/// <c>arene::base::variant_cast<ToVariant></c> is called, then the cast expression is valid and returns
/// <c>ToVariant</c> for all cvref qualifications of <c>FromVariant<Us...></c>.
TEST_F(VariantCastDuplicateTypesTest, IsInvocableToVariantWithDuplicateTypes) {
  using from_variant = variant<std::int32_t, cv_i32>;
  using to_variant = variant<std::int32_t, cv_i32, std::int64_t, std::int32_t>;
  using variant_cast_fn = decltype(variant_cast<to_variant>);

  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant&>>();
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant const&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant const&>>();
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant&&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant&&>>();
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<variant_cast_fn, from_variant const&&>));
  ::testing::StaticAssertTypeEq<to_variant, ::arene::base::invoke_result_t<variant_cast_fn, from_variant const&&>>();
}

/// @test Given <c>FromVariant<Us...></c> and <c>ToVariant<Ts...></c>, where <c>Ts...</c> is a superset of
/// <c>Us...</c> but <c>Ts...</c> contains duplicate types and <c>Us...</c> does not, when
/// <c>arene::base::variant_cast<ToVariant></c> is called, then the held value is preserved.
TEST_F(VariantCastDuplicateTypesTest, HeldValueIsPreservedWhenToVariantHasDuplicateTypes) {
  using from_variant = variant<std::int32_t, cv_i32>;
  using to_variant = variant<std::int32_t, cv_i32, std::int64_t, std::int32_t>;

  // We can't eliminate this two-phase process till variant supports in_place_index ctor.
  auto const expected_value = std::int32_t{42};
  auto src = from_variant{::arene::base::in_place_index<0U>, expected_value};
  auto const result = variant_cast<to_variant>(src);
  EXPECT_EQ(::arene::base::get<std::int32_t>(result), expected_value);
}

using move_only_type = ::testing::configurable_value<
    std::int32_t,
    ::testing::throws_on::nothing,
    ::testing::disable::copy_construct | ::testing::disable::copy_assign>;

/// @test Given a move-only source variant, when <c>arene::base::variant_cast</c> is called with an rvalue, then the
/// cast expression is valid and the held value is move-constructed into the target variant.
TEST(VariantCastMoveOnlyType, IsInvocableWithRValue) {
  using subset_variant = variant<move_only_type>;
  using superset_variant = variant<move_only_type, std::int32_t>;

  using expand_cast_fn = decltype(variant_cast<superset_variant>);
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<expand_cast_fn, subset_variant&&>));
  ::testing::StaticAssertTypeEq<superset_variant, ::arene::base::invoke_result_t<expand_cast_fn, subset_variant&&>>();

  auto const original_value = std::int32_t{42};
  auto src = subset_variant{move_only_type{original_value}};
  auto const compressed = variant_cast<superset_variant>(std::move(src));
  EXPECT_EQ(::arene::base::get<move_only_type>(compressed).value, original_value);

  using compress_cast_fn = decltype(variant_cast<subset_variant>);
  STATIC_ASSERT_TRUE((::arene::base::is_invocable_v<compress_cast_fn, superset_variant&&>));
  ::testing::StaticAssertTypeEq<subset_variant, ::arene::base::invoke_result_t<compress_cast_fn, superset_variant&&>>();

  auto src2 = superset_variant{move_only_type{original_value}};
  auto const expanded = variant_cast<subset_variant>(std::move(src2));
  EXPECT_EQ(::arene::base::get<move_only_type>(expanded).value, original_value);
}

}  // namespace variant_cast_test
