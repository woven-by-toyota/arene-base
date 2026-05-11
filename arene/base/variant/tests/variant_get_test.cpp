// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file variant_get_test.cpp
/// @brief Unit tests for arene::base::variant get APIs which do not depend on `ARENE_EXCEPTIONS_ENABLED`'s state.
///

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/variant/traits.hpp"
#include "arene/base/variant/variant.hpp"

namespace {

/// @test Given an instance of `variant<Ts...> vrnt`, when `vrnt.get<Idx>()` is invoked, then the return type is
/// `variant_alternative_t<Idx, variant<Ts...>>` with the cref qualification of `vrnt`.
TEST(VariantGet, MemberGetForIndexPreservesRefQualificationOfTheVariant) {
  using variant_type = arene::base::variant<std::int32_t, float>;
  // lvalue
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type&>().get<0>()),
      arene::base::variant_alternative_t<0, variant_type>&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type&>().get<1>()),
      arene::base::variant_alternative_t<1, variant_type>&>();

  // clvalue
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type const&>().get<0>()),
      arene::base::variant_alternative_t<0, variant_type const>&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type const&>().get<1>()),
      arene::base::variant_alternative_t<1, variant_type const>&>();

  // rvalue
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type&&>().get<0>()),
      arene::base::variant_alternative_t<0, variant_type>&&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type&&>().get<1>()),
      arene::base::variant_alternative_t<1, variant_type>&&>();

  // crvalue
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type const&&>().get<0>()),
      arene::base::variant_alternative_t<0, variant_type const>&&>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<variant_type const&&>().get<1>()),
      arene::base::variant_alternative_t<1, variant_type const>&&>();
}

/// @test Given an instance of `variant<Ts...> vrnt`, when `vrnt.get<Idx>()` is invoked with `Idx==vrnt.index()`, then
/// the return is the held alternative.
TEST(VariantGet, MemberGetForIndexReturnsHeldValue) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");
  using alternative0_t = std::int32_t;
  auto const alternative0_v = alternative0_t{-10};
  using alternative1_t = float;
  auto const alternative1_v = alternative1_t{0.1234F};
  using variant_type = arene::base::variant<alternative0_t, alternative1_t>;

  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(vrnt.get<0>(), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(vrnt.get<1>(), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      EXPECT_EQ(vrnt.get<0>(), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      EXPECT_EQ(vrnt.get<1>(), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(std::move(vrnt).get<0>(), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(std::move(vrnt).get<1>(), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(std::move(vrnt).get<0>(), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(std::move(vrnt).get<1>(), alternative1_v);
    }
  }
  ARENE_IGNORE_END();
}

/// @test Given an instance of `variant<Ts...> vrnt`, when `vrnt.get<T>()` is invoked, then the return type is
/// `T` with the cref qualification of `vrnt`.
TEST(VariantGet, MemberGetForTypePreservesRefQualificationOfTheVariant) {
  using alternative0_t = std::int32_t;
  using alternative1_t = float;
  using variant_type = arene::base::variant<alternative0_t, alternative1_t>;
  // lvalue
  ::testing::StaticAssertTypeEq<decltype(std::declval<variant_type&>().get<alternative0_t>()), alternative0_t&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<variant_type&>().get<alternative1_t>()), alternative1_t&>();

  // clvalue
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<variant_type const&>().get<alternative0_t>()), alternative0_t const&>();
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<variant_type const&>().get<alternative1_t>()), alternative1_t const&>();

  // rvalue
  ::testing::StaticAssertTypeEq<decltype(std::declval<variant_type&&>().get<alternative0_t>()), alternative0_t&&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<variant_type&&>().get<alternative1_t>()), alternative1_t&&>();

  // crvalue
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<variant_type const&&>().get<alternative0_t>()), alternative0_t const&&>(
      );
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<variant_type const&&>().get<alternative1_t>()), alternative1_t const&&>(
      );
}

/// @test Given an instance of `variant<Ts...> vrnt`, when `vrnt.get<T>()` is invoked with `T` where
// `index_of_v<Ts..., T> == vrnt.index()`, then the return is the held alternative.
TEST(VariantGet, MemberGetForTypeReturnsHeldValue) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");
  using alternative0_t = std::int32_t;
  auto const alternative0_v = alternative0_t{-10};
  using alternative1_t = float;
  auto const alternative1_v = alternative1_t{0.1234F};
  using variant_type = arene::base::variant<alternative0_t, alternative1_t>;
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(vrnt.get<alternative0_t>(), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(vrnt.get<alternative1_t>(), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      EXPECT_EQ(vrnt.get<alternative0_t>(), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      EXPECT_EQ(vrnt.get<alternative1_t>(), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(std::move(vrnt).get<alternative0_t>(), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(std::move(vrnt).get<alternative1_t>(), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(std::move(vrnt).get<alternative0_t>(), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(std::move(vrnt).get<alternative1_t>(), alternative1_v);
    }
  }
  ARENE_IGNORE_END();
}

/// @test Given an instance of `variant<Ts...> vrnt`, when `::arene::base::get<Idx>(vrnt)` is invoked, then the return
/// type is `variant_alternative_t<Idx, variant<Ts...>>` with the cref qualification of `vrnt`.
TEST(VariantGet, GetForIndexPreservesRefQualificationOfTheVariant) {
  using variant_type = arene::base::variant<std::int32_t, float>;
  // lvalue
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<0>(std::declval<variant_type&>())),
      arene::base::variant_alternative_t<0, variant_type>&>();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<1>(std::declval<variant_type&>())),
      arene::base::variant_alternative_t<1, variant_type>&>();

  // clvalue
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<0>(std::declval<variant_type const&>())),
      arene::base::variant_alternative_t<0, variant_type> const&>();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<1>(std::declval<variant_type const&>())),
      arene::base::variant_alternative_t<1, variant_type> const&>();

  // rvalue
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<0>(std::declval<variant_type&&>())),
      arene::base::variant_alternative_t<0, variant_type>&&>();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<1>(std::declval<variant_type&&>())),
      arene::base::variant_alternative_t<1, variant_type>&&>();

  // crvalue
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<0>(std::declval<variant_type const&&>())),
      arene::base::variant_alternative_t<0, variant_type> const&&>();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<1>(std::declval<variant_type const&&>())),
      arene::base::variant_alternative_t<1, variant_type> const&&>();
}

