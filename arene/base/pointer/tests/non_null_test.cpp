// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/pointer/detail/to_address.hpp"
#include "arene/base/stdlib_choice/hash.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <list>
#include <memory>
#include <vector>
#endif

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/pointer.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/exchange.hpp"
#include "arene/base/stdlib_choice/greater.hpp"
#include "arene/base/stdlib_choice/greater_equal.hpp"
#include "arene/base/stdlib_choice/less_equal.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pointer_traits.hpp"
#include "arene/base/stdlib_choice/reference_wrapper.hpp"
#include "arene/base/stdlib_choice/swap.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
using ::arene::base::make_shared;
using ::arene::base::make_unique;
using ::arene::base::non_null_shared_ptr;
using ::arene::base::non_null_unique_ptr;
#endif

using ::arene::base::make_non_null;
using ::arene::base::non_null;
using ::arene::base::non_null_ptr;
using ::arene::base::substitution_succeeds;
using ::arene::base::pointer::detail::to_address;

/// @test `non_null_ptr<T>` is `non_null<T*>`
TEST(NonNullAliases, NonNullPtrIsNonNullToRawTPtr) {
  ::testing::StaticAssertTypeEq<non_null_ptr<int>, non_null<int*>>();
  ::testing::StaticAssertTypeEq<non_null_ptr<int const>, non_null<int const*>>();
  ::testing::StaticAssertTypeEq<non_null_ptr<int const volatile>, non_null<int const volatile*>>();
}

/// @test `non_null_shared_ptr<T>` is `non_null<std::shared_ptr<T>>`
TEST(NonNullAliases, NonNullSPtrIsNonNullToSptrT) {
#if ARENE_IS_ON(ARENE_STDLIB_LIBARENECXX)
  GTEST_SKIP() << "Type alias not supported with Arene Base stdlib";
#else
  ::testing::StaticAssertTypeEq<non_null_shared_ptr<int>, non_null<std::shared_ptr<int>>>();
  ::testing::StaticAssertTypeEq<non_null_shared_ptr<int const>, non_null<std::shared_ptr<int const>>>();
  ::testing::StaticAssertTypeEq<non_null_shared_ptr<int const volatile>, non_null<std::shared_ptr<int const volatile>>>(
  );
#endif
}

/// @test `non_null_unique_ptr<T>` is `non_null<std::unique_ptr<T>>`
TEST(NonNullAliases, NonNullUPtrIsNonNullToUptrT) {
#if ARENE_IS_ON(ARENE_STDLIB_LIBARENECXX)
  GTEST_SKIP() << "Type alias not supported with Arene Base stdlib";
#else
  ::testing::StaticAssertTypeEq<non_null_unique_ptr<int>, non_null<std::unique_ptr<int>>>();
  ::testing::StaticAssertTypeEq<non_null_unique_ptr<int const>, non_null<std::unique_ptr<int const>>>();
  ::testing::StaticAssertTypeEq<non_null_unique_ptr<int const volatile>, non_null<std::unique_ptr<int const volatile>>>(
  );
#endif
}

using raw_pointer_types = ::testing::Types<int*, int const*, int const volatile*>;

template <typename T>
class simple_unique_ptr {
  T* ptr_{nullptr};

 public:
  using element_type = T;

  ~simple_unique_ptr() { delete ptr_; }

  simple_unique_ptr() = default;
  explicit simple_unique_ptr(T* ptr) noexcept
      : ptr_(ptr) {}

  simple_unique_ptr(simple_unique_ptr&& other) noexcept
      : ptr_(std::exchange(other.ptr_, nullptr)) {}
  simple_unique_ptr(simple_unique_ptr const&) = delete;

  auto operator=(simple_unique_ptr&& other) noexcept -> simple_unique_ptr& {
    simple_unique_ptr temp(std::move(other));
    std::swap(temp.ptr_, ptr_);
    return *this;
  }

  auto operator=(simple_unique_ptr const&) = delete;

  auto get() const noexcept -> T* { return ptr_; }
  auto operator*() const noexcept -> T& { return *ptr_; }
  auto operator->() const noexcept -> T* { return ptr_; }

  friend auto operator==(simple_unique_ptr const& lhs, simple_unique_ptr const& rhs) noexcept -> bool {
    return lhs.ptr_ == rhs.ptr_;
  }
  friend auto operator==(simple_unique_ptr const& lhs, std::nullptr_t) noexcept -> bool { return lhs.ptr_ == nullptr; }
  friend auto operator==(std::nullptr_t, simple_unique_ptr const& rhs) noexcept -> bool { return rhs.ptr_ == nullptr; }

  friend auto operator!=(simple_unique_ptr const& lhs, simple_unique_ptr const& rhs) noexcept -> bool {
    return lhs.ptr_ != rhs.ptr_;
  }
  friend auto operator!=(simple_unique_ptr const& lhs, std::nullptr_t) noexcept -> bool { return lhs.ptr_ != nullptr; }
  friend auto operator!=(std::nullptr_t, simple_unique_ptr const& rhs) noexcept -> bool { return rhs.ptr_ != nullptr; }
};

}  // namespace

namespace std {
template <typename T>
struct hash<simple_unique_ptr<T>> {
  /// @brief Hash the pointer
  /// @param ptr The pointer to hash
  /// @return The hashed value
  constexpr auto operator()(simple_unique_ptr<T> const& ptr) const noexcept -> size_t {
    return hash<T*>{}(ptr.get()) + 1;
  }
};

}  // namespace std

