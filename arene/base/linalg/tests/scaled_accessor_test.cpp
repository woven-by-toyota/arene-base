// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/scaled_accessor.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/elementwise_unary_transform.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace {

using arene::base::linalg::equal_elements;
using arene::base::linalg::scaled;
using arene::base::linalg::scaled_accessor;
using arene::base::linalg_testing::test_array;
using arene::base::mdspan_detail::representable_cast;

template <class T>
class ScaledAccessorTest;

template <class Element, class Extents, class Layout, template <class> class Accessor>
class ScaledAccessorTest<arene::base::mdspan<Element, Extents, Layout, Accessor<Element>>> : public ::testing::Test {
 public:
  using mutable_type_param = arene::base::mdspan<
      std::remove_const_t<Element>,
      Extents,
      Layout,
      arene::base::default_accessor<std::remove_const_t<Element>>>;
};

TYPED_TEST_SUITE(ScaledAccessorTest, arene::base::linalg_testing::unqualified_linalg_object_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests do not do arithmetic, so exact equality comparisons are OK");

/// @brief A valid accessor class with the same semantics as @c OriginalAccessor but which is a distinct type from it
template <class OriginalAccessor, bool ConversionIsExplicit>
class distinct_accessor : private OriginalAccessor {
 public:
  using offset_policy = distinct_accessor;
  using element_type = typename OriginalAccessor::element_type;
  using reference = typename OriginalAccessor::reference;
  using data_handle_type = typename OriginalAccessor::data_handle_type;

  constexpr distinct_accessor() noexcept = default;

  template <bool Explicit = ConversionIsExplicit, arene::base::constraints<std::enable_if_t<Explicit>> = nullptr>
  explicit constexpr distinct_accessor(OriginalAccessor other) noexcept
      : OriginalAccessor{std::move(other)} {}

  template <bool Explicit = ConversionIsExplicit, arene::base::constraints<std::enable_if_t<!Explicit>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) We're specifically testing implicitly convertible types here
  constexpr distinct_accessor(OriginalAccessor other) noexcept
      : OriginalAccessor{std::move(other)} {}

  using OriginalAccessor::access;
  using OriginalAccessor::offset;
};

/// @brief Check if a randomly-generated parameter of unknown type is finite; SFINAE overload for floating point
template <class T, arene::base::constraints<std::enable_if_t<std::is_floating_point<T>::value>> = nullptr>
constexpr auto is_finite(T value) noexcept -> bool {
  return arene::base::isfinite(value);
}

/// @brief Check if a randomly-generated parameter of unknown type is finite; SFINAE overload for non-floating-point
template <class T, arene::base::constraints<std::enable_if_t<!std::is_floating_point<T>::value>> = nullptr>
constexpr auto is_finite(T /* value */) noexcept -> bool {
  return true;
}

/// @test A default-constructed scaled accessor has a scale factor of zero so it returns zero for every element
ARENE_TYPED_PROPERTY(ScaledAccessorTest, DefaultConstructedScalesByZero, (test_array<TypeParam> random_test_vec)) {
  test_array<TypeParam> zero_test_vec{};
  TypeParam const zero_vec = zero_test_vec;

  TypeParam const unwrapped_random = random_test_vec;

  arene::base::linalg::scaled_detail::scaled_mdspan<TypeParam> const scaled_random{
      unwrapped_random.data_handle(),
      unwrapped_random.mapping(),
      {}  // default-constructed scaled_accessor
  };

  ARENE_PROP_ASSERT(equal_elements(scaled_random, zero_vec));
  EXPECT_EQ(scaled_random.accessor().scaling_factor(), typename TypeParam::value_type{});

  static_assert(
      std::is_nothrow_default_constructible<typename decltype(scaled_random)::accessor_type>::value ==
          (std::is_nothrow_default_constructible<typename decltype(scaled_random)::element_type>::value &&
           std::is_nothrow_default_constructible<typename TypeParam::accessor_type>::value),
      "Default constructor noexcept specification should match those of the constituent types"
  );

  static_assert(noexcept(scaled_random.accessor().scaling_factor()), "scaling_factor() should always be noexcept");
  static_assert(noexcept(scaled_random.accessor().nested_accessor()), "nested_accessor() should always be noexcept");
}

