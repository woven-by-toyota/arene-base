// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/units.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `quantity` is declared
TEST(Units, Quantity) { using ::arene::base::quantity; }

/// @test `quantity_kind` is declared
TEST(Units, QuantityKind) { using ::arene::base::quantity_kind; }

/// @test `quantity_unit` is declared
TEST(Units, QuantityUnit) { using ::arene::base::quantity_unit; }

/// @test `quantity_origin` is declared
TEST(Units, QuantityOrigin) { using ::arene::base::quantity_origin; }

/// @test `relative_origin` is declared
TEST(Units, RelativeOrigin) { using ::arene::base::relative_origin; }

/// @test `is_quantity_origin_v` is declared
TEST(Units, IsQuantityOrigin) { using ::arene::base::is_quantity_origin_v; }

/// @test `quantity_has_origin_v` is declared
TEST(Units, QuantityHasOrigin) { using ::arene::base::quantity_has_origin_v; }

/// @test `quantity_origin_t` is declared
TEST(Units, QuantityOriginT) { using ::arene::base::quantity_origin_t; }

}  // namespace
