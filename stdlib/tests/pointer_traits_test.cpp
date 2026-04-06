// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstddef"
#include "stdlib/include/memory"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

struct element_tag {};
struct difference_tag {};

struct pointerlike_with_all_types_defined {
  using pointer = pointerlike_with_all_types_defined;
  using element_type = element_tag;
  using difference_type = difference_tag;

  template <class>
  using rebind = pointerlike_with_all_types_defined;
};

/// @test @c pointer_traits forwards the element and difference types.
TEST(PointerTraitsTest, TestElementAndDifferenceTypesAreForwarded) {
  using traits_under_test = std::pointer_traits<pointerlike_with_all_types_defined>;

  testing::StaticAssertTypeEq<traits_under_test::element_type, element_tag>();
  testing::StaticAssertTypeEq<traits_under_test::difference_type, difference_tag>();
}

struct pointerlike_with_only_element_type {
  using element_type = element_tag;
};

/// @test @c pointer_traits forwards the element type and the difference type is defaulted
TEST(PointerTraitsTest, TestElementTypeIsForwarded) {
  using traits_under_test = std::pointer_traits<pointerlike_with_only_element_type>;

  testing::StaticAssertTypeEq<traits_under_test::element_type, element_tag>();
  testing::StaticAssertTypeEq<traits_under_test::difference_type, std::ptrdiff_t>();
}

template <class T>
struct pointerlike_with_template_element_no_args {};

/// @test @c pointer_traits forwards the first template parameter from the class template specialization with no
/// arguments.
TEST(PointerTraitsTest, TestElementTypeFromTemplateSpecializationNoArgs) {
  using traits_under_test = std::pointer_traits<pointerlike_with_template_element_no_args<element_tag>>;

  testing::StaticAssertTypeEq<traits_under_test::element_type, element_tag>();
  testing::StaticAssertTypeEq<traits_under_test::difference_type, std::ptrdiff_t>();
}

template <class T, class... Args>
struct pointerlike_with_template_element_with_args {};

/// @test @c pointer_traits forwards value type from the class template specialization with arguments.
TEST(PointerTraitsTest, TestElementTypeFromTemplateSpecializationWithArgs) {
  using traits_under_test =
      std::pointer_traits<pointerlike_with_template_element_with_args<element_tag, int, float, difference_tag>>;

  testing::StaticAssertTypeEq<traits_under_test::element_type, element_tag>();
  testing::StaticAssertTypeEq<traits_under_test::difference_type, std::ptrdiff_t>();
}

template <class T>
struct pointerlike_with_template_and_element_type {
  using element_type = element_tag;
};

/// @test @c pointer_traits forwards the element type when a template parameter is present.
TEST(PointerTraitsTest, TestElementTypeFromTypedefAndTemplate) {
  using traits_under_test = std::pointer_traits<pointerlike_with_template_and_element_type<difference_tag>>;

  // The `element_type` typedef should be chosen over the template parameter.
  testing::StaticAssertTypeEq<traits_under_test::element_type, element_tag>();
}

template <class T>
struct rebind_tag {};

struct pointerlike_with_rebind {
  using element_type = element_tag;

  template <class U>
  using rebind = rebind_tag<U>;
};

/// @test @c pointer_traits forwards the rebind type.
TEST(PointerTraitsTest, TestRebindIsForwarded) {
  using traits_under_test = std::pointer_traits<pointerlike_with_rebind>;

  testing::StaticAssertTypeEq<rebind_tag<int>, traits_under_test::rebind<int>>();
}

/// @test @c pointer_traits defines rebind when a template paramter is present with no arguments.
TEST(PointerTraitsTest, TestRebindFromTemplateSpecializationNoArgs) {
  using traits_under_test = std::pointer_traits<pointerlike_with_template_element_no_args<element_tag>>;

  testing::StaticAssertTypeEq<pointerlike_with_template_element_no_args<int>, traits_under_test::rebind<int>>();
}

/// @test @c pointer_traits defines rebind when a template paramter is present with additional arguments.
TEST(PointerTraitsTest, TestRebindFromTemplateSpecializationWithArgs) {
  using traits_under_test =
      std::pointer_traits<pointerlike_with_template_element_with_args<element_tag, int, float, difference_tag>>;

  testing::StaticAssertTypeEq<
      pointerlike_with_template_element_with_args<int, int, float, difference_tag>,
      traits_under_test::rebind<int>>();
}

namespace rebind_is_illformed {
struct rebind_invoker {
  template <class PointerLike, class T>
  constexpr auto operator()(PointerLike, T) -> typename std::pointer_traits<PointerLike>::template rebind<T> {
    return typename std::pointer_traits<PointerLike>::template rebind<T>{};
  }
};
}  // namespace rebind_is_illformed

