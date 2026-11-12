// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/indirectly_readable_traits.hpp"

#include <gtest/gtest.h>

#include "arene/base/iterator/detail/cond_value_type.hpp"
#include "arene/base/iterator/detail/has_member_value_type.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_extent.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"

/// @brief A type that is explicitly specialized for `indirectly_readable_traits`.
struct explicitly_specialized_type {
  struct nested {};
};

/// @brief Specialization of `indirectly_readable_traits` for `explicitly_specialized_type`.
template <>
class arene::base::indirectly_readable_traits<explicitly_specialized_type> {
 public:
  /// @brief The type held by `explicitly_specialized_type` as its `value_type`.
  using value_type = typename explicitly_specialized_type::nested;
};

namespace {
using ::arene::base::indirectly_readable_traits;
using ::arene::base::iterator_detail::cond_value_type;
using ::arene::base::iterator_detail::has_member_value_type_v;
namespace type_lists = ::arene::base::type_lists;

template <typename T>
class IndirectlyReadableTraitsArrayTypesTest : public ::testing::Test {};

// NOLINTBEGIN(hicpp-avoid-c-arrays) explicitly testing behavior for c-arrays
using non_const_array_types = ::testing::Types<int[1], double[2], char[3]>;
using const_array_types = ::testing::Types<int const[1], double const[2], char const[3]>;
using all_array_types = type_lists::concat_t<non_const_array_types, const_array_types>;

TYPED_TEST_SUITE(IndirectlyReadableTraitsArrayTypesTest, all_array_types, );

/// @test Given an array type `Arr`, then `indirectly_readable_traits<Arr>` has a member typedef `value_type` defined as
/// `std::remove_cv_t<std::remove_extent_t<Arr>>`.
TYPED_TEST(IndirectlyReadableTraitsArrayTypesTest, ValueTypeIsTypeHeldInArrayCVStripped) {
  ::testing::StaticAssertTypeEq<
      typename indirectly_readable_traits<TypeParam>::value_type,
      std::remove_cv_t<std::remove_extent_t<TypeParam>>>();
}
// NOLINTEND(hicpp-avoid-c-arrays)

using non_const_qualified_type = int;
using const_qualified_type = non_const_qualified_type const;
using value_types = ::testing::Types<non_const_qualified_type, const_qualified_type>;

template <typename T>
struct yes_value_type_no_element_type {
  using value_type = T;
};

using only_value_type_types = type_lists::apply_each_t<value_types, yes_value_type_no_element_type>;
using const_only_value_type_types = type_lists::apply_each_t<only_value_type_types, std::add_const_t>;
using all_value_type_only_types = type_lists::concat_t<only_value_type_types, const_only_value_type_types>;

template <typename T>
class IndirectlyReadableTraitsValueTypeOnlyTest : public ::testing::Test {};

TYPED_TEST_SUITE(IndirectlyReadableTraitsValueTypeOnlyTest, all_value_type_only_types, );

/// @test Given a type `T` that has a member typedef `value_type` but no member typedef `element_type`, and that type
/// satisfies `std::is_object`, then `indirectly_readable_traits<T>` has a member typedef `value_type` defined as
/// `cond_value_type<typename T::value_type>::value_type`.
TYPED_TEST(IndirectlyReadableTraitsValueTypeOnlyTest, ValueTypeIsTypeHeldInValueType) {
  ::testing::StaticAssertTypeEq<
      typename indirectly_readable_traits<TypeParam>::value_type,
      typename cond_value_type<typename TypeParam::value_type>::value_type>();
}

template <typename T>
struct no_value_type_yes_element_type {
  using element_type = T;
};

using only_element_type_types = type_lists::apply_each_t<value_types, no_value_type_yes_element_type>;
using const_only_element_type_types = type_lists::apply_each_t<only_element_type_types, std::add_const_t>;
using all_element_type_only_types = type_lists::concat_t<only_element_type_types, const_only_element_type_types>;

template <typename T>
class IndirectlyReadableTraitsElementTypeOnlyTest : public ::testing::Test {};

TYPED_TEST_SUITE(IndirectlyReadableTraitsElementTypeOnlyTest, all_element_type_only_types, );

/// @test Given a type `T` that has a member typedef `element_type` but no member typedef `value_type`, and that type
/// satisfies `std::is_object`, then `indirectly_readable_traits<T>` has a member typedef `value_type` defined as
/// `cond_value_type<typename T::element_type>::value_type`.
TYPED_TEST(IndirectlyReadableTraitsElementTypeOnlyTest, ValueTypeIsTypeHeldInElementType) {
  ::testing::StaticAssertTypeEq<
      typename indirectly_readable_traits<TypeParam>::value_type,
      typename cond_value_type<typename TypeParam::element_type>::value_type>();
}

template <typename V, typename E = V>
struct yes_value_type_yes_element_type {
  using value_type = V;
  using element_type = E;
};

template <typename T>
using yes_value_type_yes_element_type_same_type_value_const = yes_value_type_yes_element_type<T const, T>;

template <typename T>
using yes_value_type_yes_element_type_same_type_element_const = yes_value_type_yes_element_type<T, T const>;

template <typename T>
using yes_value_type_yes_element_type_same_type_both_const = yes_value_type_yes_element_type<T const, T const>;

using value_type_and_element_type_types = ::testing::Types<
    yes_value_type_yes_element_type<non_const_qualified_type>,

