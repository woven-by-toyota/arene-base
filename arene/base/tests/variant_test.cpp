// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/variant.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

/// @test `variant` is declared
TEST(Variant, Variant) { using ::arene::base::variant; }

/// @test `variant_npos` is declared
TEST(Variant, VariantNpos) { using ::arene::base::variant_npos; }

/// @test `variant_size_v` is declared
TEST(Variant, VariantSizeV) { using ::arene::base::variant_size_v; }

/// @test `variant_alternative` is declared
TEST(Variant, VariantAlternative) { using ::arene::base::variant_alternative; }

/// @test `variant_alternative_t` is declared
TEST(Variant, VariantAlternativeT) { using ::arene::base::variant_alternative_t; }

/// @test `bad_variant_access` is declared
TEST(Variant, BadVariantAccess) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  using ::arene::base::bad_variant_access;
#else
  GTEST_SKIP() << "::arene::base::bad_variant_access is not available when exceptions are disabled.";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
}

/// @test `get` is declared
TEST(Variant, Get) { using ::arene::base::get; }

/// @test `holds_alternative` is declared
TEST(Variant, HoldsAlternative) { using ::arene::base::holds_alternative; }

/// @test `get_if` is declared
TEST(Variant, GetIf) { using ::arene::base::get_if; }

/// @test `visit` is declared
TEST(Variant, Visit) { using ::arene::base::visit; }

}  // namespace