/// @test @c pointer_traits::rebind is ill-formed if rebind cannot be determined.
TEST(PointerTraitsTest, TestRebindWithNoTypedefOrTemplateIsIllformed) {
  ASSERT_TRUE(::testing::simple_is_invocable_v<rebind_is_illformed::rebind_invoker, pointerlike_with_rebind, int>);
  ASSERT_TRUE(::testing::simple_is_invocable_v<
              rebind_is_illformed::rebind_invoker,
              pointerlike_with_template_element_no_args<int>,
              int>);
  ASSERT_TRUE(::testing::simple_is_invocable_v<
              rebind_is_illformed::rebind_invoker,
              pointerlike_with_template_element_with_args<float, double>,
              int>);

  ASSERT_FALSE(::testing::
                   simple_is_invocable_v<rebind_is_illformed::rebind_invoker, pointerlike_with_only_element_type, int>);
}

struct pointerlike_with_pointer_to {
  using element_type = element_tag;

  constexpr explicit pointerlike_with_pointer_to(element_type* element_in)
      : stored(element_in) {}

  static constexpr auto pointer_to(element_type& element) -> pointerlike_with_pointer_to {
    return pointerlike_with_pointer_to{&element};
  }

  element_type* stored;
};

/// @test @c pointer_traits forwards to pointer_to correctly.
TEST(PointerTraitsTest, TestPointerToIsForwarded) {
  using traits_under_test = std::pointer_traits<pointerlike_with_pointer_to>;

  auto element = element_tag{};

  ASSERT_EQ(&element, pointerlike_with_pointer_to::pointer_to(element).stored);
  ASSERT_EQ(&element, traits_under_test::pointer_to(element).stored);
}

template <bool NoExcept>
struct pointerlike_with_conditional_noexcept {
  using element_type = element_tag;

  // Note: On gcc8, if this function is `constexpr` then the `noexcept(false)` specifier is ignored and the function is
  // marked as `noexcept(true)`.
  static auto pointer_to(element_type&) noexcept(NoExcept) -> pointerlike_with_conditional_noexcept { return {}; }
};

/// @test @c pointer_traits pointer_to is conditionallly noexcept
TEST(PointerTraitsTest, TestPointerToIsConditionallyNoExcept) {
  ASSERT_TRUE(noexcept(
      std::pointer_traits<pointerlike_with_conditional_noexcept<true>>::pointer_to(std::declval<element_tag&>())
  ));
  ASSERT_FALSE(noexcept(
      std::pointer_traits<pointerlike_with_conditional_noexcept<false>>::pointer_to(std::declval<element_tag&>())
  ));
}

struct pointerlike_with_void_element_and_point_to {
  using element_type = void;
  static constexpr auto pointer_to() -> pointerlike_with_void_element_and_point_to { return {}; }
};

/// @test @c pointer_traits is valid when element_type is void.
CONSTEXPR_TEST(PointerTraitsTest, TestPointerToWithVoidElementType) {
  using traits_under_test = std::pointer_traits<pointerlike_with_void_element_and_point_to>;

  // We only test that the trait can be instantiated. As the argument to `std::pointer_traits::pointer_to` is
  // unspecified when the `element_type` is void, it cannot actually be called.
  static_cast<void>(traits_under_test{});
}

template <typename T>
class PointerIsAlwaysSetSuite : public testing::Test {};

using pointer_is_always_set_types = ::testing::Types<
    pointerlike_with_all_types_defined,
    pointerlike_with_only_element_type,
    pointerlike_with_template_element_no_args<element_tag>,
    pointerlike_with_template_element_with_args<element_tag, int, float, difference_tag>,
    pointerlike_with_template_and_element_type<int>,
    pointerlike_with_rebind,
    pointerlike_with_pointer_to,
    pointerlike_with_void_element_and_point_to,
    pointerlike_with_conditional_noexcept<true>,
    pointerlike_with_conditional_noexcept<false>>;

TYPED_TEST_SUITE(PointerIsAlwaysSetSuite, pointer_is_always_set_types, );

/// @test @c pointer_traits always sets @c pointer member typedef
TYPED_TEST(PointerIsAlwaysSetSuite, TestPointerIsAlwaysSet) {
  using traits_under_test = std::pointer_traits<TypeParam>;
  testing::StaticAssertTypeEq<typename traits_under_test::pointer, TypeParam>();
}

namespace detectors {

template <class T, class = std::void_t<>>
constexpr auto has_pointer_member_v = false;

template <class T>
constexpr auto has_pointer_member_v<T, std::void_t<typename T::pointer>> = true;

template <class, class = std::void_t<>>
constexpr auto has_element_type_member_v = false;

template <class T>
constexpr auto has_element_type_member_v<T, std::void_t<typename T::element_type>> = true;

template <class, class = std::void_t<>>
constexpr auto has_difference_type_member_v = false;

template <class T>
constexpr auto has_difference_type_member_v<T, std::void_t<typename T::difference_type>> = true;

template <class, class = std::void_t<>>
constexpr auto has_rebind_member_v = false;

template <class T>
constexpr auto has_rebind_member_v<T, std::void_t<typename T::template rebind<int>>> = true;

}  // namespace detectors

struct pointerlike_with_no_element_type {
  using difference_type = difference_tag;

