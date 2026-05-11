// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/stdlib_detail/hash.hpp"

#include "arene/base/constraints.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_traits.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_default_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_destructible.hpp"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using types_with_specializations = arene::base::type_lists::concat_unique_t<
    testing::arithmetic_types,
    testing::Types<int*, float*, int const*, float const*>,
    testing::enum_types>;

template <typename T>
struct MakeValues {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  T values[2] = {static_cast<T>(0), static_cast<T>(1)};
};

template <typename T>
struct MakeValues<T*> {
  MakeValues<T> backing{};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  T* values[2] = {&backing.values[0], &backing.values[1]};
};

template <>
struct MakeValues<void*> {
  MakeValues<int> backing{};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  void* values[2] = {&backing.values[0], &backing.values[1]};
};

template <typename T>
class HashTest : public testing::Test {};

TYPED_TEST_SUITE(HashTest, types_with_specializations, );

/// @test Given a type `T` which is an arithmetic type, enum type, or pointer type, then `std::hash<T>` is defined.
CONSTEXPR_TYPED_TEST(HashTest, HashSpecializationExists) {
  CONSTEXPR_ASSERT(::arene::base::substitution_succeeds<std::hash, TypeParam>);
}

/// @test Given an instance type `T` with a defined specialization of `std::hash`, when `std::hash<T>{}(value)` is
/// invoked, the return is a valid hash of `T`.
TYPED_TEST(HashTest, HashFunctionIsValid) {
  auto const values = MakeValues<TypeParam>{};

  constexpr auto hasher = std::hash<TypeParam>{};
  auto const hash0 = hasher(values.values[0]);
  auto const hash1 = hasher(values.values[1]);

  // Hashes of equal values must be equal
  ASSERT_EQ(hash0, hasher(values.values[0]));
  ASSERT_EQ(hash1, hasher(values.values[1]));

  // Hashes of values near to each other in value space should not be equal
  ASSERT_NE(hash0, hash1);
}

/// @test Hash has the right type aliases
TYPED_TEST(HashTest, TypeAliasesPresent) {
  ::testing::StaticAssertTypeEq<typename std::hash<TypeParam>::result_type, std::size_t>();
  ::testing::StaticAssertTypeEq<typename std::hash<TypeParam>::argument_type, TypeParam>();
}

/// @test Hash must be swappable
TYPED_TEST(HashTest, IsSwappable) { ASSERT_TRUE(arene::base::is_swappable_v<std::hash<TypeParam>>); }

/// @test Hash must be default-constructible
TYPED_TEST(HashTest, IsDefaultConstructible) { ASSERT_TRUE(std::is_default_constructible_v<std::hash<TypeParam>>); }

/// @test Hash must be copy-constructible
TYPED_TEST(HashTest, IsCopyConstructible) { ASSERT_TRUE(std::is_copy_constructible_v<std::hash<TypeParam>>); }

/// @test Hash must be destructible
TYPED_TEST(HashTest, IsDestructible) { ASSERT_TRUE(std::is_destructible_v<std::hash<TypeParam>>); }

/// @test Hash must be nothrow-invocable
TYPED_TEST(HashTest, IsNothrowInvocable) {
  ASSERT_TRUE(arene::base::is_nothrow_invocable_r_v<std::size_t, std::hash<TypeParam>, TypeParam>);
  ASSERT_TRUE(arene::base::is_nothrow_invocable_r_v<std::size_t, std::hash<TypeParam>, TypeParam&>);
  ASSERT_TRUE(arene::base::is_nothrow_invocable_r_v<std::size_t, std::hash<TypeParam>, TypeParam const&>);
}

}  // namespace
