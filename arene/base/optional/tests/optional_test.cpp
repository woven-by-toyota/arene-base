// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/optional/optional.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/swap.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/utility/in_place.hpp"
#include "arene/base/utility/swap.hpp"
#include "testlibs/utilities/has_broken_less_than.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <vector>
#endif

namespace {

using ::arene::base::in_place;
using ::arene::base::null_opt;
using ::arene::base::null_opt_t;
using ::arene::base::nullopt;
using ::arene::base::nullopt_t;
using ::arene::base::optional;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly testing global interaction
struct instance_tracker {
  static std::uint32_t instance_count;

  instance_tracker() { ++instance_count; }
  instance_tracker(instance_tracker const&) { ++instance_count; }
  instance_tracker(instance_tracker&&) noexcept { ++instance_count; }

  ~instance_tracker() { --instance_count; }

  auto operator=(instance_tracker&&) -> instance_tracker& = default;
  auto operator=(instance_tracker const&) -> instance_tracker& = default;
};
std::uint32_t instance_tracker::instance_count = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

constexpr short s42 = 42;    // NOLINT
constexpr short s123 = 123;  // NOLINT

template <std::int32_t = 0>
auto function_template_with_local_constexpr_optional() -> bool {
  ARENE_MAYBE_UNUSED constexpr arene::base::optional<int> var{};
  return true;
}

/// @test Verify that a function template with a constexpr local variable of optional type can be called
// This must be the first test in the file, to ensure that a gcc 8 compiler bug is triggered if the workaround is not
// working
TEST(Optional, CanUseAsConstexprVar) { ASSERT_TRUE(function_template_with_local_constexpr_optional()); }

/// @test Default-constructing an `optional` is empty regardless of the type it may hold. The constructor is `noexcept`,
/// `has_value()` returns `false`, and the `optional` evaluates to `false`.
TEST(Optional, DefaultConstructedOptionalIsEmpty) {
  constexpr optional<int> opt_int;

  STATIC_ASSERT_TRUE(noexcept(optional<int>()));

  STATIC_ASSERT_FALSE(opt_int.has_value());
  STATIC_ASSERT_FALSE(opt_int);

  optional<instance_tracker> const opt_it;

  static_assert(noexcept(optional<instance_tracker>()), "Default constructor must be noexcept");

  EXPECT_FALSE(opt_it.has_value());
  EXPECT_FALSE(opt_it);
  EXPECT_EQ(instance_tracker::instance_count, 0);
}

/// @test An `optional` can be constructed from a value of the type it may hold. The constructor is `noexcept` if the
/// held type's constructor is `noexcept`. `has_value()` returns `true` and the `optional` evaluates to `true`. The held
/// value is equal to the value passed to the constructor. A single instance is held by the `optional`.
TEST(Optional, CanConstructFromValue) {
  constexpr std::int32_t int_val = 42;
  constexpr optional<int> opt_int(int_val);

  STATIC_ASSERT_TRUE(noexcept(optional<int>(int_val)));

  STATIC_ASSERT_TRUE(opt_int.has_value());
  STATIC_ASSERT_TRUE(opt_int);
  STATIC_ASSERT_EQ(*opt_int, int_val);

  arene::base::string_view const str_val = "hello";
  optional<arene::base::string_view> opt_str(str_val);

  STATIC_ASSERT_TRUE(noexcept(optional<arene::base::string_view>(str_val)));

  ASSERT_TRUE(opt_str.has_value());
  ASSERT_TRUE(opt_str);
  ASSERT_EQ(*opt_str, str_val);

  {
    optional<instance_tracker> const local_opt_str(instance_tracker{});
    ASSERT_EQ(instance_tracker::instance_count, 1);

    auto tracker_copy = instance_tracker{};
    STATIC_ASSERT_FALSE(noexcept(optional<instance_tracker>(tracker_copy)));

    ASSERT_TRUE(local_opt_str.has_value());
    ASSERT_TRUE(local_opt_str);
  }
  ASSERT_EQ(instance_tracker::instance_count, 0);
}

/// @test An `optional` held value is constructed in-place when constructing the `optional`.
TEST(Optional, ConstructionIsInPlace) {
  constexpr std::int32_t initializer = 42;

  struct non_movable {
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    non_movable(int value) noexcept
        : value_(value) {}

    non_movable(non_movable const&) = delete;
    auto operator=(non_movable const&) -> non_movable& = delete;

    non_movable(non_movable&&) = delete;
    auto operator=(non_movable&&) -> non_movable& = delete;

    ~non_movable() = default;

    std::int32_t value_;
  };

  optional<non_movable> onm(initializer);

  STATIC_ASSERT_TRUE(noexcept(optional<non_movable>(initializer)));

  ASSERT_TRUE(onm.has_value());
  ASSERT_TRUE(onm);
  ASSERT_EQ(onm->value_, initializer);
}

/// @test An `optional` can be copy-constructed and the copy of the `optional` holds a copy of the original value.
TEST(Optional, CopyingCopiesTheValue) {
  constexpr std::uint32_t u_val = 99;

  constexpr optional<unsigned> opt_unsigned(u_val);
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  constexpr optional<unsigned> opt_unsigned2(opt_unsigned);

  STATIC_ASSERT_TRUE(opt_unsigned2.has_value());
  STATIC_ASSERT_TRUE(opt_unsigned2);
  STATIC_ASSERT_EQ(*opt_unsigned2, u_val);

  char const* const hello = "hello";
  optional<arene::base::string_view> const opt_str(hello);
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  optional<arene::base::string_view> const opt_str2(opt_str);

  ASSERT_TRUE(opt_str2.has_value());
  ASSERT_TRUE(opt_str2);
  ASSERT_EQ(*opt_str2, hello);

  optional<arene::base::string_view> opt_str3(opt_str2);

  ASSERT_TRUE(opt_str3.has_value());
  ASSERT_TRUE(opt_str3);
  ASSERT_EQ(*opt_str3, hello);

  optional<instance_tracker> const opt_it(instance_tracker{});
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  optional<instance_tracker> const opt_it2(opt_it);

  ASSERT_EQ(instance_tracker::instance_count, 2);
}

namespace {
template <typename Tag>
struct convertible_instance {
  std::uint32_t val;

  constexpr explicit convertible_instance(std::uint32_t that_val)
      : val(that_val) {}

  template <typename OtherTag>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr convertible_instance(convertible_instance<OtherTag> opt_ither)
      : val(opt_ither.val) {}
};
}  // namespace

/// @test An `optional` holding a given type can be constructed from an `optional` holding a different type provided the
/// original type is convertible to the new type.
TEST(Optional, CanCopyOptionalsOfOtherType) {
  constexpr std::uint32_t u_val = 99;

  struct tag1 {};
  struct tag2 {};

  constexpr optional<convertible_instance<tag1>> opt_unsigned(in_place, u_val);
  constexpr optional<convertible_instance<tag2>> opt_unsigned2(opt_unsigned);

  STATIC_ASSERT_TRUE(opt_unsigned2.has_value());
  STATIC_ASSERT_TRUE(opt_unsigned2);
  STATIC_ASSERT_EQ(opt_unsigned2->val, u_val);

  optional<convertible_instance<tag2>> opt_unsigned3(opt_unsigned);
  ASSERT_TRUE(opt_unsigned3.has_value());
  ASSERT_TRUE(opt_unsigned);
  ASSERT_TRUE(opt_unsigned3);
  ASSERT_EQ(opt_unsigned3->val, u_val);

  optional<convertible_instance<tag1>> opt_unsigned4(opt_unsigned3);
  ASSERT_TRUE(opt_unsigned4.has_value());
  ASSERT_TRUE(opt_unsigned3);
  ASSERT_TRUE(opt_unsigned4);
  ASSERT_EQ(opt_unsigned4->val, u_val);

  optional<convertible_instance<tag1>> const opt_unsigned5{};
  optional<convertible_instance<tag2>> const opt_unsigned6(opt_unsigned5);
  ASSERT_FALSE(opt_unsigned6);
}

/// @test A destination `optional` can be move-constructed from a source `optional` holding a value. The destination
/// `optional` holds the value the source did. The source `optional` is still _engaged_. If the source `optional` is
/// const, the value it holds is not affected.
TEST(Optional, MovingLeavesTheSourceEngaged) {
  constexpr std::uint32_t u_val = 99;

  optional<unsigned> opt_unsigned(u_val);
  optional<unsigned> opt_unsigned2(std::move(opt_unsigned));

  ASSERT_TRUE(opt_unsigned2.has_value());
  ASSERT_TRUE(opt_unsigned2);
  ASSERT_EQ(*opt_unsigned2, u_val);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_TRUE(opt_unsigned.has_value());
  ASSERT_TRUE(opt_unsigned);

  char const* const hello = "hello";
  optional<arene::base::string_view> opt_str(hello);
  optional<arene::base::string_view> const opt_str2(std::move(opt_str));

  ASSERT_TRUE(opt_str2.has_value());
  ASSERT_TRUE(opt_str2);
  ASSERT_EQ(*opt_str2, hello);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_TRUE(opt_str.has_value());
  ASSERT_TRUE(opt_str);

  // NOLINTNEXTLINE(hicpp-move-const-arg,performance-move-const-arg)
  optional<arene::base::string_view> opt_str3(std::move(opt_str2));
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_TRUE(opt_str2.has_value());
  ASSERT_TRUE(opt_str2);
  ASSERT_EQ(*opt_str2, hello);
  ASSERT_TRUE(opt_str3.has_value());
  ASSERT_TRUE(opt_str3);
  ASSERT_EQ(*opt_str3, hello);

  optional<instance_tracker> opt_it(instance_tracker{});
  optional<instance_tracker> const opt_it2(std::move(opt_it));
  ASSERT_TRUE(opt_it2.has_value());
  ASSERT_TRUE(opt_it2);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_TRUE(opt_it.has_value());
  ASSERT_TRUE(opt_it);

  ASSERT_EQ(instance_tracker::instance_count, 2);
}

/// @test An `optional` can be move-constructed from an empty `optional`, having both of them _disengaged_, regardless
/// of the constness of the source.
TEST(Optional, MovingEmptyLeavesEmpty) {
  optional<int> opt_int{};
  optional<int> const opt_int2(std::move(opt_int));

  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_FALSE(opt_int);
  ASSERT_FALSE(opt_int2);

  // NOLINTNEXTLINE(hicpp-move-const-arg,performance-move-const-arg)
  optional<int> const opt_int3(std::move(opt_int2));
  ASSERT_FALSE(opt_int3);
}

/// @test `optional` can be move-constructed with move-only types.
TEST(Optional, CanMoveMoveOnlyTypes) {
  struct move_only {
    move_only() = default;

    move_only(move_only&&) noexcept = default;
    auto operator=(move_only&&) noexcept -> move_only& = default;

    move_only(move_only const&) = delete;
    auto operator=(move_only const&) -> move_only& = delete;
    ~move_only() = default;
  };

  STATIC_ASSERT_TRUE(std::is_move_constructible<optional<move_only>>::value);
  STATIC_ASSERT_FALSE(std::is_copy_constructible<optional<move_only>>::value);

  optional<move_only> opt_mo(move_only{});
  optional<move_only> const opt_mo2(std::move(opt_mo));

  struct move_only_non_trivial {
    move_only_non_trivial() = default;
    explicit move_only_non_trivial(char const* val)
        : data(val) {}

    // NOLINTBEGIN(bugprone-exception-escape) clang tidy false positives here
    move_only_non_trivial(move_only_non_trivial&&) noexcept = default;
    auto operator=(move_only_non_trivial&&) noexcept -> move_only_non_trivial& = default;
    // NOLINTEND(bugprone-exception-escape) clang tidy false positives here

    move_only_non_trivial(move_only_non_trivial const&) = delete;
    auto operator=(move_only_non_trivial const&) -> move_only& = delete;
    ~move_only_non_trivial() = default;

    arene::base::string_view data;
  };

  STATIC_ASSERT_TRUE(std::is_move_constructible<optional<move_only_non_trivial>>::value);
  STATIC_ASSERT_FALSE(std::is_copy_constructible<optional<move_only_non_trivial>>::value);

  auto const* const original_data = "foo";
  optional<move_only_non_trivial> mont(move_only_non_trivial{original_data});
  optional<move_only_non_trivial> const mont2(std::move(mont));
  ASSERT_TRUE(mont2);
  EXPECT_EQ(mont2->data, original_data);
}

/// @test `optional` can be reset with '{}'
TEST(Optional, CanResetWithBraces) {
  auto opt1 = optional<int>{42};
  opt1 = {};
  ASSERT_FALSE(opt1);

  auto opt2 = optional<int>{};
  opt2 = {};
  ASSERT_FALSE(opt2);
}

/// @test A destination `optional` can be copy-assigned a source `optional` regardless of whether the source holds a
/// value and regardless of the source constness. When the source is empty, the destination is also empty. When the
/// source holds a value, the destination holds a copy of the source value. The source is left unchanged.
TEST(Optional, AssignmentCopiesTheValue) {
  constexpr std::uint32_t u_val = 99;

  optional<unsigned> const opt_unsigned(u_val);
  optional<unsigned> opt_unsigned2;

  opt_unsigned2 = opt_unsigned;

  ASSERT_TRUE(opt_unsigned2.has_value());
  ASSERT_TRUE(opt_unsigned2);
  ASSERT_EQ(*opt_unsigned2, u_val);

  char const* const hello = "hello";
  optional<arene::base::string_view> const opt_str(hello);
  optional<arene::base::string_view> opt_str2;

  opt_str2 = opt_str;

  ASSERT_TRUE(opt_str2.has_value());
  ASSERT_TRUE(opt_str2);
  ASSERT_EQ(*opt_str2, hello);

  optional<arene::base::string_view> opt_str3;
  opt_str3 = opt_str2;

  ASSERT_TRUE(opt_str3.has_value());
  ASSERT_TRUE(opt_str3);
  ASSERT_EQ(*opt_str3, hello);

  opt_str3 = optional<arene::base::string_view>{};
  ASSERT_FALSE(opt_str3.has_value());
  ASSERT_FALSE(opt_str3);

  optional<arene::base::string_view> opt_str4(opt_str2);
  opt_str4 = opt_str3;
  ASSERT_FALSE(opt_str4.has_value());
  ASSERT_FALSE(opt_str4);

  opt_str4 = opt_str2;
  ASSERT_TRUE(opt_str2.has_value());
  ASSERT_TRUE(opt_str2);
  ASSERT_EQ(*opt_str2, hello);
  opt_str4 = static_cast<optional<arene::base::string_view> const&>(opt_str3);
  ASSERT_FALSE(opt_str4.has_value());
  ASSERT_FALSE(opt_str4);

  optional<instance_tracker> const opt_it(instance_tracker{});
  optional<instance_tracker> opt_it2;
  opt_it2 = opt_it;

  ASSERT_EQ(instance_tracker::instance_count, 2);
}

struct AssignmentTracker {
  bool assigned_to = false;
  mutable bool assigned_from_const = false;
  bool move_assigned_from = false;
  bool move_constructed = false;

