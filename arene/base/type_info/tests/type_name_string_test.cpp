// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_info/type_name_string.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/any_of.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"

struct arene_base_type_name_string_testing_global {};

namespace arene {
namespace base {
namespace type_name_string_testing {
struct struct_in_namespace;
template <typename T>
struct test_template;
}  // namespace type_name_string_testing
}  // namespace base
}  // namespace arene

namespace {
// NOLINTNEXTLINE(google-runtime-int)
using uint = unsigned;
// NOLINTNEXTLINE(google-runtime-int)
using longlong = long long;
// NOLINTNEXTLINE(google-runtime-int)
using longint = long;
// NOLINTNEXTLINE(google-runtime-int)
using shortint = short;
// NOLINTNEXTLINE(google-runtime-int)
using ushortint = unsigned short;

/// @test `type_name_v` provides strings of the correct length, for basic types.
TEST(TypeNameString, StringLengthsAreRightForBasicTypes) {
  ::testing::StaticAssertTypeEq<decltype(arene::base::type_name_v<void>), arene::base::inline_string<4> const>();
  ::testing::StaticAssertTypeEq<decltype(arene::base::type_name_v<int>), arene::base::inline_string<3> const>();
  ::testing::StaticAssertTypeEq<decltype(arene::base::type_name_v<bool>), arene::base::inline_string<4> const>();
  ::testing::StaticAssertTypeEq<decltype(arene::base::type_name_v<char>), arene::base::inline_string<4> const>();
  ::testing::
      StaticAssertTypeEq<decltype(arene::base::type_name_v<unsigned char>), arene::base::inline_string<13> const>();
  ::testing::StaticAssertTypeEq<decltype(arene::base::type_name_v<signed char>), arene::base::inline_string<11> const>(
  );
  ::testing::StaticAssertTypeEq<decltype(arene::base::type_name_v<double>), arene::base::inline_string<6> const>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<longlong>),
      arene::base::inline_string<arene::base::type_name_v<longlong>.size()> const>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<longint>),
      arene::base::inline_string<arene::base::type_name_v<longint>.size()> const>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<uint>),
      arene::base::inline_string<arene::base::type_name_v<uint>.size()> const>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<shortint>),
      arene::base::inline_string<arene::base::type_name_v<shortint>.size()> const>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<ushortint>),
      arene::base::inline_string<arene::base::type_name_v<ushortint>.size()> const>();
}

/// @brief Check if a string_view is equal to one of a set of candidate values
/// @tparam The candidate string views
/// @param value The value to check for
/// @param candidates The rest of the candidates to check against
template <class... Candidates>
constexpr auto is_any_of(arene::base::string_view value, Candidates... candidates) {
  return arene::base::any_of(value == candidates...);
}

/// @test `type_name_v` provides correct type names, for basic types.
TEST(TypeNameString, CanGetTypeNameForBasicTypes) {
  STATIC_ASSERT_EQ(arene::base::type_name_v<void>, "void");
  STATIC_ASSERT_EQ(arene::base::type_name_v<int>, "int");
  STATIC_ASSERT_EQ(arene::base::type_name_v<bool>, "bool");
  STATIC_ASSERT_EQ(arene::base::type_name_v<char>, "char");
  STATIC_ASSERT_EQ(arene::base::type_name_v<unsigned char>, "unsigned char");
  STATIC_ASSERT_EQ(arene::base::type_name_v<signed char>, "signed char");
  STATIC_ASSERT_EQ(arene::base::type_name_v<double>, "double");

  STATIC_ASSERT_TRUE(is_any_of(arene::base::type_name_v<longlong>, "long long int", "long long"));

  STATIC_ASSERT_TRUE(is_any_of(arene::base::type_name_v<longint>, "long", "long int"));

  STATIC_ASSERT_TRUE(is_any_of(arene::base::type_name_v<shortint>, "short", "short int"));

  STATIC_ASSERT_TRUE(is_any_of(arene::base::type_name_v<uint>, "unsigned", "unsigned int"));

  STATIC_ASSERT_TRUE(is_any_of(
      arene::base::type_name_v<ushortint>,
      "unsigned short",
      "unsigned short int",
      "short unsigned int",
      "short unsigned"
  ));
}

struct complete {};

struct incomplete;

enum class enum_class : std::uint8_t { enumerator };

/// @test `type_name_v` provides strings of the correct length, for types in an anonymous namespace.
TEST(TypeNameString, SizesAreRightForTypesInAnonymousNamespace) {
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<complete>),
      arene::base::inline_string<arene::base::type_name_v<complete>.size()> const>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<incomplete>),
      arene::base::inline_string<arene::base::type_name_v<incomplete>.size()> const>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<enum_class>),
      arene::base::inline_string<arene::base::type_name_v<enum_class>.size()> const>();
}

