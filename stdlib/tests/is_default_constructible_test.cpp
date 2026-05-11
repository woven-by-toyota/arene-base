// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
namespace {

using testing::is_unambiguously_publicly_derived_from_v;

template <bool Expected, class T>
constexpr auto check_is_default_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_default_constructible<T>, std::bool_constant<Expected>> &&
         (std::is_default_constructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_nothrow_default_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<
             std::is_nothrow_default_constructible<T>,
             std::bool_constant<Expected>> &&
         (std::is_nothrow_default_constructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_trivially_default_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<
             std::is_trivially_default_constructible<T>,
             std::bool_constant<Expected>> &&
         (std::is_trivially_default_constructible_v<T> == Expected);
}

class some_user_type {};

class constructible_with_args {
 public:
  constructible_with_args() noexcept;
  explicit constructible_with_args(int) noexcept;
  explicit constructible_with_args(some_user_type&, int, double) noexcept;
};

class not_default_constructible {
 public:
  explicit not_default_constructible(int) noexcept;
};

class throwing_default_constructible {
 public:
  throwing_default_constructible() noexcept(false);
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class trivial_defaulted_default {
 public:
  trivial_defaulted_default() = default;
};

class trivial_base_class_default : trivial_defaulted_default {};

struct trivial_member_default {
  trivial_defaulted_default dummy;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_default {
 public:
  non_trivial_default() noexcept;
};

class non_trivial_base_class_default : non_trivial_default {};

struct non_trivial_member_default {
  non_trivial_default dummy;
};

class requires_arg {
 public:
  requires_arg(int, int) noexcept {}
};

struct member_with_brace_init {
  requires_arg member{1, 2};
};

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor)
class non_trivial_virtual_function {
 public:
  virtual auto foo() -> void;
};

class non_trivial_virtual_base : virtual testing::class_type {};

/// @test A type is not constructible from arguments that don't match the constructors
TEST(DefaultConstructible, ThrowingDefaultConstructibleType) {
  static_assert(
      check_is_default_constructible<true, throwing_default_constructible>(),
      "Should be default constructible"
  );
  static_assert(
      check_is_nothrow_default_constructible<false, throwing_default_constructible>(),
      "Should not be nothrow default constructible"
  );
}

using nothrow_default_constructible_types = arene::base::type_lists::concat_unique_t<
    ::testing::object_types,
    ::testing::pointer_types,
    ::testing::member_pointer_types,
    ::testing::Types<
        some_user_type,
        constructible_with_args,
        member_with_brace_init,
        non_trivial_virtual_base,
        non_trivial_virtual_function>>;

template <typename T>
class DefaultConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(DefaultConstructibleTest, nothrow_default_constructible_types, );

/// @test A type is default constructible
TYPED_TEST(DefaultConstructibleTest, IsDefaultConstructible) {
  static_assert(check_is_default_constructible<true, TypeParam>(), "A type is default constructible");
  static_assert(check_is_nothrow_default_constructible<true, TypeParam>(), "A type is nothrow default constructible");
}

/// @test An array of default-constructible types is default constructible
TYPED_TEST(DefaultConstructibleTest, CanConstructArray) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  static_assert(check_is_default_constructible<true, TypeParam[3]>(), "An array is (nothrow) default constructible");
  static_assert(
      check_is_nothrow_default_constructible<true, TypeParam[3]>(),
      "An array is (nothrow) default constructible"
  );
  // NOLINTEND(hicpp-avoid-c-arrays)
}

using not_default_constructible_types = arene::base::type_lists::
    concat_unique_t<::testing::reference_types, ::testing::Types<void, not_default_constructible>>;

template <typename T>
class NotDefaultConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(NotDefaultConstructibleTest, not_default_constructible_types, );

/// @test A type is not default constructible
TYPED_TEST(NotDefaultConstructibleTest, IsNotDefaultConstructible) {
  static_assert(check_is_default_constructible<false, TypeParam>(), "A type is not default constructible");
  static_assert(
      check_is_nothrow_default_constructible<false, TypeParam>(),
      "A type is not nothrow default constructible"
  );
}

/// @test An array of not-default-constructible types is not default constructible
TEST(NotDefaultConstructibleTest, CannotConstructArray) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  static_assert(
      check_is_default_constructible<false, not_default_constructible[3]>(),
      "An array is not default constructible"
  );
  static_assert(
      check_is_nothrow_default_constructible<false, not_default_constructible[3]>(),
      "An array is not default constructible"
  );
  // NOLINTEND(hicpp-avoid-c-arrays)
}

using non_trivial_default_constructible_types = testing::Types<
    non_trivial_default,
    non_trivial_base_class_default,
    non_trivial_member_default,
    non_trivial_virtual_base,
    non_trivial_virtual_function,
    member_with_brace_init>;

template <typename T>
class NonTrivialDefaultConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(NonTrivialDefaultConstructibleTest, non_trivial_default_constructible_types, );

/// @test A type is default constructible but not trivially default constructible
TYPED_TEST(NonTrivialDefaultConstructibleTest, DefaultConstructibleNotTriviallyDefaultConstructible) {
  static_assert(check_is_default_constructible<true, TypeParam>(), "A type is not default constructible");
  static_assert(
      check_is_trivially_default_constructible<false, TypeParam>(),
      "A type is not trivially default constructible"
  );
}

using trivial_default_constructible_types = arene::base::type_lists::concat_unique_t<
    testing::scalar_types,
    testing::union_types,
    testing::class_types,
    testing::Types<trivial_defaulted_default, trivial_base_class_default, trivial_member_default>>;

template <typename T>
class TrivialDefaultConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(TrivialDefaultConstructibleTest, trivial_default_constructible_types, );

/// @test A type is trivially default constructible
TYPED_TEST(TrivialDefaultConstructibleTest, DefaultConstructibleNotTriviallyDefaultConstructible) {
  static_assert(check_is_default_constructible<true, TypeParam>(), "A type is default constructible");
  static_assert(
      check_is_trivially_default_constructible<true, TypeParam>(),
      "A type is trivially default constructible"
  );
}

}  // namespace
