// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mutex/detail/has_native_handle.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::substitution_succeeds;
using ::arene::base::mutex_detail::has_native_handle;
using ::arene::base::mutex_detail::mutex_native_handle_member_mixin;

struct without_native_handle_member {};

struct with_native_handle_member {
  using native_handle_t = int;
  auto native_handle() -> native_handle_t;
};

/// @test `has_native_handle` is `true` for a class that has a `native_handle` member function
TEST(HasNativeHandle, IsTrueForClassWithNativeHandleMember) {
  STATIC_ASSERT_TRUE(has_native_handle<with_native_handle_member>);
}

/// @test `has_native_handle` is `false` for a class that does not have a `native_handle` member function
TEST(HasNativeHandle, IsFalseForClassWithoutNativeHandleMember) {
  STATIC_ASSERT_FALSE(has_native_handle<without_native_handle_member>);
}

template <typename M>
using use_native_handle_member = typename M::native_handle_t;

/// @test `mutex_native_handle_member_mixin` has a native handle if the template argument does
TEST(MutexNativeHandleMemberMixin, PassesThroughNativeHandleTMemberIfMutexHasOne) {
  using mixed_in_native_handle = mutex_native_handle_member_mixin<with_native_handle_member>;
  STATIC_ASSERT_TRUE((substitution_succeeds<use_native_handle_member, mixed_in_native_handle>));
  ::testing::StaticAssertTypeEq<mixed_in_native_handle::native_handle_t, with_native_handle_member::native_handle_t>();
}

/// @test `mutex_native_handle_member_mixin` does not have a native handle if the template argument does not
TEST(MutexNativeHandleMemberMixin, HasNoNativeHandleTMemberIfMutexDoesNot) {
  using mixed_in_native_handle = mutex_native_handle_member_mixin<without_native_handle_member>;
  STATIC_ASSERT_FALSE((substitution_succeeds<use_native_handle_member, mixed_in_native_handle>));
}

}  // namespace
