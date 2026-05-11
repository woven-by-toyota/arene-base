// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/give_qualifiers_to.hpp"

#include <type_traits>

#include <gtest/gtest.h>

namespace {

using arene::base::give_qualifiers_to;

template <class T, class U>
constexpr bool is_same_v = std::is_same<T, U>::value;

// Types used for testing
struct a;
struct b;

/// @test `give_qualifiers_to` adds the `const` or `volatile` qualifiers of the first parameter to those of the second,
/// when neither is a reference.
TEST(GiveQualifiersTo, NeitherSourceNorTargetAreReference) {
  // Unqualified b
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a, b>, b>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const, b>, b const>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile, b>, b volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile, b>, b const volatile>));

  // const-qualified B
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a, b const>, b const>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const, b const>, b const>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile, b const>, b const volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile, b const>, b const volatile>));

  // volatile-qualified B
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a, b volatile>, b volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const, b volatile>, b const volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile, b volatile>, b volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile, b volatile>, b const volatile>));

  // cv-qualified B
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a, b const volatile>, b const volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const, b const volatile>, b const volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile, b const volatile>, b const volatile>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile, b const volatile>, b const volatile>));
}

/// @test `give_qualifiers_to` returns the target type unchanged when either the source or target is a reference.
TEST(GiveQualifiersTo, SourceOrTargetAreReference) {
  // Source lvalue-reference type (no top-level cv-qualifiers)
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a&, b>, b>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const&, b>, b>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile&, b>, b>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile&, b>, b>));

  // Source rvalue-reference type (no top-level cv-qualifiers)
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a&&, b>, b>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const&&, b>, b>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile&&, b>, b>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile&&, b>, b>));

  // Target lvalue reference type (no effect)
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a, b&>, b&>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const, b&>, b&>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile, b&>, b&>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile, b&>, b&>));

  // Target rvalue reference type (no effect)
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a, b&&>, b&&>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const, b&&>, b&&>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a volatile, b&&>, b&&>));
  EXPECT_TRUE((is_same_v<give_qualifiers_to<a const volatile, b&&>, b&&>));
}

}  // namespace
