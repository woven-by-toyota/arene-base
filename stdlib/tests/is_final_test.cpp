// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief Determine @c std::is_final_v with a cv-qualified type
/// @tparam T cv-unqualified type to check
/// @return @c std::is_final_v<T>
///
/// Returns @c std::is_final_v<T> and statically asserts that the result is
/// consistent with type trait @c std::is_final<T> for cv-qualified
/// variations of @c T.
///
template <class T>
constexpr auto all_cv_qualified_is_final() noexcept -> bool {
  static_assert(
      std::is_final_v<T> == std::is_final_v<T const>,
      "'std::is_final_v<T> has the same value for 'T' and 'T const'."
  );
  static_assert(
      std::is_final_v<T> == std::is_final_v<T volatile>,
      "'std::is_final_v<T> has the same value for 'T' and 'T volatile'."
  );
  static_assert(
      std::is_final_v<T> == std::is_final_v<T const volatile>,
      "'std::is_final_v<T> has the same value for 'T' and 'T const volatile'."
  );

  using base_characteristic = std::bool_constant<std::is_final_v<T>>;
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_final<T>, base_characteristic>,
      "'std::is_final<T>' is unambiguously publicly derived from expected base characteristic."
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_final<T const>, base_characteristic>,
      "'std::is_final<T const>' is unambiguously publicly derived from the same base characteristic as "
      "'std::is_final<T>'."
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_final<T volatile>, base_characteristic>,
      "'std::is_final<T volatile>' is unambiguously publicly derived from the same base characteristic as "
      "'std::is_final<T>'."
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_final<T const volatile>, base_characteristic>,
      "'std::is_final<T const volatile>' is unambiguously publicly derived from the same base characteristic as "
      "'std::is_final<T>'."
  );

  return std::is_final_v<T>;
}

struct final_struct final {};
class final_class final {};
union final_union final {};

using final_types = ::testing::Types<final_struct, final_class, final_union>;

template <class T>
struct IsFinal : ::testing::Test {};

TYPED_TEST_SUITE(IsFinal, final_types, );

/// @test @c is_final defines static member @c value to @c false if the type does not have the @c final specifier.
TYPED_TEST(IsFinal, FinalTypeHasValueMemberTrue) {
  static_assert(all_cv_qualified_is_final<TypeParam>(), "std::is_final_v is true if type has the final specifier.");
}

struct non_final_type {
  virtual ~non_final_type() = default;
  non_final_type() = default;

  non_final_type(non_final_type const&) = delete;
  non_final_type(non_final_type&&) = delete;
  auto operator=(non_final_type const&) -> non_final_type& = delete;
  auto operator=(non_final_type&&) -> non_final_type& = delete;

  virtual auto operator()() const -> void {}
};
struct has_final_member_function : non_final_type {
  void operator()() const final {}
};
struct has_final_struct_as_member {
  final_struct data;
};
struct has_final_class_as_member {
  final_class data;
};
struct has_final_union_as_member {
  final_union data;
};

using non_final_types = arene::base::type_lists::concat_unique_t<
    ::testing::non_referenceable_types,
    ::testing::scalar_types,
    ::testing::array_types,
    ::testing::function_types,
    ::testing::pointer_types,
    ::testing::member_pointer_types,
    ::testing::reference_types,
    ::testing::class_types,
    ::testing::union_types,
    ::testing::enum_types,
    ::testing::extend_with_reference_and_pointer_types_t<::testing::Types<
        non_final_type,
        has_final_member_function,
        has_final_struct_as_member,
        has_final_class_as_member,
        has_final_union_as_member>>>;

template <class T>
struct NotIsFinal : ::testing::Test {};

TYPED_TEST_SUITE(NotIsFinal, non_final_types, );

/// @test @c is_final defines static member @c value to @c false if the type does not have the @c final specifier.
TYPED_TEST(NotIsFinal, NonFinalHasValueMemberFalse) {
  static_assert(
      !all_cv_qualified_is_final<TypeParam>(),
      "std::is_final_v is false if type does not have the final specifier."
  );
}

}  // namespace
