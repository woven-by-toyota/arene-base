// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/is_accessor_policy.hpp"

#include <gtest/gtest.h>

#include "arene/base/mdspan/default_accessor.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/testing/gtest.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using ::arene::base::is_accessor_policy_v;

struct valid_offset_policy : arene::base::default_accessor<int> {
  // Constructible from any policy
  template <class T>
  explicit constexpr valid_offset_policy(T const&) {}
};

/// @test Given an input type `Policy`, when all of `Policy::element_type`, `Policy::data_handle_type`,
/// `Policy::reference`, `Policy::offset_policy`, and `Policy::offset` and `Policy::accessor` are defined, then
/// `is_accessor_policy_v<Policy>` is `true`
TEST(IsValidAccessorPolicyTest, IsTrueIfTheAccessorDefinesAllNeededTypedefsAndFunctions) {
  struct valid_accessor_policy {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_TRUE(is_accessor_policy_v<valid_accessor_policy>);
  STATIC_ASSERT_TRUE(is_accessor_policy_v<arene::base::default_accessor<int>>);
}

/// @test Given an input type `Policy`, if `Policy::element_type` is not defined, then `is_accessor_policy<Policy>` is
/// `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfElementTypeIsNotDefined) {
  struct no_element_type {
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<no_element_type>);
}

/// @test Given an input type `Policy`, when `Policy::element_type` is not an object, then
/// `is_accessor_policy_v<Policy>` is `false`
TEST(IsValidAccessorPolicyTest, IsFalseIfElementTypeIsNotAnObject) {
  struct not_object_element_type {
    using element_type = int&;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<not_object_element_type>);
}

/// @test Given an input type `Policy`, when `Policy::element_type` is an abstract base class, then
/// `is_accessor_policy_v<Policy>` is `false`
TEST(IsValidAccessorPolicyTest, IsFalseIfElementTypeIsAbstract) {
  // NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor)
  struct abstract_base {
    virtual void foo() = 0;
  };

  struct abstract_element_type {
    using element_type = abstract_base;
    using data_handle_type = abstract_base*;
    using reference = abstract_base&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<abstract_element_type>);
}

/// @test Given an input type `Policy`, if `Policy::data_handle_type` is not defined, then `is_accessor_policy<Policy>`
/// is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfDataHandleTypeIsNotDefined) {
  struct no_data_handle_type {
    using element_type = int;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(int* handle, std::size_t index) const -> reference;
    constexpr auto offset(int* handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<no_data_handle_type>);
}

/// @test Given an input type `Policy`, if `Policy::data_handle_type` is not copyable, then `is_accessor_policy<Policy>`
/// is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfDataHandleIsNotCopyable) {
  using not_copyable = testing::configurable_value<int, testing::throws_on::nothing, testing::disable::copy_construct>;

  struct non_copyable_data_handle {
    using element_type = int;
    using reference = int&;
    using data_handle_type = not_copyable;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type const& handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type const& handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<non_copyable_data_handle>);
}

/// @test Given an input type `Policy`, if `Policy::data_handle_type` is not nothrow move constructible, then
/// `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfDataHandleIsNotNothrowMoveConstructible) {
  using not_nothrow_move_constructible = testing::configurable_value<int, testing::throws_on::move_construct>;

  struct non_nothrow_move_constructible_data_handle {
    using element_type = int;
    using reference = int&;
    using data_handle_type = not_nothrow_move_constructible;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type const& handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type const& handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<non_nothrow_move_constructible_data_handle>);
}

/// @test Given an input type `Policy`, if `Policy::data_handle_type` is not nothrow move assignable, then
/// `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfDataHandleIsNotNothrowMoveAssignable) {
  using not_nothrow_move_assignable = testing::configurable_value<int, testing::throws_on::move_assign>;

  struct non_nothrow_move_assignable_data_handle {
    using element_type = int;
    using reference = int&;
    using data_handle_type = not_nothrow_move_assignable;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type const& handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type const& handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<non_nothrow_move_assignable_data_handle>);
}

/// @test Given an input type `Policy`, if `Policy::data_handle_type` is not nothrow move assignable, then
/// `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfDataHandleIsNotNothrowSwappable) {
  using throwing_swap_data_handle = testing::configurable_value<int, testing::throws_on::swap>;

  struct non_nothrow_swappable_data_handle {
    using element_type = int;
    using reference = int&;
    using data_handle_type = throwing_swap_data_handle;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type const& handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type const& handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<non_nothrow_swappable_data_handle>);
}

/// @test Given an input type `Policy`, if `Policy::reference` is not defined, then `is_accessor_policy<Policy>` is
/// `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfReferenceIsNotDefined) {
  struct no_reference {
    using element_type = int;
    using data_handle_type = int*;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> int&;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<no_reference>);
}

/// @test Given an input type `Policy`, if `Policy::offest_policy` is not defined, then `is_accessor_policy<Policy>` is
/// `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfOffsetPolicyIsNotDefined) {
  struct no_offset_policy {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<no_offset_policy>);
}

/// @test Given an input type `Policy`, if `Policy::offest_policy` is not constructible from `Policy`, then
/// `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfOffsetPolicyIsNotConstructibleFromPolicy) {
  struct offset_policy_not_constructible_from_policy {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = arene::base::default_accessor<int>;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<offset_policy_not_constructible_from_policy>);
}