  AssignmentTracker() = default;
  ~AssignmentTracker() = default;
  AssignmentTracker(AssignmentTracker const&) = default;
  AssignmentTracker(AssignmentTracker&&) noexcept
      : move_constructed(true) {}

  auto operator=(AssignmentTracker const& opt_ither) noexcept -> AssignmentTracker& {
    assigned_to = true;
    opt_ither.assigned_from_const = true;
    return *this;
  }
  auto operator=(AssignmentTracker&& opt_ither) noexcept -> AssignmentTracker& {
    assigned_to = true;
    opt_ither.move_assigned_from = true;
    return *this;
  }
};

/// @test A destination `optional` can be copy-assigned a source `optional` that holds a value. When the destination
/// `optional` is empty, the copy-constructor is used. When the destination `optional` already holds a value, the
/// copy-assignment is used. A destination `optional` can be move-assigned a source `optional` that holds a value. When
/// the destination `optional` is empty, the move-constructor is used. When the destination `optional` already holds a
/// value, the move-assignment is used.
TEST(Optional, AssignmentFromEngagedToEngagedAssigns) {
  optional<AssignmentTracker> oat(AssignmentTracker{});
  optional<AssignmentTracker> oat2;

  oat2 = oat;
  EXPECT_FALSE(oat->assigned_from_const);
  EXPECT_FALSE(oat->move_assigned_from);
  EXPECT_FALSE(oat->assigned_to);
  EXPECT_FALSE(oat2->assigned_to);
  EXPECT_FALSE(oat2->assigned_from_const);
  EXPECT_FALSE(oat2->move_assigned_from);

  oat2 = oat;

  EXPECT_TRUE(oat->assigned_from_const);
  EXPECT_FALSE(oat->move_assigned_from);
  EXPECT_FALSE(oat->assigned_to);
  EXPECT_TRUE(oat2->assigned_to);
  EXPECT_FALSE(oat2->assigned_from_const);
  EXPECT_FALSE(oat2->move_assigned_from);

  optional<AssignmentTracker> const oat3(AssignmentTracker{});

  oat2.reset();
  oat2 = oat3;
  EXPECT_FALSE(oat3->assigned_from_const);
  EXPECT_FALSE(oat3->move_assigned_from);
  EXPECT_FALSE(oat3->assigned_to);
  EXPECT_FALSE(oat2->assigned_to);
  EXPECT_FALSE(oat2->assigned_from_const);
  EXPECT_FALSE(oat2->move_assigned_from);

  oat2 = oat3;

  EXPECT_TRUE(oat3->assigned_from_const);
  EXPECT_FALSE(oat3->move_assigned_from);
  EXPECT_FALSE(oat3->assigned_to);
  EXPECT_TRUE(oat2->assigned_to);
  EXPECT_FALSE(oat2->assigned_from_const);
  EXPECT_FALSE(oat2->move_assigned_from);

  optional<AssignmentTracker> oat4(AssignmentTracker{});

  oat2.reset();
  oat2 = oat4;
  EXPECT_FALSE(oat4->assigned_from_const);
  EXPECT_FALSE(oat4->move_assigned_from);
  EXPECT_FALSE(oat4->assigned_to);
  EXPECT_FALSE(oat2->assigned_to);
  EXPECT_FALSE(oat2->assigned_from_const);
  EXPECT_FALSE(oat2->move_assigned_from);

  oat2 = std::move(oat4);

  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  EXPECT_FALSE(oat4->assigned_from_const);
  EXPECT_TRUE(oat4->move_assigned_from);
  EXPECT_FALSE(oat4->assigned_to);
  EXPECT_TRUE(oat2->assigned_to);
  EXPECT_FALSE(oat2->assigned_from_const);
  EXPECT_FALSE(oat2->move_assigned_from);

  optional<AssignmentTracker> oat5(AssignmentTracker{});

  oat2.reset();
  oat2 = std::move(oat5);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  EXPECT_FALSE(oat5->assigned_from_const);
  EXPECT_FALSE(oat5->move_assigned_from);
  EXPECT_FALSE(oat5->assigned_to);
  EXPECT_FALSE(oat2->assigned_to);
  EXPECT_FALSE(oat2->assigned_from_const);
  EXPECT_FALSE(oat2->move_assigned_from);
  EXPECT_TRUE(oat2->move_constructed);
}

/// @test An empty destination `optional` can be assigned an empty source `optional`, regardless the source `optional`
/// is const (copy-assigned) or is an xvalue (move-assigned).
TEST(Optional, AssignmentEmptyToEmptySucceeds) {
  optional<arene::base::string_view> const const_opt_ither{};
  optional<arene::base::string_view> const_assigned{};

  const_assigned = const_opt_ither;

  optional<arene::base::string_view> xvalue_assigned{};
  xvalue_assigned = optional<arene::base::string_view>{};
}

/// @test `optional`'s dereference operator member function returns a reference to the held value.
/// `optional`'s member access operator accesses a pointer to the value in the optional. Ensure correct access to the
/// held value.
TEST(Optional, PoptIntnterDereference) {
  struct simple {
    std::int32_t value = 42;
  };

  constexpr optional<simple> opt_sample(simple{});

  constexpr std::int32_t first = opt_sample->value;
  constexpr std::int32_t second = (*opt_sample).value;
  ASSERT_EQ(&*opt_sample, opt_sample.operator->());
  ASSERT_EQ(first, 42);
  ASSERT_EQ(second, 42);
  ASSERT_EQ(opt_sample->value, 42);

  optional<simple> opt_sample_2(opt_sample);
  opt_sample_2->value = 99;
  ++(*opt_sample_2).value;
  ASSERT_EQ(opt_sample_2->value, 100);
}

/// @test Accessing an rvalue reference of an `optional` returns an rvalue reference for the held value, with the
/// corresponding constness.
/// `value()` properly returns the held value when called on an rvalue const or non-const `optional`.
TEST(Optional, RValueAccess) {
  STATIC_ASSERT_TRUE((std::is_same<
                      decltype(*std::move(std::declval<optional<arene::base::string_view>&&>())),
                      arene::base::string_view&&>::value));
  STATIC_ASSERT_TRUE((std::is_same<
                      decltype(*std::declval<optional<arene::base::string_view> const&&>()),
                      arene::base::string_view const&&>::value));

  auto const* const msg = "hello";
  optional<arene::base::string_view> opt{msg};
  arene::base::string_view const moved = (*std::move(opt));
  ASSERT_EQ(moved, msg);

  optional<arene::base::string_view> const copt{msg};
  // NOLINTNEXTLINE(hicpp-move-const-arg) intentionally testing const&& behavior
  arene::base::string_view const cmoved = (*std::move(copt));
  std::ignore = cmoved;
  ASSERT_EQ(moved, msg);
}

/// @test `optional`'s `value_or()` method properly returns the `optional` held value if any or the provided default if
/// the `optional` is empty.
TEST(Optional, ValueOr) {
  struct wrapper {
    std::int32_t value;
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    constexpr wrapper(int val)
        : value(val) {}
  };

  constexpr optional<int> opt_int;
  constexpr optional<int> opt_int2(99);

  constexpr std::int32_t val1 = opt_int.value_or(42);
  constexpr std::int32_t val2 = opt_int2.value_or(42);
  ASSERT_EQ(val1, 42);
  ASSERT_EQ(val2, 99);

  constexpr optional<wrapper> opt_w;
  constexpr optional<wrapper> opt_w2(99);

  constexpr wrapper wrap = opt_w.value_or(42);
  constexpr wrapper wrap2 = opt_w2.value_or(42);
  ASSERT_EQ(wrap.value, 42);
  ASSERT_EQ(wrap2.value, 99);

  optional<int> opt_int3;
  std::int32_t const idx = 123;
  ASSERT_EQ(opt_int3.value_or(idx), idx);
  std::int32_t const jdx = -999;
  opt_int3 = jdx;
  ASSERT_EQ(opt_int3.value_or(idx), jdx);
}

/// @test `optional`'s `value_or()` method properly returns the `optional` held value when called on an _rvalue_.
TEST(Optional, ValueOrRValue) {
  struct wrapper {
    std::int32_t value;
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    wrapper(int val)
        : value(val) {}
    ~wrapper() = default;
    wrapper(wrapper const& opt_ither) = default;
    wrapper(wrapper&& opt_ither) = default;
    auto operator=(wrapper const& opt_ither) -> wrapper& = default;
    auto operator=(wrapper&& opt_ither) -> wrapper& = default;
  };

  optional<wrapper> opt_w(99);

  wrapper const wrap = std::move(opt_w).value_or(42);
  ASSERT_EQ(wrap.value, 99);
}

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
auto const generate_default = []() { return 10; };

/// @test `optional`'s `value_or_else()` method properly calls the provided generator when the `optional` is empty, for
/// various value categories.
TEST(OptionalValueOrElse, IfOptionalIsNullTheValueProducedByCallingTheVisitorIsReturned) {
  {
    SCOPED_TRACE("lvalue");
    optional<int> null_optional{};  // NOLINT(misc-const-correctness) testing non-const lvalue overload
    EXPECT_EQ(null_optional.value_or_else(generate_default), generate_default());
  }
  {
    SCOPED_TRACE("const-lvalue");
    optional<int> const null_optional{};
    EXPECT_EQ(null_optional.value_or_else(generate_default), generate_default());
  }
  {
    SCOPED_TRACE("rvalue");
    optional<int> null_optional{};
    EXPECT_EQ(std::move(null_optional).value_or_else(generate_default), generate_default());
  }
  {
    SCOPED_TRACE("const-rvalue");
    optional<int> const null_optional{};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(null_optional).value_or_else(generate_default), generate_default());
  }
}

