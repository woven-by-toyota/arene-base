// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <tuple>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/pointer.hpp"
#include "arene/base/pointer/detail/to_address.hpp"

namespace {

using ::arene::base::non_null;
using ::arene::base::non_null_ptr;
using ::arene::base::non_null_shared_ptr;
using ::arene::base::non_null_unique_ptr;
using ::arene::base::substitution_succeeds;
using ::arene::base::pointer::detail::to_address;

using all_pointer_types = ::testing::Types<
    int*,
    int const*,
    int const volatile*,
    std::shared_ptr<int>,
    std::shared_ptr<int const>,
    std::unique_ptr<int>,
    std::unique_ptr<int const>>;

using nullptr_on_move_types = ::testing::
    Types<std::unique_ptr<int>, std::unique_ptr<int const>, std::shared_ptr<int>, std::shared_ptr<int const>>;

/// @brief  because we sometimes allocate to a raw pointer, and sometimes to a smart pointer, we need something that
/// cleans up only in the former case.
/// @{
struct do_delete_if_needed {
  template <typename T>
  constexpr void operator()(T* ptr) const {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) intentionally working with raw allocated memory
    delete ptr;
  }

  template <typename Pointer>
  constexpr void operator()(Pointer const& ptr ARENE_MAYBE_UNUSED) const {}
};

constexpr auto delete_if_needed = do_delete_if_needed{};
/// @}

template <typename Pointer>
struct NonNullAllPointersTest : ::testing::Test {
  using held_pointer_element_type = typename std::pointer_traits<Pointer>::element_type;
  Pointer original_pointer{new held_pointer_element_type{}};
  held_pointer_element_type* pointer_to_original_value = to_address(original_pointer);

  NonNullAllPointersTest() = default;
  ~NonNullAllPointersTest() override { delete_if_needed(original_pointer); }

  NonNullAllPointersTest(NonNullAllPointersTest const&) = delete;
  NonNullAllPointersTest(NonNullAllPointersTest&&) = delete;
  auto operator=(NonNullAllPointersTest const&) -> NonNullAllPointersTest& = delete;
  auto operator=(NonNullAllPointersTest&&) -> NonNullAllPointersTest& = delete;
};

template <typename Pointer>
class NonNullConstructionForAnyTypeDeathTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullConstructionForAnyTypeDeathTest, all_pointer_types, );

/// @test Constructing `non_null` from a NULL pointer value terminates the program with a precondition violation
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForAnyTypeDeathTest, ConstructionFromPtrThatIsNullptrIsPreconditionViolation) {
  ASSERT_DEATH(non_null<TypeParam>{TypeParam{nullptr}};, ::testing::HasSubstr("Precondition"));
}

template <typename Pointer>
struct NonNullPreConstructed : NonNullAllPointersTest<Pointer> {
  using non_null_type = non_null<Pointer>;
  non_null_type non_null_ptr{std::move(this->original_pointer)};
};

template <typename Pointer>
class NonNullGetDeathTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullGetDeathTest, nullptr_on_move_types, );

/// @test Invoking `get` on a moved-from `non_null` holding a `std::unique_ptr` terminates the program with an invariant
/// violation
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullGetDeathTest, GetOnNullptrIsInvariantViolation) {
  // the only way to get a nullptr in a non-null is use-after-move, so we'll push ignores in case static checks for that
  // are enabled.
  ARENE_MAYBE_UNUSED decltype(this->non_null_ptr) const consume = std::move(this->non_null_ptr);
  // NOLINTBEGIN(bugprone-use-after-move)
  ASSERT_DEATH(std::ignore = this->non_null_ptr.get(), ::testing::HasSubstr("Invariant"));
  // NOLINTEND(bugprone-use-after-move)
}

template <typename Pointer>
class NonNullResetDeathTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullResetDeathTest, all_pointer_types, );

/// @test Invoking `reset` with a NULL pointer terminates the program with a precondition violation
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullResetDeathTest, ResetOnPtrThatIsNullptrIsPreconditionViolation) {
  ASSERT_DEATH(this->non_null_ptr.reset(TypeParam{nullptr}), ::testing::HasSubstr("Precondition"));
}

template <typename Pointer>
class NonNullDereferenceOperatorsDeathTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullDereferenceOperatorsDeathTest, nullptr_on_move_types, );

/// @test Invoking the arrow dereference operator on a moved-from `non_null` holding a `std::unique_ptr` terminates the
/// program with an invariant violation
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullDereferenceOperatorsDeathTest, ArrowOnNullptrIsInvariantViolation) {
  // the only way to get a nullptr in a non-null is use-after-move, so we'll push ignores in case static checks for that
  // are enabled.
  ARENE_MAYBE_UNUSED decltype(this->non_null_ptr) const consume = std::move(this->non_null_ptr);
  // NOLINTBEGIN(bugprone-use-after-move)
  ASSERT_DEATH(std::ignore = this->non_null_ptr.operator->(), ::testing::HasSubstr("Invariant"));
  // NOLINTEND(bugprone-use-after-move)
}

/// @test Invoking the dereference operator on a moved-from `non_null` holding a `std::unique_ptr` terminates the
/// program with an invariant violation
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullDereferenceOperatorsDeathTest, DerefOnNullptrIsInvariantViolation) {
  // the only way to get a nullptr in a non-null is use-after-move, so we'll push ignores in case static checks for that
  // are enabled.
  ARENE_MAYBE_UNUSED decltype(this->non_null_ptr) const consume = std::move(this->non_null_ptr);
  // NOLINTBEGIN(bugprone-use-after-move)
  ASSERT_DEATH(std::ignore = *(this->non_null_ptr), ::testing::HasSubstr("Invariant"));
  // NOLINTEND(bugprone-use-after-move)
}

/// @test Constructing from arguments that yield a NULL held pointer terminates the program with a precondition
/// violation
TEST(NonNullConversionsDeathTest, ConstructionFromPointerConvertibleToHeldThatIsNullptrIsPreconditionViolation) {
  struct base {};
  struct derived : base {};

  derived const* const null = nullptr;

  ASSERT_DEATH(non_null_ptr<base const>{null};, ::testing::HasSubstr("Precondition"));
}

}  // namespace
