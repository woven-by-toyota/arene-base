// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/bind_overloads.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/optional/optional_resetter.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using ::arene::base::bind_overloads;

/// @test Given a visitor created with `bind_overloads`, then the resulting visitor is invocable with the same
/// combinations of arguments and return types as the input invocables.
TEST(MakeVisitor, VisitorIsInvocableWithSameSignaturesAsInputs) {
  auto const visitor = bind_overloads([](int) -> float { return {}; }, [](int*) -> int* { return {}; });
  STATIC_ASSERT_TRUE(::arene::base::is_invocable_r_v<float, decltype(visitor), int>);
  STATIC_ASSERT_TRUE(::arene::base::is_invocable_r_v<int*, decltype(visitor), int*>);
}

/// @test Given a visitor created with `bind_overloads`, when the visitor is invoked with an argument, then the input
/// invocable matching that argument is invoked.
TEST(MakeVisitor, InvokingTheVisitorInvokesTheAppropriateOverload) {
  ::arene::base::optional<int> int_called;
  ::arene::base::optional<int const*> int_ptr_called;
  ::arene::base::optional<int> int_rvalue_called;
  auto const visitor = bind_overloads(
      [&int_called](int const& val) -> int {
        int_called = val;
        return val;
      },
      [&int_ptr_called](int const* val_ptr) -> int const* {
        int_ptr_called = val_ptr;
        return val_ptr;
      },
      [&int_rvalue_called](int&& val_rvalue) -> int {
        int_rvalue_called = val_rvalue;
        return val_rvalue;
      }
  );

  int const int_val = 10;
  EXPECT_EQ(visitor(int_val), int_val);
  EXPECT_EQ(int_called, int_val);

  EXPECT_EQ(visitor(&int_val), &int_val);
  EXPECT_EQ(int_ptr_called, &int_val);

  EXPECT_EQ(visitor(10), 10);
  EXPECT_EQ(int_rvalue_called, 10);
}

/// @test When `bind_overloads` is invoked with a callable, the callable is perfectly forwarded into the visitor.
TEST(MakeVisitor, CallablesArePerfectlyForwardedIntoVisitor) {
  struct counts {
    int copy_count = 0;
    int move_count = 0;
  };
  class counting_callable {
    counts* counts_;

   public:
    explicit counting_callable(counts* counts)
        : counts_(counts) {}
    counting_callable(counting_callable const& copy)
        : counts_(copy.counts_) {
      ++counts_->copy_count;
    }
    counting_callable(counting_callable&& move) noexcept
        : counts_(move.counts_) {
      ++counts_->move_count;
    }
    auto operator=(counting_callable const&) -> counting_callable& = delete;
    auto operator=(counting_callable&&) -> counting_callable& = delete;
    ~counting_callable() = default;

    ARENE_MAYBE_UNUSED auto operator()(int) const -> int { return counts_->copy_count; }
    ARENE_MAYBE_UNUSED auto operator()(float) const -> int { return counts_->move_count; }
  };

  counts ctor_counts{};
  std::ignore = bind_overloads(counting_callable{&ctor_counts});
  EXPECT_EQ(ctor_counts.copy_count, 0);
  EXPECT_EQ(ctor_counts.move_count, 1);

  ctor_counts = {};
  counting_callable const will_be_copied{&ctor_counts};
  std::ignore = bind_overloads(will_be_copied);
  EXPECT_EQ(ctor_counts.copy_count, 1);
  EXPECT_EQ(ctor_counts.move_count, 0);
}

}  // namespace
