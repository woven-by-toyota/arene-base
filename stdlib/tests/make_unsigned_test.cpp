// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/priority_tag.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

using invalid_types = ::testing::Types<
    bool,
    std::nullptr_t,
    int&,
    unsigned&,
    int const&,
    unsigned const&,
    int&&,
    unsigned&&,
    int const&&,
    unsigned const&&,
    float,
    double,
    long double,
    some_user_type,
    void (*)(int, double),
    int*,
    int const*,
    int volatile*,
    int const volatile*,
    void*,
    void const*,
    void volatile*,
    void const volatile*,
    int some_user_type::*,
    int (some_user_type::*)()>;

template <typename T>
class InvalidMakeUnsignedTest : public testing::Test {};

TYPED_TEST_SUITE(InvalidMakeUnsignedTest, invalid_types, );

/// @test @c make_unsigned is not defined if given something that is not an integral type
TYPED_TEST(InvalidMakeUnsignedTest, NotDefinedForUnsupportedTypes) {
  ASSERT_FALSE(::testing::has_member_type_v<std::make_unsigned<TypeParam>>);
}

template <typename Type, typename = arene::base::constraints<>>
constexpr bool make_unsigned_t_is_defined{false};

template <typename Type>
constexpr bool make_unsigned_t_is_defined<Type, arene::base::constraints<std::make_unsigned_t<Type>>>{true};

/// @test @c make_unsigned_t is not defined if given something that is not an integral type
TYPED_TEST(InvalidMakeUnsignedTest, TypeAliasNotDefinedForUnsupportedTypes) {
  ASSERT_FALSE(make_unsigned_t_is_defined<TypeParam>);
}

/// @brief Class template for source/target type correspondence
template <typename SourceType, typename TargetType>
struct source_target_pair {
  using source_type = SourceType;
  using mapped_type = TargetType;
};

using mapped_types = ::testing::Types<
    source_target_pair<unsigned char, unsigned char>,
    source_target_pair<unsigned short, unsigned short>,  // NOLINT
    source_target_pair<unsigned, unsigned>,
    source_target_pair<unsigned long, unsigned long>,            // NOLINT
    source_target_pair<unsigned long long, unsigned long long>,  // NOLINT
    source_target_pair<char, std::conditional_t<std::is_signed_v<char>, unsigned char, char>>,
    source_target_pair<
        wchar_t,
        std::conditional_t<
            sizeof(wchar_t) == sizeof(char),
            unsigned char,
            std::conditional_t<
                // NOLINTNEXTLINE(google-runtime-int)
                sizeof(wchar_t) == sizeof(short),
                // NOLINTNEXTLINE(google-runtime-int)
                unsigned short,
                std::conditional_t<
                    sizeof(wchar_t) == sizeof(int),
                    unsigned,
                    // NOLINTNEXTLINE(google-runtime-int)
                    std::conditional_t<sizeof(wchar_t) == sizeof(long), unsigned long, unsigned long long>>>>>,
    source_target_pair<signed char, unsigned char>,
    // NOLINTNEXTLINE(google-runtime-int)
    source_target_pair<char16_t, unsigned short>,
    source_target_pair<char32_t, unsigned int>,
    source_target_pair<short, unsigned short>,  // NOLINT
    source_target_pair<int, unsigned>,
    source_target_pair<long, unsigned long>,             // NOLINT
    source_target_pair<long long, unsigned long long>>;  // NOLINT

template <typename T>
class MappedMakeUnsignedTest : public testing::Test {};

TYPED_TEST_SUITE(MappedMakeUnsignedTest, mapped_types, );

/// @test @c make_unsigned has a @c type member that is the corresponding unsigned type for types where it is defined
TYPED_TEST(MappedMakeUnsignedTest, IdentifyMappingForMappedTypes) {
  ASSERT_EQ(
      sizeof(typename std::make_unsigned<typename TypeParam::source_type>::type),
      sizeof(typename TypeParam::source_type)
  );
  testing::StaticAssertTypeEq<
      typename std::make_unsigned<typename TypeParam::source_type>::type,
      typename TypeParam::mapped_type>();
  testing::StaticAssertTypeEq<
      typename std::make_unsigned<typename TypeParam::source_type const>::type,
      typename TypeParam::mapped_type const>();
  testing::StaticAssertTypeEq<
      typename std::make_unsigned<typename TypeParam::source_type volatile>::type,
      typename TypeParam::mapped_type volatile>();
  testing::StaticAssertTypeEq<
      typename std::make_unsigned<typename TypeParam::source_type const volatile>::type,
      typename TypeParam::mapped_type const volatile>();
}