  template <class U>
  using rebind = rebind_tag<U>;
};

/// @test @c pointer_traits is empty when there is no element_type set.
// See https://cplusplus.github.io/LWG/issue3545
CONSTEXPR_TEST(PointerTraitsTest, TestWithNoElementType) {
  // Confirm all members set in the positive case.
  {
    using traits_under_test = std::pointer_traits<pointerlike_with_all_types_defined>;

    ASSERT_TRUE(detectors::has_pointer_member_v<traits_under_test>);
    ASSERT_TRUE(detectors::has_element_type_member_v<traits_under_test>);
    ASSERT_TRUE(detectors::has_difference_type_member_v<traits_under_test>);
    ASSERT_TRUE(detectors::has_rebind_member_v<traits_under_test>);
  }

  // Check that no members are set if element_type cannot be determined.
  {
    using traits_under_test = std::pointer_traits<pointerlike_with_no_element_type>;

    ASSERT_FALSE(detectors::has_pointer_member_v<traits_under_test>);
    ASSERT_FALSE(detectors::has_element_type_member_v<traits_under_test>);
    ASSERT_FALSE(detectors::has_difference_type_member_v<traits_under_test>);
    ASSERT_FALSE(detectors::has_rebind_member_v<traits_under_test>);
  }
}

template <typename T>
class SpecializationForPointerTypesTestSuite : public testing::Test {};

TYPED_TEST_SUITE(SpecializationForPointerTypesTestSuite, ::testing::non_array_object_types, );

/// @test @c pointer_traits sets pointer correctly with raw pointer
TYPED_TEST(SpecializationForPointerTypesTestSuite, PointerIsSetCorrectly) {
  using traits_under_test = std::pointer_traits<TypeParam*>;

  testing::StaticAssertTypeEq<typename traits_under_test::pointer, TypeParam*>();
}

/// @test @c pointer_traits sets element_type correctly with raw pointer
TYPED_TEST(SpecializationForPointerTypesTestSuite, ElementTypeIsSetCorrectly) {
  using traits_under_test = std::pointer_traits<TypeParam*>;

  testing::StaticAssertTypeEq<typename traits_under_test::element_type, TypeParam>();
}

/// @test @c pointer_traits sets difference_type correctly with raw pointer
TYPED_TEST(SpecializationForPointerTypesTestSuite, DifferenceTypeIsSetCorrectly) {
  using traits_under_test = std::pointer_traits<TypeParam*>;

  testing::StaticAssertTypeEq<typename traits_under_test::difference_type, std::ptrdiff_t>();
}

/// @test @c pointer_traits sets rebind correctly with raw pointer
TYPED_TEST(SpecializationForPointerTypesTestSuite, RebindIsSetCorrectly) {
  using traits_under_test = std::pointer_traits<TypeParam*>;

  testing::StaticAssertTypeEq<int*, typename traits_under_test::template rebind<int>>();
}

/// @test @c pointer_traits sets pointer_to correctly with raw pointer
CONSTEXPR_TYPED_TEST(SpecializationForPointerTypesTestSuite, PointerToIsSetCorrectly) {
  using traits_under_test = std::pointer_traits<TypeParam*>;

  auto element = TypeParam{};

  ASSERT_EQ(&element, traits_under_test::pointer_to(element));
  ASSERT_TRUE(noexcept(traits_under_test::pointer_to(std::declval<TypeParam&>())));
}

/// @test @c pointer_traits with const element works correctly
CONSTEXPR_TYPED_TEST(SpecializationForPointerTypesTestSuite, PointerToWithConstElement) {
  using traits_under_test = std::pointer_traits<TypeParam const*>;

  auto mutable_element = TypeParam{};
  auto const_element = TypeParam{};

  ASSERT_EQ(&mutable_element, traits_under_test::pointer_to(mutable_element));
  ASSERT_EQ(&const_element, traits_under_test::pointer_to(const_element));

  ASSERT_TRUE(traits_under_test::pointer_to(TypeParam{}) != nullptr);
  ASSERT_TRUE(traits_under_test::pointer_to(static_cast<TypeParam const&&>(TypeParam{})) != nullptr);
}

struct type_with_overloaded_address_operator {
  // NOLINTNEXTLINE(google-runtime-operator)
  constexpr auto operator&() const -> type_with_overloaded_address_operator const* { return nullptr; }
  constexpr auto expected_result() const -> type_with_overloaded_address_operator const* { return this; }
};

/// @test @c pointer_to calls @c std::addressof if @c element_type has overloaded address operator
CONSTEXPR_TEST(SpecializationForPointerTypesTestSuite, PointerToWorksWithOverloadedAddressOperator) {
  using traits_under_test = std::pointer_traits<type_with_overloaded_address_operator*>;

  auto element = type_with_overloaded_address_operator{};

  ASSERT_EQ(nullptr, &element);
  ASSERT_EQ(element.expected_result(), traits_under_test::pointer_to(element));
}

}  // namespace