    yes_value_type_yes_element_type_same_type_value_const<non_const_qualified_type>,
    yes_value_type_yes_element_type_same_type_element_const<non_const_qualified_type>,
    yes_value_type_yes_element_type_same_type_both_const<non_const_qualified_type>>;
using const_value_type_and_element_type_types =
    type_lists::apply_each_t<value_type_and_element_type_types, std::add_const_t>;
using all_value_type_and_element_type_types =
    type_lists::concat_t<value_type_and_element_type_types, const_value_type_and_element_type_types>;

template <typename T>
class IndirectlyReadableTraitsValueTypeAndElementTypeTest : public ::testing::Test {};

TYPED_TEST_SUITE(IndirectlyReadableTraitsValueTypeAndElementTypeTest, all_value_type_and_element_type_types, );

/// @test Given a type `T` that has a member typedef `value_type` and a member typedef `element_type` that is the same
/// type, and that type satisfies `std::is_object`, then `indirectly_readable_traits<T>` has a member typedef
/// `value_type` defined as `cond_value_type<typename T::value_type>::value_type`.
TYPED_TEST(IndirectlyReadableTraitsValueTypeAndElementTypeTest, ValueTypeIsSameAsValueType) {
  ::testing::StaticAssertTypeEq<
      typename indirectly_readable_traits<TypeParam>::value_type,
      typename cond_value_type<typename TypeParam::value_type>::value_type>();
}

template <typename T>
class IndirectlyReadableTraitsExplicitlySpecializedTypeTest : public ::testing::Test {};

using explicitly_specialized_type_list =
    ::testing::Types<explicitly_specialized_type, explicitly_specialized_type const>;

TYPED_TEST_SUITE(IndirectlyReadableTraitsExplicitlySpecializedTypeTest, explicitly_specialized_type_list, );

/// @test Given a type `T` that is explicitly specialized for `indirectly_readable_traits`, then
/// `indirectly_readable_traits<T>` is defined as the explicit specialization.
TYPED_TEST(IndirectlyReadableTraitsExplicitlySpecializedTypeTest, ValueTypeIsDefinedAsNested) {
  ::testing::StaticAssertTypeEq<
      typename ::arene::base::indirectly_readable_traits<TypeParam>::value_type,
      explicitly_specialized_type::nested>();
}

using yes_value_type_yes_element_type_different_type = yes_value_type_yes_element_type<int, double>;

using different_value_type_and_element_type_types = ::testing::
    Types<yes_value_type_yes_element_type_different_type, yes_value_type_yes_element_type_different_type const>;

template <typename T>
class IndirectlyReadableTraitsDifferentValueTypeAndElementTypeTest : public ::testing::Test {};

TYPED_TEST_SUITE(
    IndirectlyReadableTraitsDifferentValueTypeAndElementTypeTest,
    different_value_type_and_element_type_types,
);

/// @test Given a type `T` that has a member typedef `value_type` and a member typedef `element_type`  which are not the
/// same type, then `indirectly_readable_traits<T>` does not define a member typedef `value_type`
TYPED_TEST(IndirectlyReadableTraitsDifferentValueTypeAndElementTypeTest, ValueTypeIsNotDefined) {
  STATIC_ASSERT_FALSE(has_member_value_type_v<indirectly_readable_traits<TypeParam>>);
}

using non_object_value_types = ::testing::Types<int&, void, int()>;
using value_type_non_object_types = type_lists::apply_each_t<non_object_value_types, yes_value_type_no_element_type>;
using const_value_type_non_object_types = type_lists::apply_each_t<value_type_non_object_types, std::add_const_t>;
using element_type_non_object_types = type_lists::apply_each_t<non_object_value_types, no_value_type_yes_element_type>;
using const_element_type_non_object_types = type_lists::apply_each_t<element_type_non_object_types, std::add_const_t>;
using non_object_value_type_and_element_type_types =
    type_lists::apply_each_t<non_object_value_types, yes_value_type_yes_element_type>;
using const_non_object_value_type_and_element_type_types =
    type_lists::apply_each_t<non_object_value_type_and_element_type_types, std::add_const_t>;
using all_non_object_value_type_and_element_type_types = type_lists::concat_t<
    value_type_non_object_types,
    const_value_type_non_object_types,
    element_type_non_object_types,
    const_element_type_non_object_types,
    non_object_value_type_and_element_type_types,
    const_non_object_value_type_and_element_type_types>;

template <typename T>
class IndirectlyReadableTraitsNonObjectValueTypeAndElementTypeTest : public ::testing::Test {};
TYPED_TEST_SUITE(
    IndirectlyReadableTraitsNonObjectValueTypeAndElementTypeTest,
    all_non_object_value_type_and_element_type_types,
);

/// @test Given a type `T` satisfies one of the valid combinations of member typedefs, but where that type does not
/// satisfy `std::is_object`, then `indirectly_readable_traits<T>` does not define a member typedef `value_type`
TYPED_TEST(IndirectlyReadableTraitsNonObjectValueTypeAndElementTypeTest, ValueTypeIsNotDefined) {
  STATIC_ASSERT_FALSE(has_member_value_type_v<indirectly_readable_traits<TypeParam>>);
}

}  // namespace