/// @test `optional`'s `value_or_else()` method properly returns the `optional` held value, for various value
/// categories.
TEST(OptionalValueOrElse, IfOptionalIsNotNullTheValueHeldIsReturned) {
  constexpr int original_value = -10;

  {
    SCOPED_TRACE("lvalue");
    optional<int> value_optional{original_value};  // NOLINT(misc-const-correctness) testing non-const lvalue overload
    EXPECT_EQ(value_optional.value_or_else(generate_default), original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    optional<int> const value_optional{original_value};
    EXPECT_EQ(value_optional.value_or_else(generate_default), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    optional<int> value_optional{original_value};
    EXPECT_EQ(std::move(value_optional).value_or_else(generate_default), original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    optional<int> const value_optional{original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(value_optional).value_or_else(generate_default), original_value);
  }
}

/// @test `optional`'s `emplace()` method can construct a held value by taking arguments supported by the held type
/// constructors.
TEST(Optional, Emplace) {
  struct wrapper {
    std::int32_t x_val = -1;
    std::int32_t y_val = -1;
    wrapper() = default;
    wrapper(int valx, std::int32_t valy)
        : x_val(valx),
          y_val(valy) {}
  };

  optional<wrapper> opt_w;

  opt_w.emplace(99, 42);

  ASSERT_TRUE(opt_w.has_value());
  ASSERT_EQ(opt_w->x_val, 99);
  ASSERT_EQ(opt_w->y_val, 42);

  opt_w.emplace();
  ASSERT_TRUE(opt_w.has_value());
  ASSERT_EQ(opt_w->x_val, -1);
  ASSERT_EQ(opt_w->y_val, -1);
}

struct custom_swap {
  bool swap_lhs = false;
  bool swap_rhs = false;

  friend void swap(custom_swap& lhs, custom_swap& rhs) {
    lhs.swap_lhs = true;
    rhs.swap_rhs = true;
  }
};

namespace {
struct constexpr_swappable {
  std::int32_t value;

  friend constexpr void swap(constexpr_swappable& lhs, constexpr_swappable& rhs) noexcept {
    ::arene::base::swap(lhs.value, rhs.value);
  }
};

constexpr auto do_swap(optional<constexpr_swappable> lhs, optional<constexpr_swappable> rhs)
    -> std::pair<optional<constexpr_swappable>, optional<constexpr_swappable>> {
  lhs.swap(rhs);
  return {lhs, rhs};
}
}  // namespace

/// @test `optional`'s `swap()` method properly exchanges the state of two `optional`s. The behavior is also checked in
/// `constexpr` context. If the held value type supports a custom `swap()` method, it is properly used to swap the two
/// `optional`s.
TEST(Optional, Swap) {
  optional<int> opt_int;
  optional<int> opt_int2(42);
  optional<int> opt_int3(99);

  opt_int.swap(opt_int2);
  ASSERT_TRUE(opt_int.has_value());
  ASSERT_TRUE(!opt_int2.has_value());
  ASSERT_EQ(opt_int, 42);
  opt_int.swap(opt_int3);
  ASSERT_TRUE(opt_int.has_value());
  ASSERT_TRUE(opt_int3.has_value());
  ASSERT_EQ(opt_int3, 42);
  ASSERT_EQ(opt_int, 99);
  opt_int.reset();
  opt_int3.swap(opt_int);
  ASSERT_FALSE(opt_int3.has_value());
  ASSERT_EQ(opt_int, 42);

  constexpr auto swapped = do_swap(
      optional<constexpr_swappable>{constexpr_swappable{123}},
      optional<constexpr_swappable>{constexpr_swappable{999}}
  );
  static_assert(swapped.first->value == 999, "First has right value");
  static_assert(swapped.second->value == 123, "Second has right value");
  auto nc_swapped = do_swap(
      optional<constexpr_swappable>{constexpr_swappable{123}},
      optional<constexpr_swappable>{constexpr_swappable{999}}
  );
  ASSERT_EQ(nc_swapped.first->value, 999);
  ASSERT_EQ(nc_swapped.second->value, 123);
  nc_swapped = do_swap(optional<constexpr_swappable>{}, optional<constexpr_swappable>{constexpr_swappable{456}});
  ASSERT_EQ(nc_swapped.first->value, 456);
  ASSERT_FALSE(nc_swapped.second.has_value());

  optional<custom_swap> ocs(custom_swap{});
  optional<custom_swap> ocs2(custom_swap{});

  ocs.swap(ocs2);
  ASSERT_TRUE(ocs->swap_lhs);
  ASSERT_FALSE(ocs->swap_rhs);
  ASSERT_FALSE(ocs2->swap_lhs);
  ASSERT_TRUE(ocs2->swap_rhs);

  ocs.reset();
  ocs2.emplace();
  ocs.swap(ocs2);
  ASSERT_TRUE(ocs.has_value());
  ASSERT_FALSE(ocs2.has_value());
  ASSERT_FALSE(ocs->swap_lhs);
  ASSERT_FALSE(ocs->swap_rhs);
}

/// @test `optional` can be assigned a value of the type it may hold or of a type convertible to the type it may hold.
/// No additional instances of the value are kept.
/// Given a const value
/// - when the `optional` already holds a value, the value copy-assignment is used.
/// - when the `optional` is empty, the value copy-constructor is used.
/// Given a value that is an rvalule
/// - when the `optional` is empty, the value move-constructor is used.
TEST(Optional, AssignFromValue) {
  constexpr std::uint32_t u_val = 99;
  optional<unsigned> opt_unsigned;
  opt_unsigned = u_val;

  EXPECT_TRUE(opt_unsigned.has_value());
  EXPECT_TRUE(opt_unsigned);
  EXPECT_EQ(opt_unsigned, u_val);

  char const* const hello = "hello";
  optional<arene::base::string_view> opt_str;

  opt_str = hello;

  EXPECT_TRUE(opt_str.has_value());
  EXPECT_TRUE(opt_str);
  EXPECT_EQ(opt_str, hello);

  optional<arene::base::string_view> opt_str2("goodbye");
  opt_str2 = opt_str;

  EXPECT_TRUE(opt_str2.has_value());
  EXPECT_TRUE(opt_str2);
  EXPECT_EQ(opt_str2, hello);

  optional<instance_tracker> opt_it;
  opt_it = instance_tracker{};
  EXPECT_TRUE(opt_it.has_value());
  EXPECT_TRUE(opt_it);

  EXPECT_EQ(instance_tracker::instance_count, 1);

  optional<AssignmentTracker> opt_tracked(AssignmentTracker{});

  AssignmentTracker const tracked;
  opt_tracked = tracked;

  EXPECT_TRUE(opt_tracked->assigned_to);
  EXPECT_TRUE(tracked.assigned_from_const);
  EXPECT_FALSE(tracked.move_assigned_from);

  opt_tracked.reset();
  AssignmentTracker const tracked2;
  opt_tracked = tracked2;

  EXPECT_FALSE(tracked2.assigned_from_const);
  EXPECT_FALSE(tracked2.move_assigned_from);
  EXPECT_FALSE(opt_tracked->assigned_to);

  opt_tracked.reset();
  opt_tracked = AssignmentTracker();
  EXPECT_FALSE(tracked2.assigned_from_const);
  EXPECT_FALSE(tracked2.move_assigned_from);
  EXPECT_FALSE(opt_tracked->assigned_to);
  EXPECT_TRUE(opt_tracked->move_constructed);

  opt_tracked.emplace();
  AssignmentTracker const tracked3;
  opt_tracked = tracked3;
  EXPECT_TRUE(tracked3.assigned_from_const);
  EXPECT_FALSE(tracked3.move_assigned_from);
  EXPECT_TRUE(opt_tracked->assigned_to);
}

struct construction_checker {
  enum class construct { lvalue, const_lvalue, rvalue, multiple, optional };
  construct constructed;
  arene::base::string_view svalue;
  std::int32_t ivalue = 0;

  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  construction_checker(arene::base::string_view& sval)
      : constructed(construct::lvalue),
        svalue(sval) {}
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  construction_checker(arene::base::string_view const& sval)
      : constructed(construct::const_lvalue),
        svalue(sval) {}
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  construction_checker(arene::base::string_view&& sval)
      : constructed(construct::rvalue),
        svalue(sval) {}
  // NOLINTNEXTLINE(modernize-pass-by-value)
  construction_checker(arene::base::string_view const& sval, std::int32_t ival)
      : constructed(construct::multiple),
        svalue(sval),
        ivalue(ival) {}
};

/// @test `optional`'s constructor invoked with `in_place` tag properly constructs a held value in-place for various
/// constructor overloads and value categories.
TEST(Optional, InPlaceConstruction) {
  arene::base::string_view str("hello");
  arene::base::string_view const const_str("goodbye");
  optional<construction_checker> opt_lvalue(in_place, str);
  ASSERT_TRUE(opt_lvalue);
  ASSERT_EQ(opt_lvalue->constructed, construction_checker::construct::lvalue);
  ASSERT_EQ(opt_lvalue->svalue, str);
  optional<construction_checker> opt_clvalue(in_place, const_str);
  ASSERT_TRUE(opt_clvalue);
  ASSERT_EQ(opt_clvalue->constructed, construction_checker::construct::const_lvalue);
  ASSERT_EQ(opt_clvalue->svalue, const_str);
  optional<construction_checker> opt_rvalue(in_place, arene::base::string_view("world"));
  ASSERT_TRUE(opt_rvalue);
  ASSERT_EQ(opt_rvalue->constructed, construction_checker::construct::rvalue);
  ASSERT_EQ(opt_rvalue->svalue, "world");
  optional<construction_checker> opt_multiple(in_place, const_str, 42);
  ASSERT_TRUE(opt_multiple);
  ASSERT_EQ(opt_multiple->constructed, construction_checker::construct::multiple);
  ASSERT_EQ(opt_multiple->svalue, const_str);
  ASSERT_EQ(opt_multiple->ivalue, 42);
}

/// @test `nullopt` can be passed to an `optional` constructor or assignment operator to make the `optional` empty.
TEST(Optional, NullOpt) {
  optional<int> opt_int(nullopt);
  ASSERT_TRUE(!opt_int);
  opt_int = 42;
  ASSERT_TRUE(opt_int);
  opt_int = nullopt;
  ASSERT_TRUE(!opt_int);

  optional<arene::base::string_view> opt_str(nullopt);
  ASSERT_TRUE(!opt_str);
  opt_str = "hello";
  ASSERT_TRUE(opt_str);
  opt_str = nullopt;
  ASSERT_TRUE(!opt_str);
}

TEST(Optional, NullOptAliasesAreEquivalentToNonAliases) {
  ::testing::StaticAssertTypeEq<nullopt_t, null_opt_t>();
  constexpr optional<int> aliased(null_opt);
  STATIC_ASSERT_TRUE(!aliased);
  constexpr optional<int> native(nullopt);
  STATIC_ASSERT_TRUE(!native);
}

/// @test `optional` equality operator returns the expected result for empty `optional`s and `optional`s holding a
/// value.
TEST(Optional, Equality) {
  constexpr optional<int> opt_int;
  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(opt_int == opt_int, "Must equal self");
  static_assert(!(opt_int == 42), "Empty must not equal int");
  static_assert(!(42 == opt_int), "Empty must not equal int");
  static_assert(nullopt == opt_int, "Empty must equal NullOpt");
  static_assert(opt_int == nullopt, "Empty must equal NullOpt");

  constexpr optional<int> opt_int2(42);
  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(opt_int2 == opt_int2, "Must equal self");
  static_assert(opt_int2 == 42, "Value must equal int");
  static_assert(42 == opt_int2, "Value must equal int");
  static_assert(!(opt_int2 == 99), "Value must not equal different int");
  static_assert(!(99 == opt_int2), "Value must not equal different int");
  static_assert(!(nullopt == opt_int2), "Full must not equal NullOpt");
  static_assert(!(opt_int2 == nullopt), "Full must not equal NullOpt");

  static_assert(!(opt_int == opt_int2), "Empty must not equal value");
  static_assert(!(opt_int2 == opt_int), "Empty must not equal value");

  constexpr optional<int> opt_int3(99);
  static_assert(!(opt_int2 == opt_int3), "Value must not equal different int");
}

/// @test `optional` inequality operator returns the expected result for empty `optional`s and `optional`s holding a
/// value.
TEST(Optional, Inequality) {
  constexpr optional<int> opt_int;
  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(!(opt_int != opt_int), "Must equal self");
  static_assert(opt_int != 42, "Empty must not equal int");
  static_assert(42 != opt_int, "Empty must not equal int");
  static_assert(!(nullopt != opt_int), "Empty must equal NullOpt");
  static_assert(!(opt_int != nullopt), "Empty must equal NullOpt");

  constexpr optional<int> opt_int2(42);
  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(!(opt_int2 != opt_int2), "Must equal self");
  static_assert(!(opt_int2 != 42), "Value must equal int");
  static_assert(!(42 != opt_int2), "Value must equal int");
  static_assert(opt_int2 != 99, "Value must not equal different int");
  static_assert(99 != opt_int2, "Value must not equal different int");
  static_assert(nullopt != opt_int2, "Full must not equal NullOpt");
  static_assert(opt_int2 != nullopt, "Full must not equal NullOpt");

  static_assert((opt_int != opt_int2), "Empty must not equal value");
  static_assert((opt_int2 != opt_int), "Empty must not equal value");

  constexpr optional<int> opt_int3(99);
  static_assert(opt_int2 != opt_int3, "Value must not equal different int");
}

namespace {
class comparable_with_fast_inequality {
  unsigned data_;

 public:
  explicit constexpr comparable_with_fast_inequality(unsigned data) noexcept
      : data_(data) {}
  static constexpr auto fast_inequality_check(
      comparable_with_fast_inequality const& lhs,
      comparable_with_fast_inequality const& rhs
  ) noexcept -> arene::base::inequality_heuristic {
    return ((lhs.data_ % 3) == 0 || (rhs.data_ % 3) == 0)
               ? arene::base::inequality_heuristic::definitely_not_equal
               : arene::base::inequality_heuristic::may_be_equal_or_not_equal;
  }
  static constexpr auto three_way_compare(
      comparable_with_fast_inequality const& lhs,
      comparable_with_fast_inequality const& rhs
  ) noexcept -> arene::base::strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data_);
  }

  friend constexpr auto
  operator==(comparable_with_fast_inequality const& lhs, comparable_with_fast_inequality const& rhs) noexcept {
    if ((lhs.data_ % 3) == 0 || (rhs.data_ % 3) == 0) {
      return false;
    }
    return lhs.data_ == rhs.data_;
  }
};
}  // namespace

/// @test `optional` comparisons return the expected results for held types having `fast_inequality_check`.
TEST(Optional, InequalityWithFastCheck) {
  constexpr optional<comparable_with_fast_inequality> empty{};
  constexpr comparable_with_fast_inequality small_multiple_of_3{6};
  constexpr comparable_with_fast_inequality large_multiple_of_3{600};
  constexpr comparable_with_fast_inequality non_multiple_of_3{7};
  constexpr optional<comparable_with_fast_inequality> opt_small_multiple_of_3{small_multiple_of_3};
  constexpr optional<comparable_with_fast_inequality> opt_large_multiple_of_3{large_multiple_of_3};
  constexpr optional<comparable_with_fast_inequality> opt_non_multiple_of_3{non_multiple_of_3};

  STATIC_ASSERT_EQ(empty, empty);
  STATIC_ASSERT_NE(empty, opt_small_multiple_of_3);
  STATIC_ASSERT_NE(empty, small_multiple_of_3);
  STATIC_ASSERT_NE(opt_small_multiple_of_3, empty);
  STATIC_ASSERT_NE(small_multiple_of_3, empty);
  STATIC_ASSERT_NE(opt_small_multiple_of_3, small_multiple_of_3);
  STATIC_ASSERT_NE(large_multiple_of_3, opt_large_multiple_of_3);
  STATIC_ASSERT_NE(opt_large_multiple_of_3, opt_large_multiple_of_3);
  STATIC_ASSERT_EQ(opt_non_multiple_of_3, non_multiple_of_3);
  STATIC_ASSERT_EQ(non_multiple_of_3, opt_non_multiple_of_3);
  STATIC_ASSERT_EQ(opt_non_multiple_of_3, opt_non_multiple_of_3);
  STATIC_ASSERT_LT(empty, small_multiple_of_3);
  STATIC_ASSERT_LT(empty, opt_small_multiple_of_3);
  STATIC_ASSERT_GT(small_multiple_of_3, empty);
  STATIC_ASSERT_GT(opt_small_multiple_of_3, empty);
  STATIC_ASSERT_LT(opt_small_multiple_of_3, opt_large_multiple_of_3);
  STATIC_ASSERT_LT(small_multiple_of_3, opt_large_multiple_of_3);
  STATIC_ASSERT_LT(opt_small_multiple_of_3, large_multiple_of_3);
  STATIC_ASSERT_GT(opt_large_multiple_of_3, opt_small_multiple_of_3);
  STATIC_ASSERT_GT(opt_large_multiple_of_3, small_multiple_of_3);
  STATIC_ASSERT_GT(large_multiple_of_3, opt_small_multiple_of_3);
}

struct non_copyable_comparable {
  std::int32_t value;
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr non_copyable_comparable(int val)
      : value(val) {}
  non_copyable_comparable(non_copyable_comparable const&) = delete;
  auto operator=(non_copyable_comparable const&) -> non_copyable_comparable& = delete;
  non_copyable_comparable(non_copyable_comparable&&) = default;
  auto operator=(non_copyable_comparable&&) -> non_copyable_comparable& = default;
  ~non_copyable_comparable() = default;

  friend constexpr auto operator==(non_copyable_comparable const& lhs, non_copyable_comparable const& rhs) -> bool {
    return lhs.value == rhs.value;
  }
  friend constexpr auto operator<(non_copyable_comparable const& lhs, non_copyable_comparable const& rhs) -> bool {
    return lhs.value < rhs.value;
  }
};

/// @test Given an @c arene::base::optional<T> where @c T is comparable but non-copyable, @c arene::base::optional<T>
/// has passthrough comparison operations equivalent to comparing @c T .
TEST(Optional, NonCopyableComparisons) {
  constexpr non_copyable_comparable ncc{42};
  constexpr non_copyable_comparable opt_ither{99};
  constexpr optional<non_copyable_comparable> oncc{};
  constexpr optional<non_copyable_comparable> oncc2{in_place, 42};

  // NOLINTNEXTLINE(misc-redundant-expression)
  STATIC_ASSERT_EQ(oncc, oncc);
  STATIC_ASSERT_FALSE(oncc == ncc);
  STATIC_ASSERT_FALSE(ncc == oncc);
  STATIC_ASSERT_EQ(nullopt, oncc);
  STATIC_ASSERT_EQ(oncc, nullopt);

  // NOLINTNEXTLINE(misc-redundant-expression)
  STATIC_ASSERT_EQ(oncc2, oncc2);
  STATIC_ASSERT_EQ(oncc2, ncc);
  STATIC_ASSERT_EQ(ncc, oncc2);
  STATIC_ASSERT_FALSE(oncc2 == opt_ither);
  STATIC_ASSERT_FALSE(opt_ither == oncc2);
  STATIC_ASSERT_FALSE(nullopt == oncc2);
  STATIC_ASSERT_FALSE(oncc2 == nullopt);

  STATIC_ASSERT_FALSE(oncc == oncc2);
  STATIC_ASSERT_FALSE(oncc2 == oncc);

  // NOLINTNEXTLINE(misc-redundant-expression)
  STATIC_ASSERT_FALSE(oncc != oncc);
  STATIC_ASSERT_NE(oncc, ncc);
  STATIC_ASSERT_NE(ncc, oncc);
  STATIC_ASSERT_FALSE(nullopt != oncc);
  STATIC_ASSERT_FALSE(oncc != nullopt);

  // NOLINTNEXTLINE(misc-redundant-expression)
  STATIC_ASSERT_FALSE(oncc2 != oncc2);
  STATIC_ASSERT_FALSE(oncc2 != ncc);
  STATIC_ASSERT_FALSE(ncc != oncc2);
  STATIC_ASSERT_NE(oncc2, opt_ither);
  STATIC_ASSERT_NE(opt_ither, oncc2);
  STATIC_ASSERT_NE(nullopt, oncc2);
  STATIC_ASSERT_NE(oncc2, nullopt);

  STATIC_ASSERT_NE(oncc, oncc2);
  STATIC_ASSERT_NE(oncc2, oncc);

  ASSERT_EQ(oncc, oncc);
  ASSERT_EQ(oncc, nullopt);
  ASSERT_NE(oncc2, nullopt);
  ASSERT_EQ(oncc2, oncc2);
  ASSERT_NE(oncc, ncc);
  ASSERT_NE(ncc, oncc);
  ASSERT_NE(oncc, oncc2);
  ASSERT_NE(opt_ither, oncc2);
  ASSERT_NE(oncc2, opt_ither);

  optional<non_copyable_comparable> const oncc3(in_place, 99);

  ASSERT_EQ(oncc3, opt_ither);
  ASSERT_EQ(opt_ither, oncc3);
}
/// @test Given an @c arene::base::optional<T> and @c arene::base::optional<U> where @c
/// arene::base::is_equality_comparable<T,U> is @c true , the optionals are equality comparable.
TEST(Optional, MixedOptionalEquality) {
  constexpr optional<int> opt_int{};
  constexpr optional<std::int16_t> opt_str{};
  constexpr optional<int> opt_int2{42};
  constexpr optional<std::int16_t> opt_str2{s42};

  static_assert(opt_int == opt_str, "Empty must equal empty");
  static_assert(opt_str == opt_int, "Empty must equal empty");

  static_assert(opt_int2 == opt_str2, "Value must equal value");
  static_assert(opt_str2 == opt_int2, "Value must equal value");

  static_assert(!(opt_int == opt_str2), "Empty must not equal value");
  static_assert(!(opt_str2 == opt_int), "Empty must not equal value");

  static_assert(!(opt_int != opt_str), "Empty must equal empty");
  static_assert(!(opt_str != opt_int), "Empty must equal empty");

  static_assert(!(opt_int2 != opt_str2), "Value must equal value");
  static_assert(!(opt_str2 != opt_int2), "Value must equal value");

  static_assert(opt_int != opt_str2, "Empty must not equal value");
  static_assert(opt_str2 != opt_int, "Empty must not equal value");

  ASSERT_EQ(opt_int, opt_str);
  ASSERT_EQ(opt_str2, opt_int2);
  ASSERT_NE(opt_int, opt_str2);
  ASSERT_NE(opt_str2, opt_int);
}

namespace {
struct type_with_equals {
  int data;

  friend constexpr auto operator==(type_with_equals const& lhs, type_with_equals const& rhs) noexcept -> bool {
    return lhs.data == rhs.data;
  }
};
}  // namespace
/// @test Given an @c arene::base::optional<T> where @c T only defines @c operator== , @c operator!= is synthesized for
/// the optional as the inverse of @c operator==.
TEST(Optional, CanCompareWhenOnlyEqualsSupported) {
  using opt = optional<type_with_equals>;
  constexpr opt val1{type_with_equals{42}};
  constexpr opt val2{type_with_equals{99}};
  constexpr opt empty{};

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<opt>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<opt>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<opt>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<opt>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<opt>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<opt>);

  STATIC_ASSERT_EQ(val1, val1);
  STATIC_ASSERT_EQ(empty, empty);
  STATIC_ASSERT_FALSE(val1 == empty);
  STATIC_ASSERT_FALSE(empty == val1);
  STATIC_ASSERT_NE(val1, val2);
  STATIC_ASSERT_NE(val1, empty);
  STATIC_ASSERT_NE(empty, val2);
  STATIC_ASSERT_FALSE(empty != empty);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<opt, type_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<opt, type_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<opt, type_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<opt, type_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<opt, type_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<opt, type_with_equals>);

  STATIC_ASSERT_EQ(val1, *val1);
  STATIC_ASSERT_FALSE(empty == *val1);
  STATIC_ASSERT_NE(val1, *val2);
  STATIC_ASSERT_NE(empty, *val2);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<type_with_equals, opt>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<type_with_equals, opt>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<type_with_equals, opt>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<type_with_equals, opt>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<type_with_equals, opt>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<type_with_equals, opt>);

  STATIC_ASSERT_EQ(*val1, val1);
  STATIC_ASSERT_FALSE(*val1 == empty);
  STATIC_ASSERT_NE(*val1, val2);
  STATIC_ASSERT_NE(*val1, empty);
}
/// @test @c arene::base::optional<T> is constructable from @c arene::base:optional<U> if @c T can be constructed from
/// @c U .
TEST(Optional, ConstructionFromOtherOptional) {
  std::int16_t const i16v = 42;

  struct number {
    constexpr explicit number(int num)
        : num_(num) {}

    explicit constexpr operator int() const { return num_; }

   private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    std::int32_t const num_;
  };

  constexpr optional<number> opt_number{number(42)};
  constexpr optional<std::int16_t> opt_i16{i16v};
  constexpr optional<int> opt_from_i16(opt_i16);
  constexpr optional<int> opt_from_number(opt_number);

  ASSERT_TRUE(opt_from_i16);
  ASSERT_EQ(opt_from_i16, 42);

  ASSERT_TRUE(opt_from_number);
  ASSERT_EQ(opt_from_number, 42);

  struct value {
    arene::base::string_view val = "hello";
  };

  struct convertible_from_value {
    arene::base::string_view val;

    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    convertible_from_value(value&& from)
        : val(from.val) {}
  };

  optional<convertible_from_value> from_opt_value(optional<value>{in_place});

  ASSERT_TRUE(from_opt_value);
  ASSERT_EQ(from_opt_value->val, "hello");

  optional<convertible_from_value> const from_nullopt_value(optional<value>{});
  ASSERT_TRUE(!from_nullopt_value);
}

