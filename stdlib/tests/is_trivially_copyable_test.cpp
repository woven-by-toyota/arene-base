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
constexpr auto check_is_trivially_copyable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_trivially_copyable<T>, std::bool_constant<Expected>> &&
         (std::is_trivially_copyable_v<T> == Expected);
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class deleted_copy {
 public:
  deleted_copy(deleted_copy const&) = delete;
  auto operator=(deleted_copy const&) -> deleted_copy& = delete;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_copyable {
 private:
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  not_copyable(not_copyable const&);
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  auto operator=(not_copyable const&) -> not_copyable&;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_destructor {
 public:
  ~non_trivial_destructor();
};

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor)
class non_trivial_virtual_function {
 public:
  virtual auto foo() -> void;
};

class non_trivial_virtual_base : virtual testing::class_type {};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_copy_constructor {
 public:
  non_trivial_copy_constructor(non_trivial_copy_constructor const&);
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_move_constructor {
 public:
  non_trivial_move_constructor(non_trivial_move_constructor&&) noexcept;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_copy_assignment {
 public:
  auto operator=(non_trivial_copy_assignment const&) -> non_trivial_copy_assignment&;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_move_assignment {
 public:
  auto operator=(non_trivial_move_assignment&&) noexcept -> non_trivial_move_assignment&;
};

class non_trivial_base_class_default : non_trivial_copy_constructor {};

struct non_trivial_member_default {
  non_trivial_copy_constructor dummy;
};

class trivial_defaulted_special_members {
 public:
  trivial_defaulted_special_members(trivial_defaulted_special_members const&) = default;
  trivial_defaulted_special_members(trivial_defaulted_special_members&&) = default;
  auto operator=(trivial_defaulted_special_members const&) -> trivial_defaulted_special_members& = default;
  auto operator=(trivial_defaulted_special_members&&) -> trivial_defaulted_special_members& = default;
  ~trivial_defaulted_special_members() = default;
};

class trivial_base_class_default : trivial_defaulted_special_members {};

struct trivial_member_default {
  trivial_defaulted_special_members dummy;
};

using not_copyable_types =
    arene::base::type_lists::concat_unique_t<::testing::reference_types, ::testing::Types<void, not_copyable>>;

template <typename T>
class NotCopyableTest : public testing::Test {};

TYPED_TEST_SUITE(NotCopyableTest, not_copyable_types, );

/// @test A type is not trivially copyable
TYPED_TEST(NotCopyableTest, IsNotCopyable) {
  static_assert(check_is_trivially_copyable<false, TypeParam>(), "A type is not trivially copyable");
}

/// @test An array of not-default-constructible types is not trivially copyable
TEST(NotCopyableTest, CannotConstructArray) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  static_assert(check_is_trivially_copyable<false, not_copyable[3]>(), "An array is not trivially copyable");
  // NOLINTEND(hicpp-avoid-c-arrays)
}

using non_trivial_copyable_types = testing::Types<
    non_trivial_destructor,
    non_trivial_copy_constructor,
    non_trivial_move_constructor,
    non_trivial_copy_assignment,
    non_trivial_move_assignment,
    non_trivial_base_class_default,
    non_trivial_member_default,
    non_trivial_virtual_function,
    non_trivial_virtual_base>;

template <typename T>
class NonTrivialCopyableTest : public testing::Test {};

TYPED_TEST_SUITE(NonTrivialCopyableTest, non_trivial_copyable_types, );

/// @test A type is copyable but not trivially copyable
TYPED_TEST(NonTrivialCopyableTest, CopyableNotTriviallyCopyable) {
  static_assert(check_is_trivially_copyable<false, TypeParam>(), "A type is not trivially copyable");
}

using trivial_copyable_types = arene::base::type_lists::concat_unique_t<
    testing::scalar_types,
    testing::union_types,
    testing::class_types,
    testing::Types<
        int const,
        int volatile,
        testing::class_type volatile,
        testing::class_type const,
        deleted_copy,
        trivial_defaulted_special_members,
        trivial_base_class_default,
        trivial_member_default,
        // NOLINTBEGIN(hicpp-avoid-c-arrays)
        deleted_copy[3],
        int[3],
        int const[3][6][10],
        int[],
        ::testing::class_type[][3][6],
        trivial_defaulted_special_members[3],
        trivial_defaulted_special_members[]>>;
// NOLINTEND(hicpp-avoid-c-arrays)

template <typename T>
class TrivialCopyableTest : public testing::Test {};

TYPED_TEST_SUITE(TrivialCopyableTest, trivial_copyable_types, );

/// @test A type is trivially copyable
TYPED_TEST(TrivialCopyableTest, CopyableNotTriviallyCopyable) {
  static_assert(check_is_trivially_copyable<true, TypeParam>(), "A type is trivially copyable");
}

}  // namespace
