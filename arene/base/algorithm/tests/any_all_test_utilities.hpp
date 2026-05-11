// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_ANY_ALL_TEST_UTILITIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_ANY_ALL_TEST_UTILITIES_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/iterator_types.hpp"
#include "testlibs/utilities/throws_on.hpp"

namespace test {

/// @brief return an array of values, converting to type 'T'
/// @tparam To type to convert to
/// @tparam From type to convert from
/// @tparam Size number of elements in the array
/// @note Works around a deficiency in GCC8 related to type deduction,
///   preventing use of 'arene::base::to_array'.
///
/// @{
// NOLINTBEGIN(hicpp-avoid-c-arrays)
template <class To, class From, std::size_t Size, std::size_t... Is>
constexpr auto to_array_of_impl(From const (&values)[Size], std::index_sequence<Is...>) {
  return arene::base::array<To, Size>{To{values[Is]}...};
}
template <class To, class From, std::size_t Size>
constexpr auto to_array_of(From const (&values)[Size]) {
  return to_array_of_impl<To>(values, std::make_index_sequence<Size>{});
}
// NOLINTEND(hicpp-avoid-c-arrays)
/// @}

struct is_even {
  constexpr auto operator()(int value) const -> bool { return value % 2 == 0; }
};

struct is_odd {
  constexpr auto operator()(int value) const -> bool { return value % 2 != 0; }
};

struct wrapped_int {
  int value;

