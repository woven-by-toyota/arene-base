// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_only_explicitly_constructible.hpp"

#include <gtest/gtest.h>

// IWYU pragma: begin_keep
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/testing/gtest.hpp"
// IWYU pragma: end_keep

namespace {
class explicitly_default_constructible;
template <typename T>
class explicitly_constructible_from;
template <typename T>
class explicitly_constructible_from_multiple;

using ::arene::base::is_nothrow_only_explicitly_constructible;
using ::arene::base::is_nothrow_only_explicitly_constructible_v;
using ::arene::base::is_only_explicitly_constructible;
using ::arene::base::is_only_explicitly_constructible_v;

template <class T, class... Args>
constexpr bool
    is_only_explicitly_constructible_check_v{is_only_explicitly_constructible<T, Args...>::value && is_only_explicitly_constructible_v<T, Args...>};

template <class T, class... Args>
constexpr bool is_nothrow_only_explicitly_constructible_check_v{
  is_only_explicitly_constructible_check_v<T, Args...> &&         //
         is_nothrow_only_explicitly_constructible<T, Args...>::value &&  //
         is_nothrow_only_explicitly_constructible_v<T, Args...>
};

template <typename T>
class implicitly_constructible_from {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr implicitly_constructible_from(T) noexcept {}
};

template <typename T>
class explicitly_constructible_from {
 public:
  explicit explicitly_constructible_from(T) noexcept {}
};

/// @test Can check the implicit constructiblity of single argument constructors
CONSTEXPR_TEST(IsExplicitlyConstructibleTest, SingleArgumentConstructorsAreCorrectlyDetected) {
  CONSTEXPR_ASSERT(is_nothrow_only_explicitly_constructible_check_v<
                   explicitly_constructible_from<std::int32_t>,
                   std::int32_t>);
  CONSTEXPR_ASSERT(is_nothrow_only_explicitly_constructible_check_v<
                   explicitly_constructible_from<implicitly_constructible_from<std::int32_t>>,
                   std::int32_t>);

  CONSTEXPR_ASSERT_FALSE(is_only_explicitly_constructible_check_v<
                         implicitly_constructible_from<std::int32_t>,
                         std::int32_t>);
}

template <typename T>
class explicitly_constructible_from_multiple {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr explicitly_constructible_from_multiple(T) noexcept {}
  constexpr explicit explicitly_constructible_from_multiple(T, T) noexcept {}
};

/// @test Can check the implicit constructiblity of multiple argument constructors
CONSTEXPR_TEST(IsExplicitlyConstructibleTest, MultipleArgumentConstructorsAreCorrectlyDetected) {
  CONSTEXPR_ASSERT(is_nothrow_only_explicitly_constructible_check_v<
                   explicitly_constructible_from_multiple<std::int32_t>,
                   std::int32_t,
                   std::int32_t>);

  CONSTEXPR_ASSERT(is_nothrow_only_explicitly_constructible_check_v<
                   explicitly_constructible_from_multiple<std::int32_t>,
                   std::int8_t,
                   std::int16_t>);

  CONSTEXPR_ASSERT_FALSE(is_only_explicitly_constructible_check_v<
                         explicitly_constructible_from_multiple<std::int32_t>,
                         std::int32_t>);
}

class implicitly_default_constructible {
 public:
  constexpr implicitly_default_constructible() noexcept = default;
};

class explicitly_default_constructible {
 public:
  explicit constexpr explicitly_default_constructible() noexcept = default;
};

/// @test Can check the implicit constructiblity of default constructors
CONSTEXPR_TEST(IsExplicitlyConstructibleTest, DefaultConstructorsAreCorrectlyDetected) {
  CONSTEXPR_ASSERT(is_nothrow_only_explicitly_constructible_check_v<explicitly_default_constructible>);
  CONSTEXPR_ASSERT_FALSE(is_only_explicitly_constructible_check_v<implicitly_default_constructible>);
}

class make_non_aggregate {};

class target {
 public:
  constexpr target() = default;
  explicit constexpr target(make_non_aggregate) {}
};

class convertible_source {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator target() const { return target{}; }
};

/// @test Check convertible is not explicitly constructible
CONSTEXPR_TEST(IsExplicitlyConstructibleTest, ConvertibilityDoesNotImplyImplicitConstruction) {
  CONSTEXPR_ASSERT_FALSE(is_only_explicitly_constructible_check_v<target, convertible_source>);
}

template <typename T>
class throwing_explicitly_constructible_from {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr explicit throwing_explicitly_constructible_from(T) noexcept(false) {}
};

/// @test Can check the nothrow implicit constructiblity of single argument constructors
CONSTEXPR_TEST(IsExplicitlyConstructibleTest, SingleArgumentThrowingConstructorsAreCorrectlyDetected) {
  CONSTEXPR_ASSERT(is_only_explicitly_constructible_check_v<
                   throwing_explicitly_constructible_from<std::int32_t>,
                   std::int32_t>);
  CONSTEXPR_ASSERT_FALSE(is_nothrow_only_explicitly_constructible_check_v<
                         throwing_explicitly_constructible_from<std::int32_t>,
                         std::int32_t>);

  CONSTEXPR_ASSERT(is_only_explicitly_constructible_check_v<
                   throwing_explicitly_constructible_from<std::int32_t>,
                   std::int8_t>);
  CONSTEXPR_ASSERT_FALSE(is_nothrow_only_explicitly_constructible_check_v<
                         throwing_explicitly_constructible_from<std::int32_t>,
                         std::int8_t>);
}

template <typename T>
class throwing_explicitly_constructible_from_multiple {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr explicit throwing_explicitly_constructible_from_multiple(T) noexcept {}
  constexpr explicit throwing_explicitly_constructible_from_multiple(T, T) noexcept(false) {}
};

/// @test Can check the nothrow explicit constructiblity of multiple argument constructors
CONSTEXPR_TEST(IsExplicitlyConstructibleTest, MultipleArgumentThrowingConstructorsAreCorrectlyDetected) {
  CONSTEXPR_ASSERT(is_only_explicitly_constructible_check_v<
                   throwing_explicitly_constructible_from_multiple<std::int32_t>,
                   std::int32_t,
                   std::int32_t>);
  CONSTEXPR_ASSERT_FALSE(is_nothrow_only_explicitly_constructible_check_v<
                         throwing_explicitly_constructible_from_multiple<std::int32_t>,
                         std::int32_t,
                         std::int32_t>);

  CONSTEXPR_ASSERT(is_only_explicitly_constructible_check_v<
                   throwing_explicitly_constructible_from_multiple<std::int32_t>,
                   std::int8_t,
                   std::int16_t>);
  CONSTEXPR_ASSERT_FALSE(is_nothrow_only_explicitly_constructible_check_v<
                         throwing_explicitly_constructible_from_multiple<std::int32_t>,
                         std::int8_t,
                         std::int16_t>);
}
}  // namespace
