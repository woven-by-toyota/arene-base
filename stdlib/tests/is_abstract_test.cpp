// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

// NOLINTBEGIN(hicpp-special-member-functions)

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

template <class T>
struct IsAbstract : ::testing::Test {};

struct abstract_type {
  virtual ~abstract_type() = 0;
};

struct abstract_type_with_pure_member_function {
  virtual void foo() = 0;
  virtual ~abstract_type_with_pure_member_function() = default;
};

struct inherits_from_abstract_type : abstract_type_with_pure_member_function {};

using abstract_types = ::testing::Types<  //
    abstract_type,
    abstract_type_with_pure_member_function,
    inherits_from_abstract_type>;

TYPED_TEST_SUITE(IsAbstract, abstract_types, );

/// @test @c is_base_of defines static member @c value to @c true if @c Base is a base class of @c Derived
TYPED_TEST(IsAbstract, AbstractTypes) {
  static_assert(std::is_abstract_v<TypeParam>, "");
  static_assert(std::is_abstract_v<TypeParam const>, "");
  static_assert(std::is_abstract_v<TypeParam volatile>, "");
  static_assert(std::is_abstract_v<TypeParam const volatile>, "");

  static_assert(  //
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam>, std::true_type>,
      ""
  );
  static_assert(  //
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam const>, std::true_type>,
      ""
  );
  static_assert(  //
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam volatile>, std::true_type>,
      ""
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam const volatile>, std::true_type>,
      ""
  );
}

template <class T>
struct IsNotAbstract : ::testing::Test {};

struct class_with_virtual_functions {
  virtual ~class_with_virtual_functions() = default;
};
struct class_derived_from_abstract_base : abstract_type {
  ~class_derived_from_abstract_base() override = default;
};

using non_abstract_types = arene::base::type_lists::concat_unique_t<
    ::testing::non_referenceable_types,
    ::testing::scalar_types,
    ::testing::array_types,
    ::testing::function_types,
    ::testing::pointer_types,
    ::testing::member_pointer_types,
    ::testing::reference_types,
    ::testing::union_types,
    ::testing::enum_types,
    ::testing::Types<class_with_virtual_functions, class_derived_from_abstract_base>>;

TYPED_TEST_SUITE(IsNotAbstract, non_abstract_types, );

/// @test @c is_abstract defines static member @c value to @c false if the type is not abstract
TYPED_TEST(IsNotAbstract, NonAbstractTypes) {
  static_assert(!std::is_abstract_v<TypeParam>, "");
  static_assert(!std::is_abstract_v<TypeParam const>, "");
  static_assert(!std::is_abstract_v<TypeParam volatile>, "");
  static_assert(!std::is_abstract_v<TypeParam const volatile>, "");

  static_assert(  //
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam>, std::false_type>,
      ""
  );
  static_assert(  //
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam const>, std::false_type>,
      ""
  );
  static_assert(  //
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam volatile>, std::false_type>,
      ""
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_abstract<TypeParam const volatile>, std::false_type>,
      ""
  );
}

}  // namespace

// NOLINTEND(hicpp-special-member-functions)