/// @test Given an instance of `variant<Ts...> vrnt`, when `::arene::base::get<Idx>(vrnt)` is invoked with
/// `Idx==vrnt.index()`, then the return is the held alternative.
TEST(VariantGet, GetForIndexReturnsHeldValue) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");
  using alternative0_t = std::int32_t;
  auto const alternative0_v = alternative0_t{-10};
  using alternative1_t = float;
  auto const alternative1_v = alternative1_t{0.1234F};
  using variant_type = arene::base::variant<alternative0_t, alternative1_t>;
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(::arene::base::get<0>(vrnt), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(::arene::base::get<1>(vrnt), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      EXPECT_EQ(::arene::base::get<0>(vrnt), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      EXPECT_EQ(::arene::base::get<1>(vrnt), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(::arene::base::get<0>(std::move(vrnt)), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(::arene::base::get<1>(std::move(vrnt)), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(::arene::base::get<0>(std::move(vrnt)), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(::arene::base::get<1>(std::move(vrnt)), alternative1_v);
    }
  }
  ARENE_IGNORE_END();
}

/// @test Given an instance of `variant<Ts...> vrnt`, when `::arene::base::get<T>(vrnt)` is invoked, then the return
/// type is `T` with the cref qualification of `vrnt`.
TEST(VariantGet, GetForTypePreservesRefQualificationOfTheVariant) {
  using alternative0_t = std::int32_t;
  using alternative1_t = float;
  using variant_type = arene::base::variant<alternative0_t, alternative1_t>;
  // lvalue
  ::testing::
      StaticAssertTypeEq<decltype(::arene::base::get<alternative0_t>(std::declval<variant_type&>())), alternative0_t&>(
      );
  ::testing::
      StaticAssertTypeEq<decltype(::arene::base::get<alternative1_t>(std::declval<variant_type&>())), alternative1_t&>(
      );

  // clvalue
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<alternative0_t>(std::declval<variant_type const&>())),
      alternative0_t const&>();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<alternative1_t>(std::declval<variant_type const&>())),
      alternative1_t const&>();

  // rvalue
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<alternative0_t>(std::declval<variant_type&&>())),
      alternative0_t&&>();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<alternative1_t>(std::declval<variant_type&&>())),
      alternative1_t&&>();

  // crvalue
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<alternative0_t>(std::declval<variant_type const&&>())),
      alternative0_t const&&>();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::get<alternative1_t>(std::declval<variant_type const&&>())),
      alternative1_t const&&>();
}

/// @test Given an instance of `variant<Ts...> vrnt`, when `::arene::base::get<T>(vrnt)` is invoked with `T` where
// `index_of_v<Ts..., T> == vrnt.index()`, then the return is the held alternative.
TEST(VariantGet, GetForTypeReturnsHeldValue) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");
  using alternative0_t = std::int32_t;
  auto const alternative0_v = alternative0_t{-10};
  using alternative1_t = float;
  auto const alternative1_v = alternative1_t{0.1234F};
  using variant_type = arene::base::variant<alternative0_t, alternative1_t>;
  {
    SCOPED_TRACE("lvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(::arene::base::get<alternative0_t>(vrnt), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(::arene::base::get<alternative1_t>(vrnt), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("clvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      EXPECT_EQ(::arene::base::get<alternative0_t>(vrnt), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      EXPECT_EQ(::arene::base::get<alternative1_t>(vrnt), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("rvalue");
    {
      auto vrnt = variant_type{alternative0_v};
      EXPECT_EQ(::arene::base::get<alternative0_t>(std::move(vrnt)), alternative0_v);
    }
    {
      auto vrnt = variant_type{alternative1_v};
      EXPECT_EQ(::arene::base::get<alternative1_t>(std::move(vrnt)), alternative1_v);
    }
  }

  {
    SCOPED_TRACE("crvalue");
    {
      auto const vrnt = variant_type{alternative0_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(::arene::base::get<alternative0_t>(std::move(vrnt)), alternative0_v);
    }
    {
      auto const vrnt = variant_type{alternative1_v};
      // NOLINTNEXTLINE(hicpp-move-const-arg) explicitly testing crvalue overload
      EXPECT_EQ(::arene::base::get<alternative1_t>(std::move(vrnt)), alternative1_v);
    }
  }
  ARENE_IGNORE_END();
}

}  // namespace
