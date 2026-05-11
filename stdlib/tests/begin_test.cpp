// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/iterator"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

struct local {};

using begin_array_types = arene::base::type_lists::concat_unique_t<::testing::Types<local>, ::testing::object_types>;

template <typename T>
class BeginArrayTests : public testing::Test {};

TYPED_TEST_SUITE(BeginArrayTests, begin_array_types, );

/// @test Verify that calling @c std::begin on an array returns a pointer to the first element
CONSTEXPR_TYPED_TEST(BeginArrayTests, BeginOnArrayReturnsPointerToFirstElement) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam data[5] = {};

  ::testing::StaticAssertTypeEq<decltype(std::begin(data)), TypeParam*>();

  TypeParam* result = std::begin(data);

  ASSERT_EQ(result, &data[0]);
}

/// @test Verify that calling @c std::begin on an array is @c noexcept
TYPED_TEST(BeginArrayTests, BeginOnArrayIsNoexcept) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  static_assert(noexcept(std::begin(std::declval<TypeParam(&)[5]>())), "Must be noexcept");
}

template <typename T>
class CBeginArrayTests : public testing::Test {};

TYPED_TEST_SUITE(CBeginArrayTests, begin_array_types, );

/// @test Verify that calling @c std::cbegin on an array returns a pointer to the first element
CONSTEXPR_TYPED_TEST(CBeginArrayTests, CBeginOnArrayReturnsPointerToFirstElement) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam data[5] = {};

  ::testing::StaticAssertTypeEq<decltype(std::cbegin(data)), TypeParam const*>();

  TypeParam const* result = std::cbegin(data);

  ASSERT_EQ(result, &data[0]);
}

/// @test Verify that calling @c std::cbegin on an array is @c noexcept
TYPED_TEST(CBeginArrayTests, CBeginOnArrayIsNoexcept) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  static_assert(noexcept(std::cbegin(std::declval<TypeParam(&)[5]>())), "Must be noexcept");
}

class my_container {
  class flag {
   public:
    constexpr explicit flag() = default;
  };

 public:
  class non_const_begin_result {
   public:
    constexpr explicit non_const_begin_result(flag) {}
  };

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() -> non_const_begin_result { return non_const_begin_result{flag{}}; }

  class const_begin_result {
   public:
    constexpr explicit const_begin_result(flag) {}
  };

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() const -> const_begin_result { return const_begin_result{flag{}}; }
};

class noexcept_begin_container {
 public:
  class begin_result {};

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() noexcept -> begin_result { return {}; }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() const noexcept -> begin_result { return {}; }
};

class noexcept_const_begin_container {
 public:
  class begin_result {};

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() -> begin_result { return {}; }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() const noexcept -> begin_result { return {}; }
};

class noexcept_non_const_begin_container {
 public:
  class begin_result {};

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() noexcept -> begin_result { return {}; }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto begin() const -> begin_result { return {}; }
};

using begin_container_types = ::testing::
    Types<my_container, noexcept_begin_container, noexcept_const_begin_container, noexcept_non_const_begin_container>;

template <typename T>
class BeginContainerTests : public testing::Test {};

TYPED_TEST_SUITE(BeginContainerTests, begin_container_types, );

/// @test Invoking @c std::begin on a container invokes the @c std::begin member function and returns the result
CONSTEXPR_TYPED_TEST(BeginContainerTests, InvokingBeginOnANonConstContainerReturnsTheResultOfTheBeginMemberFunction) {
  testing::StaticAssertTypeEq<
      decltype(std::begin(std::declval<TypeParam&>())),
      decltype(std::declval<TypeParam&>().begin())>();

  TypeParam cont{};
  auto const nc_result{std::begin(cont)};
  static_cast<void>(nc_result);
}

/// @test Invoking @c std::begin on a const reference to a container invokes the @c std::begin member function and
/// returns the result
CONSTEXPR_TYPED_TEST(BeginContainerTests, InvokingBeginOnAConstContainerReturnsTheResultOfTheBeginMemberFunction) {
  testing::StaticAssertTypeEq<
      decltype(std::begin(std::declval<TypeParam const&>())),
      decltype(std::declval<TypeParam const&>().begin())>();

  TypeParam const c_cont{};
  auto const c_result{std::begin(c_cont)};
  static_cast<void>(c_result);
}

/// @test Invoking @c std::begin on an rvalue container invokes the @c std::begin member function and returns the result
CONSTEXPR_TYPED_TEST(BeginContainerTests, InvokingBeginOnAnRvalueContainerReturnsTheResultOfTheBeginMemberFunction) {
  testing::StaticAssertTypeEq<
      decltype(std::begin(std::declval<TypeParam&&>())),
      decltype(std::declval<TypeParam const&>().begin())>();

  auto const temp_result{std::begin(TypeParam{})};
  static_cast<void>(temp_result);
}

