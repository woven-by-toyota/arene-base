// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file variant_get_test.cpp
/// @brief Unit tests for arene::base::variant get APIs which depend on `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)` being
/// `false`.
///

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/move.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/variant/variant.hpp"

namespace {

static_assert(ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED), "This test file requires exceptions to be disabled");

using ::arene::base::get;

using alternative0_t = std::int32_t;
using alternative1_t = float;
using variant_type = arene::base::variant<alternative0_t, alternative1_t>;

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `vrnt.get<Idx>()`
/// is invoked with `Idx!=vrnt.index()`, then it is an `ARENE_PRECONDITION` violation.
TEST(VariantGetExceptionsDisabledDeathTest, MemberGetForIncorrectIndexIsPreconditionViolation) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(vrnt.get<1>(), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(vrnt.get<0>(), "Precondition");
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(vrnt.get<1>(), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(vrnt.get<0>(), "Precondition");
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(std::move(vrnt).get<1>(), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(std::move(vrnt).get<0>(), "Precondition");
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(std::move(vrnt).get<1>(), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(std::move(vrnt).get<0>(), "Precondition");
    }
  }
}

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `vrnt.get<T>()`
/// is invoked with `T` which is not the active alternative, then it is an `ARENE_PRECONDITION` violation.
TEST(VariantGetExceptionsDisabledDeathTest, MemberGetForIncorrectTypeIsPreconditionViolation) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(vrnt.get<alternative1_t>(), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(vrnt.get<alternative0_t>(), "Precondition");
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(vrnt.get<alternative1_t>(), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(vrnt.get<alternative0_t>(), "Precondition");
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(std::move(vrnt).get<alternative1_t>(), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(std::move(vrnt).get<alternative0_t>(), "Precondition");
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(std::move(vrnt).get<alternative1_t>(), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(std::move(vrnt).get<alternative0_t>(), "Precondition");
    }
  }
}

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `get<Idx>(vrnt)`
/// is invoked with `Idx!=vrnt.index()`, then then it is an `ARENE_PRECONDITION` violation.
TEST(VariantGetExceptionsDisabledDeathTest, GetForIncorrectIndexIsPreconditionViolation) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(get<1>(vrnt), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(get<0>(vrnt), "Precondition");
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(get<1>(vrnt), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(get<0>(vrnt), "Precondition");
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(get<1>(std::move(vrnt)), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(get<0>(std::move(vrnt)), "Precondition");
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(get<1>(std::move(vrnt)), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(get<0>(std::move(vrnt)), "Precondition");
    }
  }
}

/// @test Given an instance of `variant<Ts...> vrnt` and `ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)`, when `get<T>(vrnt)`
/// is invoked with `T` which is not the active alternative, then it is an `ARENE_PRECONDITION` violation.
TEST(VariantGetExceptionsDisabledDeathTest, GetForIncorrectTypeIsPreconditionViolation) {
  auto const alternative0_v = alternative0_t{-10};
  auto const alternative1_v = alternative1_t{0.1234F};
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(get<alternative1_t>(vrnt), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(get<alternative0_t>(vrnt), "Precondition");
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(get<alternative1_t>(vrnt), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(get<alternative0_t>(vrnt), "Precondition");
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      ASSERT_DEATH(get<alternative1_t>(std::move(vrnt)), "Precondition");
    }
    {
      auto vrnt = variant_type{alternative1_v};
      ASSERT_DEATH(get<alternative0_t>(std::move(vrnt)), "Precondition");
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(get<alternative1_t>(std::move(vrnt)), "Precondition");
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      ASSERT_DEATH(get<alternative0_t>(std::move(vrnt)), "Precondition");
    }
  }
}

}  // namespace