namespace {

using fancy_pointer_types = ::testing::Types<
    simple_unique_ptr<int>,
    simple_unique_ptr<int const>
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::shared_ptr<int>,
    std::shared_ptr<int const>,
    std::unique_ptr<int>,
    std::unique_ptr<int const>
#endif
    >;

using copyable_pointer_types = ::testing::Types<
    int*,
    int const*,
    int const volatile*
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::shared_ptr<int>,
    std::shared_ptr<int const>
#endif
    >;

using movable_pointer_types = ::testing::Types<
    simple_unique_ptr<int>,
    simple_unique_ptr<int const>
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::shared_ptr<int>,
    std::shared_ptr<int const>,
    std::unique_ptr<int>,
    std::unique_ptr<int const>
#endif
    >;

using nullptr_on_move_types = ::testing::Types<
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    std::unique_ptr<int>,
    std::unique_ptr<int const>,
    std::shared_ptr<int>,
    std::shared_ptr<int const>
#endif
    >;

using all_pointer_types = ::testing::Types<
    int*,
    int const*,
    int const volatile*,
    simple_unique_ptr<int>,
    simple_unique_ptr<int const>
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::shared_ptr<int>,
    std::shared_ptr<int const>,
    std::unique_ptr<int>,
    std::unique_ptr<int const>
#endif
    >;

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
class NonNullSizeOfTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullSizeOfTest, all_pointer_types, );

/// @test The size of `non_null<T>` is the same size or less than `T`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullSizeOfTest, IsNoLongerThanSizeOfHeldPointer) {
  STATIC_ASSERT_TRUE(sizeof(non_null<TypeParam>) <= sizeof(TypeParam));
}

template <typename Pointer>
class NonNullMemberTypesTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullMemberTypesTest, all_pointer_types, );

/// @test The type alias `held_pointer` is the type parameter to `non_null`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullMemberTypesTest, HeldPointerIsInputPointerType) {
  ::testing::StaticAssertTypeEq<typename non_null<TypeParam>::held_pointer, TypeParam>();
}

/// @test The type alias `pointer` is a raw pointer to the element type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullMemberTypesTest, PointerIsEquivalentToPointerToElementType) {
  ::testing::StaticAssertTypeEq<typename non_null<TypeParam>::pointer, typename non_null<TypeParam>::element_type*>();
}

/// @test The type alias `element_type` is the type of the pointed-to element
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullMemberTypesTest, ElementTypeIsEquivalentToPointerTraitsElementTypeForInputPointerType) {
  ::testing::StaticAssertTypeEq<
      typename non_null<TypeParam>::element_type,
      typename std::pointer_traits<TypeParam>::element_type>();
}

/// @test `std::pointer_traits<non_null<T>>` has the same `element_type` as `std::pointer_traits<T>`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullMemberTypesTest, PointerTraitsElementTypeIsEquivalentToPointerTraitsElementTypeForInputPointerType) {
  ::testing::StaticAssertTypeEq<
      typename std::pointer_traits<non_null<TypeParam>>::element_type,
      typename std::pointer_traits<TypeParam>::element_type>();
}

template <typename Pointer>
class NonNullConstructionForAnyTypeTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullConstructionForAnyTypeTest, all_pointer_types, );

/// @test `non_null` cannot be default-constructed
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForAnyTypeTest, CannotBeDefaultConstructed) {
  STATIC_ASSERT_FALSE(std::is_constructible<non_null<TypeParam>>::value);
}

/// @test `non_null` cannot be constructed from a `nullptr` literal
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForAnyTypeTest, CannotBeConstructedFromNullptrLiteral) {
  STATIC_ASSERT_FALSE((std::is_constructible<non_null<TypeParam>, std::nullptr_t>::value));
}

template <typename Pointer>
class NonNullConstructionForCopyableTypesTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullConstructionForCopyableTypesTest, copyable_pointer_types, );

/// @test `non_null` can be constructed from a value of the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForCopyableTypesTest, CanBeCopyConstructedFromHeldPointer) {
  auto const non_null_ptr = non_null<TypeParam>{this->original_pointer};
  EXPECT_EQ(non_null_ptr.get(), this->pointer_to_original_value);
}

/// @test `non_null` can be assigned from a value of the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForCopyableTypesTest, CanBeCopyAssignedFromHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{std::move(this->original_pointer)};
  auto new_pointer = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto const pointer_to_new_value = to_address(new_pointer);
  non_null_ptr = new_pointer;
  EXPECT_EQ(non_null_ptr.get(), pointer_to_new_value);
  delete_if_needed(new_pointer);
}

/// @test `non_null` can be assigned from another `non_null` object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForCopyableTypesTest, CanBeCopyAssignedFromOtherNonNull) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  auto new_non_null_ptr = non_null<TypeParam>{TypeParam{new typename TestFixture::held_pointer_element_type{}}};
  auto const pointer_to_new_value = new_non_null_ptr.get();
  non_null_ptr = new_non_null_ptr;
  EXPECT_EQ(non_null_ptr.get(), pointer_to_new_value);
  delete_if_needed(new_non_null_ptr.unwrap());
}

template <typename Pointer>
class NonNullConstructionForMoveableTypesTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullConstructionForMoveableTypesTest, movable_pointer_types, );

/// @test `non_null` can be move-constructed from an instance of the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForMoveableTypesTest, CanBeMoveConstructedFromHeldPointer) {
  auto const non_null_ptr = non_null<TypeParam>{std::move(this->original_pointer)};
  EXPECT_EQ(non_null_ptr.get(), this->pointer_to_original_value);
}

/// @test After move-construction the source is Null
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForMoveableTypesTest, AfterMoveSourceIsNull) {
  auto non_null_ptr = non_null<TypeParam>{std::move(this->original_pointer)};
  auto const second_non_null_ptr = non_null<TypeParam>{std::move(non_null_ptr)};
  ASSERT_EQ(second_non_null_ptr.get(), this->pointer_to_original_value);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_EQ(non_null_ptr, nullptr);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_EQ(non_null<TypeParam>::three_way_compare(non_null_ptr, nullptr), arene::base::strong_ordering::equal);
}

/// @test `non_null` can be move-assigned from an instance of the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForMoveableTypesTest, CanBeMoveAssignedFromHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{std::move(this->original_pointer)};
  auto new_pointer = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto const pointer_to_new_value = to_address(new_pointer);
  non_null_ptr = std::move(new_pointer);
  EXPECT_EQ(non_null_ptr.get(), pointer_to_new_value);
  delete_if_needed(non_null_ptr.unwrap());
}