/// @test @c make_unsigned_t is defined as alias of mapped type if given something that has a mapping
TYPED_TEST(MappedMakeUnsignedTest, TypeAliasDefinedForMappedTypes) {
  ASSERT_TRUE(make_unsigned_t_is_defined<typename TypeParam::source_type>);
  ASSERT_TRUE(make_unsigned_t_is_defined<typename TypeParam::source_type const>);
  ASSERT_TRUE(make_unsigned_t_is_defined<typename TypeParam::source_type volatile>);
  ASSERT_TRUE(make_unsigned_t_is_defined<typename TypeParam::source_type const volatile>);
  testing::StaticAssertTypeEq<std::make_unsigned_t<typename TypeParam::source_type>, typename TypeParam::mapped_type>();
  testing::StaticAssertTypeEq<
      std::make_unsigned_t<typename TypeParam::source_type const>,
      typename TypeParam::mapped_type const>();
  testing::StaticAssertTypeEq<
      std::make_unsigned_t<typename TypeParam::source_type volatile>,
      typename TypeParam::mapped_type volatile>();
  testing::StaticAssertTypeEq<
      std::make_unsigned_t<typename TypeParam::source_type const volatile>,
      typename TypeParam::mapped_type const volatile>();
}

// NOLINTBEGIN(google-runtime-int)

enum raw_enum {};
enum raw_enum_with_large_values { a = 1, b = 1234567890123LL };
enum raw_enum_char : char {};
enum raw_enum_int : int {};
enum raw_enum_short : short {};
enum raw_enum_long : long {};
enum raw_enum_uchar : unsigned char {};
enum raw_enum_uint : unsigned {};
enum raw_enum_ushort : unsigned short {};
enum raw_enum_ulong : unsigned long {};

enum class enum_class_char : char {};
enum class enum_class_int : int {};
enum class enum_class_short : short {};
enum class enum_class_long : long {};
enum class enum_class_uchar : unsigned char {};
enum class enum_class_uint : unsigned {};
enum class enum_class_ushort : unsigned short {};
enum class enum_class_ulong : unsigned long {};

using mapped_enum_types = ::testing::Types<
    enum_class_char,
    enum_class_int,
    enum_class_short,
    enum_class_long,
    enum_class_uchar,
    enum_class_uint,
    enum_class_ushort,
    enum_class_ulong,
    raw_enum_with_large_values,
    raw_enum_char,
    raw_enum_int,
    raw_enum_short,
    raw_enum_long,
    raw_enum_uchar,
    raw_enum_uint,
    raw_enum_ushort,
    raw_enum_ulong>;

template <typename T>
class MakeUnsignedEnumTest : public testing::Test {};

class unsigned_with_smallest_rank_and_same_size {
  // clang-format off
  template <class T> static auto impl(arene::base::priority_tag<4>) -> std::enable_if_t<sizeof(T) == sizeof(unsigned char), unsigned char>;
  template <class T> static auto impl(arene::base::priority_tag<3>) -> std::enable_if_t<sizeof(T) == sizeof(unsigned short), unsigned short>;
  template <class T> static auto impl(arene::base::priority_tag<2>) -> std::enable_if_t<sizeof(T) == sizeof(unsigned int), unsigned int>;
  template <class T> static auto impl(arene::base::priority_tag<1>) -> std::enable_if_t<sizeof(T) == sizeof(unsigned long), unsigned long>;
  template <class T> static auto impl(arene::base::priority_tag<0>) -> std::enable_if_t<sizeof(T) == sizeof(unsigned long long), unsigned long long>;
  // clang-format on

 public:
  template <class T>
  auto operator()(T) const -> decltype(impl<T>(arene::base::priority_tag<4>{}));
};

template <class T>
using unsigned_with_smallest_rank_and_same_size_t =
    decltype(unsigned_with_smallest_rank_and_same_size{}(std::declval<T>()));

// NOLINTEND(google-runtime-int)

TYPED_TEST_SUITE(MakeUnsignedEnumTest, mapped_enum_types, );

/// @test @c make_unsigned_t provides the unsigned version of the underlying type for enum types
TYPED_TEST(MakeUnsignedEnumTest, EnumTypes) {
  ASSERT_TRUE(make_unsigned_t_is_defined<TypeParam>);
  ASSERT_TRUE(make_unsigned_t_is_defined<TypeParam const>);
  ASSERT_TRUE(make_unsigned_t_is_defined<TypeParam volatile>);
  ASSERT_TRUE(make_unsigned_t_is_defined<TypeParam const volatile>);

  using mapped_type = std::make_unsigned_t<TypeParam>;
  testing::
      StaticAssertTypeEq<unsigned_with_smallest_rank_and_same_size_t<std::underlying_type_t<TypeParam>>, mapped_type>();

  testing::StaticAssertTypeEq<std::make_unsigned_t<TypeParam const>, mapped_type const>();
  testing::StaticAssertTypeEq<std::make_unsigned_t<TypeParam volatile>, mapped_type volatile>();
  testing::StaticAssertTypeEq<std::make_unsigned_t<TypeParam const volatile>, mapped_type const volatile>();
}

}  // namespace