/// @test @c arene::base::optional<T>::operator< is defined if @c T is relationally comparable. @c nullopt compares less
/// than all optionals.
TEST(Optional, LessThanOrderingComparisons) {
  constexpr optional<int> opt_int;
  constexpr optional<int> opt_int2(42);
  constexpr optional<int> opt_int3(99);

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(!(opt_int < opt_int), "Must not be less than self");
  static_assert(opt_int < 42, "Empty must be less than int");
  static_assert(!(42 < opt_int), "Empty must not be greater than int");
  static_assert(!(nullopt < opt_int), "NullOpt must not be less than Empty");
  static_assert(!(opt_int < nullopt), "Empty must not be less than NullOpt");

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(!(opt_int2 < opt_int2), "Must equal self");
  static_assert(!(opt_int2 < 42), "Value must not be less than equal int");
  static_assert(opt_int2 < 43, "Value must be less than bigger int");
  static_assert(!(opt_int2 < 41), "Value must not be less than smaller int");
  static_assert(!(42 < opt_int2), "Value must not be greater than equal int");
  static_assert(!(43 < opt_int2), "Value must not be greater than bigger int");
  static_assert(41 < opt_int2, "Value must be greater than than smaller int");
  static_assert(nullopt < opt_int2, "NullOpt less than value");
  static_assert(!(opt_int2 < nullopt), "Value must not not be less than NullOpt");

  static_assert((opt_int < opt_int2), "Empty must be less than value");
  static_assert(!((opt_int2 < opt_int)), "value is not less than equal");

  static_assert(opt_int2 < opt_int3, "Value must not be less than bigger value");
  static_assert(!(opt_int3 < opt_int2), "Value must not be less than bigger value");

  ASSERT_LT(opt_int2, opt_int3);
  ASSERT_LT(opt_int2, 99);
  ASSERT_LT(12, opt_int2);
  ASSERT_FALSE(opt_int2 < nullopt);
  ASSERT_LT(nullopt, opt_int2);

  constexpr optional<std::int16_t> opt_str{s123};

  static_assert(!(opt_str < opt_int), "Value not less than empty");
  static_assert(opt_int2 < opt_str, "Value less than bigger value");
  static_assert(!(opt_str < opt_int2), "Value not less than smaller value");

  ASSERT_LT(opt_int2, opt_str);
  ASSERT_FALSE(opt_str < opt_int2);

  constexpr non_copyable_comparable ncc{42};
  constexpr non_copyable_comparable opt_ither{99};
  constexpr optional<non_copyable_comparable> oncc{};
  constexpr optional<non_copyable_comparable> oncc2{in_place, 42};

  static_assert(oncc < ncc, "Empty less than value");
  static_assert(!(oncc2 < ncc), "not less than equal");
  static_assert(oncc2 < opt_ither, "less than bigger");
  static_assert(!(opt_ither < oncc2), "less than bigger");
  static_assert(!(ncc < oncc2), "not less than equal");
}
/// @test @c arene::base::optional<T>::operator> is defined if @c T is relationally comparable. @c nullopt compares less
/// than all optionals.
TEST(Optional, GreaterThanOrderingComparisons) {
  constexpr optional<int> opt_int;
  constexpr optional<int> opt_int2(42);
  constexpr optional<int> opt_int3(99);

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(!(opt_int > opt_int), "Must not be greater than self");
  static_assert(!(opt_int > 42), "Empty must not be greater than int");
  static_assert(42 > opt_int, "Empty must not be greater than int");
  static_assert(!(nullopt > opt_int), "NullOpt must not be greater than Empty");
  static_assert(!(opt_int > nullopt), "Empty must not be greater than NullOpt");

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(!(opt_int2 > opt_int2), "Must equal self");
  static_assert(!(opt_int2 > 42), "Value must not be greater than equal int");
  static_assert(!(opt_int2 > 43), "Value must be greater than bigger int");
  static_assert(opt_int2 > 41, "Value must not be greater than smaller int");
  static_assert(!(42 > opt_int2), "Value must not be greater than equal int");
  static_assert(43 > opt_int2, "Value must not be greater than bigger int");
  static_assert(!(41 > opt_int2), "Value must be greater than than smaller int");
  static_assert(!(nullopt > opt_int2), "NullOpt greater than value");
  static_assert(opt_int2 > nullopt, "Value must not not be greater than NullOpt");

  static_assert(!((opt_int > opt_int2)), "Empty must not be greater than value");
  static_assert((opt_int2 > opt_int), "value is greater than empty");

  static_assert(!(opt_int2 > opt_int3), "Value must not be greater than bigger value");
  static_assert(opt_int3 > opt_int2, "Value must not be greater than bigger value");

  ASSERT_GT(opt_int3, opt_int2);
  ASSERT_GT(opt_int3, 42);
  ASSERT_GT(123, opt_int3);

  constexpr optional<std::int16_t> opt_str{s123};

  static_assert(opt_str > opt_int, "Value greater than empty");
  static_assert(!(opt_int2 > opt_str), "Value greater than bigger value");
  static_assert(opt_str > opt_int2, "Value not greater than smaller value");

  ASSERT_GT(opt_str, opt_int);
  ASSERT_FALSE(opt_int2 > opt_str);

  constexpr non_copyable_comparable ncc{42};
  constexpr non_copyable_comparable opt_ither{99};
  constexpr optional<non_copyable_comparable> oncc{};
  constexpr optional<non_copyable_comparable> oncc2{in_place, 42};

  static_assert(!(oncc > ncc), "Empty not greater than value");
  static_assert(!(oncc2 > ncc), "not greater than equal");
  static_assert(!(oncc2 > opt_ither), "greater than bigger");
  static_assert(opt_ither > oncc2, "greater than bigger");
  static_assert(!(ncc > oncc2), "not greater than equal");
}
/// @test @c arene::base::optional<T>::operator<= is defined if @c T is relationally comparable. @c nullopt compares
/// less than all optionals.
TEST(Optional, LessThanOrEqualOrderingComparisons) {
  constexpr optional<int> opt_int;
  constexpr optional<int> opt_int2(42);
  constexpr optional<int> opt_int3(99);

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(opt_int <= opt_int, "Must be equal to self");
  static_assert(opt_int <= 42, "Empty must be less than int");
  static_assert(!(42 <= opt_int), "Empty must not be greater than int");
  static_assert(nullopt <= opt_int, "NullOpt must be equal to Empty");
  static_assert(opt_int <= nullopt, "Empty must be equal to NullOpt");

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(opt_int2 <= opt_int2, "Must equal self");
  static_assert(opt_int2 <= 42, "Value must be equal to equal int");
  static_assert(opt_int2 <= 43, "Value must be less than bigger int");
  static_assert(!(opt_int2 <= 41), "Value must not be less than smaller int");
  static_assert(42 <= opt_int2, "Value must equal to equal int");
  static_assert(!(43 <= opt_int2), "Value must not be greater than bigger int");
  static_assert(41 <= opt_int2, "Value must be greater than than smaller int");
  static_assert(nullopt <= opt_int2, "NullOpt less than value");
  static_assert(!(opt_int2 <= nullopt), "Value must not not be less than NullOpt");

  static_assert((opt_int <= opt_int2), "Empty must be less than value");
  static_assert(!((opt_int2 <= opt_int)), "value is not less than empty");

  static_assert(opt_int2 <= opt_int3, "Value must not be less than bigger value");
  static_assert(!(opt_int3 <= opt_int2), "Value must not be less than bigger value");

  ASSERT_LE(opt_int2, opt_int3);
  ASSERT_LE(opt_int2, 99);
  ASSERT_LE(10, opt_int2);

  constexpr optional<std::int16_t> opt_str{s123};

  static_assert(!(opt_str <= opt_int), "Value not less than empty");
  static_assert(opt_int2 <= opt_str, "Value less than bigger value");
  static_assert(!(opt_str <= opt_int2), "Value not less than smaller value");

  ASSERT_LE(opt_int2, opt_str);
  ASSERT_FALSE(opt_str <= opt_int2);

  constexpr non_copyable_comparable ncc{42};
  constexpr non_copyable_comparable opt_ither{99};
  constexpr optional<non_copyable_comparable> oncc{};
  constexpr optional<non_copyable_comparable> oncc2{in_place, 42};

  static_assert(oncc <= ncc, "Empty less than value");
  static_assert(oncc2 <= ncc, "Equal to equal");
  static_assert(oncc2 <= opt_ither, "less than bigger");
  static_assert(!(opt_ither <= oncc2), "less than bigger");
  static_assert(ncc <= oncc2, "Equal to equal");
}
/// @test @c arene::base::optional<T>::operator>= is defined if @c T is relationally comparable. @c nullopt compares
/// less than all optionals.
TEST(Optional, GreaterThanOrEqualOrderingComparisons) {
  constexpr optional<int> opt_int;
  constexpr optional<int> opt_int2(42);
  constexpr optional<int> opt_int3(99);

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(opt_int >= opt_int, "Must equal self");
  static_assert(!(opt_int >= 42), "Empty must not be greater than int");
  static_assert(42 >= opt_int, "Empty must not be greater than int");
  static_assert(nullopt >= opt_int, "NullOpt must be equal to Empty");
  static_assert(opt_int >= nullopt, "Empty must be equal to NullOpt");

  // NOLINTNEXTLINE(misc-redundant-expression)
  static_assert(opt_int2 >= opt_int2, "Must equal self");
  static_assert(opt_int2 >= 42, "Value must equal to than equal int");
  static_assert(!(opt_int2 >= 43), "Value must be greater than bigger int");
  static_assert(opt_int2 >= 41, "Value must not be greater than smaller int");
  static_assert(42 >= opt_int2, "Value must be equal to equal int");
  static_assert(43 >= opt_int2, "Value must not be greater than bigger int");
  static_assert(!(41 >= opt_int2), "Value must be greater than than smaller int");
  static_assert(!(nullopt >= opt_int2), "NullOpt greater than value");
  static_assert(opt_int2 >= nullopt, "Value must not not be greater than NullOpt");

  static_assert(!((opt_int >= opt_int2)), "Empty must not be greater than value");
  static_assert((opt_int2 >= opt_int), "value is greater than empty");

  static_assert(!(opt_int2 >= opt_int3), "Value must not be greater than bigger value");
  static_assert(opt_int3 >= opt_int2, "Value must not be greater than bigger value");

  ASSERT_GE(opt_int3, opt_int2);
  ASSERT_GE(opt_int3, 42);
  ASSERT_GE(123, opt_int3);

  constexpr optional<std::int16_t> opt_str{s123};

  static_assert(opt_str >= opt_int, "Value greater than empty");
  static_assert(!(opt_int2 >= opt_str), "Value greater than bigger value");
  static_assert(opt_str >= opt_int2, "Value not greater than smaller value");

  ASSERT_GT(opt_str, opt_int);
  ASSERT_FALSE(opt_int2 >= opt_str);

  constexpr non_copyable_comparable ncc{42};
  constexpr non_copyable_comparable opt_ither{99};
  constexpr optional<non_copyable_comparable> oncc{};
  constexpr optional<non_copyable_comparable> oncc2{in_place, 42};

  static_assert(!(oncc >= ncc), "Empty not greater than value");
  static_assert(oncc2 >= ncc, "greater or equal to equal");
  static_assert(!(oncc2 >= opt_ither), "greater than bigger");
  static_assert(opt_ither >= oncc2, "greater than bigger");
  static_assert(ncc >= oncc2, "greater or equal to equal");
}

