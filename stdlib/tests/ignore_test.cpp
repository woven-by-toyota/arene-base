// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/tuple"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {
/// @test Ensure @c ignore is a @c const value
TEST(Ignore, IgnoreIsAValueOfTypeIgnoreT) {
  ::testing::StaticAssertTypeEq<decltype(std::ignore), std::ignore_detail::ignore_t const&>();
}

/// @brief A simple user-defined type
// NOLINTNEXTLINE(hicpp-special-member-functions)
class some_user_type {
 public:
  explicit constexpr some_user_type(int) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  some_user_type(some_user_type const&) noexcept(false) {}
};

using ignore_assign_types = ::testing::Types<
    char,
    signed char,
    unsigned char,
    short,           // NOLINT
    unsigned short,  // NOLINT
    int,
    unsigned,
    long,                // NOLINT
    unsigned long,       // NOLINT
    long long,           // NOLINT
    unsigned long long,  // NOLINT
    float,
    double,
    some_user_type>;

template <typename T>
class IgnoreAssignment : public testing::Test {};

TYPED_TEST_SUITE(IgnoreAssignment, ignore_assign_types, );

/// @test @c ignore can be assigned to from any type
TYPED_TEST(IgnoreAssignment, CanAssignToIgnoreFromAnyType) {
  TypeParam const val{42};
  auto& result = std::ignore = val;
  testing::StaticAssertTypeEq<decltype(result), std::ignore_detail::ignore_t const&>();
  ASSERT_EQ(&std::ignore, &result);
}

template <typename T>
constexpr auto do_assign_to_ignore(T const& val) noexcept -> bool {
  std::ignore = val;
  return true;
}

/// @test @c ignore can be assigned to from any type in a constant expression
TYPED_TEST(IgnoreAssignment, CanAssignToIgnoreFromAnyTypeInConstexpr) {
  constexpr TypeParam val{42};
  static_assert(do_assign_to_ignore(val), "Assignment should work in constexpr");
}

/// @test Assigning to @c ignore is noexcept
TYPED_TEST(IgnoreAssignment, AssignmentIsNoexcept) {
  TypeParam const val{42};
  static_assert(noexcept(std::ignore = val), "Assignment to ignore must be noexcept");
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class tracked {
  mutable unsigned copies_ = 0;

 public:
  auto get_copies() const noexcept -> unsigned { return copies_; }

  tracked() = default;
  tracked(tracked const& other) { ++other.copies_; }
  auto operator=(tracked const& other) noexcept -> tracked& {
    ++other.copies_;
    return *this;
  }
};

/// @test Assigning to @c ignore does not copy or move the source
TEST(Ignore, AssignDoesNotCopySource) {
  tracked const source{};
  ASSERT_EQ(source.get_copies(), 0);
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  tracked const copy{source};
  ASSERT_EQ(source.get_copies(), 1);
  ASSERT_EQ(copy.get_copies(), 0);

  std::ignore = source;

  ASSERT_EQ(source.get_copies(), 1);

  // NOLINTNEXTLINE(hicpp-move-const-arg)
  std::ignore = std::move(source);

  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_EQ(source.get_copies(), 1);

  // NOLINTNEXTLINE(misc-const-correctness)
  tracked non_const_source{};

  std::ignore = non_const_source;

  ASSERT_EQ(non_const_source.get_copies(), 0);

  // NOLINTNEXTLINE(hicpp-move-const-arg)
  std::ignore = std::move(non_const_source);

  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_EQ(non_const_source.get_copies(), 0);
}
}  // namespace