/// @test `non_null` can be move-assigned from another `non_null` instance
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullConstructionForMoveableTypesTest, CanBeMoveAssignedFromOtherNonNull) {
  auto non_null_ptr = non_null<TypeParam>{std::move(this->original_pointer)};
  auto new_non_null_ptr = non_null<TypeParam>{TypeParam{new typename TestFixture::held_pointer_element_type{}}};
  auto const pointer_to_new_value = new_non_null_ptr.get();
  non_null_ptr = std::move(new_non_null_ptr);
  EXPECT_EQ(non_null_ptr.get(), pointer_to_new_value);
  delete_if_needed(non_null_ptr.unwrap());
}

template <typename Pointer>
struct NonNullPreConstructed : NonNullAllPointersTest<Pointer> {
  using non_null_type = non_null<Pointer>;
  non_null_type non_null_ptr{std::move(this->original_pointer)};
};

template <typename Pointer>
class NonNullGetTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullGetTest, all_pointer_types, );

/// @test `get` returns a raw pointer to the element type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullGetTest, ReturnTypeIsPointerMemberType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type>().get()),
      typename TestFixture::non_null_type::pointer>();
}

/// @test `get` returns the stored value
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullGetTest, ReturnsPointerToPointedToElement) {
  EXPECT_EQ(this->non_null_ptr.get(), this->pointer_to_original_value);
}

template <typename Pointer>
class NonNullFancyPointerUnwrapTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullFancyPointerUnwrapTest, fancy_pointer_types, );

/// @test `unwrap` returns a reference to the held pointer
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullFancyPointerUnwrapTest, ReturnTypeForLvalueIsConstRefToHeldPointerMemberType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type const&>().unwrap()),
      typename TestFixture::non_null_type::held_pointer const&>();

  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type&>().unwrap()),
      typename TestFixture::non_null_type::held_pointer const&>();
}

/// @test `unwrap` on an rvalue returns a rvalue reference to the held pointer
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullFancyPointerUnwrapTest, ReturnTypeForRvalueIsRvalueRefToHeldPointerMemberType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type&&>().unwrap()),
      typename TestFixture::non_null_type::held_pointer&&>();
}

/// @test Invoking `to_address` on the result of `unwrap` returns the original pointer value
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullFancyPointerUnwrapTest, ReturnsReferenceToHeldPointer) {
  EXPECT_EQ(to_address(this->non_null_ptr.unwrap()), this->pointer_to_original_value);
}

template <typename Pointer>
class NonNullRawPointerUnwrapTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullRawPointerUnwrapTest, raw_pointer_types, );

/// @test `unwrap` on a `const` lvalue returns a `const` reference to the held pointer
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullRawPointerUnwrapTest, ReturnTypeForLvalueIsConstRefToHeldPointerMemberType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type const&>().unwrap()),
      typename TestFixture::non_null_type::held_pointer const&>();

  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type&>().unwrap()),
      typename TestFixture::non_null_type::held_pointer const&>();
}

/// @test `unwrap` on an rvalue returns a `const` reference to the held pointer
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullRawPointerUnwrapTest, ReturnTypeForRvalueIsConstRefToHeldPointerMemberType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type&&>().unwrap()),
      typename TestFixture::non_null_type::held_pointer const&>();
}

/// @test Invoking `to_address` on the result of `unwrap` returns the original pointer value
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullRawPointerUnwrapTest, ReturnsReferenceToHeldPointer) {
  EXPECT_EQ(to_address(this->non_null_ptr.unwrap()), this->pointer_to_original_value);
}

template <typename Pointer>
class NonNullResetTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullResetTest, all_pointer_types, );

/// @test Invoking `reset` is equivalent to construction
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullResetTest, AfterReturnPtrIsAsIfConstructedViaInput) {
  auto new_pointer = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto const pointer_to_new_value = to_address(new_pointer);
  this->non_null_ptr.reset(std::move(new_pointer));
  EXPECT_EQ(this->non_null_ptr.get(), pointer_to_new_value);
  delete_if_needed(this->non_null_ptr.unwrap());
}

template <typename Pointer>
class NonNullSwapTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullSwapTest, all_pointer_types, );

/// @test `swap` exchanges the values of two `non_null` objects
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullSwapTest, SwapsValues) {
  auto new_non_null_ptr = non_null<TypeParam>{TypeParam{new typename TestFixture::held_pointer_element_type{}}};
  auto const pointer_to_new_value = new_non_null_ptr.get();

  // method swap
  this->non_null_ptr.swap(new_non_null_ptr);
  EXPECT_EQ(this->non_null_ptr.get(), pointer_to_new_value);
  EXPECT_EQ(new_non_null_ptr.get(), this->pointer_to_original_value);

  // function swap
  swap(this->non_null_ptr, new_non_null_ptr);
  EXPECT_EQ(this->non_null_ptr.get(), this->pointer_to_original_value);
  EXPECT_EQ(new_non_null_ptr.get(), pointer_to_new_value);

  // the hand-allocated pointer is back in its original place, delete it if needed
  delete_if_needed(new_non_null_ptr.unwrap());
}

template <typename Pointer>
class NonNullDereferenceOperatorsTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullDereferenceOperatorsTest, all_pointer_types, );

/// @test `non_null` can be dereferenced with the arrow operator
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullDereferenceOperatorsTest, ArrowReturnTypeIsPointerMemberType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type>().operator->()),
      typename TestFixture::non_null_type::pointer>();
}

/// @test `non_null` can be dereferenced with the arrow operator to retrieve the stored pointer value
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullDereferenceOperatorsTest, ArrowIsEquivalentToArrowOperatorOnHeldPointer) {
  EXPECT_EQ(this->non_null_ptr.operator->(), this->pointer_to_original_value);
}

/// @test `non_null` can be dereferenced with the dereference operator
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullDereferenceOperatorsTest, DerefReturnTypeIsReferenceToElementMemberType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<typename TestFixture::non_null_type>().operator*()),
      typename TestFixture::non_null_type::element_type&>();
}