namespace test {
struct throwing_move {
  throwing_move() = default;
  ~throwing_move() = default;
  // NOLINTBEGIN(hicpp-noexcept-move) explicitly testing a throwing move ctor
  throwing_move(throwing_move&&) {}
  auto operator=(throwing_move&&) -> throwing_move& { return *this; }
  // NOLINTEND(hicpp-noexcept-move)

  throwing_move(throwing_move const&) = default;
  auto operator=(throwing_move const&) -> throwing_move& = default;
};

ARENE_MAYBE_UNUSED void swap(throwing_move&, throwing_move&) {}
}  // namespace test

/// @test @c arene::base::optional<T> is nothrow swappable if @c T is nothrow swappable.
TEST(Optional, NoThrowSwap) {
  static_assert(
      noexcept(std::swap(std::declval<optional<int>&>(), std::declval<optional<int>&>())),
      "Swap optional std::int32_t must be noexcept"
  );
  static_assert(
      noexcept(std::swap(
          std::declval<optional<arene::base::string_view>&>(),
          std::declval<optional<arene::base::string_view>&>()
      )),
      "Swap optional string is noexcept"
  );
  static_assert(
      noexcept(std::swap(
          std::declval<optional<arene::base::array<int, 10>>&>(),
          std::declval<optional<arene::base::array<int, 10>>&>()
      )),
      "Swap optional array is noexcept"
  );

  static_assert(
      !noexcept(
          std::swap(std::declval<optional<test::throwing_move>&>(), std::declval<optional<test::throwing_move>&>())
      ),
      "Swap optional throwing_move is not noexcept"
  );
}

