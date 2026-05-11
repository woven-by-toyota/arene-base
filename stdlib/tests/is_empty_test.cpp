// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief Determine @c std::is_empty_v with a cv-qualified type
/// @tparam T cv-unqualified type to check
/// @return @c std::is_empty_v<T>
///
/// Returns @c std::is_empty_v<T> and statically asserts that the result is
/// consistent with type trait @c std::is_empty<T> for cv-qualified
/// variations of @c T.
///
template <class T>
constexpr auto all_cv_qualified_is_empty() noexcept -> bool {
  static_assert(
      std::is_empty_v<T> == std::is_empty_v<T const>,
      "'std::is_empty_v<T> has the same value for 'T' and 'T const'."
  );
  static_assert(
      std::is_empty_v<T> == std::is_empty_v<T volatile>,
      "'std::is_empty_v<T> has the same value for 'T' and 'T volatile'."
  );
  static_assert(
      std::is_empty_v<T> == std::is_empty_v<T const volatile>,
      "'std::is_empty_v<T> has the same value for 'T' and 'T const volatile'."
  );

  using base_characteristic = std::bool_constant<std::is_empty_v<T>>;
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_empty<T>, base_characteristic>,
      "'std::is_empty<T>' is unambiguously publicly derived from expected base characteristic."
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_empty<T const>, base_characteristic>,
      "'std::is_empty<T const>' is unambiguously publicly derived from the same base characteristic as "
      "'std::is_empty<T>'."
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_empty<T volatile>, base_characteristic>,
      "'std::is_empty<T volatile>' is unambiguously publicly derived from the same base characteristic as "
      "'std::is_empty<T>'."
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_empty<T const volatile>, base_characteristic>,
      "'std::is_empty<T const volatile>' is unambiguously publicly derived from the same base characteristic as "
      "'std::is_empty<T>'."
  );

  return std::is_empty_v<T>;
}

struct empty_struct {};
class empty_class {};
struct empty_struct_with_member_function {
  auto operator()() const -> void {}
};
struct struct_with_empty_bases
    : empty_struct
    , empty_class {};
struct struct_with_static_data_member {
  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wunused-const-variable", "static member required for testing type properties");
  ARENE_IGNORE_ARMCLANG("-Wunused-const-variable", "static member required for testing type properties");
  static constexpr auto value = false;
  ARENE_IGNORE_END();
};

struct struct_with_bitfield_of_size_zero {
  int : 0;
};

using empty_types = ::testing::Types<
    empty_struct,
    empty_class,
    empty_struct_with_member_function,
    struct_with_empty_bases,
    struct_with_static_data_member,
    struct_with_bitfield_of_size_zero>;

template <class T>
struct IsEmpty : ::testing::Test {};

TYPED_TEST_SUITE(IsEmpty, empty_types, );

/// @test @c is_empty defines static member @c value to @c false if the type is not empty
TYPED_TEST(IsEmpty, EmptyTypeHasValueMemberTrue) {
  static_assert(all_cv_qualified_is_empty<TypeParam>(), "std::is_empty_v is true if type is empty.");
}

union union_with_no_members {};

struct has_data_member {
  int value{};
};
struct has_virtual_member_function {  // NOLINT(cppcoreguidelines-virtual-class-destructor)
  virtual auto operator()() const -> void {}
};
struct has_empty_struct_as_member {
  empty_struct data;
};
struct has_empty_class_as_member {
  empty_class data;
};

struct has_non_empty_base : has_data_member {};

struct has_virtual_base : virtual empty_struct {};

using non_empty_types = arene::base::type_lists::concat_unique_t<
    ::testing::non_referenceable_types,
    ::testing::scalar_types,
    ::testing::array_types,
    ::testing::function_types,
    ::testing::pointer_types,
    ::testing::member_pointer_types,
    ::testing::reference_types,
    ::testing::union_types,
    ::testing::enum_types,
    ::testing::extend_with_reference_and_pointer_types_t<::testing::Types<
        union_with_no_members,
        has_data_member,
        has_virtual_member_function,
        has_empty_struct_as_member,
        has_empty_class_as_member,
        has_non_empty_base,
        has_virtual_base>>>;

template <class T>
struct NotIsEmpty : ::testing::Test {};

TYPED_TEST_SUITE(NotIsEmpty, non_empty_types, );

/// @test @c is_empty defines static member @c value to @c false if the type is empty
TYPED_TEST(NotIsEmpty, NonEmptyHasValueMemberFalse) {
  static_assert(!all_cv_qualified_is_empty<TypeParam>(), "std::is_empty_v is false if type is not empty.");
}

}  // namespace
