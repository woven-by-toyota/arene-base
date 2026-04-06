// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file pointer_test.cpp
/// @brief Unit tests for arene/base/pointer.hpp
///
///
///

#include "arene/base/pointer.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

/// @test `non_owning_ptr` is declared
TEST(PointerExports, NonOwningPointer) { using ::arene::base::non_owning_ptr; }
/// @test `make_non_owning_ptr` is declared
TEST(PointerExports, MakeNonOwningPointer) { using ::arene::base::make_non_owning_ptr; }

/// @test `non_null` is declared
TEST(PointerExports, NonNull) { using ::arene::base::non_null; }
/// @test `non_null_ptr` is declared
TEST(PointerExports, NonNullPtr) { using ::arene::base::non_null_ptr; }

/// @test `non_null_shared_ptr` is declared
TEST(PointerExports, NonNullSharedPtr) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  using ::arene::base::non_null_shared_ptr;
#endif
}

/// @test `non_null_unique_ptr` is declared
TEST(PointerExports, NonNullUniquePtr) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  using ::arene::base::non_null_unique_ptr;
#endif
}

/// @test `make_non_null` is declared
TEST(PointerExports, MakeNonNullPtr) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  using ::arene::base::make_non_null;
#endif
}

/// @test `make_shared` is declared
TEST(PointerExports, MakeShared) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  using ::arene::base::make_shared;
#endif
}

/// @test `make_unique` is declared
TEST(PointerExports, MakeUnique) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  using ::arene::base::make_unique;
#endif
}

}  // namespace