/// @test `non_null` can be dereferenced with the dereference operator to return a reference to the pointed to value
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullDereferenceOperatorsTest, DerefIsEquivalentToDereferenceOperatorOnHeldPointer) {
  EXPECT_EQ(&(this->non_null_ptr.operator*()), this->pointer_to_original_value);
}

/// @test The arrow dereference operator returns a pointer to the object the normal dereference operator returns a
/// reference to
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullDereferenceOperatorsTest, DerefAndArrowAreSymmetric) {
  EXPECT_EQ(&(this->non_null_ptr.operator*()), this->non_null_ptr.operator->());
}

template <typename Pointer>
class NonNullBooleanConversionOperatorsTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullBooleanConversionOperatorsTest, all_pointer_types, );

/// @test `non_null` can be explicitly converted to `bool`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullBooleanConversionOperatorsTest, ConversionToBoolIsExplicit) {
  STATIC_ASSERT_TRUE((std::is_constructible<bool, non_null<TypeParam>>::value));
  STATIC_ASSERT_TRUE(!(std::is_convertible<bool, non_null<TypeParam>>::value));
}

/// @test `non_null` converted to `bool` yields `true`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullBooleanConversionOperatorsTest, ConversionToBoolIsTrue) {
  ASSERT_TRUE(static_cast<bool>(this->non_null_ptr));
}

/// @test The `!` operator on a `non_null` yields `false`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullBooleanConversionOperatorsTest, BoolNegationOperatorIsFalse) { ASSERT_FALSE(!this->non_null_ptr); }

template <typename Pointer>
class NonNullFancyPointerImplicitConversionToHeldPointerTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullFancyPointerImplicitConversionToHeldPointerTest, fancy_pointer_types, );

/// @test `non_null` is implicitly convertible to the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullFancyPointerImplicitConversionToHeldPointerTest, IsImplicitlyConvertibleToConstLvalue) {
  STATIC_ASSERT_TRUE((std::is_convertible<typename TestFixture::non_null_type const&, TypeParam const&>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<typename TestFixture::non_null_type&, TypeParam const&>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<typename TestFixture::non_null_type&&, TypeParam const&>::value));
}

/// @test An rvalue `non_null` is implicitly convertible to an rvalue of the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullFancyPointerImplicitConversionToHeldPointerTest, IsImplicitlyConvertibleToRvalue) {
  STATIC_ASSERT_TRUE((std::is_convertible<typename TestFixture::non_null_type&&, TypeParam&&>::value));
}

/// @test A non-`const` `non_null` is implicitly convertible to a reference to the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullFancyPointerImplicitConversionToHeldPointerTest, IsNotImplicitlyConvertibleToLvalue) {
  STATIC_ASSERT_FALSE((std::is_convertible<typename TestFixture::non_null_type const&, TypeParam&>::value));
  STATIC_ASSERT_FALSE((std::is_convertible<typename TestFixture::non_null_type&, TypeParam&>::value));
  STATIC_ASSERT_FALSE((std::is_convertible<typename TestFixture::non_null_type&&, TypeParam&>::value));
}

/// @test Implicitly converting to the held pointer type returns a reference to the held value
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullFancyPointerImplicitConversionToHeldPointerTest, ImplicitConversionReturnsHeldPointer) {
  auto const return_held = [](TypeParam const& held) -> decltype(auto) { return held; };
  EXPECT_EQ(&*return_held(this->non_null_ptr), this->pointer_to_original_value);

  auto const return_held_rvalue = [](TypeParam&& held) -> decltype(auto) { return std::move(held); };
  EXPECT_EQ(&*return_held_rvalue(std::move(this->non_null_ptr)), this->pointer_to_original_value);
}

template <typename Pointer>
class NonNullRawPointerImplicitConversionToHeldPointerTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullRawPointerImplicitConversionToHeldPointerTest, raw_pointer_types, );

/// @test `non_null` is implicitly convertible to the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullRawPointerImplicitConversionToHeldPointerTest, IsImplicitlyConvertibleToConstLvalue) {
  STATIC_ASSERT_TRUE((std::is_convertible<typename TestFixture::non_null_type const&, TypeParam const&>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<typename TestFixture::non_null_type&, TypeParam const&>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<typename TestFixture::non_null_type&&, TypeParam const&>::value));
}

/// @test An rvalue `non_null` is implicitly convertible to an rvalue of the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullRawPointerImplicitConversionToHeldPointerTest, IsNotImplicitlyConvertibleToRvalue) {
  STATIC_ASSERT_FALSE((std::is_convertible<typename TestFixture::non_null_type&&, TypeParam&&>::value));
}

/// @test A non-`const` `non_null` is implicitly convertible to a reference to the held pointer type
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullRawPointerImplicitConversionToHeldPointerTest, IsNotImplicitlyConvertibleToLvalue) {
  STATIC_ASSERT_FALSE((std::is_convertible<typename TestFixture::non_null_type const&, TypeParam&>::value));
  STATIC_ASSERT_FALSE((std::is_convertible<typename TestFixture::non_null_type&, TypeParam&>::value));
  STATIC_ASSERT_FALSE((std::is_convertible<typename TestFixture::non_null_type&&, TypeParam&>::value));
}

/// @test Implicitly converting to the held pointer type returns a reference to the held value
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullRawPointerImplicitConversionToHeldPointerTest, ImplicitConversionReturnsHeldPointer) {
  auto const return_held = [](TypeParam const& held) -> decltype(auto) { return held; };
  EXPECT_EQ(&*return_held(this->non_null_ptr), this->pointer_to_original_value);
}

template <typename Pointer>
class NonNullComparisonTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullComparisonTest, copyable_pointer_types, );

