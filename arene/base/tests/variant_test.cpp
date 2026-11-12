// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/variant.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

/// @test `variant` is declared
TEST(VariantExport, Variant) { using ::arene::base::variant; }

/// @test `variant_npos` is declared
TEST(VariantExport, VariantNpos) { using ::arene::base::variant_npos; }

/// @test `variant_size_v` is declared
TEST(VariantExport, VariantSizeV) { using ::arene::base::variant_size_v; }

/// @test `variant_alternative` is declared
TEST(VariantExport, VariantAlternative) { using ::arene::base::variant_alternative; }

/// @test `variant_alternative_t` is declared
TEST(VariantExport, VariantAlternativeT) { using ::arene::base::variant_alternative_t; }

/// @test `bad_variant_access` is declared
TEST(VariantExport, BadVariantAccess) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  using ::arene::base::bad_variant_access;
#else
  GTEST_SKIP() << "::arene::base::bad_variant_access is not available when exceptions are disabled.";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
}

/// @test `get` is declared
TEST(VariantExport, Get) { using ::arene::base::get; }

/// @test `holds_alternative` is declared
TEST(VariantExport, HoldsAlternative) { using ::arene::base::holds_alternative; }

/// @test `get_if` is declared
TEST(VariantExport, GetIf) { using ::arene::base::get_if; }

/// @test `visit` is declared
TEST(VariantExport, Visit) { using ::arene::base::visit; }

}  // namespace