/// @test @c arene::base::optional<T>::operator= is noexcept if @c T is noexcept-assignable.
TEST(Optional, AssignmentNoExcept) {
  struct throwing_assign_from_int {
    explicit throwing_assign_from_int(int) noexcept {}
    auto operator=(int) -> throwing_assign_from_int&;
  };

  struct non_throwing_assign_from_int {
    explicit non_throwing_assign_from_int(int) noexcept {}
    auto operator=(int) noexcept -> non_throwing_assign_from_int&;
  };

  struct throwing_construct_from_int {
    explicit throwing_construct_from_int(int) {}
    auto operator=(int) noexcept -> throwing_construct_from_int&;
  };

  struct non_throwing_construct_from_int {
    explicit non_throwing_construct_from_int(int) noexcept {}
    auto operator=(int) noexcept -> non_throwing_construct_from_int&;
  };

  static_assert(
      noexcept(std::declval<optional<non_throwing_assign_from_int>>() = std::declval<int>()),
      "Nothrow assign from another type"
  );
  static_assert(
      !noexcept(std::declval<optional<throwing_assign_from_int>>() = std::declval<int>()),
      "throwing assign from another type"
  );
  static_assert(
      noexcept(std::declval<optional<non_throwing_construct_from_int>>() = std::declval<int>()),
      "Nothrow construct from another type"
  );
  static_assert(
      !noexcept(std::declval<optional<throwing_construct_from_int>>() = std::declval<int>()),
      "throwing construct from another type"
  );
  static_assert(
      noexcept(std::declval<optional<int>>() = std::declval<std::int16_t>()),
      "Nothrow assign from another type"
  );
  static_assert(
      noexcept(std::declval<optional<int>>().emplace(std::declval<std::int16_t>())),
      "Nothrow emplace from another type"
  );

  static_assert(
      !noexcept(std::declval<optional<throwing_construct_from_int>>().emplace(std::declval<int>())),
      "Throwing emplace from another type"
  );
  static_assert(
      noexcept(std::declval<optional<non_throwing_construct_from_int>>().emplace(std::declval<int>())),
      "Non-throwing emplace from another type"
  );
}

// The following tests and functions are here to verify that the clang-tidy bugprone-noexcept-xxx warnings are
// not triggered: prior to the commit where these were introduced, these tests and the functions they call triggered
// the warning, now they do not. Move-Return.
namespace tests_that_verify_bugprone_noexcept_clangtidy_warnings_not_generated {
auto example_with_implicit_move() noexcept -> arene::base::optional<int> { return arene::base::nullopt; }

/// @test Validates that move-return'ing an @c arene::base::optional<T> does not trigger clang-tidy's bugprone-noexcept
/// warnings.
TEST(OptionalTestsForBugProneNoexceptClangTidy, NoexceptMoveReturn) {
  ASSERT_FALSE(example_with_implicit_move().has_value());
  static_assert(noexcept(example_with_implicit_move()), "Must be noexcept");
}

class NonTrivialDestructor {
  int value_{0};