/// @test Equality comparison on `non_null` and another object returns the same result as comparing the held pointer to
/// the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, EQSameAsHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  auto non_null_ptr_from_same = non_null<TypeParam>{this->original_pointer};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  auto non_null_ptr_from_copy = non_null_ptr;

  EXPECT_EQ((non_null_ptr == non_null_ptr), (non_null_ptr.unwrap() == non_null_ptr.unwrap()));

  EXPECT_EQ(non_null_ptr, this->original_pointer);
  EXPECT_EQ((non_null_ptr == non_null_ptr_from_same), (non_null_ptr.unwrap() == non_null_ptr_from_same.unwrap()));
  EXPECT_EQ((non_null_ptr == non_null_ptr_from_copy), (non_null_ptr.unwrap() == non_null_ptr_from_copy.unwrap()));

  EXPECT_EQ(this->original_pointer, non_null_ptr);
  EXPECT_EQ((non_null_ptr_from_same == non_null_ptr), (non_null_ptr_from_same.unwrap() == non_null_ptr.unwrap()));
  EXPECT_EQ((non_null_ptr_from_copy == non_null_ptr), (non_null_ptr_from_copy.unwrap() == non_null_ptr.unwrap()));
}

/// @test Inequality comparison on `non_null` and another object returns the same result as comparing the held pointer
/// to the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, NESameAsHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  auto other_ptr = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto other_non_null_ptr = non_null<TypeParam>{other_ptr};

  EXPECT_EQ((non_null_ptr != other_non_null_ptr), (non_null_ptr.unwrap() != other_non_null_ptr.unwrap()));

  EXPECT_EQ((non_null_ptr != other_ptr), (non_null_ptr.unwrap() != other_ptr));
  EXPECT_EQ((other_ptr != non_null_ptr), (other_ptr != non_null_ptr.unwrap()));
  delete_if_needed(other_non_null_ptr.unwrap());
}

/// @test Less-than comparison on `non_null` and another object returns the same result as comparing the held pointer to
/// the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, LTSameAsHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  auto other_ptr = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto other_non_null_ptr = non_null<TypeParam>{other_ptr};

  EXPECT_EQ((non_null_ptr < other_non_null_ptr), (std::less<>{}(non_null_ptr.unwrap(), other_non_null_ptr.unwrap())));

  EXPECT_EQ((non_null_ptr < other_ptr), (std::less<>{}(non_null_ptr.unwrap(), other_ptr)));
  EXPECT_EQ((other_ptr < non_null_ptr), (std::less<>{}(other_ptr, non_null_ptr.unwrap())));
  delete_if_needed(other_non_null_ptr.unwrap());
}

/// @test Less-than-or-equal comparison on `non_null` and another object returns the same result as comparing the held
/// pointer to the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, LESameAsHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  auto other_ptr = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto other_non_null_ptr = non_null<TypeParam>{other_ptr};
  EXPECT_EQ(
      (non_null_ptr <= other_non_null_ptr),
      (std::less_equal<>{}(non_null_ptr.unwrap(), other_non_null_ptr.unwrap()))
  );

  EXPECT_EQ((non_null_ptr <= other_ptr), (std::less_equal<>{}(non_null_ptr.unwrap(), other_ptr)));
  EXPECT_EQ((other_ptr <= non_null_ptr), (std::less_equal<>{}(other_ptr, non_null_ptr.unwrap())));
  delete_if_needed(other_non_null_ptr.unwrap());
}

/// @test Greater-than comparison on `non_null` and another object returns the same result as comparing the held pointer
/// to the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, GTSameAsHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  auto other_ptr = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto other_non_null_ptr = non_null<TypeParam>{other_ptr};

  EXPECT_EQ(
      (non_null_ptr > other_non_null_ptr),
      (std::greater<>{}(non_null_ptr.unwrap(), other_non_null_ptr.unwrap()))
  );

  EXPECT_EQ((non_null_ptr > other_ptr), (std::greater<>{}(non_null_ptr.unwrap(), other_ptr)));
  EXPECT_EQ((other_ptr > non_null_ptr), (std::greater<>{}(other_ptr, non_null_ptr.unwrap())));
  delete_if_needed(other_non_null_ptr.unwrap());
}

/// @test Greather-than-or-equal comparison on `non_null` and another object returns the same result as comparing the
/// held pointer to the other object
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, GESameAsHeldPointer) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  auto other_ptr = TypeParam{new typename TestFixture::held_pointer_element_type{}};
  auto other_non_null_ptr = non_null<TypeParam>{other_ptr};
  EXPECT_EQ(
      (non_null_ptr >= other_non_null_ptr),
      (std::greater_equal<>{}(non_null_ptr.unwrap(), other_non_null_ptr.unwrap()))
  );
  EXPECT_EQ((non_null_ptr >= other_ptr), (std::greater_equal<>{}(non_null_ptr.unwrap(), other_ptr)));
  delete_if_needed(other_non_null_ptr.unwrap());
}

/// @test `non_null` objects are never equal to `nullptr`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, EQToNullptrIsAlwaysFalse) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  ASSERT_FALSE(non_null_ptr == nullptr);
}

/// @test `non_null` objects are never equal to `nullptr`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, NEToNullptrIsAlwaysTrue) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  ASSERT_TRUE(non_null_ptr != nullptr);
}

/// @test Relational comparisons of `non_null` and `nullptr_t` returns the same result as comparing the held pointer to
/// `nullptr_t`
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullComparisonTest, RelationalToNullptrIsSameAsUnderlyingPointer) {
  auto non_null_ptr = non_null<TypeParam>{this->original_pointer};
  EXPECT_EQ((non_null_ptr < nullptr), (std::less<TypeParam>{}(non_null_ptr.unwrap(), nullptr)));
  EXPECT_EQ((non_null_ptr <= nullptr), (std::less_equal<TypeParam>{}(non_null_ptr.unwrap(), nullptr)));
  EXPECT_EQ((non_null_ptr > nullptr), (std::greater<TypeParam>{}(non_null_ptr.unwrap(), nullptr)));
  EXPECT_EQ((non_null_ptr >= nullptr), (std::greater_equal<TypeParam>{}(non_null_ptr.unwrap(), nullptr)));
}

template <typename Pointer>
class NonNullPointerArithmeticTest : public NonNullAllPointersTest<Pointer> {};

TYPED_TEST_SUITE(NonNullPointerArithmeticTest, all_pointer_types, );