  constexpr explicit wrapped_int(int arg)
      : value{arg} {}
};

struct value_of {
  constexpr auto operator()(wrapped_int value) const -> int { return value.value; }
};

struct only_int_ref_arg {
  auto operator()(int&) const -> bool { return {}; }
};

struct result_not_boolean_convertible {
  auto operator()(int) const -> decltype(nullptr) { return {}; }
};

template <class Algorithm>
auto static_assert_not_invocable_with_invalid_predicate(Algorithm) -> void {
  static_assert(
      arene::base::is_invocable_v<Algorithm, int const*, int const*>,
      "must be invocable with valid predicate (the default)"
  );

  // 'value_of' requires a 'wrapped_int' as an argument
  using not_valid_with_element = value_of;
  static_assert(
      !arene::base::is_invocable_v<Algorithm, int const*, int const*, not_valid_with_element>,
      "must not be invocable with invalid predicate"
  );

  static_assert(
      !arene::base::is_invocable_v<Algorithm, int const*, int const*, only_int_ref_arg>,
      "must not be invocable with invalid predicate"
  );

  static_assert(
      !arene::base::is_invocable_v<Algorithm, int const*, int const*, result_not_boolean_convertible>,
      "must not be invocable with invalid predicate"
  );
}

template <class Algorithm>
auto static_assert_not_invocable_with_invalid_projection(Algorithm) -> void {
  static_assert(
      arene::base::is_invocable_v<Algorithm, int const*, int const*, is_even>,
      "must be invocable with valid projection (the default)"
  );

  // 'value_of' requires a 'wrapped_int' as an argument
  using not_valid_with_element = value_of;
  static_assert(
      !arene::base::is_invocable_v<Algorithm, int const*, int const*, is_even, not_valid_with_element>,
      "must not be invocable with invalid projection"
  );

  using not_valid_with_predicate = test::result_not_boolean_convertible;

  static_assert(
      !arene::base::is_invocable_v<Algorithm, int const*, int const*, is_even, not_valid_with_predicate>,
      "must not be invocable with invalid projection"
  );
}

template <class Algorithm>
auto static_assert_not_invocable_with_invalid_iterator(Algorithm) -> void {
  static_assert(  //
      !arene::base::is_invocable_v<Algorithm, int, int>,
      "must not be invocable with invalid iterator"
  );
}

/// @brief test that the result of an algorithm matches the expected result
/// @note applies 'demoted_iterator' to test with different iterator categories
/// @{
template <class IterCategory, class Algorithm, class Iter, class Predicate, class... Projection>
constexpr auto
assert_result_is_impl(bool expected, Algorithm algo, Iter first, Iter last, Predicate pred, Projection... proj)
    -> void {
  CONSTEXPR_ASSERT(
      expected == algo(  //
                      testing::make_demoted_iterator<IterCategory>(first),
                      testing::make_demoted_iterator<IterCategory>(last),
                      pred,
                      proj...
                  )
  );
}
template <class Algorithm, class Range, class Predicate, class... Projection>
constexpr auto assert_result_is(bool expected, Algorithm algo, Range const& range, Predicate pred, Projection... proj)
    -> void {
  assert_result_is_impl<std::random_access_iterator_tag>(expected, algo, range.begin(), range.end(), pred, proj...);
  assert_result_is_impl<std::bidirectional_iterator_tag>(expected, algo, range.begin(), range.end(), pred, proj...);
  assert_result_is_impl<std::forward_iterator_tag>(expected, algo, range.begin(), range.end(), pred, proj...);
  assert_result_is_impl<std::input_iterator_tag>(expected, algo, range.begin(), range.end(), pred, proj...);
}
/// @}

/// @brief test that the result of an algorithm matches the expected result
/// @note applies 'demoted_iterator' to test with different iterator categories
/// @{
///
template <bool Throws>
struct throws {
  template <class... Ts>
  auto operator()(Ts const&...) const noexcept(!Throws) -> bool {
    return true;
  }
};
template <bool Throws>
using pred_throws = throws<Throws>;
template <bool Throws>
using proj_throws = throws<Throws>;

template <class Algorithm, class Iterator, class... Tail>
constexpr auto is_nothrow_invocable_with_v = arene::base::is_nothrow_invocable_v<  //
    Algorithm&,
    Iterator,
    Iterator,
    Tail...>;

template <class Algorithm>
auto static_assert_has_correct_noexcept_specification(Algorithm) -> void {
  static_assert(                    //
      is_nothrow_invocable_with_v<  //
          Algorithm const&,         //
          int const*                //
          >,
      "must be noexcept(true)"
  );
  static_assert(                    //
      is_nothrow_invocable_with_v<  //
          Algorithm const&,         //
          int const*,               //
          pred_throws<false>,       //
          proj_throws<false>        //
          >,
      "must be noexcept(true)"
  );
  static_assert(                     //
      !is_nothrow_invocable_with_v<  //
          Algorithm const&,          //
          int const*,                //
          pred_throws<false>,        //
          proj_throws<true>          //
          >,
      "must be noexcept(false)"
  );
  static_assert(                     //
      !is_nothrow_invocable_with_v<  //
          Algorithm const&,          //
          int const*,                //
          pred_throws<true>,         //
          proj_throws<false>         //
          >,
      "must be noexcept(false)"
  );
  static_assert(                    //
      is_nothrow_invocable_with_v<  //
          Algorithm const&,         //
          int const*,               //
          pred_throws<false>        //
          >,
      "must be noexcept(true)"
  );
  static_assert(                     //
      !is_nothrow_invocable_with_v<  //
          Algorithm const&,          //
          int const*,                //
          pred_throws<true>          //
          >,
      "must be noexcept(false)"
  );

  using throws_on = testing::throws_on_specifiers;
  static_assert(                                                               //
      is_nothrow_invocable_with_v<                                             //
          Algorithm const&,                                                    //
          testing::noexcept_configurable_forward_iterator<throws_on::nothing>  //
          >,
      "must be noexcept(true)"
  );
  static_assert(                                                                      //
      !is_nothrow_invocable_with_v<                                                   //
          Algorithm const&,                                                           //
          testing::noexcept_configurable_forward_iterator<throws_on::copy_construct>  //
          >,
      "must be noexcept(false)"
  );
  static_assert(                                                                   //
      !is_nothrow_invocable_with_v<                                                //
          Algorithm const&,                                                        //
          testing::noexcept_configurable_forward_iterator<throws_on::dereference>  //
          >,
      "must be noexcept(false)"
  );
  static_assert(                                                                     //
      !is_nothrow_invocable_with_v<                                                  //
          Algorithm const&,                                                          //
          testing::noexcept_configurable_forward_iterator<throws_on::pre_increment>  //
          >,
      "must be noexcept(false)"
  );
  static_assert(                                                                 //
      !is_nothrow_invocable_with_v<                                              //
          Algorithm const&,                                                      //
          testing::noexcept_configurable_forward_iterator<throws_on::not_equal>  //
          >,
      "must be noexcept(false)"
  );
}
/// @}

}  // namespace test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_ANY_ALL_TEST_UTILITIES_HPP_