/// @test A scaled accessor is not convertible from another if the scaling factors use different types
TYPED_TEST(ScaledAccessorTest, DifferentScalingFactorTypesBlockConvertibility) {
  using original_acc = typename TypeParam::accessor_type;

  using scaled_type_param = arene::base::linalg::scaled_detail::scaled_mdspan<TypeParam, std::int32_t>;
  using scaled_original_acc = typename scaled_type_param::accessor_type;

  using differently_scaled_type_param = arene::base::linalg::scaled_detail::scaled_mdspan<TypeParam, std::uint64_t>;
  using differently_scaled_original_acc = typename differently_scaled_type_param::accessor_type;

  EXPECT_FALSE((std::is_constructible<original_acc, scaled_original_acc>::value));
  EXPECT_FALSE((std::is_constructible<original_acc, differently_scaled_original_acc>::value));

  EXPECT_FALSE((std::is_constructible<scaled_original_acc, original_acc>::value));
  EXPECT_FALSE((std::is_constructible<scaled_original_acc, differently_scaled_original_acc>::value));

  EXPECT_FALSE((std::is_constructible<differently_scaled_original_acc, original_acc>::value));
  EXPECT_FALSE((std::is_constructible<differently_scaled_original_acc, scaled_original_acc>::value));
}

/// @test A scaled accessor is implicitly convertible from another if the nested accessors are implicitly convertible
TYPED_TEST(ScaledAccessorTest, ConstructibilityIsInherited) {
  using original_acc = typename TypeParam::accessor_type;

  using scaled_type_param = arene::base::linalg::scaled_detail::scaled_mdspan<TypeParam>;
  using scaled_original_acc = typename scaled_type_param::accessor_type;

  using explicit_distinct_acc = distinct_accessor<original_acc, true>;
  using explicit_scaled_distinct_acc = scaled_accessor<typename TypeParam::value_type, explicit_distinct_acc>;

  using implicit_distinct_acc = distinct_accessor<original_acc, false>;
  using implicit_scaled_distinct_acc = scaled_accessor<typename TypeParam::value_type, implicit_distinct_acc>;

  EXPECT_FALSE((std::is_constructible<original_acc, scaled_original_acc>::value));
  EXPECT_FALSE((std::is_constructible<original_acc, explicit_scaled_distinct_acc>::value));
  EXPECT_FALSE((std::is_constructible<original_acc, implicit_scaled_distinct_acc>::value));

  // distinct -> original construction is not implemented
  EXPECT_FALSE((std::is_constructible<scaled_original_acc, original_acc>::value));
  EXPECT_FALSE((std::is_constructible<scaled_original_acc, explicit_scaled_distinct_acc>::value));
  EXPECT_FALSE((std::is_constructible<scaled_original_acc, implicit_scaled_distinct_acc>::value));

  // original -> distinct construction works but it's explicit
  EXPECT_FALSE((std::is_constructible<explicit_scaled_distinct_acc, original_acc>::value));
  EXPECT_TRUE((std::is_constructible<explicit_scaled_distinct_acc, scaled_original_acc>::value));
  EXPECT_FALSE((std::is_convertible<scaled_original_acc, explicit_scaled_distinct_acc>::value));
  EXPECT_FALSE((std::is_constructible<explicit_scaled_distinct_acc, implicit_scaled_distinct_acc>::value));

  // original -> distinct construction works implicitly
  EXPECT_FALSE((std::is_constructible<implicit_scaled_distinct_acc, original_acc>::value));
  EXPECT_TRUE((std::is_constructible<implicit_scaled_distinct_acc, scaled_original_acc>::value));
  EXPECT_TRUE((std::is_convertible<scaled_original_acc, implicit_scaled_distinct_acc>::value));
  EXPECT_FALSE((std::is_constructible<implicit_scaled_distinct_acc, explicit_scaled_distinct_acc>::value));
}