 public:
  NonTrivialDestructor() = default;
  NonTrivialDestructor(NonTrivialDestructor const&) = default;
  NonTrivialDestructor(NonTrivialDestructor&&) = default;
  auto operator=(NonTrivialDestructor const&) -> NonTrivialDestructor& = default;
  auto operator=(NonTrivialDestructor&&) -> NonTrivialDestructor& = default;
  ~NonTrivialDestructor() { value_ = 15; }
};

auto example_with_implicit_move_non_trivial() noexcept -> arene::base::optional<NonTrivialDestructor> {
  return arene::base::nullopt;
}

/// @test Validates that move-return'ing an @c arene::base::optional<T> where @c T has a non-trivial dtor does not
/// trigger clang-tidy's bugprone-noexcept warnings.
TEST(OptionalTestsForBugProneNoexceptClangTidy, NoexceptMoveReturnNonTrivialDestructor) {
  static_assert(
      !std::is_trivially_destructible<NonTrivialDestructor>::value,
      "Class NonTrivialDestructor must not have a trivial destructor"
  );

  ASSERT_FALSE(example_with_implicit_move_non_trivial().has_value());
  static_assert(noexcept(example_with_implicit_move_non_trivial()), "Must be noexcept");
}

void example_with_move_assign() noexcept {
  arene::base::optional<int> op1;
  arene::base::optional<int> op2;
  static_assert(noexcept(arene::base::optional<int>{}), "Must be noexcept");
  static_assert(noexcept(op1 = std::move(op2)), "Must be noexcept");
  op1 = std::move(op2);
}
/// @test Validates that move-assigning'ing an @c arene::base::optional<T> does not trigger clang-tidy's
/// bugprone-noexcept warnings.
TEST(OptionalTestsForBugProneNoexceptClangTidy, NoexceptMoveAssign) {
  example_with_move_assign();
  static_assert(noexcept(example_with_move_assign()), "Must be noexcept");
}

void example_with_copy_assign() noexcept {
  arene::base::optional<int> op1;
  arene::base::optional<int> const op2;
  static_assert(noexcept(arene::base::optional<int>{}), "Must be noexcept");
  static_assert(noexcept(op1 = op2), "Must be noexcept");
  op1 = op2;
}
/// @test Validates that copy-assigning'ing an @c arene::base::optional<T> does not trigger clang-tidy's
/// bugprone-noexcept warnings.
TEST(OptionalTestsForBugProneNoexceptClangTidy, NoexceptCopyAssign) {
  example_with_copy_assign();
  static_assert(noexcept(example_with_copy_assign()), "Must be noexcept");
}

void example_with_const_copy_assign() noexcept {
  arene::base::optional<int> op1;
  arene::base::optional<int> const op2;
  static_assert(noexcept(arene::base::optional<int>{}), "Must be noexcept");
  static_assert(noexcept(op1 = op2), "Must be noexcept");
  op1 = op2;
}
/// @test Validates that copy-assigning'ing an @c arene::base::optional<T> does not trigger clang-tidy's
/// bugprone-noexcept warnings.
TEST(OptionalTestsForBugProneNoexceptClangTidy, NoexceptConstCopyAssign) {
  example_with_const_copy_assign();
  static_assert(noexcept(example_with_const_copy_assign()), "Must be noexcept");
}
}  // namespace tests_that_verify_bugprone_noexcept_clangtidy_warnings_not_generated

struct visitor_base {
  using original_value_type = std::int32_t;
  using optional_t = optional<original_value_type>;
  /// the return value type is a different integral type to prove that we can actually change the value type
  using chained_value_type = std::int64_t;
  enum parameter_qualification { lvalue, clvalue, rvalue, crvalue, not_called };
  parameter_qualification selected_overload = not_called;
};

struct engaged_visitor : visitor_base {
  /// An optional is returned in all cases so this can prove both and_then is a monad and that transform is not.
  using return_optional_t = optional<visitor_base::chained_value_type>;
  ///
  /// @brief Overloads for introspecting a visited result.
  /// @return optional<visitor_base::chained_value> containing the value the functor was called with cast to
  ///         chained_value_type
  /// @post selected_overload will be set to the state matching the overload that was selected.
  /// @{
  auto operator()(visitor_base::original_value_type& arg) -> return_optional_t {
    selected_overload = parameter_qualification::lvalue;
    return {static_cast<chained_value_type>(arg)};
  }
  auto operator()(visitor_base::original_value_type const& arg) -> return_optional_t {
    selected_overload = parameter_qualification::clvalue;
    return {static_cast<chained_value_type>(arg)};
  }
  auto operator()(visitor_base::original_value_type&& arg) -> return_optional_t {
    selected_overload = parameter_qualification::rvalue;
    return {static_cast<chained_value_type>(arg)};
  }
  auto operator()(visitor_base::original_value_type const&& arg) -> return_optional_t {
    selected_overload = parameter_qualification::crvalue;
    return {static_cast<chained_value_type>(arg)};
  }
  /// @}
};