/// @test `type_name_v` provides correct type names, for types in an anonymous namespace.
TEST(TypeNameString, NamesOfTypesInAnonymousNamespace) {
  STATIC_ASSERT_NE(arene::base::type_name_v<complete>.find("::complete"), arene::base::string_view::npos);

  STATIC_ASSERT_NE(arene::base::type_name_v<incomplete>.find("::incomplete"), arene::base::string_view::npos);

  STATIC_ASSERT_NE(arene::base::type_name_v<enum_class>.find("::enum_class"), arene::base::string_view::npos);
}

/// @test `type_name_v` provides strings of the correct length and containing correct type names, for types in the
/// global namespace.
TEST(TypeNameString, NamesOfTypeInGlobalNamespace) {
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<arene_base_type_name_string_testing_global>),
      arene::base::inline_string<arene::base::type_name_v<arene_base_type_name_string_testing_global>.size()> const>();
  STATIC_ASSERT_FALSE(arene::base::type_name_v<arene_base_type_name_string_testing_global>.empty());
  ASSERT_STREQ(
      arene::base::type_name_v<arene_base_type_name_string_testing_global>.c_str(),
      "arene_base_type_name_string_testing_global"
  );
}

/// @test `type_name_v` provides strings of the correct length and containing correct type names, for types in nested
/// namespaces.
TEST(TypeNameString, NamesOfTypeInNestedNamespace) {
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<arene::base::type_name_string_testing::struct_in_namespace>),
      arene::base::inline_string<
          arene::base::type_name_v<arene::base::type_name_string_testing::struct_in_namespace>.size()> const>();
  STATIC_ASSERT_FALSE(arene::base::type_name_v<arene::base::type_name_string_testing::struct_in_namespace>.empty());
  ASSERT_STREQ(
      arene::base::type_name_v<arene::base::type_name_string_testing::struct_in_namespace>.c_str(),
      "arene::base::type_name_string_testing::struct_in_namespace"
  );
}

/// @test `type_name_v` provides strings of the correct length and containing correct type names, for templated types.
TEST(TypeNameString, NamesOfTemplate) {
  using arg = arene::base::type_name_string_testing::struct_in_namespace;
  using type_under_test = arene::base::type_name_string_testing::test_template<arg>;
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::type_name_v<type_under_test>),
      arene::base::inline_string<arene::base::type_name_v<type_under_test>.size()> const>();
  STATIC_ASSERT_FALSE(arene::base::type_name_v<type_under_test>.empty());
  STATIC_ASSERT_STREQ(
      arene::base::type_name_v<type_under_test>.c_str(),
      (arene::base::inline_string<100>("arene::base::type_name_string_testing::test_template<") +
       arene::base::type_name_v<arg> + ">")
          .c_str()
  );
}

/// @test Internal functions of `type_name_string_detail` work as intended.
TEST(TypeNameStringInternalCoverage, ChecksOfInternalFunctions) {
  ASSERT_NE(arene::base::type_name_string_detail::get_big_raw_type_name_length_hint<int>(), 0);
  ASSERT_EQ(arene::base::type_name_string_detail::get_big_raw_type_name_length_hint<void>(), arene::base::type_name_string_detail::void_length_hint<>);

  ASSERT_EQ(arene::base::type_name_string_detail::get_big_raw_type_name<int>().size(), arene::base::type_name_string_detail::raw_type_name_buffer_size<int>);
  ASSERT_EQ(arene::base::type_name_string_detail::get_big_raw_type_name<void>().size(), arene::base::type_name_string_detail::raw_type_name_buffer_size<void>);

  ASSERT_EQ(
      arene::base::type_name_string_detail::get_raw_type_name_string_view<int>(),
      arene::base::string_view{arene::base::type_name_string_detail::get_big_raw_type_name<int>().data()}
  );
  ASSERT_EQ(
      arene::base::type_name_string_detail::get_raw_type_name_string_view<void>(),
      arene::base::string_view{arene::base::type_name_string_detail::get_big_raw_type_name<void>().data()}
  );

  ASSERT_NE(arene::base::type_name_string_detail::get_name_offsets().characters_before, 0);

  ASSERT_EQ(
      arene::base::type_name_string_detail::get_type_name_string_length<int>(),
      arene::base::type_name_v<int>.size()
  );
  ASSERT_EQ(
      arene::base::type_name_string_detail::get_type_name_string_length<void>(),
      arene::base::type_name_v<void>.size()
  );

  ASSERT_EQ(arene::base::type_name_string_detail::get_type_name_string<int>(), arene::base::type_name_v<int>);
  ASSERT_EQ(arene::base::type_name_string_detail::get_type_name_string<void>(), arene::base::type_name_v<void>);
}
}  // namespace
