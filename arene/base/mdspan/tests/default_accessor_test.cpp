// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/default_accessor.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "stdlib/include/type_traits"

namespace {

namespace test {

template <class T>
struct implicitly_constructible_from {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr implicitly_constructible_from(T const&) noexcept {}
};

struct user_defined_type {};

struct user_defined_non_trivially_copyable_type {
  // NOLINTNEXTLINE(hicpp-use-equals-default) : intentional to disable trivial copyability
  constexpr user_defined_non_trivially_copyable_type() {}

  user_defined_non_trivially_copyable_type(user_defined_non_trivially_copyable_type const&) = delete;
  auto operator=(user_defined_non_trivially_copyable_type const&) -> user_defined_non_trivially_copyable_type& = delete;

  constexpr user_defined_non_trivially_copyable_type(user_defined_non_trivially_copyable_type&&) noexcept {}
  auto operator=(user_defined_non_trivially_copyable_type&&) noexcept -> user_defined_non_trivially_copyable_type& {
    return *this;
  }

  ~user_defined_non_trivially_copyable_type() = default;
};

static_assert(
    !std::is_trivially_copyable<user_defined_non_trivially_copyable_type>::value,
    "`user_defined_non_trivially_copyable_type` is intended to *not* be trivially copyable"
);

}  // namespace test

template <class T>
class DefaultAccessor : public testing::Test {};

// NOLINTBEGIN(google-runtime-int)
using TestTypes = ::testing::Types<  //
    int,
    int const,
    int*,
    int const*,
    test::user_defined_type,
    test::user_defined_type const,
    test::user_defined_non_trivially_copyable_type,
    test::user_defined_non_trivially_copyable_type const>;
// NOLINTEND(google-runtime-int)

TYPED_TEST_SUITE(DefaultAccessor, TestTypes, );

/// @test @c default_accessor has expected type aliases
TYPED_TEST(DefaultAccessor, HasTypeAliases) {
  using accessor_type = arene::base::default_accessor<TypeParam>;

  static_assert(
      std::is_same<accessor_type, typename accessor_type::offset_policy>::value,
      "`offset_policy` must match `default_accessor<TypeParam>`"
  );
  static_assert(
      std::is_same<TypeParam, typename accessor_type::element_type>::value,
      "`element_type` must match `TypeParam`"
  );
  static_assert(
      std::is_same<TypeParam&, typename accessor_type::reference>::value,
      "`reference` must match `TypeParam&`"
  );
  static_assert(
      std::is_same<TypeParam*, typename accessor_type::data_handle_type>::value,
      "`data_handle_type` must match `TypeParam*`"
  );
}

/// @test @c default_accessor is semiregular
TYPED_TEST(DefaultAccessor, IsSemiregular) {
  using accessor_type = arene::base::default_accessor<TypeParam>;

  static_assert(
      std::is_default_constructible<accessor_type>::value,
      "`default_accessor<TypeParam>` must be default constructible"
  );
  static_assert(
      std::is_copy_constructible<accessor_type>::value,
      "`default_accessor<TypeParam>` must be copy constructible"
  );
  static_assert(
      std::is_copy_assignable<accessor_type>::value,  //
      "`default_accessor<TypeParam>` must be copy assignable"
  );
  static_assert(
      std::is_move_constructible<accessor_type>::value,
      "`default_accessor<TypeParam>` must be move constructible"
  );
  static_assert(
      std::is_move_assignable<accessor_type>::value,  //
      "`default_accessor<TypeParam>` must be move assignable"
  );
  static_assert(arene::base::is_swappable_v<accessor_type>, "`default_accessor<TypeParam>` must be swappable");
}

/// @test @c default_accessor is trivially copyable
TYPED_TEST(DefaultAccessor, IsTriviallyCopyable) {
  using accessor_type = arene::base::default_accessor<TypeParam>;

  static_assert(
      std::is_trivially_copyable<accessor_type>::value,
      "`default_accessor<TypeParam>` must be trivally copyable"
  );
}

/// @test @c default_accessor is always nothrow default constructible
TYPED_TEST(DefaultAccessor, NothrowDefaultConstructible) {
  using accessor_type = arene::base::default_accessor<TypeParam>;

  static_assert(
      std::is_nothrow_default_constructible<accessor_type>::value,
      "`default_accessor<TypeParam>` must be nothrow default constructible"
  );
}

/// @test @c default_accessor is constructible from a different @c
/// default_accessor with an @c ElementType with "stronger" cv-qualification
TYPED_TEST(DefaultAccessor, ConvertibleFromOther) {
  using arene::base::default_accessor;
  using accessor_type = default_accessor<TypeParam>;

  static_assert(
      std::is_nothrow_constructible<default_accessor<TypeParam const>, accessor_type>::value,
      "`default_accessor<cv-qualified TypeParam>` must be nothrow constructible from `default_accessor<TypeParam>`"
  );
  static_assert(
      std::is_nothrow_constructible<default_accessor<TypeParam volatile>, accessor_type>::value,
      "`default_accessor<cv-qualified TypeParam>` must be nothrow constructible from `default_accessor<TypeParam>`"
  );
  static_assert(
      std::is_nothrow_constructible<default_accessor<TypeParam const volatile>, accessor_type>::value,
      "`default_accessor<cv-qualified TypeParam>` must be nothrow constructible from `default_accessor<TypeParam>`"
  );
}

/// @test @c default_accessor is not constructible from a different
/// @c default_accessor with a convertible @c ElementType
TYPED_TEST(DefaultAccessor, NotConvertibleFromOther) {
  using accessor_type = arene::base::default_accessor<TypeParam>;
  using other_accessor_type = arene::base::default_accessor<test::implicitly_constructible_from<TypeParam>>;

  static_assert(
      !std::is_constructible<other_accessor_type, accessor_type>::value,
      "`default_accessor` where the element type is implicitly constructible from `TypeParam` must _not_ be "
      "constructible from `default_accessor<TypeParam>`"
  );
}

// NOLINTBEGIN(hicpp-no-array-decay,cppcoreguidelines-pro-bounds-pointer-arithmetic)

/// @test @c default_accessor::access returns a reference to the element at the specifed index and is always nothrow
/// invocable
CONSTEXPR_TYPED_TEST(DefaultAccessor, Access) {
  using accessor_type = arene::base::default_accessor<TypeParam>;
  auto const accessor = accessor_type{};

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam values[3] = {};

  CONSTEXPR_ASSERT(&values[0] == &accessor.access(values, 0U));
  CONSTEXPR_ASSERT(&values[1] == &accessor.access(values, 1U));
  CONSTEXPR_ASSERT(&values[2] == &accessor.access(values, 2U));
  CONSTEXPR_ASSERT(&values[1] == &accessor.access(values + 1, 0U));
  CONSTEXPR_ASSERT(&values[2] == &accessor.access(values + 1, 1U));
  CONSTEXPR_ASSERT(&values[2] == &accessor.access(values + 2, 0U));

  static_assert(
      std::is_same<typename accessor_type::reference, decltype(accessor.access(values, 0U))>::value,
      "`default_accessor<>::reference` must match the return type of `default_accessor<>::access()`"
  );
  static_assert(
      noexcept(accessor.access(values, 0U)),  //
      "`default_accessor<>::access()` must always be `noexcept`"
  );
}

/// @test @c default_accessor::offset returns a pointer offset from the intial pointer and is always nothrow invocable
CONSTEXPR_TYPED_TEST(DefaultAccessor, Offset) {
  using accessor_type = arene::base::default_accessor<TypeParam>;
  auto const accessor = accessor_type{};

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam values[3] = {};

  CONSTEXPR_ASSERT(&values[0] == accessor.offset(values, 0U));
  CONSTEXPR_ASSERT(&values[1] == accessor.offset(values, 1U));
  CONSTEXPR_ASSERT(&values[2] == accessor.offset(values, 2U));
  CONSTEXPR_ASSERT(&values[1] == accessor.offset(values + 1, 0U));
  CONSTEXPR_ASSERT(&values[2] == accessor.offset(values + 1, 1U));
  CONSTEXPR_ASSERT(&values[2] == accessor.offset(values + 2, 0U));

  static_assert(
      std::is_same<typename accessor_type::data_handle_type, decltype(accessor.offset(values, 0U))>::value,
      "`default_accessor<>::data_handle_type` must match the return type of `default_accessor<>::offset()`"
  );
  static_assert(
      noexcept(accessor.offset(values, 0U)),  //
      "`default_accessor<>::offset()` must always be `noexcept`"
  );
}

// NOLINTEND(hicpp-no-array-decay,cppcoreguidelines-pro-bounds-pointer-arithmetic)

}  // namespace