/// @test Converting a scaled accessor converts the nested accessor correctly
ARENE_TYPED_PROPERTY(
    ScaledAccessorTest,
    ConstructibilityWorks,
    (typename TypeParam::value_type scale, test_array<TypeParam> random_test_vec)
) {
  // As of the time of writing, there's no built-in way to reject NaNs and limit ranges of generated parameters that
  // works for both floating point and integers, so we use this custom check instead.
  ARENE_PROP_PRE(is_finite(scale));
  constexpr typename TypeParam::value_type max_random{100};
  constexpr auto random_divisor = std::numeric_limits<typename TypeParam::value_type>::max() / max_random;
  scale /= random_divisor;

  using scaled_type_param = arene::base::linalg::scaled_detail::scaled_mdspan<TypeParam>;
  using scaled_original_acc = typename scaled_type_param::accessor_type;

  scaled_type_param const original_scaled = scaled(scale, TypeParam{random_test_vec});

  // Another scaled mdspan with the same scale but a distinct, explicitly-convertible nested accessor type
  using explicit_distinct_acc = distinct_accessor<typename TypeParam::accessor_type, true>;
  using explicit_scaled_distinct_acc = scaled_accessor<typename TypeParam::value_type, explicit_distinct_acc>;
  using explicit_scaled_type_param = arene::base::mdspan<
      typename explicit_scaled_distinct_acc::element_type,
      typename TypeParam::extents_type,
      typename TypeParam::layout_type,
      explicit_scaled_distinct_acc>;

  explicit_scaled_type_param const explicit_scaled{original_scaled};
  ARENE_PROP_ASSERT(equal_elements(explicit_scaled, original_scaled));

  // Another scaled mdspan with the same scale but a distinct, implicitly-convertible nested accessor type
  using implicit_distinct_acc = distinct_accessor<typename TypeParam::accessor_type, false>;
  using implicit_scaled_distinct_acc = scaled_accessor<typename TypeParam::value_type, implicit_distinct_acc>;
  using implicit_scaled_type_param = arene::base::mdspan<
      typename implicit_scaled_distinct_acc::element_type,
      typename TypeParam::extents_type,
      typename TypeParam::layout_type,
      implicit_scaled_distinct_acc>;

  implicit_scaled_type_param const implicit_scaled{original_scaled};
  ARENE_PROP_ASSERT(equal_elements(implicit_scaled, original_scaled));

  // These conversions should have the right noexcept characteristics

  using arene::base::remove_cvref_t;
  using scaling_factor_type = remove_cvref_t<decltype(std::declval<scaled_original_acc>().scaling_factor())>;

  static_assert(
      std::is_nothrow_constructible<explicit_scaled_distinct_acc, scaled_original_acc>::value ==
          (std::is_nothrow_copy_constructible<scaling_factor_type>::value &&
           std::is_nothrow_constructible<explicit_distinct_acc, typename TypeParam::accessor_type const&>::value),
      "Explicit converting constructor noexcept specification should match those of the constituent types"
  );

  static_assert(
      std::is_nothrow_constructible<implicit_scaled_distinct_acc, scaled_original_acc>::value ==
          (std::is_nothrow_copy_constructible<scaling_factor_type>::value &&
           std::is_nothrow_constructible<implicit_distinct_acc, typename TypeParam::accessor_type const&>::value),
      "Implicit converting constructor noexcept specification should match those of the constituent types"
  );
}

