// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file to_address_test.cpp
/// @brief Unit tests for pointer/detail/to_address.hpp
///
#include "arene/base/pointer/detail/to_address.hpp"

#include <memory>
#include <string>
#include <tuple>

#include <gtest/gtest.h>

/// Provides some fancy pointers for pointer_traits specialization. They need to be in a real namespace as
/// pointer_traits requires opening namespace std.
namespace source_local_to_address_test {

/// A fancy pointer which has a pointer_traits specialization but no to_address
class my_fancy_pointer_ptraits_no_to_address {
 public:
  using element_type = int const&;
  explicit my_fancy_pointer_ptraits_no_to_address(int const* ptr)
      : ptr_(ptr) {}

  auto operator->() const -> int const* { return ptr_; }

 private:
  int const* ptr_;
};

/// A fancy pointer which has a pointer_traits specialization that defines to_address
class my_fancy_pointer_ptraits_to_address : public my_fancy_pointer_ptraits_no_to_address {
 public:
  using my_fancy_pointer_ptraits_no_to_address::element_type;
  using my_fancy_pointer_ptraits_no_to_address::my_fancy_pointer_ptraits_no_to_address;
};
}  // namespace source_local_to_address_test

namespace std {
template <>
struct pointer_traits<::source_local_to_address_test::my_fancy_pointer_ptraits_no_to_address> {
  using pointer = ::source_local_to_address_test::my_fancy_pointer_ptraits_no_to_address;
  using element_type = ::source_local_to_address_test::my_fancy_pointer_ptraits_no_to_address::element_type;
};

template <>
struct pointer_traits<::source_local_to_address_test::my_fancy_pointer_ptraits_to_address> {
  using pointer = ::source_local_to_address_test::my_fancy_pointer_ptraits_no_to_address;
  using element_type = ::source_local_to_address_test::my_fancy_pointer_ptraits_no_to_address::element_type;

  /// @return always returns @c &value
  static auto to_address(pointer const& ptr) -> int const* {
    std::ignore = &ptr;
    static int const value = 10;  ///< A constant value to take the address of
    return &value;
  }
};
}  // namespace std

namespace {

using ::arene::base::pointer::detail::to_address;
using ::source_local_to_address_test::my_fancy_pointer_ptraits_no_to_address;
using ::source_local_to_address_test::my_fancy_pointer_ptraits_to_address;

template <typename P>
using to_address_return_t = decltype(to_address(std::declval<P>()));

template <typename P>
struct ToAddressRawPtrTest : ::testing::Test {};

using raw_ptr_types = ::testing::Types<int*, int const*, std::string*, std::string const*>;

TYPED_TEST_SUITE(ToAddressRawPtrTest, raw_ptr_types, );

/// @test The return type of `to_address` on a raw pointer is just the raw pointer
/// @tparam TypeParam the pointer type
TYPED_TEST(ToAddressRawPtrTest, ReturnTypeForRawPointerIsInputPointerType) {
  ::testing::StaticAssertTypeEq<to_address_return_t<TypeParam>, TypeParam>();
}

/// @test The return value of `to_address` on a raw pointer is just the raw pointer
/// @tparam TypeParam the pointer type
TYPED_TEST(ToAddressRawPtrTest, ReturnValueIsInputPointerValue) {
  typename std::pointer_traits<TypeParam>::element_type value{};
  TypeParam const ptr{&value};
  ASSERT_EQ(to_address(ptr), ptr);
}

template <typename T>
struct ToAddressSmartPtrTest : ::testing::Test {};

using smart_ptr_types = ::testing::Types<
    std::shared_ptr<int>,
    std::shared_ptr<int const>,
    std::shared_ptr<std::string>,
    std::shared_ptr<std::string const>,
    std::unique_ptr<int>,
    std::unique_ptr<int const>,
    std::unique_ptr<std::string>,
    std::unique_ptr<std::string const>>;

TYPED_TEST_SUITE(ToAddressSmartPtrTest, smart_ptr_types, );

/// @test The return type of `to_address` on a smart pointer is a raw pointer to the element type
/// @tparam TypeParam the pointer type
TYPED_TEST(ToAddressSmartPtrTest, ReturnTypeForRawSmartPtrIsSmartPtrToAddressType) {
  ::testing::StaticAssertTypeEq<to_address_return_t<TypeParam>, decltype(std::declval<TypeParam>().get())>();
}

/// @test The return value of `to_address` on a smart pointer is a raw pointer to the element pointed to by the smart
/// pointer
/// @tparam TypeParam the pointer type
TYPED_TEST(ToAddressSmartPtrTest, ReturnValueIsInputPointerValue) {
  TypeParam const ptr{new typename std::pointer_traits<TypeParam>::element_type{}};
  ASSERT_EQ(to_address(ptr), ptr.get());
}

/// @test The return value of `to_address` on a smart pointer without a `pointer_traits::to_address` operation is the
/// result of `operator->` on that smart pointer
TEST(ToAddressPointerTraitsSpecialized, WithoutToAddressReturnIsOperatorArrow) {
  int const value = 10;
  auto const ptr = my_fancy_pointer_ptraits_no_to_address{&value};
  ASSERT_EQ(to_address(ptr), &value);
}

/// @test The return value of `to_address` on a smart pointer with a `pointer_traits::to_address` operation is the
/// result of that `pointer_traits::to_address` operation on that pointer
TEST(ToAddressPointerTraitsSpecialized, WithToAddressReturnIsPointerTraitsToAddress) {
  int const value = 10;
  auto const ptr = my_fancy_pointer_ptraits_to_address{&value};
  ASSERT_EQ(to_address(ptr), std::pointer_traits<my_fancy_pointer_ptraits_to_address>::to_address(ptr));
}
}  // namespace
