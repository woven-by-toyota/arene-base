// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file variant_get_test.cpp
/// @brief Unit tests for arene::base::variant get APIs which depend on `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)` being
/// `true`.
///

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/variant/detail/bad_variant_access_exceptions_enabled.hpp"
#include "arene/base/variant/variant.hpp"

namespace {

static_assert(ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED), "This test file requires exceptions to be enabled");

using ::arene::base::bad_variant_access;
using ::arene::base::get;

using alternative0_t = std::int32_t;
using alternative1_t = float;
using variant_type = arene::base::variant<alternative0_t, alternative1_t>;

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `vrnt.get<Idx>()`
/// is invoked with `Idx!=vrnt.index()`, then `arene::base::bad_variant_access` is thrown.
TEST(VariantGet, MemberGetForIncorrectIndexThrowsBadVariantAccess) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(vrnt.get<1>(), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(vrnt.get<0>(), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_THROW(vrnt.get<1>(), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_THROW(vrnt.get<0>(), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(std::move(vrnt).get<1>(), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(std::move(vrnt).get<0>(), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(std::move(vrnt).get<1>(), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(std::move(vrnt).get<0>(), bad_variant_access);
    }
  }
}

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `vrnt.get<T>()`
/// is invoked with `T` which is not the active alternative, then `arene::base::bad_variant_access` is thrown.
TEST(VariantGet, MemberGetForIncorrectTypeThrowsBadVariantAccess) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(vrnt.get<alternative1_t>(), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(vrnt.get<alternative0_t>(), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_THROW(vrnt.get<alternative1_t>(), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_THROW(vrnt.get<alternative0_t>(), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(std::move(vrnt).get<alternative1_t>(), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(std::move(vrnt).get<alternative0_t>(), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(std::move(vrnt).get<alternative1_t>(), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(std::move(vrnt).get<alternative0_t>(), bad_variant_access);
    }
  }
}

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `get<Idx>(vrnt)`
/// is invoked with `Idx!=vrnt.index()`, then `arene::base::bad_variant_access` is thrown.
TEST(VariantGet, GetForIncorrectIndexThrowsBadVariantAccess) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(get<1>(vrnt), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(get<0>(vrnt), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_THROW(get<1>(vrnt), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_THROW(get<0>(vrnt), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(get<1>(std::move(vrnt)), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(get<0>(std::move(vrnt)), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(get<1>(std::move(vrnt)), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(get<0>(std::move(vrnt)), bad_variant_access);
    }
  }
}

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `get<T>(vrnt)`
/// is invoked with `T` which is not the active alternative, then `arene::base::bad_variant_access` is thrown.
TEST(VariantGet, GetForIncorrectTypeThrowsBadVariantAccess) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(get<alternative1_t>(vrnt), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(get<alternative0_t>(vrnt), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_THROW(get<alternative1_t>(vrnt), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_THROW(get<alternative0_t>(vrnt), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_THROW(get<alternative1_t>(std::move(vrnt)), bad_variant_access);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_THROW(get<alternative0_t>(std::move(vrnt)), bad_variant_access);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(get<alternative1_t>(std::move(vrnt)), bad_variant_access);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_THROW(get<alternative0_t>(std::move(vrnt)), bad_variant_access);
    }
  }
}

}  // namespace