/// @test Accessing an array of zeros through an accessor with any finite scale factor still yields zero
ARENE_TYPED_PROPERTY(ScaledAccessorTest, ScaledZeroIsZero, (typename TypeParam::value_type scale)) {
  // As of the time of writing, there's no built-in way to reject NaNs and limit ranges of generated parameters that
  // works for both floating point and integers, so we use this custom check instead.
  ARENE_PROP_PRE(is_finite(scale));
  constexpr typename TypeParam::value_type max_random{100};
  constexpr auto random_divisor = std::numeric_limits<typename TypeParam::value_type>::max() / max_random;
  scale /= random_divisor;

  test_array<TypeParam> zero_test_vec{};
  TypeParam const unwrapped = zero_test_vec;

  auto const scaled_vec = scaled(scale, unwrapped);

  ARENE_PROP_ASSERT(equal_elements(scaled_vec, unwrapped));

  static_assert(
      noexcept(scaled(scale, unwrapped)) ==
          (std::is_nothrow_copy_constructible<typename TypeParam::data_handle_type>::value &&
           std::is_nothrow_copy_constructible<typename TypeParam::mapping_type>::value &&
           std::is_nothrow_copy_constructible<typename TypeParam::value_type /* (the scale is this type) */>::value &&
           std::is_nothrow_copy_constructible<typename TypeParam::accessor_type>::value),
      "Scaling an mdspan should have a noexcept-specification maching the noexcept-copyability of its constituents"
  );
}

/// @test Accessing a random array with a random scale results in the scale being multiplied in
ARENE_TYPED_PROPERTY(
    ScaledAccessorTest,
    ScaleIsApplied,
    (typename TypeParam::value_type scale, test_array<TypeParam> random_test_vec)
) {
  // As of the time of writing, there's no built-in way to reject NaNs and limit ranges of generated parameters that
  // works for both floating point and integers, so we use this custom check instead.
  ARENE_PROP_PRE(is_finite(scale));
  constexpr typename TypeParam::value_type max_random{100};
  constexpr auto random_divisor = std::numeric_limits<typename TypeParam::value_type>::max() / max_random;
  scale /= random_divisor;

  TypeParam const unwrapped = random_test_vec;
  auto const scaled_vec = scaled(scale, unwrapped);

  using mutable_type_param = typename ScaledAccessorTest<TypeParam>::mutable_type_param;

  test_array<mutable_type_param> scaled_test_vec{};
  mutable_type_param const unwrapped_vec_to_scaled_data = scaled_test_vec;

  arene::base::linalg::elementwise_unary_transform(unwrapped, unwrapped_vec_to_scaled_data, [scale](auto entry) {
    return scale * entry;
  });

  // scaled_test_vec (via unwrapped_vec_to_scaled_data) now contains scaled copies of the original entries.
  // Accessing random_test_vec through scaled_vec and scaled_test_vec through unwrapped_vec_to_scaled_data yields the
  // same value for each entry.
  ARENE_PROP_ASSERT(equal_elements(scaled_vec, unwrapped_vec_to_scaled_data));
}

/// @test For all indices, the offset for those indices of the scaled accessor is the same as that of the nested one
TYPED_TEST(ScaledAccessorTest, OffsetIsSameAsNested) {
  test_array<TypeParam> zero_test_vec{};
  TypeParam const zero_vec = zero_test_vec;
  auto const scaled_vec = scaled(typename TypeParam::value_type{}, zero_vec);

  testing::StaticAssertTypeEq<decltype(zero_vec.data_handle()), decltype(scaled_vec.data_handle())>();

  auto const handle = scaled_vec.data_handle();
  auto const span_size = representable_cast<std::size_t>(scaled_vec.mapping().required_span_size());
  for (std::size_t idx{}; idx < span_size; ++idx) {
    EXPECT_EQ(zero_vec.accessor().offset(handle, idx), scaled_vec.accessor().offset(handle, idx));

    static_assert(
        noexcept(scaled_vec.accessor().offset(handle, idx)) == noexcept(zero_vec.accessor().offset(handle, idx)),
        "offset noexcept should match that of the nested accessor"
    );
  }
}

/// @test A scaled accessor and the nested accessor that it contains have the same address
TYPED_TEST(ScaledAccessorTest, NestedAccessorHasSameAddress) {
  test_array<TypeParam> zero_test_vec{};
  auto const scaled_vec = scaled(typename TypeParam::value_type{}, TypeParam{zero_test_vec});

  EXPECT_EQ(
      static_cast<void const*>(std::addressof(scaled_vec.accessor())),
      static_cast<void const*>(std::addressof(scaled_vec.accessor().nested_accessor()))
  );
}

ARENE_IGNORE_END();

}  // namespace