template <typename T>
using use_index = decltype(std::declval<T>()[std::size_t{}]);
template <typename T>
constexpr bool has_index = substitution_succeeds<use_index, T>;

/// @test `non_null` is not indexable
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullPointerArithmeticTest, IndexDeleted) { STATIC_ASSERT_FALSE(has_index<non_null<TypeParam>>); }

template <typename T, typename U = T>
using use_plus = decltype(std::declval<T>() + std::declval<U>());
template <typename T, typename U = T>
constexpr bool has_plus = substitution_succeeds<use_plus, T, U>;

/// @test `non_null` is not addable
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullPointerArithmeticTest, PlusDeleted) {
  STATIC_ASSERT_FALSE(has_plus<non_null<TypeParam>>);
  STATIC_ASSERT_FALSE((has_plus<non_null<TypeParam>, std::size_t>));
}

template <typename T, typename U = T>
using use_minus = decltype(std::declval<T>() - std::declval<U>());
template <typename T, typename U = T>
constexpr bool has_minus = substitution_succeeds<use_minus, T, U>;

/// @test `non_null` cannot be subtracted from
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullPointerArithmeticTest, MinusDeleted) {
  STATIC_ASSERT_FALSE(has_minus<non_null<TypeParam>>);
  STATIC_ASSERT_FALSE((has_minus<non_null<TypeParam>, std::size_t>));
}

template <typename T>
using use_preincrement = decltype(std::declval<T>()++);
template <typename T>
constexpr bool has_preincrement = substitution_succeeds<use_preincrement, T>;
template <typename T>
using use_postincrement = decltype(++std::declval<T>());
template <typename T>
constexpr bool has_postincrement = substitution_succeeds<use_postincrement, T>;

/// @test `non_null` is not incrementable
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullPointerArithmeticTest, IncrementDeleted) {
  STATIC_ASSERT_FALSE(has_preincrement<non_null<TypeParam>>);
  STATIC_ASSERT_FALSE(has_postincrement<non_null<TypeParam>>);
}

template <typename T>
using use_predecrement = decltype(std::declval<T>()--);
template <typename T>
constexpr bool has_predecrement = substitution_succeeds<use_predecrement, T>;
template <typename T>
using use_postdecrement = decltype(--std::declval<T>());
template <typename T>
constexpr bool has_postdecrement = substitution_succeeds<use_postdecrement, T>;

/// @test `non_null` is not decrementable
/// @tparam TypeParam The type of the argument
TYPED_TEST(NonNullPointerArithmeticTest, DecrementDeleted) {
  STATIC_ASSERT_FALSE(has_predecrement<non_null<TypeParam>>);
  STATIC_ASSERT_FALSE(has_postdecrement<non_null<TypeParam>>);
}

/// @test `non_null` can be constructed from anything the held pointer can be constructed from
TEST(NonNullConversions, CanBeConstructedFromPointerHeldIsConstructibleFrom) {
  STATIC_ASSERT_TRUE((std::is_constructible<non_null_ptr<int const>, int*>::value));
  struct base {};
  struct derived : base {};
  STATIC_ASSERT_TRUE((std::is_constructible<non_null_ptr<base>, derived*>::value));
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  STATIC_ASSERT_TRUE((std::is_constructible<non_null_unique_ptr<base>, std::unique_ptr<derived>&&>::value));
  STATIC_ASSERT_TRUE((std::is_constructible<non_null_shared_ptr<base const>, std::unique_ptr<derived>&&>::value));
#endif
}

/// @test Invoking `get` on a `non_null` returns the pointer constructed from the arguments
TEST(NonNullConversions, AfterConstructingFromAConvertedPointerGetReturnsInputPointer) {
  struct base {};
  struct derived : base {};

  auto derived_instance = derived{};

  auto const base_from_derived = non_null_ptr<base>{&derived_instance};
  EXPECT_EQ(base_from_derived.get(), &derived_instance);
}

/// @test `non_null` cannot be constructed from anything the held pointer cannot be constructed from
TEST(NonNullConversions, CannotBeConstructedFromPointerHeldIsNotConstructibleFrom) {
  STATIC_ASSERT_FALSE((std::is_constructible<non_null<int*>, int const*>::value));
}

/// @test `non_null` can be assigned from anything the held pointer can be assigned from
TEST(NonNullConversions, CanBeAssignedFromPointerHeldIsAssignableFrom) {
  STATIC_ASSERT_TRUE((std::is_assignable<non_null_ptr<int const>, int*>::value));
  struct base {};
  struct derived : base {};
  STATIC_ASSERT_TRUE((std::is_assignable<non_null_ptr<base>&, derived*>::value));
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  STATIC_ASSERT_TRUE((std::is_assignable<non_null_unique_ptr<base>&, std::unique_ptr<derived>&&>::value));
  STATIC_ASSERT_TRUE((std::is_assignable<non_null_shared_ptr<base const>&, std::unique_ptr<derived>&&>::value));
#endif
}

/// @test After assignment, `get` returns the assigned-to held pointer value
TEST(NonNullConversions, AfterAssigningFromAConvertedPointerGetReturnsAssignedFromPointer) {
  struct base {};
  struct derived : base {};

  auto original_instance = base{};

  auto base_from_derived = non_null_ptr<base>{&original_instance};

  auto derived_instance_ptr = derived{};
  base_from_derived = &derived_instance_ptr;
  EXPECT_EQ(base_from_derived.get(), &derived_instance_ptr);
}

/// @test `non_null` cannot be assigned from anything the held pointer cannot be assigned from
TEST(NonNullConversions, CannotBeAssignedFromPointerHeldIsNotAssignableFrom) {
  STATIC_ASSERT_FALSE((std::is_assignable<non_null<int*>&, int const*>::value));
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  STATIC_ASSERT_FALSE((std::is_assignable<non_null_unique_ptr<int>&, std::unique_ptr<int> const&>::value));
#endif
}

