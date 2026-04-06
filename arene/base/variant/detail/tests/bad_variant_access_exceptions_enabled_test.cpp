// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/variant/detail/bad_variant_access_exceptions_enabled.hpp"

#include <gtest/gtest.h>

#include "arene/base/variant/traits.hpp"

namespace {

using ::arene::base::bad_variant_access;
using ::arene::base::variant_npos;
using ::arene::base::variant_detail::variant_must_have_value;

/// @test A default-constructed `bad_variant_access` exception type has a `what()` function that returns the correct
/// string
TEST(BadVariantAccess, DefaultReason) {
  auto const exception = arene::base::bad_variant_access{};
  ASSERT_STREQ(exception.what(), "Unknown reason");
}

/// @test If a string is supplied to the constructor of `bad_variant_access`, that string is returned from `what`
TEST(BadVariantAccess, VariantBadAccessExceptionRemembersItsWhatString) {
  char const* empty = "";

  {
    auto const exception = arene::base::bad_variant_access{empty};
    ASSERT_EQ(exception.what(), empty);
  }

  char const* hello = "hello";

  {
    auto const exception = arene::base::bad_variant_access{hello};
    ASSERT_EQ(exception.what(), hello);
  }

  char const* nothing = nullptr;

  {
    auto const exception = arene::base::bad_variant_access{nothing};
    ASSERT_EQ(exception.what(), nothing);
  }
}

/// @test The result of copying a `bad_variant_access` object has the same value returned from `what` as the original
TEST(BadVariantAccess, VariantBadAccessExceptionRemembersItsWhatStringAfterCopy) {
  char const* empty = "";

  {
    auto const exception = arene::base::bad_variant_access{empty};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const copy = exception;
    ASSERT_EQ(exception.what(), empty);
    ASSERT_EQ(exception.what(), copy.what());
  }

  char const* hello = "hello";

  {
    auto const exception = arene::base::bad_variant_access{hello};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const copy = exception;
    ASSERT_EQ(exception.what(), hello);
    ASSERT_EQ(exception.what(), copy.what());
  }

  char const* nothing = nullptr;

  {
    auto const exception = arene::base::bad_variant_access{nothing};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const copy = exception;
    ASSERT_EQ(exception.what(), nothing);
    ASSERT_EQ(exception.what(), copy.what());
  }
}

/// @test The result of copy-assignment of a `bad_variant_access` object has the same value returned from `what` as the
/// original
TEST(BadVariantAccess, VariantBadAccessExceptionRemembersItsWhatStringAfterAssign) {
  char const* nothing = nullptr;
  auto empty_exception = arene::base::bad_variant_access{nothing};

  char const* empty = "";

  {
    auto const exception = arene::base::bad_variant_access{empty};
    empty_exception = exception;
    ASSERT_EQ(exception.what(), empty);
    ASSERT_EQ(exception.what(), empty_exception.what());
  }

  char const* hello = "hello";

  {
    auto const exception = arene::base::bad_variant_access{hello};
    empty_exception = exception;
    ASSERT_EQ(exception.what(), hello);
    ASSERT_EQ(exception.what(), empty_exception.what());
  }

  {
    auto const exception = arene::base::bad_variant_access{nothing};
    empty_exception = exception;
    ASSERT_EQ(exception.what(), nothing);
    ASSERT_EQ(exception.what(), empty_exception.what());
  }
}

/// @test When `arene::base::variant_detail::variant_must_have_value(requested, index)` is invoked with
/// `requested==index`, then it is a nop.
TEST(VariantMustHaveValue, RequestedEqualsIndexIsNop) { variant_must_have_value(0U, 0U); }

/// @test When `arene::base::variant_detail::variant_must_have_value(requested, index)` is invoked with
/// `index==variant_npos`, then `arene::base::bad_variant_access` is thrown.
TEST(VariantMustHaveValue, IndexEqualsVariantNposThrowsBadVariantAccess) {
  ASSERT_THROW(variant_must_have_value(0U, variant_npos), bad_variant_access);
}

/// @test When `arene::base::variant_detail::variant_must_have_value(requested, index)` is invoked with
/// `requested!=index`, then `arene::base::bad_variant_access` is thrown.
TEST(VariantMustHaveValue, RequestedNotEqualsIndexThrowsBadVariantAccess) {
  ASSERT_THROW(variant_must_have_value(0U, 1U), bad_variant_access);
  ASSERT_THROW(variant_must_have_value(1U, 0U), bad_variant_access);
  ASSERT_THROW(variant_must_have_value(1U, 2U), bad_variant_access);
  ASSERT_THROW(variant_must_have_value(2U, 1U), bad_variant_access);
}

}  // namespace
