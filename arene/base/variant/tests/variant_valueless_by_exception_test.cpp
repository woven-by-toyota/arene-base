// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/variant/detail/bad_variant_access_exceptions_enabled.hpp"
#include "arene/base/variant/traits.hpp"
#include "arene/base/variant/variant.hpp"

namespace {

class throw_when {
 public:
  enum type { never, constructed, moved, copied, moved_or_copied };
  type when_{never};

  throw_when() = default;
  explicit throw_when(type when)
      : when_{when} {
    if (when_ == constructed) {
      throw std::runtime_error{"Construct"};
    }
  }
  ~throw_when() = default;
  // NOLINTNEXTLINE(bugprone-exception-escape,hicpp-noexcept-move)
  throw_when(throw_when const& other)
      : when_{other.when_} {
    if (when_ == copied || when_ == moved_or_copied) {
      throw std::runtime_error{"CopyThrow"};
    }
  }
  // NOLINTNEXTLINE(bugprone-exception-escape,hicpp-noexcept-move)
  throw_when(throw_when&& other)
      : when_{other.when_} {
    if (when_ == moved || when_ == moved_or_copied) {
      throw std::runtime_error{"MoveThrow"};
    }
  }
  // NOLINTNEXTLINE(bugprone-exception-escape,hicpp-noexcept-move)
  auto operator=(throw_when const& rhs) -> throw_when& {
    if (when_ == copied || when_ == moved_or_copied) {
      throw std::runtime_error{"CopyThrow"};
    }
    when_ = rhs.when_;
    return *this;
  }
  auto operator==(throw_when const&) const -> bool { return true; }
  auto operator!=(throw_when const&) const -> bool { return true; }
  auto operator<(throw_when const&) const -> bool { return true; }
  auto operator>(throw_when const&) const -> bool { return true; }
  // NOLINTNEXTLINE(bugprone-exception-escape,hicpp-noexcept-move)
  auto operator=(throw_when&& rhs) -> throw_when& {
    if (when_ == moved || when_ == moved_or_copied) {
      throw std::runtime_error{"MoveThrow"};
    }
    when_ = rhs.when_;
    return *this;
  }
};

/// @test If an exception is thrown during `emplace` then the `variant` is valueless-by-exception
TEST(Variant, ValuelessByException) {
  // Copy from valueless_by_exception variant
  {
    using variant_type = arene::base::variant<throw_when, std::int32_t>;
    variant_type value;
    ASSERT_THROW(value.emplace<0>(throw_when{throw_when::moved}), std::runtime_error);
    EXPECT_EQ(value.index(), arene::base::variant_npos);
    EXPECT_TRUE(value.valueless_by_exception());

    variant_type const value2{value};
    EXPECT_TRUE(value2.valueless_by_exception());
  }

  // Exception happens in @c emplace(T&& value)
  {
    using variant_type = arene::base::variant<throw_when, std::int32_t>;
    variant_type value;
    ASSERT_THROW(value.emplace<0>(throw_when{throw_when::moved}), std::runtime_error);
    EXPECT_EQ(value.index(), arene::base::variant_npos);
    EXPECT_TRUE(value.valueless_by_exception());
    ASSERT_THROW(value.get<0>(), arene::base::bad_variant_access);
    ASSERT_THROW(value.get<1>(), arene::base::bad_variant_access);
  }

  // Exception happens in @c emplace(const T& value)
  {
    using variant_type = arene::base::variant<throw_when, std::int32_t>;
    variant_type value;
    auto const non_copyable = throw_when{throw_when::copied};
    ASSERT_THROW(value.emplace<0>(non_copyable), std::runtime_error);
    EXPECT_EQ(value.index(), arene::base::variant_npos);
    EXPECT_TRUE(value.valueless_by_exception());
    ASSERT_THROW(value.get<0>(), arene::base::bad_variant_access);
    ASSERT_THROW(value.get<1>(), arene::base::bad_variant_access);
  }
}

/// @test Copy-assigning a `variant` that is valueless-by-exception makes the target also valueless-by-exception
TEST(Variant, CopyAssignmentValueless) {
  using variant_type = arene::base::variant<throw_when>;
  variant_type value{};
  // Make value valueless
  throw_when source{throw_when::moved};
  ASSERT_THROW(value.emplace<0>(std::move(source)), std::runtime_error);
  EXPECT_EQ(value.index(), arene::base::variant_npos);
  EXPECT_TRUE(value.valueless_by_exception());

  variant_type value2;
  value2 = value;
  EXPECT_TRUE(value2.valueless_by_exception());
}

/// @test Move-assignment of a value-less `variant` leaves the target also valueless
TEST(Variant, MoveAssignmentValueless) {
  using variant_type = arene::base::variant<throw_when>;
  variant_type value;
  // Make value valueless
  ASSERT_THROW(value.emplace<0>(throw_when{throw_when::moved}), std::runtime_error);
  EXPECT_EQ(value.index(), arene::base::variant_npos);
  EXPECT_TRUE(value.valueless_by_exception());

  variant_type value2;
  value2 = std::move(value);
  EXPECT_TRUE(value2.valueless_by_exception());
}

/// @test A @c variant that is valueless-by-exception is equal to another valueless variant, but not equal to a variant
/// with a value
TEST(Variant, CompareValuelessWithValueless) {
  using variant_type = arene::base::variant<throw_when, std::int32_t>;
  variant_type value;
  // Make value valueless
  ASSERT_THROW(value.emplace<0>(throw_when{throw_when::moved}), std::runtime_error);
  EXPECT_EQ(value.index(), arene::base::variant_npos);
  EXPECT_TRUE(value.valueless_by_exception());

  variant_type value2{42};
  EXPECT_NE(value, value2);
  value2 = value;
  EXPECT_EQ(value, value2);
}

/// @test Given two instances of `arene::base::variant<Ts...>` where both instance are valueless-by-exception, when they
/// are compared by all comparison operators, then they are considered equal.
TEST(Variant, AValuelessByExceptionVariantComparesEqualToAnotherValuelessByExceptionVariant) {
  using variant_type = arene::base::variant<throw_when, std::int32_t>;

  variant_type valueless_by_exception1;
  variant_type valueless_by_exception2;
  // Make the values valueless
  ASSERT_THROW(valueless_by_exception1.emplace<0>(throw_when{throw_when::moved}), std::runtime_error);
  ASSERT_THROW(valueless_by_exception2.emplace<0>(throw_when{throw_when::moved}), std::runtime_error);

  EXPECT_TRUE(valueless_by_exception1 == valueless_by_exception2);
  EXPECT_FALSE(valueless_by_exception1 != valueless_by_exception2);
  EXPECT_FALSE(valueless_by_exception1 < valueless_by_exception2);
  EXPECT_FALSE(valueless_by_exception1 > valueless_by_exception2);
  EXPECT_TRUE(valueless_by_exception1 <= valueless_by_exception2);
  EXPECT_TRUE(valueless_by_exception1 >= valueless_by_exception2);
}

/// @test Given two instances of `arene::base::variant<Ts...>` where one instance is valueless-by-exception and the
/// other is not, when they are compared by the relational comparison operators, then the valueless-by-exception
/// instance is always considered to be less than the other instance.
TEST(Variant, AValuelessByExceptionVariantComparesLessThanAValuedVariant) {
  using variant_type = arene::base::variant<throw_when, std::int32_t>;

  variant_type valueless_by_exception;
  // Make value valueless
  ASSERT_THROW(valueless_by_exception.emplace<0>(throw_when{throw_when::moved}), std::runtime_error);

  variant_type const valued_variant{42};

  EXPECT_FALSE(valueless_by_exception == valued_variant);
  EXPECT_TRUE(valueless_by_exception != valued_variant);
  EXPECT_TRUE(valueless_by_exception < valued_variant);
  EXPECT_FALSE(valueless_by_exception > valued_variant);
  EXPECT_TRUE(valueless_by_exception <= valued_variant);
  EXPECT_FALSE(valueless_by_exception >= valued_variant);
}

/// @test A `variant` holding a single type that could potentially throw can be copied and moved, and compared
TEST(VariantCoverage, InstantiateAndRunCorrectlyMethodsForVariantWithThrowWhenOnly) {
  using variant_type = arene::base::variant<throw_when>;

  variant_type const source{throw_when{throw_when::never}};
  ASSERT_EQ(source.index(), 0);

  variant_type copied_one{source};  // copy
  ASSERT_EQ(copied_one.index(), 0);

  variant_type moved_one{std::move(copied_one)};  // move
  ASSERT_EQ(moved_one.index(), 0);

  variant_type copy_assigned{throw_when{throw_when::never}};
  copy_assigned = source;
  ASSERT_EQ(copy_assigned.index(), 0);

  variant_type move_assigned{throw_when{throw_when::never}};
  move_assigned = std::move(moved_one);
  ASSERT_EQ(move_assigned.index(), 0);

  // check for comparisons (both equal and less_than), they are not mean anything for this type
  // NOLINTNEXTLINE(bugprone-use-after-move)
  (void)(source == moved_one);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  (void)(source < moved_one);

  ASSERT_NO_THROW(source.get<0>());
  ASSERT_NO_THROW(moved_one.get<0>());
}

/// @test A `variant` type that could be either a type with potentially throwing operations, or an integer, can be
/// correctly copied and moved, whichever type it actually holds
TEST(VariantCoverage, InstantiateAndRunCorrectlyMethodsForVariantWithThrowWhen) {
  using variant_type = arene::base::variant<throw_when, std::int32_t>;

  {  // throw_when type
    variant_type const source{throw_when{throw_when::never}};
    ASSERT_EQ(source.index(), 0);

    variant_type copied_one{source};  // copy
    ASSERT_EQ(copied_one.index(), 0);

    variant_type moved_one{std::move(copied_one)};  // move
    ASSERT_EQ(moved_one.index(), 0);

    variant_type copy_assigned{throw_when::copied};  // will never throw, as it will be overwriten by other one
    copy_assigned = source;
    ASSERT_EQ(copy_assigned.index(), 0);

    variant_type move_assigned{throw_when::moved};  // will never throw, as it will be overwriten by other one
    move_assigned = std::move(moved_one);
    ASSERT_EQ(move_assigned.index(), 0);

    // check for comparisons (both equal and less_than), they are not mean anything for this type
    // NOLINTNEXTLINE(bugprone-use-after-move)
    (void)(source == moved_one);
    // NOLINTNEXTLINE(bugprone-use-after-move)
    (void)(source < moved_one);
  }

  {  // int32_t
    variant_type const source{42};
    ASSERT_EQ(source.index(), 1);

    variant_type copied_one{source};  // copy
    ASSERT_EQ(copied_one.index(), 1);

    variant_type moved_one{std::move(copied_one)};  // move
    ASSERT_EQ(moved_one.index(), 1);

    variant_type copy_assigned{2};  // will never throw, as it will be overwriten by other one
    copy_assigned = source;
    ASSERT_EQ(copy_assigned.index(), 1);

    variant_type move_assigned{3};  // will never throw, as it will be overwriten by other one
    move_assigned = std::move(moved_one);
    ASSERT_EQ(move_assigned.index(), 1);

    // check for comparisons (both equal and less_than), they are not mean anything for this type
    EXPECT_EQ(source, move_assigned);
    ASSERT_LE(move_assigned, source);
  }
}

}  // namespace
