// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/semantic_version.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `semantic_version` is exported
TEST(SemanticVersion, SemanticVersionIsExported) { using ::arene::base::semantic_version; }

/// @test Ensure `sem_ver_diff` is exported
TEST(SemanticVersion, SemVerDiffIsExported) { using ::arene::base::sem_ver_diff; }

/// @test Ensure `contains_major_change` is exported
TEST(SemanticVersion, ContainsMajorChangeIsExported) { using ::arene::base::contains_major_change; }

/// @test Ensure `is_major_change` is exported
TEST(SemanticVersion, IsMajorChangeIsExported) { using ::arene::base::is_major_change; }

/// @test Ensure `contains_minor_change` is exported
TEST(SemanticVersion, ContainsMinorChangeIsExported) { using ::arene::base::contains_minor_change; }

/// @test Ensure `is_minor_change` is exported
TEST(SemanticVersion, IsMinorChangeIsExported) { using ::arene::base::is_minor_change; }

/// @test Ensure `contains_patch_change` is exported
TEST(SemanticVersion, ContainsPatchChangeIsExported) { using ::arene::base::contains_patch_change; }

/// @test Ensure `is_patch_change` is exported
TEST(SemanticVersion, IsPatchChangeIsExported) { using ::arene::base::is_patch_change; }

}  // namespace
