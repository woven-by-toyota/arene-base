// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "stdlib/include/memory"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

template <typename T>
class AddressofTest : public testing::Test {};

TYPED_TEST_SUITE(AddressofTest, ::testing::object_types, );

/// @test The return type of @c addressof is @c T*
TYPED_TEST(AddressofTest, HasRightReturnType) {
  testing::StaticAssertTypeEq<decltype(std::addressof(std::declval<TypeParam&>())), TypeParam*>();
  testing::StaticAssertTypeEq<decltype(std::addressof(std::declval<TypeParam const&>())), TypeParam const*>();
  testing::StaticAssertTypeEq<decltype(std::addressof(std::declval<TypeParam volatile&>())), TypeParam volatile*>();
  testing::StaticAssertTypeEq<
      decltype(std::addressof(std::declval<TypeParam const volatile&>())),
      TypeParam const volatile*>();
}

/// @test @c addressof is @c noexcept
TYPED_TEST(AddressofTest, IsNoexcept) {
  ASSERT_TRUE(noexcept(std::addressof(std::declval<TypeParam&>())));
  ASSERT_TRUE(noexcept(std::addressof(std::declval<TypeParam const&>())));
  ASSERT_TRUE(noexcept(std::addressof(std::declval<TypeParam volatile&>())));
  ASSERT_TRUE(noexcept(std::addressof(std::declval<TypeParam const volatile&>())));
}

/// @test The return value of @c std::addressof is the address of the supplied argument
CONSTEXPR_TYPED_TEST(AddressofTest, TheReturnValueIsAddressofArgument) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_GCC(
      "-Wignored-qualifiers",
      "This triggers on marking a pointer to member function as const. Since this is a test, its ok if this is not "
      "applied to a single type."
  );
  TypeParam const const_value{};
  ARENE_IGNORE_END();
  auto const* const const_result = std::addressof(const_value);
  ASSERT_EQ(const_result, &const_value);

  TypeParam mutable_value{};
  auto const mutable_result = std::addressof(mutable_value);
  ASSERT_EQ(mutable_result, &mutable_value);
}

struct type_with_overloaded_address_operator {
  // NOLINTNEXTLINE(google-runtime-operator)
  constexpr auto operator&() const -> type_with_overloaded_address_operator const* { return nullptr; }
  constexpr auto expected_result() const -> type_with_overloaded_address_operator const* { return this; }
};

/// @test @c std::addressof works correctly with overloaded address operator
CONSTEXPR_TEST(AddressofTest, ReturnsAddressWithOverloadedAddressOperator) {
  auto const value = type_with_overloaded_address_operator{};
  auto const* const result = std::addressof(value);
  ASSERT_EQ(nullptr, &value);
  ASSERT_EQ(result, value.expected_result());
}

/// @test @c std::addressof should not be able to be called with rvalue references.
TYPED_TEST(AddressofTest, AddressofNotInvocableWithRvalueReferences) {
  // Lift std::addressof into a function object to test invocability.
  auto const lifted_addressof = [](auto&& value) -> decltype(std::addressof(std::forward<decltype(value)>(value))) {
    return std::addressof(std::forward<decltype(value)>(value));
  };

  ASSERT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_addressof), TypeParam&>);
  ASSERT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_addressof), TypeParam&&>);

  ASSERT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_addressof), TypeParam const&>);
  ASSERT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_addressof), TypeParam const&&>);
}

namespace address_operator_illformed {
// See example from the notes at https://en.cppreference.com/w/cpp/memory/addressof

struct incomplete_type;

template <class T>
struct holder {
  T value;
};

// Helper type used to get the actual address of `held`, even though `&held` is illformed.
struct address_helper {
  holder<holder<incomplete_type>*> held{};
};

// Helper function object to invoke `&val.held` if it is valid.
struct invoker {
  // The additional indirection of template parameter `U` is required, otherwise the compiler will find `&val.held` as
  // invalid when substituting into the call to `decltype`.
  template <class T, class U = T>
  constexpr auto operator()(U val) -> decltype(&val.held) {
    return &val.held;
  }
};

}  // namespace address_operator_illformed

/// @test @c std::addressof works when the address operator is ill-formed.
CONSTEXPR_TEST(AddressofTest, ReturnsAddressWhenAddressOperatorIsIllformed) {
  auto const value = address_operator_illformed::address_helper{};

  // Confirm that `&value.held` will be illformed.
  // &value.held will be invalid with error: 'holder<T>::value' has incomplete type
  ASSERT_FALSE(::testing::simple_is_invocable_v<
               address_operator_illformed::invoker,
               address_operator_illformed::address_helper>);

  auto* const result = std::addressof(value.held);
  auto* const expected_result = &value;

  // As `address_helper` and `holder` are both standard layout types, the address of an instance of `address_helper`
  // will be the same as the address of the data member `held` inside the `address_helper` instance.
  ASSERT_EQ(static_cast<void const*>(result), static_cast<void const*>(expected_result));
}

}  // namespace