/// @test The return type of @c arene::base::optional<T>::and_then(value_handler) is @c arene::base::optional<U> , which
/// is the return type of @c value_handler .
TEST(OptionalAndThen, ReturnTypeIsReturnTypeFromHandler) {
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t&>().and_then(std::declval<engaged_visitor>())),
        engaged_visitor::return_optional_t>();
  }
  {
    SCOPED_TRACE("const-lvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t const&>().and_then(std::declval<engaged_visitor>())),
        engaged_visitor::return_optional_t>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t&&>().and_then(std::declval<engaged_visitor>())),
        engaged_visitor::return_optional_t>();
  }
  {
    SCOPED_TRACE("const-rvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t const&&>().and_then(std::declval<engaged_visitor>())),
        engaged_visitor::return_optional_t>();
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.and_then(value_handler) when @c
/// optional.has_value() is @c true invokes @c value_handler with a type equivalent to the return type of
/// @c *optional.
TEST(OptionalAndThen, CallsProvidedVisitorWithValueQualifiedAsOptionalIfValueIsPopulated) {
  {
    SCOPED_TRACE("lvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t value_optional{10};
    value_optional.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::lvalue);
  }
  {
    SCOPED_TRACE("const-lvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t const value_optional{10};
    value_optional.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::clvalue);
  }
  {
    SCOPED_TRACE("rvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t value_optional{10};
    std::move(value_optional).and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::rvalue);
  }
  {
    SCOPED_TRACE("const-rvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t const value_optional{10};
    std::move(value_optional).and_then(handle_value);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::crvalue);
  }
  {
    SCOPED_TRACE("lvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t value_optional{10};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    value_optional.and_then(std::move(handle_value));
    // NOLINTNEXTLINE(bugprone-use-after-move)
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::lvalue);
  }
  {
    SCOPED_TRACE("const-lvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t const value_optional{10};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    value_optional.and_then(std::move(handle_value));
    // NOLINTNEXTLINE(bugprone-use-after-move)
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::clvalue);
  }
  {
    SCOPED_TRACE("rvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t value_optional{10};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    std::move(value_optional).and_then(std::move(handle_value));
    // NOLINTNEXTLINE(bugprone-use-after-move)
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::rvalue);
  }
  {
    SCOPED_TRACE("const-rvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t const value_optional{10};
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    std::move(value_optional)
        .and_then(std::move(handle_value));  // NOLINT(hicpp-move-const-arg) testing crvalue overload
                                             // NOLINTNEXTLINE(bugprone-use-after-move)
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::crvalue);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.and_then(value_handler) when
/// @c optional.has_value() is @c true invokes @c value_handler with the return of @c *optional and its return is
/// the return of @c value_handler .
TEST(OptionalAndThen, TheVisitorIsCalledWithTheHeldValueAndReturnsAnOptionalProducedFromTheReturn) {
  auto const original_value = engaged_visitor::optional_t::value_type{10};
  engaged_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto value_optional = engaged_visitor::optional_t{original_value};
    EXPECT_EQ(value_optional.and_then(handle_value), original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const value_optional = engaged_visitor::optional_t{original_value};
    EXPECT_EQ(value_optional.and_then(handle_value), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    auto value_optional = engaged_visitor::optional_t{original_value};
    EXPECT_EQ(std::move(value_optional).and_then(handle_value), original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const value_optional = engaged_visitor::optional_t{original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(value_optional).and_then(handle_value), original_value);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.and_then(value_handler) when
/// @c optional.has_value() is @c false does not invoke @c value_handler .
TEST(OptionalAndThen, TheVisitorIsNotCalledIfOptionalIsNull) {
  {
    SCOPED_TRACE("lvalue");
    engaged_visitor handle_value;
    auto null_optional = engaged_visitor::optional_t{};
    null_optional.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-lvalue");
    engaged_visitor handle_value;
    auto const null_optional = engaged_visitor::optional_t{};
    null_optional.and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("rvalue");
    engaged_visitor handle_value;
    auto null_optional = engaged_visitor::optional_t{};
    std::move(null_optional).and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-rvalue");
    engaged_visitor handle_value;
    auto const null_optional = engaged_visitor::optional_t{};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    std::move(null_optional).and_then(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.and_then(value_handler) when
/// @c optional.has_value() is @c false returns a null optional.
TEST(OptionalAndThen, ANullOptionalIsReturnedIfTheOptionalIsNull) {
  engaged_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto null_optional = engaged_visitor::optional_t{};
    EXPECT_FALSE(null_optional.and_then(handle_value));
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const null_optional = engaged_visitor::optional_t{};
    EXPECT_FALSE(null_optional.and_then(handle_value));
  }
  {
    SCOPED_TRACE("rvalue");
    auto null_optional = engaged_visitor::optional_t{};
    EXPECT_FALSE(std::move(null_optional).and_then(handle_value));
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const null_optional = engaged_visitor::optional_t{};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_FALSE(std::move(null_optional).and_then(handle_value));
  }
}

/// @test The return type of @c arene::base::optional<T>::transform(value_handler) is @c arene::base::optional<U> ,
/// where @c U is the return type of @c value_handler .
TEST(OptionalTransform, ReturnTypeIsOptionalContainingReturnTypeFromHandler) {
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t&>().transform(std::declval<engaged_visitor>())),
        optional<engaged_visitor::return_optional_t>>();
  }
  {
    SCOPED_TRACE("const-lvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t const&>().transform(std::declval<engaged_visitor>())),
        optional<engaged_visitor::return_optional_t>>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t&&>().transform(std::declval<engaged_visitor>())),
        optional<engaged_visitor::return_optional_t>>();
  }
  {
    SCOPED_TRACE("const-rvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<engaged_visitor::optional_t const&&>().transform(std::declval<engaged_visitor>())),
        optional<engaged_visitor::return_optional_t>>();
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.transform(value_handler) when @c
/// optional.has_value() is @c true invokes @c value_handler with a type equivalent to the return type of
/// @c *optional.
TEST(OptionalTransform, CallsProvidedVisitorWithValueQualifiedAsResultIfValueIsPopulated) {
  {
    SCOPED_TRACE("lvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t value_optional{10};
    value_optional.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::lvalue);
  }
  {
    SCOPED_TRACE("const-lvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t const value_optional{10};
    value_optional.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::clvalue);
  }
  {
    SCOPED_TRACE("rvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t value_optional{10};
    std::move(value_optional).transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::rvalue);
  }
  {
    SCOPED_TRACE("const-rvalue");
    engaged_visitor handle_value;
    engaged_visitor::optional_t const value_optional{10};
    std::move(value_optional).transform(handle_value);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::crvalue);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.transform(value_handler) when
/// @c optional.has_value() is @c true invokes @c value_handler with the return of @c *optional and its return is
/// a new @c optional<U> in-place constructed from the return of @c value_handler .
TEST(OptionalTransform, TheVisitorIsCalledWithTheHeldValueAndReturnsAnOptionalProducedFromTheReturn) {
  auto const original_value = engaged_visitor::optional_t::value_type{10};
  engaged_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto value_optional = engaged_visitor::optional_t{original_value};
    EXPECT_EQ(value_optional.transform(handle_value), original_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const value_optional = engaged_visitor::optional_t{original_value};
    EXPECT_EQ(value_optional.transform(handle_value), original_value);
  }
  {
    SCOPED_TRACE("rvalue");
    auto value_optional = engaged_visitor::optional_t{original_value};
    EXPECT_EQ(std::move(value_optional).transform(handle_value), original_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const value_optional = engaged_visitor::optional_t{original_value};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(value_optional).transform(handle_value), original_value);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.transform(value_handler) when
/// @c optional.has_value() is @c false does not invoke @c value_handler .
TEST(OptionalTransform, TheVisitorIsNotCalledIfOptionalIsNull) {
  {
    SCOPED_TRACE("lvalue");
    engaged_visitor handle_value;
    auto null_optional = engaged_visitor::optional_t{};
    null_optional.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-lvalue");
    engaged_visitor handle_value;
    auto const null_optional = engaged_visitor::optional_t{};
    null_optional.transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("rvalue");
    engaged_visitor handle_value;
    auto null_optional = engaged_visitor::optional_t{};
    std::move(null_optional).transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-rvalue");
    engaged_visitor handle_value;
    auto const null_optional = engaged_visitor::optional_t{};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    std::move(null_optional).transform(handle_value);
    EXPECT_EQ(handle_value.selected_overload, engaged_visitor::parameter_qualification::not_called);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.transform(value_handler) when
/// @c optional.has_value() is @c false returns a null optional.
TEST(OptionalTransform, ANullOptionalIsReturnedIfTheOptionalIsNull) {
  engaged_visitor handle_value;
  {
    SCOPED_TRACE("lvalue");
    auto null_optional = engaged_visitor::optional_t{};
    EXPECT_FALSE(null_optional.transform(handle_value));
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const null_optional = engaged_visitor::optional_t{};
    EXPECT_FALSE(null_optional.transform(handle_value));
  }
  {
    SCOPED_TRACE("rvalue");
    auto null_optional = engaged_visitor::optional_t{};
    EXPECT_FALSE(std::move(null_optional).transform(handle_value));
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const null_optional = engaged_visitor::optional_t{};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_FALSE(std::move(null_optional).transform(handle_value));
  }
}

struct nullopt_visitor : visitor_base {
  optional_t::value_type const return_value = 11;
  ///
  /// @brief Overloads for introspecting a visited optional.
  /// @return optional_t containing return_value.
  /// @post selected_overload will be set to a value other than not_called
  /// @{
  auto operator()() -> optional_t {
    selected_overload = parameter_qualification::lvalue;
    return {return_value};
  }
};
/// @test The return type of @c arene::base::optional<T>::or_else(nullopt_handler) is @c arene::base::optional<U> ,
/// which is the return type of @c nullopt_handler .
TEST(OptionalOrElse, ReturnTypeIsReturnTypeFromHandler) {
  {
    SCOPED_TRACE("lvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<nullopt_visitor::optional_t&>().or_else(std::declval<nullopt_visitor>())),
        nullopt_visitor::optional_t>();
  }
  {
    SCOPED_TRACE("const-lvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<nullopt_visitor::optional_t const&>().or_else(std::declval<nullopt_visitor>())),
        nullopt_visitor::optional_t>();
  }
  {
    SCOPED_TRACE("rvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<nullopt_visitor::optional_t&&>().or_else(std::declval<nullopt_visitor>())),
        nullopt_visitor::optional_t>();
  }
  {
    SCOPED_TRACE("const-rvalue");
    ::testing::StaticAssertTypeEq<
        decltype(std::declval<nullopt_visitor::optional_t const&&>().or_else(std::declval<nullopt_visitor>())),
        nullopt_visitor::optional_t>();
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.or_else(nullopt_handler) when
/// @c optional.has_value() is @c false invokes @c nullopt_handler .
TEST(OptionalOrElse, IfOptionalIsNullTheVisitorIsCalled) {
  {
    SCOPED_TRACE("lvalue");
    nullopt_visitor handle_null;
    nullopt_visitor::optional_t null_optional{};  // NOLINT(misc-const-correctness) testing non-const lvalue overload
    null_optional.or_else(handle_null);
    EXPECT_NE(handle_null.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-lvalue");
    nullopt_visitor handle_null;
    nullopt_visitor::optional_t const null_optional{};
    null_optional.or_else(handle_null);
    EXPECT_NE(handle_null.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("rvalue");
    nullopt_visitor handle_null;
    nullopt_visitor::optional_t null_optional{};
    std::move(null_optional).or_else(handle_null);
    EXPECT_NE(handle_null.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-rvalue");
    nullopt_visitor handle_null;
    nullopt_visitor::optional_t const null_optional{};
    std::move(null_optional).or_else(handle_null);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_NE(handle_null.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.or_else(nullopt_handler) when
/// @c optional.has_value() is @c false invokes @c nullopt_handler and its return is the return from @c nullopt_handler.
TEST(OptionalOrElse, IfOptionalIsNullTheReturnFromTheVisitorIsReturned) {
  nullopt_visitor handle_null;
  {
    SCOPED_TRACE("lvalue");
    auto null_optional = nullopt_visitor::optional_t{};
    EXPECT_EQ(null_optional.or_else(handle_null), handle_null.return_value);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const null_optional = nullopt_visitor::optional_t{};
    EXPECT_EQ(null_optional.or_else(handle_null), handle_null.return_value);
  }
  {
    SCOPED_TRACE("rvalue");
    auto null_optional = engaged_visitor::optional_t{};
    EXPECT_EQ(std::move(null_optional).or_else(handle_null), handle_null.return_value);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const null_optional = engaged_visitor::optional_t{};
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(null_optional).or_else(handle_null), handle_null.return_value);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.or_else(nullopt_handler) when
/// @c optional.has_value() is @c true does not invoke @c nullopt_handler .
TEST(OptionalOrElse, IfOptionalIsNotNullTheVisitorIsNotCalled) {
  {
    SCOPED_TRACE("lvalue");
    nullopt_visitor handle_null;
    nullopt_visitor::optional_t value_optional{10};  // NOLINT(misc-const-correctness) testing non-const lvalue overload
    value_optional.or_else(handle_null);
    EXPECT_EQ(handle_null.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-lvalue");
    nullopt_visitor handle_value;
    nullopt_visitor::optional_t const value_optional{10};
    value_optional.or_else(handle_value);
    EXPECT_EQ(handle_value.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("rvalue");
    nullopt_visitor handle_value;
    nullopt_visitor::optional_t value_optional{10};
    std::move(value_optional).or_else(handle_value);
    EXPECT_EQ(handle_value.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
  {
    SCOPED_TRACE("const-rvalue");
    nullopt_visitor handle_value;
    nullopt_visitor::optional_t const value_optional{10};
    std::move(value_optional).or_else(handle_value);  // NOLINT(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(handle_value.selected_overload, nullopt_visitor::parameter_qualification::not_called);
  }
}
/// @test Given an instance of @c arene::base::optional<T> , invoking @c optional.or_else(nullopt_handler) when
/// @c optional.has_value() is @c true returns a null optional.
TEST(OptionalOrElse, IfOptionalIsNotNullTheOptionalIsReturned) {
  nullopt_visitor handle_null;
  auto const original_optional = nullopt_visitor::optional_t{10};
  {
    SCOPED_TRACE("lvalue");
    auto value_optional = original_optional;  // NOLINT(performance-unnecessary-copy-initialization)
    EXPECT_EQ(value_optional.or_else(handle_null), original_optional);
  }
  {
    SCOPED_TRACE("const-lvalue");
    auto const value_optional = original_optional;  // NOLINT(performance-unnecessary-copy-initialization)
    EXPECT_EQ(value_optional.or_else(handle_null), original_optional);
  }
  {
    SCOPED_TRACE("rvalue");
    auto value_optional = original_optional;
    EXPECT_EQ(std::move(value_optional).or_else(handle_null), original_optional);
  }
  {
    SCOPED_TRACE("const-rvalue");
    auto const value_optional = original_optional;  // NOLINT(performance-unnecessary-copy-initialization)
    // NOLINTNEXTLINE(hicpp-move-const-arg) testing crvalue overload
    EXPECT_EQ(std::move(value_optional).or_else(handle_null), original_optional);
  }
}

/// @test Validates that @c arene::base::optional<T> can be placed in a standard container if @c T can be placed in a
/// standard container.
TEST(Optional, OptionalInStandardContainers) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  std::vector<arene::base::optional<std::int32_t>> vec{1, 2, 3, {}, 4};

  std::size_t count{0};
  for (auto& elem : vec) {
    if (elem) {
      ++count;
      ++*elem;
    }
  }
  ASSERT_EQ(count, 4);
#else
  GTEST_SKIP() << "std::vector is not supported by Arene Base standard library";
#endif
}

/// @test An `optional` still has working equality operators even when its `value_type` has broken comparisons.
TEST(Optional, EqualityComparisonWithBrokenLessThan) {
  constexpr arene::base::optional<testing::has_broken_less_than> empty{};
  constexpr arene::base::optional<testing::has_broken_less_than> zero{0};
  constexpr arene::base::optional<testing::has_broken_less_than> five{5};

  STATIC_ASSERT_EQ(empty, empty);
  STATIC_ASSERT_EQ(zero, zero);
  STATIC_ASSERT_EQ(five, five);

  STATIC_ASSERT_NE(empty, zero);
  STATIC_ASSERT_NE(five, empty);
  STATIC_ASSERT_NE(zero, five);
}

/// @test Validates that @c arene::base::optional<T> can be placed in an arene-base container if @c T can be placed in
/// an arene-base container.
TEST(Optional, OptionalInAreneBaseContainers) {
  arene::base::array<arene::base::optional<std::int32_t>, 5> arr{1, 2, 3, {}, 4};

  std::size_t count{0};
  for (auto& elem : arr) {
    if (elem) {
      ++count;
      ++*elem;
    }
  }
  ASSERT_EQ(count, 4);
}

template <typename OptionalT>
using use_optional_value = decltype(std::declval<OptionalT>().value());

template <typename OptionalT>
constexpr bool is_value_invocable_v = arene::base::substitution_succeeds<use_optional_value, OptionalT>;

/// @test Given `optional<T>`, then `optional<T>::value()`'s invocability matches `detail::are_exceptions_enabled_v`.
TEST(Optional, ValueIsInvocableOnlyIfExceptionsEnabled) {
  STATIC_ASSERT_EQ(is_value_invocable_v<optional<int>&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(is_value_invocable_v<optional<int> const&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(is_value_invocable_v<optional<int>&&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(is_value_invocable_v<optional<int> const&&>, arene::base::detail::are_exceptions_enabled_v);
}

}  // namespace