/// @test Invoking @c std::begin on a non-const reference to a container is @c noexcept if the corresponding @c
/// std::begin member is
CONSTEXPR_TYPED_TEST(BeginContainerTests, InvokingBeginOnANonConstContainerIsNoexceptIfTheMemberIs) {
  static_assert(
      noexcept(std::begin(std::declval<TypeParam&>())) == noexcept(std::declval<TypeParam&>().begin()),
      "Noexcept must be the same as the member function"
  );
}

/// @test Invoking @c std::begin on a const reference to a container is @c noexcept if the corresponding @c std::begin
/// member is
CONSTEXPR_TYPED_TEST(BeginContainerTests, InvokingBeginOnAConstContainerIsNoexceptIfTheMemberIs) {
  static_assert(
      noexcept(std::begin(std::declval<TypeParam const&>())) == noexcept(std::declval<TypeParam const&>().begin()),
      "Noexcept must be the same as the member function"
  );
}

/// @test Invoking @c std::begin on rvalue container is @c noexcept if the corresponding @c const @c std::begin member
/// is
CONSTEXPR_TYPED_TEST(BeginContainerTests, InvokingBeginOnAnRvalueContainerIsNoexceptIfTheMemberIs) {
  static_assert(
      noexcept(std::begin(std::declval<TypeParam&&>())) == noexcept(std::declval<TypeParam const&>().begin()),
      "Noexcept must be the same as the const member function"
  );
}

template <typename T>
class CBeginContainerTests : public testing::Test {};

TYPED_TEST_SUITE(CBeginContainerTests, begin_container_types, );

/// @test Invoking @c std::cbegin on a non-const container is equivalent to invoking @c std::begin on a const reference
/// to the container
CONSTEXPR_TYPED_TEST(
    CBeginContainerTests,
    InvokingBeginOnANonConstContainerIsEquivalentToInvokingBeginOnAConstQualifiedContainer
) {
  testing::StaticAssertTypeEq<
      decltype(std::cbegin(std::declval<TypeParam&>())),
      decltype(std::begin(std::declval<TypeParam const&>()))>();

  TypeParam cont{};
  auto const nc_result{std::cbegin(cont)};
  static_cast<void>(nc_result);
}

/// @test Invoking @c std::cbegin on a const reference to a container is equivalent to invoking @c std::begin on a
/// const reference to the container
CONSTEXPR_TYPED_TEST(
    CBeginContainerTests,
    InvokingCBeginOnAConstContainerReturnsTheResultOfInvokingBeginOnAConstReference
) {
  testing::StaticAssertTypeEq<
      decltype(std::cbegin(std::declval<TypeParam const&>())),
      decltype(std::begin(std::declval<TypeParam const&>()))>();

  TypeParam const c_cont{};
  auto const c_result{std::cbegin(c_cont)};
  static_cast<void>(c_result);
}

/// @test Invoking @c std::cbegin on an rvalue container is equivalent to invoking @c std::begin on a const reference to
/// the container
CONSTEXPR_TYPED_TEST(
    CBeginContainerTests,
    InvokingCBeginOnAnRvalueContainerReturnsTheResultOfInvokingBeginOnAConstReference
) {
  testing::StaticAssertTypeEq<
      decltype(std::cbegin(std::declval<TypeParam&&>())),
      decltype(std::begin(std::declval<TypeParam const&>()))>();

  auto const temp_result{std::cbegin(TypeParam{})};
  static_cast<void>(temp_result);
}

/// @test Invoking @c std::cbegin on a non-const reference to a container is @c noexcept if @c std::begin on a const
/// reference to the container is
CONSTEXPR_TYPED_TEST(CBeginContainerTests, InvokingCBeginOnANonConstContainerIsNoexceptIfBeginOnConstIs) {
  static_assert(
      noexcept(std::cbegin(std::declval<TypeParam&>())) == noexcept(std::begin(std::declval<TypeParam const&>())),
      "Noexcept must be the same as begin on a const reference to the container"
  );
}

/// @test Invoking @c std::cbegin on a const reference to a container is @c noexcept if @c std::begin on a const
/// reference to the container is
CONSTEXPR_TYPED_TEST(CBeginContainerTests, InvokingCBeginOnAConstContainerIsNoexceptIfBeginOnConstIs) {
  static_assert(
      noexcept(std::cbegin(std::declval<TypeParam const&>())) == noexcept(std::begin(std::declval<TypeParam const&>())),
      "Noexcept must be the same as begin on a const reference to the container"
  );
}

/// @test Invoking @c std::cbegin on rvalue container is @c noexcept if @c std::begin on a const
/// reference to the container is
CONSTEXPR_TYPED_TEST(CBeginContainerTests, InvokingBeginOnAnRvalueContainerIsNoexceptIfBeginOnConstIs) {
  static_assert(
      noexcept(std::cbegin(std::declval<TypeParam&&>())) == noexcept(std::begin(std::declval<TypeParam const&>())),
      "Noexcept must be the same as begin on a const reference to the container"
  );
}

}  // namespace
