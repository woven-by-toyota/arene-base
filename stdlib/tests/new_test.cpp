// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/new"

#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {
/// @test Verify that placement new operator returns source pointer
TEST(New, NewReturnsPointer) {
  int some_object{3};
  void* const ptr{static_cast<void*>(&some_object)};
  ASSERT_EQ(::operator new(5, ptr), ptr);
  double some_other_object{3.4};
  void* const other_ptr{static_cast<void*>(&some_other_object)};
  ASSERT_EQ(::operator new(42, other_ptr), other_ptr);
}

/// @test Verify that placement new operator for arrays returns source pointer
TEST(New, ArrayNewReturnsPointer) {
  int some_object{3};
  void* const ptr{static_cast<void*>(&some_object)};
  ASSERT_EQ(::operator new[](5, ptr), ptr);
  double some_other_object{3.4};
  void* const other_ptr{static_cast<void*>(&some_other_object)};
  ASSERT_EQ(::operator new[](42, other_ptr), other_ptr);
}

/// @test Verify that placement new returns source pointer and constructs the object
TEST(New, PlacementNewReturnsPointerAndConstructsObject) {
  int some_object{3};
  void* const ptr{static_cast<void*>(&some_object)};
  constexpr int new_value{99};
  ASSERT_EQ(::new (ptr) int(new_value), ptr);
  ASSERT_EQ(some_object, new_value);
  std::uint64_t some_other_object{34};
  constexpr std::uint64_t new_value2{0x123456789ABCDEF0};
  void* const other_ptr{static_cast<void*>(&some_other_object)};
  ASSERT_EQ(::new (other_ptr) std::uint64_t(new_value2), other_ptr);
  ASSERT_EQ(some_other_object, new_value2);
}

/// @test Verify that placement new returns source pointer and constructs the object
TEST(New, PlacementArrayNewReturnsPointerAndConstructsObject) {
  constexpr std::int32_t array_size{6};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  alignas(int) char some_storage[array_size * sizeof(std::int32_t)];
  void* const ptr{static_cast<void*>(&some_storage)};
  constexpr std::int32_t new_value{99};
  constexpr std::int32_t new_value1{-99};
  constexpr std::int32_t new_value2{123};
  constexpr std::int32_t new_value3{-456};
  constexpr std::int32_t new_value4{-12731687};
  constexpr std::int32_t new_value5{2013176487};
  // NOLINTNEXTLINE(hicpp-use-auto)
  std::int32_t* array_ptr =
      ::new (ptr) std::int32_t[array_size]{new_value, new_value1, new_value2, new_value3, new_value4, new_value5};
  ASSERT_EQ(array_ptr, ptr);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(array_ptr[0], new_value);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(array_ptr[1], new_value1);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(array_ptr[2], new_value2);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(array_ptr[3], new_value3);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(array_ptr[4], new_value4);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(array_ptr[5], new_value5);
}

/// @test Verify that placement delete operator can be called
TEST(Delete, DeleteAndArrayDeleteDoNothing) {
  int some_object{3};
  void* const ptr{static_cast<void*>(&some_object)};
  ::operator delete(ptr, ptr);
  ::operator delete[](ptr, ptr);
}

}  // namespace
