// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/iterator"
#include "stdlib/include/memory"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

struct local {};

class local_with_non_standard_address_operator {
 private:
  // NOLINTNEXTLINE(google-runtime-operator)
  void operator&() {}
};

using end_array_types = arene::base::type_lists::
    concat_unique_t<::testing::Types<local, local_with_non_standard_address_operator>, ::testing::object_types>;

template <typename T>
class EndArrayTests : public testing::Test {};

TYPED_TEST_SUITE(EndArrayTests, end_array_types, );

/// @test Verify that calling @c std::end on an array returns a pointer to the one-past-the-end element
CONSTEXPR_TYPED_TEST(EndArrayTests, EndOnArrayReturnsPointerToFirstElement) {
  constexpr std::uint32_t count{5U};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam data[count] = {};

  ::testing::StaticAssertTypeEq<decltype(std::end(data)), TypeParam*>();

  TypeParam* result = std::end(data);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(result, std::begin(data) + count);
}

/// @test Verify that calling @c std::end on an array is @c noexcept
TYPED_TEST(EndArrayTests, EndOnArrayIsNoexcept) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  static_assert(noexcept(std::end(std::declval<TypeParam(&)[5]>())), "Must be noexcept");
}

template <typename T>
class CEndArrayTests : public testing::Test {};

TYPED_TEST_SUITE(CEndArrayTests, end_array_types, );

/// @test Verify that calling @c std::end on an array returns a pointer to the one-past-the-end element
CONSTEXPR_TYPED_TEST(CEndArrayTests, CEndOnArrayReturnsPointerToFirstElement) {
  constexpr std::uint32_t count{5U};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam data[count] = {};

  ::testing::StaticAssertTypeEq<decltype(std::cend(data)), TypeParam const*>();

  TypeParam const* result = std::cend(data);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(result, std::begin(data) + count);
}

/// @test Verify that calling @c std::end on an array is @c noexcept
TYPED_TEST(CEndArrayTests, CEndOnArrayIsNoexcept) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  static_assert(noexcept(std::cend(std::declval<TypeParam(&)[5]>())), "Must be noexcept");
}

class my_container {
  class flag {
   public:
    constexpr explicit flag() = default;
  };

 public:
  class non_const_end_result {
   public:
    constexpr explicit non_const_end_result(flag) {}
  };

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() -> non_const_end_result { return non_const_end_result{flag{}}; }

  class const_end_result {
   public:
    constexpr explicit const_end_result(flag) {}
  };

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() const -> const_end_result { return const_end_result{flag{}}; }
};

class noexcept_end_container {
 public:
  class end_result {};

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() noexcept -> end_result { return {}; }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() const noexcept -> end_result { return {}; }
};

class noexcept_const_end_container {
 public:
  class end_result {};

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() -> end_result { return {}; }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() const noexcept -> end_result { return {}; }
};

class noexcept_non_const_end_container {
 public:
  class end_result {};

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() noexcept -> end_result { return {}; }
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto end() const -> end_result { return {}; }
};

using end_container_types = ::testing::
    Types<my_container, noexcept_end_container, noexcept_const_end_container, noexcept_non_const_end_container>;

template <typename T>
class EndContainerTests : public testing::Test {};

TYPED_TEST_SUITE(EndContainerTests, end_container_types, );

/// @test Invoking @c std::end on a container invokes the @c end member function and returns the result
CONSTEXPR_TYPED_TEST(EndContainerTests, InvokingEndOnANonConstContainerReturnsTheResultOfTheEndMemberFunction) {
  testing::
      StaticAssertTypeEq<decltype(std::end(std::declval<TypeParam&>())), decltype(std::declval<TypeParam&>().end())>();

  TypeParam cont{};
  auto const nc_result{std::end(cont)};
  static_cast<void>(nc_result);
}

/// @test Invoking @c std::end on a const reference to a container invokes the @c const @c end member function and
/// returns the result
CONSTEXPR_TYPED_TEST(EndContainerTests, InvokingEndOnAConstContainerReturnsTheResultOfTheEndMemberFunction) {
  testing::StaticAssertTypeEq<
      decltype(std::end(std::declval<TypeParam const&>())),
      decltype(std::declval<TypeParam const&>().end())>();

  TypeParam const c_cont{};
  auto const c_result{std::end(c_cont)};
  static_cast<void>(c_result);
}

/// @test Invoking @c std::end on an rvalue container invokes the @c const @c end member function and returns the result
CONSTEXPR_TYPED_TEST(EndContainerTests, InvokingEndOnAnRvalueContainerReturnsTheResultOfTheEndMemberFunction) {
  testing::StaticAssertTypeEq<
      decltype(std::end(std::declval<TypeParam&&>())),
      decltype(std::declval<TypeParam const&>().end())>();

  auto const temp_result{std::end(TypeParam{})};
  static_cast<void>(temp_result);
}