/// @test `non_null` can be explicitly converted to anything the held pointer explicitly converted to
TEST(NonNullConversions, CanBeExplicitlyConvertedToPointerConvertibleFromHeld) {
  struct base {};
  struct derived : base {};

  auto derived_instance = derived{};
  auto const derived_non_null = non_null_ptr<derived>{&derived_instance};

  base* const base_from_derived{derived_non_null};
  EXPECT_EQ(base_from_derived, derived_non_null.get());

  auto const const_derived_non_null = non_null_ptr<derived const>{&derived_instance};

  base const* const const_base_from_derived{const_derived_non_null};
  EXPECT_EQ(const_base_from_derived, const_derived_non_null.get());

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto derived_non_null_uptr = non_null_unique_ptr<derived>{std::make_unique<derived>()};
  EXPECT_NO_FATAL_FAILURE(std::shared_ptr<base const> const const_base_sptr_from_derived{std::move(derived_non_null_uptr
  )});
#endif
}

/// @test `non_null` can be compared to anything the held pointer compared to
TEST(NonNullConversions, CanBeComparedToPointerHeldIsComparableTo) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  struct base {};
  struct derived : base {};

  auto const base_non_null = non_null_unique_ptr<base const>{std::make_unique<base>()};
  auto const derived_non_null = non_null_unique_ptr<derived const>{std::make_unique<derived>()};

  EXPECT_EQ(base_non_null == derived_non_null, base_non_null.unwrap() == derived_non_null.unwrap());
  EXPECT_EQ(base_non_null != derived_non_null, base_non_null.unwrap() != derived_non_null.unwrap());
  EXPECT_EQ(base_non_null < derived_non_null, std::less<>{}(base_non_null.unwrap(), derived_non_null.unwrap()));
  EXPECT_EQ(base_non_null <= derived_non_null, std::less_equal<>{}(base_non_null.unwrap(), derived_non_null.unwrap()));
  EXPECT_EQ(base_non_null > derived_non_null, std::greater<>{}(base_non_null.unwrap(), derived_non_null.unwrap()));
  EXPECT_EQ(
      base_non_null >= derived_non_null,
      std::greater_equal<>{}(base_non_null.unwrap(), derived_non_null.unwrap())
  );
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

/// @test `non_null` cannot be compared to anything the held pointer cannot be compared to
TEST(NonNullConversions, CannotBeComparedToPointerHeldIsNotComparableTo) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  STATIC_ASSERT_FALSE(
      (arene::base::is_equality_comparable_v<non_null_unique_ptr<int> const&, non_null_shared_ptr<int> const&>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::is_equality_comparable_v<non_null_unique_ptr<int> const&, std::shared_ptr<int> const&>)
  );
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

template <typename Pointer>
class NonNullImplicitConversionToHeldPointerTest : public NonNullPreConstructed<Pointer> {};

TYPED_TEST_SUITE(NonNullImplicitConversionToHeldPointerTest, all_pointer_types, );

/// @test `std::hash` on a `non_null` returns the hash of the held pointer
/// @tparam TypeParam The argument type
TYPED_TEST(NonNullImplicitConversionToHeldPointerTest, StdHashIsStdHashOfUnderlyingPointer) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  EXPECT_EQ(
      std::hash<typename TestFixture::non_null_type>{}(this->non_null_ptr),
      std::hash<TypeParam>{}(this->non_null_ptr.unwrap())
  );
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

/// @test The return type of `make_non_null` is deduced from the arguments
TEST(MakeNonNullPtr, ReturnTypeIsNonNullToDeducedType) {
  ::testing::StaticAssertTypeEq<decltype(make_non_null(std::declval<int*>())), non_null_ptr<int>>();
  ::testing::StaticAssertTypeEq<decltype(make_non_null(std::declval<int const*>())), non_null_ptr<int const>>();
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  ::testing::
      StaticAssertTypeEq<decltype(make_non_null(std::declval<std::shared_ptr<int>>())), non_null_shared_ptr<int>>();
  ::testing::StaticAssertTypeEq<
      decltype(make_non_null(std::declval<std::shared_ptr<int const>>())),
      non_null_shared_ptr<int const>>();
  ::testing::
      StaticAssertTypeEq<decltype(make_non_null(std::declval<std::unique_ptr<int>>())), non_null_unique_ptr<int>>();
  ::testing::StaticAssertTypeEq<
      decltype(make_non_null(std::declval<std::unique_ptr<int const>>())),
      non_null_unique_ptr<int const>>();
#endif
}

/// @test The return value of `make_non_null` is constructed from the input arguments
TEST(MakeNonNullPtr, ReturnIsNonNullConstructedFromInputPtr) {
  int const value = 10;
  EXPECT_EQ(make_non_null(&value), &value);

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto const fancy_pointer = std::make_shared<int>(10);
  EXPECT_EQ(make_non_null(fancy_pointer).get(), fancy_pointer.get());
#endif
}

/// @test The return type of `make_shared` is deduced from the arguments as a `non_null_shared_ptr`
TEST(MakeShared, ReturnTypeIsNonNullSptrOfT) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  ::testing::StaticAssertTypeEq<decltype(make_shared<int>(std::declval<int>())), non_null_shared_ptr<int>>();
  ::testing::StaticAssertTypeEq<decltype(make_shared<int const>(std::declval<int>())), non_null_shared_ptr<int const>>(
  );
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

/// @test The return value of `make_shared` is constructed from the input arguments
TEST(MakeShared, ReturnIsNonNullSptrConstructedFromInputs) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  int const value = 10;
  auto const result = make_shared<int const>(value);
  EXPECT_EQ(*result, value);
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

/// @test The return value of `make_unique` is constructed from the input arguments as a `non_null_unique_ptr`
TEST(MakeUnique, ReturnTypeIsNonNullUptrOfT) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  ::testing::StaticAssertTypeEq<decltype(make_unique<int>(std::declval<int>())), non_null_unique_ptr<int>>();
  ::testing::StaticAssertTypeEq<decltype(make_unique<int const>(std::declval<int>())), non_null_unique_ptr<int const>>(
  );
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

/// @test The return value of `make_unique` is constructed from the input arguments
TEST(MakeUnique, ReturnIsNonNullUptrConstructedFromInputs) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  int const value = 10;
  auto const result = make_unique<int const>(value);
  EXPECT_EQ(*result, value);
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

