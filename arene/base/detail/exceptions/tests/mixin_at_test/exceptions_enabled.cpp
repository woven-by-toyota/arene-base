// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/detail/exceptions/are_exceptions_enabled.hpp"
#include "arene/base/detail/exceptions/tests/mixin_at_test/simple_container.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

static_assert(
    arene::base::detail::are_exceptions_enabled_v,
    "Tests in this file are only valid if exceptions are enabled"
);

using ::mixin_at_test::container_t;
using ::mixin_at_test::disabled_qualification;
using ::mixin_at_test::size_wrapper;
using ::mixin_at_test::types;

template <typename TypeAndSize>
class MixinAtExceptionsEnabledTest : public ::testing::Test {};

TYPED_TEST_SUITE(MixinAtExceptionsEnabledTest, types, );

template <typename Container>
using use_index_operator = decltype(std::declval<Container>()[0U]);
template <typename Container>
constexpr bool has_index_operator_v = ::arene::base::substitution_succeeds<use_index_operator, Container>;

template <typename Container>
using use_member_at = decltype(std::declval<Container>().at(0U));
template <typename Container>
constexpr bool has_member_at_v = ::arene::base::substitution_succeeds<use_member_at, Container>;

/// @test Given a container type `Container` that mixes in `mixin_at<Container,IndexType>` and
/// `are_exceptions_enabled_v` is true, then `Container::at(IndexType)` is declared if
/// `Container::operator[](IndexType)` is declared.
TYPED_TEST(MixinAtExceptionsEnabledTest, AtIsDeclaredIfIndexOperatorIsDeclared) {
  STATIC_ASSERT_EQ(
      (has_index_operator_v<container_t<TypeParam, disabled_qualification::lvalue>>),
      (has_member_at_v<container_t<TypeParam, disabled_qualification::lvalue>>)
  );
  STATIC_ASSERT_EQ(
      (has_index_operator_v<container_t<TypeParam, disabled_qualification::const_lvalue>>),
      (has_member_at_v<container_t<TypeParam, disabled_qualification::const_lvalue>>)
  );
  STATIC_ASSERT_EQ(
      (has_index_operator_v<container_t<TypeParam, disabled_qualification::rvalue>>),
      (has_member_at_v<container_t<TypeParam, disabled_qualification::rvalue>>)
  );
  STATIC_ASSERT_EQ(
      (has_index_operator_v<container_t<TypeParam, disabled_qualification::const_rvalue>>),
      (has_member_at_v<container_t<TypeParam, disabled_qualification::const_rvalue>>)
  );
}

/// @test Given a container type `Container` that mixes in `mixin_at<Container,IndexType>` and
/// `are_exceptions_enabled_v` is true, then `Container::at(IndexType)` is defined and returns a type equivalent to
/// `Container::operator[](IndexType)`.
TYPED_TEST(MixinAtExceptionsEnabledTest, AtReturnsEquivalentTypeToIndexOperator) {
  using container_type = container_t<TypeParam>;
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<container_type&>().at(0U)),
      decltype(std::declval<container_type&>()[0U])>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<container_type const&>().at(0U)),
      decltype(std::declval<container_type const&>()[0U])>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<container_type&&>().at(0U)),
      decltype(std::declval<container_type&&>()[0U])>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<container_type const&&>().at(0U)),
      decltype(std::declval<container_type const&&>()[0U])>();
}

/// @test Given an instance `container` of a type `Container` that mixes in `mixin_at<Container,IndexType>` and
/// `are_exceptions_enabled_v` is true, when `container.at(idx)` is invoked with `idx<container.size()`, then
/// `container.at(idx)` is equivalent to `container[idx]`.
CONSTEXPR_TYPED_TEST(MixinAtExceptionsEnabledTest, EquivalentToIndexOperatorIfIndexLessThanSize) {
  using container_type = container_t<TypeParam>;
  auto container = container_type{};
  // NOLINTBEGIN(bugprone-use-after-move) Non-destructive move, needed to test overload resolution.
  for (auto idx = 0U; idx < container.size(); ++idx) {
    CONSTEXPR_ASSERT_EQ(container.at(idx), container[idx]);
  }
  for (auto idx = 0U; idx < container.size(); ++idx) {
    CONSTEXPR_ASSERT_EQ(std::move(container).at(idx), std::move(container)[idx]);
  }
  container_type const& const_container = container;
  for (auto idx = 0U; idx < const_container.size(); ++idx) {
    CONSTEXPR_ASSERT_EQ(const_container.at(idx), const_container[idx]);
  }
  for (auto idx = 0U; idx < const_container.size(); ++idx) {
    CONSTEXPR_ASSERT_EQ(std::move(const_container).at(idx), std::move(const_container)[idx]);
  }
  // NOLINTEND(bugprone-use-after-move)
}

/// @test Given an instance `container` of a type `Container` that mixes in `mixin_at<Container,IndexType>` and
/// `are_exceptions_enabled_v` is true, when `container.at(idx)` is invoked with `idx>=container.size()`, then
/// `std::out_of_range` is thrown.
TYPED_TEST(MixinAtExceptionsEnabledTest, ThrowsOutOfRangeIfIndexGreaterThanSize) {
  using container_type = container_t<TypeParam>;
  auto container = container_type{};
  // we have to explicitly capture container.size() to avoid use-after-move errors. We can't just capture it once as a
  // variable outside the scope of the lambdas because clang and gcc disagree on if it's required to explicitly capture
  // an integer name; gcc errors if you don't capture it, clang errors if you do.
  ASSERT_THROW(([&container, size = container.size()] { container.at(size); })(), std::out_of_range);
  ASSERT_THROW(([&container, size = container.size()] { container.at(size + 1); })(), std::out_of_range);

  ASSERT_THROW(([&container, size = container.size()] { std::move(container).at(size); })(), std::out_of_range);
  ASSERT_THROW(([&container, size = container.size()] { std::move(container).at(size + 1); })(), std::out_of_range);

  container_type const const_container = container;
  ASSERT_THROW(([&const_container, size = container.size()] { const_container.at(size); })(), std::out_of_range);
  ASSERT_THROW(([&const_container, size = container.size()] { const_container.at(size + 1); })(), std::out_of_range);

  ASSERT_THROW(
      ([&const_container, size = container.size()] { std::move(const_container).at(size); })(),
      std::out_of_range
  );
  ASSERT_THROW(
      ([&const_container, size = container.size()] { std::move(const_container).at(size + 1); })(),
      std::out_of_range
  );
}

}  // namespace