/// @test Invoking @c std::end on a container is @c noexcept if the corresponding @c end member is
CONSTEXPR_TYPED_TEST(EndContainerTests, InvokingEndOnANonConstContainerIsNoexceptIfTheMemberIs) {
  static_assert(
      noexcept(std::end(std::declval<TypeParam&>())) == noexcept(std::declval<TypeParam&>().end()),
      "Noexcept must be the same as the member function"
  );
}

/// @test Invoking @c std::end on a const reference to a container is @c noexcept if the corresponding @c end member is
CONSTEXPR_TYPED_TEST(EndContainerTests, InvokingEndOnAConstContainerIsNoexceptIfTheMemberIs) {
  static_assert(
      noexcept(std::end(std::declval<TypeParam const&>())) == noexcept(std::declval<TypeParam const&>().end()),
      "Noexcept must be the same as the member function"
  );
}

/// @test Invoking @c std::end on an rvalue container is @c noexcept if the corresponding @c const @c end member is
CONSTEXPR_TYPED_TEST(EndContainerTests, InvokingEndOnAnRvalueContainerIsNoexceptIfTheMemberIs) {
  static_assert(
      noexcept(std::end(std::declval<TypeParam&&>())) == noexcept(std::declval<TypeParam const&>().end()),
      "Noexcept must be the same as the const member function"
  );
}

template <typename T>
class CEndContainerTests : public testing::Test {};

TYPED_TEST_SUITE(CEndContainerTests, end_container_types, );

/// @test Invoking @c std::cend on a non-const container is equivalent to invoking @c std::end on a const reference to
/// the container
CONSTEXPR_TYPED_TEST(
    CEndContainerTests,
    InvokingCEndOnANonConstContainerIsEquivalentToInvokingCEndOnAConstQualifiedContainer
) {
  testing::StaticAssertTypeEq<
      decltype(std::cend(std::declval<TypeParam&>())),
      decltype(std::end(std::declval<TypeParam const&>()))>();

  TypeParam cont{};
  auto const nc_result{std::cend(cont)};
  static_cast<void>(nc_result);
}

/// @test Invoking @c std::cend on a const reference to a container is equivalent to invoking @c std::end on a
/// const reference to the container
CONSTEXPR_TYPED_TEST(CEndContainerTests, InvokingCEndOnAConstContainerReturnsTheResultInvokingEndOnAConstReference) {
  testing::StaticAssertTypeEq<
      decltype(std::cend(std::declval<TypeParam const&>())),
      decltype(std::end(std::declval<TypeParam const&>()))>();

  TypeParam const c_cont{};
  auto const c_result{std::cend(c_cont)};
  static_cast<void>(c_result);
}

/// @test Invoking @c std::cend on an rvalue container is equivalent to invoking @c std::end on a const reference to
/// the container
CONSTEXPR_TYPED_TEST(
    CEndContainerTests,
    InvokingCEndOnAnRvalueContainerReturnsTheResultOfInvokingEndOnAConstReference
) {
  testing::StaticAssertTypeEq<
      decltype(std::cend(std::declval<TypeParam&&>())),
      decltype(std::cend(std::declval<TypeParam const&>()))>();

  auto const temp_result{std::cend(TypeParam{})};
  static_cast<void>(temp_result);
}

/// @test Invoking @c std::cend on a non-const reference to a container is @c noexcept if @c std::end on a const
/// reference to the container is
CONSTEXPR_TYPED_TEST(CEndContainerTests, InvokingCEndOnANonConstContainerIsNoexceptIfEndOnConstIs) {
  static_assert(
      noexcept(std::cend(std::declval<TypeParam&>())) == noexcept(std::end(std::declval<TypeParam const&>())),
      "Noexcept must be the same as end on a const reference to the container"
  );
}

/// @test Invoking @c std::cend on a const reference to a container is @c noexcept if @c std::end on a const
/// reference to the container is
CONSTEXPR_TYPED_TEST(CEndContainerTests, InvokingCEndOnAConstContainerIsNoexceptIfEndOnConstIs) {
  static_assert(
      noexcept(std::cend(std::declval<TypeParam const&>())) == noexcept(std::end(std::declval<TypeParam const&>())),
      "Noexcept must be the same as end on a const reference to the container"
  );
}

/// @test Invoking @c std::cend on rvalue container is @c noexcept if @c std::end on a const
/// reference to the container is
CONSTEXPR_TYPED_TEST(CEndContainerTests, InvokingCEndOnAnRvalueContainerIsNoexceptIfEndOnConstIs) {
  static_assert(
      noexcept(std::cend(std::declval<TypeParam&&>())) == noexcept(std::end(std::declval<TypeParam const&>())),
      "Noexcept must be the same as end on a const reference to the container"
  );
}

}  // namespace