/// @test Comparisons with types that pull in the `non_null` comparisons via ADL are OK
TEST(NonNull, ComparisonsOfTypesADLDependentOnNonNullAreOK) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  using vec = std::vector<non_null_unique_ptr<int>>;
  static_assert(
      arene::base::is_equality_comparable_v<vec::iterator, vec::iterator>,
      "There must be a valid comparison operator"
  );
  static_assert(
      arene::base::is_inequality_comparable_v<vec::iterator, vec::iterator>,
      "There must be a valid comparison operator"
  );
  static_assert(
      arene::base::is_less_than_comparable_v<vec::iterator, vec::iterator>,
      "There must be a valid comparison operator"
  );
  static_assert(
      arene::base::is_greater_than_comparable_v<vec::iterator, vec::iterator>,
      "There must be a valid comparison operator"
  );

  using lst = std::list<non_null_unique_ptr<int>>;
  static_assert(
      arene::base::is_equality_comparable_v<lst::iterator, lst::iterator>,
      "There must be a valid comparison operator"
  );
  static_assert(
      arene::base::is_inequality_comparable_v<lst::iterator, lst::iterator>,
      "There must be a valid comparison operator"
  );
#else
  GTEST_SKIP() << "Test not meaningful with Arene Base stdlib";
#endif
}

namespace {
class pointer_like {
 private:
  int data_;

 public:
  using element_type = int;

  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pointer_like(int data) noexcept
      : data_(data) {}
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pointer_like(std::reference_wrapper<pointer_like> data) noexcept
      : data_(*(data.get())) {}
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  pointer_like(std::reference_wrapper<non_null<pointer_like>> data) noexcept
      : data_(*(data.get())) {}

  auto operator*() const noexcept -> int const& { return data_; }
  auto operator->() const noexcept -> int const* { return &data_; }

  auto operator!=(std::nullptr_t) const noexcept -> bool { return true; }

  friend auto operator!=(pointer_like lhs, pointer_like rhs) noexcept -> bool { return lhs.data_ != rhs.data_; }
  friend auto operator==(pointer_like lhs, pointer_like rhs) noexcept -> bool { return lhs.data_ == rhs.data_; }
  friend auto operator<(pointer_like lhs, pointer_like rhs) noexcept -> bool { return lhs.data_ < rhs.data_; }
  friend auto operator<=(pointer_like lhs, pointer_like rhs) noexcept -> bool { return lhs.data_ <= rhs.data_; }
  friend auto operator>(pointer_like lhs, pointer_like rhs) noexcept -> bool { return lhs.data_ > rhs.data_; }
  friend auto operator>=(pointer_like lhs, pointer_like rhs) noexcept -> bool { return lhs.data_ >= rhs.data_; }
};
}  // namespace

/// @test Inequality comparison via implicit conversions has the expected result
TEST(NonNull, InequalityComparisonWithImplicitConversions) {
  non_null<pointer_like> nnptr1{pointer_like{42}};
  non_null<pointer_like> nnptr2{pointer_like{23}};

  ASSERT_EQ(nnptr1, nnptr1);
  ASSERT_NE(nnptr1, nnptr2);

  auto ref1 = std::ref(nnptr1);
  auto ref2 = std::ref(nnptr2);

  ASSERT_NE(ref1, ref2);
  ASSERT_NE(1, ref2);
  ASSERT_NE(ref1, 2);
}

/// @test less-than-or-equal comparison via implicit conversions has the expected result
TEST(NonNull, LessThanOrEqualComparisonWithImplicitConversions) {
  non_null<pointer_like> nnptr1{pointer_like{23}};
  non_null<pointer_like> nnptr2{pointer_like{42}};

  ASSERT_LE(nnptr1, nnptr1);
  ASSERT_LE(nnptr1, nnptr2);

  auto ref1 = std::ref(nnptr1);
  auto ref2 = std::ref(nnptr2);

  ASSERT_LE(ref1, ref2);
  ASSERT_LE(1, ref2);
  ASSERT_LE(ref1, 99);
}

/// @test greater-than-or-equal comparison via implicit conversions has the expected result
TEST(NonNull, GreaterThanOrEqualComparisonWithImplicitConversions) {
  non_null<pointer_like> nnptr1{pointer_like{42}};
  non_null<pointer_like> nnptr2{pointer_like{23}};

  ASSERT_GE(nnptr1, nnptr1);
  ASSERT_GE(nnptr1, nnptr2);

  auto ref1 = std::ref(nnptr1);
  auto ref2 = std::ref(nnptr2);

  ASSERT_GE(ref1, ref2);
  ASSERT_GE(99, ref2);
  ASSERT_GE(ref1, 1);
}

/// @test Greater-than comparison via implicit conversions has the expected result
TEST(NonNull, GreaterThanComparisonWithImplicitConversions) {
  non_null<pointer_like> nnptr1{pointer_like{42}};
  non_null<pointer_like> nnptr2{pointer_like{23}};

  ASSERT_GT(nnptr1, nnptr2);

  auto ref1 = std::ref(nnptr1);
  auto ref2 = std::ref(nnptr2);

  ASSERT_GT(ref1, ref2);
  ASSERT_GT(99, ref2);
  ASSERT_GT(ref1, 1);
}

/// @test `is_non_null` ignores `const` and `reference` parts of the type
TEST(NonNull, IsNonNullVStripsConstAndReferenceQualification) {
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<int*>>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<int*>&>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<int*> const>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<int*> const&>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<int*>&&>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<pointer_like>>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<pointer_like>&>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<pointer_like> const>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<pointer_like> const&>);
  STATIC_ASSERT_TRUE(arene::base::is_non_null_v<non_null<pointer_like>&&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int*>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int* const>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int*&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int* const&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int*&&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int const>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int const&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<int&&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<pointer_like>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<pointer_like const>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<pointer_like&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<pointer_like const&>);
  STATIC_ASSERT_FALSE(arene::base::is_non_null_v<pointer_like&&>);
}

}  // namespace
