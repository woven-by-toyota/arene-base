// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/constraints.hpp"
#include "arene/base/detail/exceptions/are_exceptions_enabled.hpp"
#include "arene/base/detail/exceptions/tests/mixin_at_test/simple_container.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::mixin_at_test::container_t;
using ::mixin_at_test::size_wrapper;
using ::mixin_at_test::types;

static_assert(
    !arene::base::detail::are_exceptions_enabled_v,
    "Tests in this file are only valid if exceptions are disabled"
);

template <typename ContainerType>
using use_member_at = decltype(std::declval<ContainerType>().at(0U));

template <typename ContainerType>
constexpr bool has_member_at_v = arene::base::substitution_succeeds<use_member_at, ContainerType>;

template <typename TypeAndSize>
class MixinAtExceptionsDisabledTest : public ::testing::Test {};

TYPED_TEST_SUITE(MixinAtExceptionsDisabledTest, types, );

/// @test Given a container type `Container` that mixes in `mixin_at<Container,IndexType>` and
/// `are_exceptions_enabled_v` is false, then `Container::at(IndexType)` is not defined.
TYPED_TEST(MixinAtExceptionsDisabledTest, HasNoMemberAt) {
  using container_type = container_t<TypeParam>;
  STATIC_ASSERT_FALSE(has_member_at_v<container_type&>);
  STATIC_ASSERT_FALSE(has_member_at_v<container_type const&>);
  STATIC_ASSERT_FALSE(has_member_at_v<container_type&&>);
  STATIC_ASSERT_FALSE(has_member_at_v<container_type const&&>);
}

}  // namespace
