// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/at.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

// NOLINTBEGIN(hicpp-avoid-c-arrays,readability-identifier-length,llvm-qualified-auto)

namespace test {

/// function object used to invoke a function per index of a tuple type
///
/// @{

// We don't use `arene::base::for_each_index` since we don't have a `std::tuple`
// value to pass as a parameter.

template <class List>
class for_each_element_of_t {
  template <std::size_t... Is, class Func>
  static constexpr auto impl(std::index_sequence<Is...>, Func func) {
    arene::base::consume_values({(func(std::integral_constant<std::size_t, Is>{}), true)...});
  }

 public:
  template <class Func>
  constexpr auto operator()(Func func) const -> void {
    return impl(std::make_index_sequence<std::tuple_size<List>::value>{}, func);
  }
};

template <class List, class Func>
constexpr auto for_each_element_of(Func func) -> void {
  for_each_element_of_t<List>{}(func);
}

/// @}

}  // namespace test

namespace {

template <class T>
struct TupleElement : testing::Test {};

// clang-format off
using tuple_types = ::testing::Types<
  std::tuple<int>,
  std::tuple<float>,
  std::tuple<void*>,
  std::tuple<int, float, void*>,
  std::tuple<int, int, int, int>,
  std::tuple<int, const int, volatile int, const volatile int>,
  std::tuple<int, int&, int const&>,
  std::tuple<void*, char, int, float, double>
>;
// clang-format on

TYPED_TEST_SUITE(TupleElement, tuple_types, );

/// @test <tt> tuple_element_t<I, Tuple> </tt> defines the same type as <tt> tuple_element<I, Tuple>::type </tt>
TYPED_TEST(TupleElement, TupleElementAndTupleElementTMatch) {
  test::for_each_element_of<TypeParam>([](auto index_constant) {
    static_assert(
        std::is_same<
            std::tuple_element_t<index_constant, TypeParam>,
            typename std::tuple_element<index_constant, TypeParam>::type>::value,
        "'tuple_element<>::type' and 'tuple_element_t<>' must specify the same type"
    );
  });
}

/// @test <tt> tuple_element_t<I, Tuple> </tt> returns the element type at index I
TYPED_TEST(TupleElement, TupleElementReturnsIthElement) {
  test::for_each_element_of<TypeParam>([](auto index_constant) {
    static_assert(
        std::is_same<
            std::tuple_element_t<index_constant, TypeParam>,
            arene::base::type_lists::at_t<TypeParam, index_constant>>::value,
        "'arene::base:type_lists::at_t<>::type' and 'tuple_element_t<>' must specify the same type"
    );
  });
}

/// @test <tt> tuple_element_t<I, const Tuple> is the same as <tt> add_const_t<tuple_element_t<I, Tuple>> </tt>
TYPED_TEST(TupleElement, ConstTupleElement) {
  test::for_each_element_of<TypeParam>([](auto index_constant) {
    static_assert(
        std::is_same<
            std::tuple_element_t<index_constant, const TypeParam>,
            std::add_const_t<std::tuple_element_t<index_constant, TypeParam>>>::value,
        "'tuple_element_t<I, const T>' and 'add_const_t<tuple_element_t<I, T>>' must specify the same type"
    );
  });
}

/// @test <tt> tuple_element_t<I, volatile Tuple> </tt> is the same as <tt> add_volatile_t<tuple_element_t<I, Tuple>>
/// </tt>
TYPED_TEST(TupleElement, VolatileTupleElement) {
  test::for_each_element_of<TypeParam>([](auto index_constant) {
    static_assert(
        std::is_same<
            std::tuple_element_t<index_constant, volatile TypeParam>,
            std::add_volatile_t<std::tuple_element_t<index_constant, TypeParam>>>::value,
        "'tuple_element_t<I, volatile T>' and 'add_volatile_t<tuple_element_t<I, T>>' must specify the same type"
    );
  });
}

/// @test <tt> tuple_element_t<I, const volatile Tuple> </tt> is the same as <tt> add_cv_t<tuple_element_t<I, Tuple>>
/// </tt>
TYPED_TEST(TupleElement, ConstVolatileTupleElement) {
  test::for_each_element_of<TypeParam>([](auto index_constant) {
    static_assert(
        std::is_same<
            std::tuple_element_t<index_constant, const volatile TypeParam>,
            std::add_volatile_t<std::add_const_t<std::tuple_element_t<index_constant, TypeParam>>>>::value,
        "'tuple_element_t<I, const volatile T>' and 'add_cv_t<tuple_element_t<I, T>>' must specify the same type"
    );
  });
}

}  // namespace

// NOLINTEND(hicpp-avoid-c-arrays,readability-identifier-length,llvm-qualified-auto)