/// @test Given an input type `Policy`, if `Policy::offset_policy::element_type` is not the same as
/// `Policy::element_type`, then `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfOffsetPolicyElemenTypeIsDifferentFromPolicyElementType) {
  struct different_offset_policy_element_type {
    using element_type = double;
    using data_handle_type = double*;
    using reference = double&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };
  STATIC_ASSERT_FALSE(is_accessor_policy_v<different_offset_policy_element_type>);
}

/// @test Given an input type `Policy`, if `Policy::access` is not defined, then `is_accessor_policy<Policy>` is
/// `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfAccessFunctionIsNotDefined) {
  struct no_access_function {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<no_access_function>);
}

/// @test Given an input type `Policy`, if `Policy::offset` is not defined, then `is_accessor_policy<Policy>` is
/// `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfOffsetFunctionIsNotDefined) {
  struct no_offset_function {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<no_offset_function>);
}

/// @test Given an input type `Policy`, if `Policy::access` does not return `Policy::reference`, then
/// `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfAccessFunctionDoesNotReturnReference) {
  struct wrong_access_return_type {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> int;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<wrong_access_return_type>);
}

/// @test Given an input type `Policy`, if `Policy::offset` does not return `Policy::data_handle_type`, then
/// `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfOffsetFunctionDoesNotReturnReference) {
  struct wrong_offset_return_type {
    using element_type = double;
    using data_handle_type = double*;
    using reference = double&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<wrong_offset_return_type>);
}

/// @test Given an input type `Policy`, if `Policy::access` does take `Policy::data_handle_type` and `std::size_t` as
/// input arguments, then `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfAccessFunctionDoesNotTakeDataHandleTypeAndSizetAsInputArguments) {
  struct wrong_access_handle_type {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(element_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<wrong_access_handle_type>);

  struct index_type {};
  struct wrong_access_index_type {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, index_type index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<wrong_access_index_type>);
}

/// @test Given an input type `Policy`, if `Policy::offset` does take `Policy::data_handle_type` and `std::size_t` as
/// input arguments, then `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfOffsetFunctionDoesNotTakeDataHandleTypeAndSizetAsInputArguments) {
  struct wrong_offset_handle_type {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(element_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<wrong_offset_handle_type>);

  struct index_type {};
  struct wrong_offset_index_type {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, index_type index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<wrong_offset_index_type>);
}

/// @test Given an input type `Policy`, if `Policy` is not copyable, then `is_accessor_policy<Policy>` is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfPolicyIsNotCopyable) {
  struct not_copyable
      : testing::configurable_value<int, testing::throws_on::nothing, testing::disable::copy_construct> {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<not_copyable>);
}

/// @test Given an input type `Policy`, if `Policy` has a throwing move construction, then `is_accessor_policy<Policy>`
/// is `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfPolicyHasThrowingMoveConstruction) {
  struct throwing_move_construction : testing::configurable_value<int, testing::throws_on::move_construct> {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<throwing_move_construction>);
}

/// @test Given an input type `Policy`, if `Policy` has a throwing move assignment, then `is_accessor_policy<Policy>` is
/// `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfPolicyHasThrowingMoveAssignment) {
  struct throwing_move_assignment : testing::configurable_value<int, testing::throws_on::move_assign> {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_FALSE(is_accessor_policy_v<throwing_move_assignment>);
}

struct throwing_swap : testing::configurable_value<int, testing::throws_on::swap> {
  using element_type = int;
  using data_handle_type = int*;
  using reference = int&;
  using offset_policy = valid_offset_policy;

  constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
  constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
};

/// @test Given an input type `Policy`, if `Policy` has a throwing swap function, then `is_accessor_policy<Policy>` is
/// `false`.
TEST(IsValidAccessorPolicyTest, IsFalseIfPolicyHasThrowingSwapFunction) {
  STATIC_ASSERT_FALSE(is_accessor_policy_v<throwing_swap>);
}

/// @test Given an input type `Policy`, if `Policy` deletes the rvalue reference overloads of `access` then
/// `is_accessor_policy<Policy>` is still `true`.
TEST(IsValidAccessorPolicyTest, IsTrueIfAccessorDeletesRvalueOverloadsOfAccess) {
  struct valid_accessor_policy {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type const& handle, std::size_t index) const -> reference;
    constexpr auto access(data_handle_type const&& handle, std::size_t index) const -> reference = delete;
    constexpr auto offset(data_handle_type handle, std::size_t index) const -> offset_policy::data_handle_type;
  };

  STATIC_ASSERT_TRUE(is_accessor_policy_v<valid_accessor_policy>);
}

/// @test Given an input type `Policy`, if `Policy` deletes the rvalue reference overloads of `offset` then
/// `is_accessor_policy<Policy>` is still `true`.
TEST(IsValidAccessorPolicyTest, IsTrueIfAccessorDeletesRvalueOverloadsOfOffset) {
  struct valid_accessor_policy {
    using element_type = int;
    using data_handle_type = int*;
    using reference = int&;
    using offset_policy = valid_offset_policy;

    constexpr auto access(data_handle_type handle, std::size_t index) const -> reference;
    constexpr auto offset(data_handle_type const& handle, std::size_t index) const -> offset_policy::data_handle_type;
    constexpr auto offset(data_handle_type const&& handle, std::size_t index) const
        -> offset_policy::data_handle_type = delete;
  };

  STATIC_ASSERT_TRUE(is_accessor_policy_v<valid_accessor_policy>);
}

}  // namespace
