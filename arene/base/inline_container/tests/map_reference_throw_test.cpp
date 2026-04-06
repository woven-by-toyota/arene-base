// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_container/map_reference.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

template <bool IsNoexcept>
struct reverse_less_than {
  template <typename T, typename U>
  constexpr auto operator()(T const& lhs, U const& rhs) const noexcept(IsNoexcept) -> bool {
    return rhs < lhs;
  }

  friend constexpr auto operator==(reverse_less_than, reverse_less_than) noexcept -> bool { return true; }
};

class custom_key {
  int value_{};

 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr custom_key(int value)
      : value_{value} {}

  friend constexpr auto operator<(custom_key lhs, custom_key rhs) noexcept -> bool { return lhs.value_ < rhs.value_; }
};

template <std::size_t Capacity, bool IsNoexcept = true>
auto make_map_with_default_values() {
  return arene::base::inline_map<custom_key, int, Capacity, reverse_less_than<IsNoexcept>>{
      std::pair<custom_key, int>{3, {}},
      {2, {}},
      {1, {}},
  };
}

template <std::size_t>
struct capacity {};
template <std::size_t>
struct param_value {};
template <bool>
struct compare_is_noexcept {};

template <class Capacity, class ParamValue, class CompareIsNoexcept>
struct test_case {};
template <std::size_t Capacity, std::size_t ParamValue, bool CompareIsNoexcept>
struct test_case<capacity<Capacity>, param_value<ParamValue>, compare_is_noexcept<CompareIsNoexcept>> {
  static constexpr auto capacity = Capacity;
  static constexpr auto param_value = ParamValue;
  static constexpr auto compare_is_noexcept = CompareIsNoexcept;
};

using MyTypes = ::testing::Types<
    test_case<capacity<5>, param_value<0>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<1>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<2>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<3>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<4>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<0>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<1>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<2>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<3>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<4>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<0>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<1>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<2>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<3>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<4>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<0>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<1>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<2>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<3>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<4>, compare_is_noexcept<false>>>;

template <class T>
struct ConstMapReferenceThrow : ::testing::Test {};

template <class T>
struct MapReferenceThrow : ::testing::Test {};

TYPED_TEST_SUITE(ConstMapReferenceThrow, MyTypes, );
TYPED_TEST_SUITE(MapReferenceThrow, MyTypes, );

template <bool IsNoexcept = true>
using const_map_ref = arene::base::const_inline_map_reference<custom_key, int, reverse_less_than<IsNoexcept>>;

template <bool IsNoexcept = true>
using map_ref = arene::base::inline_map_reference<custom_key, int, reverse_less_than<IsNoexcept>>;

/// @test A `const_inline_map_reference` returns the same value or throws the
/// same exception as `inline_map` for const member functions that take one key
/// arg and throw if the key is not in the map.
TYPED_TEST(ConstMapReferenceThrow, SameReturnForUnaryMemberFunctionThatThrow) {
  auto const map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;

  try {
    typename decltype(map)::mapped_type const* const direct_address = &map.at(key);
    typename const_map_ref::mapped_type const* const ref_address = &const_map_ref{map}.at(key);
    testing::StaticAssertTypeEq<decltype(direct_address), decltype(ref_address)>();
    ASSERT_EQ(direct_address, ref_address);
  } catch (std::out_of_range const&) {
    ASSERT_THROW(const_map_ref{map}.at(key), std::out_of_range);
  }
}

/// @test An `inline_map_reference` returns the same value or throws the
/// same exception as `inline_map` for const member functions that take one key
/// arg and throw if the key is not in the map.
TYPED_TEST(MapReferenceThrow, SameReturnForUnaryMemberFunctionThatThrow) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  try {
    typename decltype(map)::mapped_type const* const direct_address = &map.at(key);
    typename map_ref::mapped_type const* const ref_address = &map_ref{map}.at(key);
    testing::StaticAssertTypeEq<decltype(direct_address), decltype(ref_address)>();
    ASSERT_EQ(direct_address, ref_address);
  } catch (std::out_of_range const&) {
    ASSERT_THROW(map_ref{map}.at(key), std::out_of_range);
  }
}

/// @test member @c at() of @c inline_map_reference is invocable with a @c const qualified @c inline_map_reference
TYPED_TEST(MapReferenceThrow, MemberAtIsConstInvocable) {
  using map_reference = ::map_ref<TypeParam::compare_is_noexcept>;
  using key_type = typename map_reference::key_type;

  using arene::base::is_invocable_v;

  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::template at<>), map_reference const&, key_type const&>));
}

/// @test The `noexcept`-ness of member @c at() of `inline_map_reference` matches the `noexcept`-ness of the
/// corresponding member function of `inline_map`.
TYPED_TEST(MapReferenceThrow, MemberAtNoexceptNess) {
  using map_type = decltype(make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>());
  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;
  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;

  using key_type = typename map_ref::key_type;

  static_assert(
      noexcept(std::declval<map_type&>().at(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().at(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().at(std::declval<key_type>())) ==
          noexcept(std::declval<const_map_ref&>().at(std::declval<key_type>())),
      "noexcept should match"
  );
}

}  // namespace
