// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG("-Wc++11-narrowing", "Test code: implicit conversion of `int` to `float` is intentional");
ARENE_IGNORE_CLANG(
    "-Wimplicit-int-float-conversion",
    "Test code: implicit conversion of `int` to `float` is intentional"
);
ARENE_IGNORE_GCC("-Wnarrowing", "Test code: implicit conversion of `int` to `float` is intentional");
ARENE_IGNORE_GCC("-Wconversion", "Test code: implicit conversion of `int` to `float` is intentional");
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests do not perform arithmetic so equality is OK even for floating point");
#include "stdlib/include/tuple"
ARENE_IGNORE_END();

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/for_each.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_only_explicitly_constructible.hpp"
#include "arene/base/utility/forward_like.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/iterator"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/constexpr_traits.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"
#include "testlibs/utilities/mutate.hpp"

// NOLINTBEGIN(readability-identifier-length,llvm-qualified-auto)

namespace test {

/// @brief determine if a meta-function is true for all types in a list
/// @tparam List type-list
/// @tparam Trait unary meta-function -- provides static member `value`
///
/// ~~~{.cpp}
/// static_assert(all_of_v<type_list<int, float, double>, std::is_arithmetic>, "");
/// static_assert(!all_of_v<std::tuple<int, int&>, std::is_reference>, "");
/// ~~~
///
/// @{
template <class List, template <class> class Trait>
struct all_of;

template <template <class...> class List, class... Ts, template <class> class Trait>
struct all_of<List<Ts...>, Trait> : std::integral_constant<bool, arene::base::all_of_v<Trait<Ts>::value...>> {};

template <class List, template <class> class Trait>
constexpr auto all_of_v = all_of<List, Trait>::value;
/// @}

/// @brief create a type-list by repeating a type
/// @tparam N number of repetitions
/// @tparam T type to repeat
/// @tparam List type-list to use
///
/// ~~~{.cpp}
/// static_assert(
///   is_same_v<
///     std::tuple<int, int, int>,
///     repeat_t<3, int>
///   >, "");
/// ~~~
///
/// @{
template <std::size_t N, class T, template <class...> class List = std::tuple>
struct repeat {
  static auto type_at(std::size_t) -> T;

  template <std::size_t... Is>
  static auto impl(std::index_sequence<Is...>) -> std::tuple<decltype(type_at(Is))...>;

  using type = decltype(impl(std::make_index_sequence<N>{}));
};

template <std::size_t N, class T, template <class...> class List = std::tuple>
using repeat_t = typename repeat<N, T, List>::type;

/// @}

/// @brief apply a reduction operation to multiple traits
/// @tparam Traits unary meta-function -- provides static member `value`
///
/// ~~~{.cpp}
/// static_assert(
///   is_same_v<
///     true_type,
///     reduce<std::is_arithmetic, std::is_floating_point>::using_and<float>
///   >, "");
/// static_assert(
///   is_same_v<
///     false_type,
///     reduce<std::is_arithmetic, std::is_floating_point>::using_and<int>
///   >, "");
/// ~~~
///
/// @{
template <template <class> class... Traits>
struct reduce {
  template <class T>
  using using_and = std::integral_constant<bool, arene::base::all_of_v<Traits<T>::value...>>;
};
/// @}

/// @brief invoke a callable with a parameter pack of integral_constant values
/// @tparam Tuple tuple-like determining the number of integral_constant values
/// @tparam ConstantType integral_constant value type
///
/// Invokes the passed callable with a parameter pack of integral_constant
/// values with increasing values, starting from 0. The number of
/// integral_constant values is determined by @c tuple_size_v<Tuple> .
///
/// Unlike @c arene::base::for_each_index, @c with_constant_sequence_for
/// provides all indices at the same time to the passed callable.
///
/// ~~~{.cpp}
/// using T = std::tuple<int, float, double>;
/// ASSERT_EQ(
///   6,
///   with_constant_sequence_for<T>([](auto... constant) {
///     return 0U + ... + constant::value;
///   })
/// );
/// ~~~
///
/// @{
template <class Tuple, class ConstantType = std::size_t>
class with_constant_sequence_for_t {
  template <std::size_t... Is, class F>
  static constexpr auto impl(std::index_sequence<Is...>, F func)
      -> decltype(func(std::integral_constant<ConstantType, Is>{}...)) {
    return func(std::integral_constant<ConstantType, Is>{}...);
  }

 public:
  template <class F>
  constexpr auto operator()(F func) const
      -> decltype(impl(std::make_index_sequence<std::tuple_size_v<Tuple>>{}, func)) {
    return impl(std::make_index_sequence<std::tuple_size_v<Tuple>>{}, func);
  }
};
template <class Tuple, class ConstantType = std::size_t>
extern constexpr auto with_constant_sequence_for = with_constant_sequence_for_t<Tuple, ConstantType>{};
/// @}

/// @brief determine if a type is (constexpr) constructible
/// @tparam To type to construct
/// @tparam From type to construct from
///
/// @{
template <class To, class From>
struct constexpr_construct_from {
  template <class U = From, class = std::enable_if_t<std::is_constructible<To, U>::value>>
  constexpr auto operator()() const noexcept(std::is_nothrow_constructible<To, U>::value) -> void {
    static_assert(
        testing::is_constexpr_default_constructible<From>::value,
        "'From' needs to be default-constructible in order to test 'constexpr' constructibility of 'To'"
    );
    std::ignore = To{U{}};
  }
};

template <class From>
struct from {
  template <class To>
  using is_constructible = std::is_constructible<To, From>;

  template <class To>
  using is_nothrow_constructible = std::is_nothrow_constructible<To, From>;

  template <class To>
  using is_constexpr_constructible = testing::is_constexpr_invocable<constexpr_construct_from<To, From>>;

  template <class To>
  using is_assignable = std::is_assignable<To&, From>;

  template <class To>
  using is_nothrow_assignable = std::is_nothrow_assignable<To&, From>;
};
/// @}

/// @brief construct a tuple element from an integral_contant
/// @tparam IntegralConstant integral_constant type
/// @tparam Tuple tuple type
///
/// Constructs the I'th tuple_element of @c Tuple with a parameter of @c I,
/// where @c I is @c IntegralConstant::value.
///
/// @note Requires the tuple element type to be constructible from an
///   @c std::integral_constant or integer, which is the case for all test types
///
template <class IntegralConstant, class Tuple>
constexpr auto convert_to_ith_element()
    -> decltype(std::tuple_element_t<IntegralConstant::value, Tuple>{IntegralConstant::value}) {
  return std::tuple_element_t<IntegralConstant::value, Tuple>{IntegralConstant::value};
}

/// @brief constant default constructed value
/// @tparam T type to default construct
/// @note This constant value can be used where linkage is required (e.g. a non-type template parameter)
///
template <class T>
constexpr auto constant_value = T{};

/// @brief constant value of one
/// @note This constant value can be used where linkage is required (e.g. a non-type template parameter)
///
constexpr auto one = int{1};

/// @brief compile-time reference to 'one'
/// @note Used in testing compile-time construction of tuples with implicit conversions of element types
///
using reference_constant_of_one = std::integral_constant<decltype((one)), one>;

/// @brief function object that converts any reference to an lvalue reference to const
/// @note Used to ensure selection of a specific constructor overload
///
constexpr auto const_ref = arene::base::forward_like<int const&>;

struct dummy_with_adl_get {
  friend auto get(dummy_with_adl_get) noexcept -> dummy_with_adl_get { return {}; }
};

}  // namespace test

namespace {

template <class T>
struct Tuple : testing::Test {};

template <class... Types>
struct Tuple<std::tuple<Types...>> : testing::Test {
 protected:
  static constexpr bool elements_nothrow_equality_comparable{::arene::base::all_of_v<
      ::arene::base::is_nothrow_equality_comparable_v<Types, Types>...>};

  static constexpr bool elements_nothrow_less_than_comparable{::arene::base::all_of_v<
      ::arene::base::is_nothrow_less_than_comparable_v<Types, Types>...>};
};

template <class T>
struct TupleUnique : Tuple<T> {};

// clang-format off
using non_unique_tuple_types = ::testing::Types<
  std::tuple<>,
  std::tuple<int, int, int>,
  std::tuple<double, int, double>,
  std::tuple<test::reference_constant_of_one, test::reference_constant_of_one>,
  std::tuple<test::reference_constant_of_one, test::reference_constant_of_one, test::reference_constant_of_one>
>;

using unique_tuple_types = ::testing::Types<
  std::tuple<int>,
  std::tuple<float>,
  std::tuple<void*>,
  std::tuple<int, float, void*>,
  std::tuple<test::reference_constant_of_one>,
  std::tuple<testing::configurable_value<int, testing::throws_on::nothing, testing::disable::default_construct>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::nothing, testing::disable::copy_construct>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::nothing, testing::disable::move_construct>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::everything, testing::disable::default_construct>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::everything, testing::disable::copy_construct>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::nothing, testing::disable::nothing, testing::is_constexpr::no>>,
  std::tuple<testing::configurable_value<int, testing::throws_on::everything, testing::disable::nothing, testing::is_constexpr::no>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::nothing, testing::disable::default_construct>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::nothing, testing::disable::copy_construct>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::everything, testing::disable::default_construct>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::everything, testing::disable::copy_construct>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::nothing, testing::disable::nothing, testing::is_constexpr::no>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::everything, testing::disable::nothing, testing::is_constexpr::no>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::nothing, testing::disable::copy_assign>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::copy_assign>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::nothing, testing::disable::move_assign>>,
  std::tuple<int, testing::configurable_value<int, testing::throws_on::move_assign>>
>;

using tuple_types = arene::base::type_lists::concat_t<non_unique_tuple_types, unique_tuple_types>;
// clang-format on

TYPED_TEST_SUITE(Tuple, tuple_types, );
TYPED_TEST_SUITE(TupleUnique, unique_tuple_types, );

/// @test unqualified calls to get succeed
TEST(Tuple, UnqualifiedGet) {
  using std::get;

  auto const _ = get(test::dummy_with_adl_get{});
  std::ignore = _;

  auto const values = std::tuple<int>{};
  ASSERT_EQ(int{}, get<0>(values));
  ASSERT_EQ(int{}, get<int>(values));
}

/// @test if all the types in a tuple are empty, then the tuple is empty
TEST(Tuple, Empty) {
  static_assert(
      std::is_empty_v<std::tuple<>>  //
      ,
      "tuple of distinct and empty types should be empty"
  );
  static_assert(
      std::is_empty_v<std::tuple<std::false_type>>  //
      ,
      "tuple of distinct and empty types should be empty"
  );
  static_assert(
      std::is_empty_v<std::tuple<std::false_type, std::true_type>>  //
      ,
      "tuple of distinct and empty types should be empty"
  );
  static_assert(
      std::is_empty_v<std::tuple<std::false_type, std::false_type>>  //
      ,
      "tuple of empty types should be empty"
  );
  static_assert(
      std::is_empty_v<
          std::
              tuple<std::integral_constant<int, 0>, std::integral_constant<int, 1>, std::integral_constant<int, 2>>>  //
      ,
      "tuple of distinct and empty types should be empty"
  );
}

/// @test a tuple is (nothrow) default-constructible if and only if all the element types are
/// (nothrow) default-constructible
TYPED_TEST(Tuple, DefaultConstructible) {
  static_assert(
      std::is_default_constructible_v<TypeParam> ==  //
          test::all_of_v<TypeParam, std::is_default_constructible>,
      ""
  );
  static_assert(
      std::is_nothrow_default_constructible_v<TypeParam> ==  //
          test::all_of_v<TypeParam, std::is_nothrow_default_constructible>,
      ""
  );
}

/// @test the default constructor is usable in a constant expression if and only if all elements are constexpr default
/// constructible
TYPED_TEST(Tuple, ConstexprDefaultConstructible) {
  static_assert(
      testing::is_constexpr_default_constructible_v<TypeParam> ==  //
          test::all_of_v<TypeParam, testing::is_constexpr_default_constructible>,
      ""
  );
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "No arithmetic is performed on these values so float equality is OK");

/// @test a default constructed tuple has elements that are value initialized
CONDITIONAL_TYPED_TEST(Tuple, DefaultConstructor, std::is_default_constructible_v<TypeParam>) {
  TypeParam const values{};

  arene::base::for_each(                            //
      values,                                       //
      [](auto const& element) {                     //
        using T = std::decay_t<decltype(element)>;  //
        ASSERT_EQ(T{}, element);                    //
      }                                             //
  );
}

/// @test `std::get<Idx>` works with mutable lvalue reference tuples
CONDITIONAL_TYPED_TEST(Tuple, MutableLvalueGet, std::is_default_constructible_v<TypeParam>) {
  TypeParam values{};

  arene::base::for_each(                            //
      values,                                       //
      [](auto& element) {                           //
        using T = std::decay_t<decltype(element)>;  //
        ASSERT_EQ(element, T{});                    //
      }                                             //
  );
}

/// @test `std::get<Idx>` works with const lvalue reference tuples
CONDITIONAL_TYPED_TEST(Tuple, ConstLvalueGet, std::is_default_constructible_v<TypeParam>) {
  auto const& values = TypeParam{};

  arene::base::for_each(                            //
      values,                                       //
      [](auto const& element) {                     //
        using T = std::decay_t<decltype(element)>;  //
        ASSERT_EQ(element, T{});                    //
      }                                             //
  );
}

/// @test `std::get<Idx>` works with mutable rvalue reference tuples
CONDITIONAL_TYPED_TEST(Tuple, MutableRvalueGet, std::is_default_constructible_v<TypeParam>) {
  TypeParam values{};

  arene::base::for_each(                            //
      std::move(values),                            //
      [](auto&& element) {                          //
        using T = std::decay_t<decltype(element)>;  //
        ASSERT_EQ(element, T{});                    //
      }                                             //
  );
}

/// @test `std::get<Idx>` works with const rvalue reference tuples
CONDITIONAL_TYPED_TEST(Tuple, ConstRvalueGet, std::is_default_constructible_v<TypeParam>) {
  auto const&& values = TypeParam{};

  arene::base::for_each(                            //
      std::move(values),                            //
      [](auto const&& element) {                    //
        using T = std::decay_t<decltype(element)>;  //
        ASSERT_EQ(element, T{});                    //
      }                                             //
  );
}

/// @test `std::get<T>` works with mutable lvalue reference tuples
CONDITIONAL_TYPED_TEST(TupleUnique, MutableLvalueGet, std::is_default_constructible_v<TypeParam>) {
  TypeParam values{};

  arene::base::for_each_index(                                         //
      values,                                                          //
      [&values](auto idx_const, auto&&) {                              //
        using element = std::tuple_element_t<idx_const(), TypeParam>;  //
        using value = std::decay_t<element>;                           //
        ASSERT_EQ(std::get<element>(values), value{});                 //
      }                                                                //
  );
}

/// @test `std::get<T>` works with const lvalue reference tuples
CONDITIONAL_TYPED_TEST(TupleUnique, ConstLvalueGet, std::is_default_constructible_v<TypeParam>) {
  auto const& values = TypeParam{};

  arene::base::for_each_index(                                         //
      values,                                                          //
      [&values](auto idx_const, auto&&) {                              //
        using element = std::tuple_element_t<idx_const(), TypeParam>;  //
        using value = std::decay_t<element>;                           //
        ASSERT_EQ(std::get<element>(values), value{});                 //
      }                                                                //
  );
}

/// @test `std::get<T>` works with mutable rvalue reference tuples
CONDITIONAL_TYPED_TEST(TupleUnique, MutableRvalueGet, std::is_default_constructible_v<TypeParam>) {
  TypeParam values{};

  arene::base::for_each_index(                                         //
      values,                                                          //
      [&values](auto idx_const, auto&&) {                              //
        using element = std::tuple_element_t<idx_const(), TypeParam>;  //
        using value = std::decay_t<element>;                           //
        ASSERT_EQ(std::get<element>(std::move(values)), value{});      //
      }                                                                //
  );
}

/// @test `std::get<T>` works with const rvalue reference tuples
CONDITIONAL_TYPED_TEST(TupleUnique, ConstRvalueGet, std::is_default_constructible_v<TypeParam>) {
  auto const&& values = TypeParam{};

  arene::base::for_each_index(                                         //
      values,                                                          //
      [&values](auto idx_const, auto&&) {                              //
        using element = std::tuple_element_t<idx_const(), TypeParam>;  //
        using value = std::decay_t<element>;                           //
        ASSERT_EQ(std::get<element>(std::move(values)), value{});      //
      }                                                                //
  );
}

template <typename... Elements, std::size_t... Indices>
constexpr auto
all_are_default_constructed_by_index(std::tuple<Elements...> const& tup, std::index_sequence<Indices...>) noexcept {
  return arene::base::all_of({std::get<Indices>(tup) == Elements{}...});
}

template <typename... Elements, std::size_t... Indices>
constexpr auto
all_are_default_constructed_by_type(std::tuple<Elements...> const& tup, std::index_sequence<Indices...>) noexcept {
  return arene::base::all_of({std::get<Elements>(tup) == Elements{}...});
}

/// @test `std::get<Idx>` works in constexpr contexts
CONDITIONAL_TYPED_TEST(Tuple, GetIsConstexpr, testing::is_constexpr_default_constructible_v<TypeParam>) {
  constexpr TypeParam values{};

  static_assert(
      all_are_default_constructed_by_index(values, std::make_index_sequence<std::tuple_size<TypeParam>::value>{}),
      "std::get<Idx> should work in constexpr"
  );
}

/// @test `std::get<T>` works in constexpr contexts
CONDITIONAL_TYPED_TEST(TupleUnique, GetIsConstexpr, testing::is_constexpr_default_constructible_v<TypeParam>) {
  constexpr TypeParam values{};

  static_assert(
      all_are_default_constructed_by_type(values, std::make_index_sequence<std::tuple_size<TypeParam>::value>{}),
      "std::get<T> should work in constexpr"
  );
}

/// @test `std::get<Idx>` is always noexcept for all tuples where it can be used at all
CONDITIONAL_TYPED_TEST(Tuple, GetIsNoexcept, std::tuple_size<TypeParam>() > 0UL) {
  static_assert(noexcept(std::get<0>(std::declval<TypeParam&>())), "std::get should always be noexcept");
  static_assert(noexcept(std::get<0>(std::declval<TypeParam const&>())), "std::get should always be noexcept");
  static_assert(noexcept(std::get<0>(std::declval<TypeParam&&>())), "std::get should always be noexcept");
  static_assert(noexcept(std::get<0>(std::declval<TypeParam const&&>())), "std::get should always be noexcept");
}

/// @test `std::get<T>` is always noexcept for all tuples where it can be used at all
CONSTEXPR_TYPED_TEST(TupleUnique, GetIsNoexcept) {
  ASSERT_TRUE(noexcept(std::get<std::tuple_element_t<0, TypeParam>>(std::declval<TypeParam&>())));
  ASSERT_TRUE(noexcept(std::get<std::tuple_element_t<0, TypeParam>>(std::declval<TypeParam const&>())));
  ASSERT_TRUE(noexcept(std::get<std::tuple_element_t<0, TypeParam>>(std::declval<TypeParam&&>())));
  ASSERT_TRUE(noexcept(std::get<std::tuple_element_t<0, TypeParam>>(std::declval<TypeParam const&&>())));
}

/// @test std::get returns a reference with shallow constness similar to accessing a member of a struct
TEST(TupleUntypedTest, GetConstnessIsShallow) {
  using ref_tuple = std::tuple<int&, int const&, int&&, int const&&>;

  // On an lvalue reference tuple, returned references are always lvalues due to reference collapsing
  testing::StaticAssertTypeEq<decltype(std::get<0>(std::declval<ref_tuple&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<1>(std::declval<ref_tuple&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<2>(std::declval<ref_tuple&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<3>(std::declval<ref_tuple&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&>(std::declval<ref_tuple&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&>(std::declval<ref_tuple&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&&>(std::declval<ref_tuple&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&&>(std::declval<ref_tuple&>())), int const&>();

  // Even if the tuple is constant, the references that it returns are not
  testing::StaticAssertTypeEq<decltype(std::get<0>(std::declval<ref_tuple const&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<1>(std::declval<ref_tuple const&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<2>(std::declval<ref_tuple const&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<3>(std::declval<ref_tuple const&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&>(std::declval<ref_tuple const&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&>(std::declval<ref_tuple const&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&&>(std::declval<ref_tuple const&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&&>(std::declval<ref_tuple const&>())), int const&>();

  // On an rvalue reference tuple, returned references can be rvalues
  testing::StaticAssertTypeEq<decltype(std::get<0>(std::declval<ref_tuple&&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<1>(std::declval<ref_tuple&&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<2>(std::declval<ref_tuple&&>())), int&&>();
  testing::StaticAssertTypeEq<decltype(std::get<3>(std::declval<ref_tuple&&>())), int const&&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&>(std::declval<ref_tuple&&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&>(std::declval<ref_tuple&&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&&>(std::declval<ref_tuple&&>())), int&&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&&>(std::declval<ref_tuple&&>())), int const&&>();

  // Constness is shallow for rvalues as well (this overload was added to C++14 retroactively by a defect report)
  testing::StaticAssertTypeEq<decltype(std::get<0>(std::declval<ref_tuple const&&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<1>(std::declval<ref_tuple const&&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<2>(std::declval<ref_tuple const&&>())), int&&>();
  testing::StaticAssertTypeEq<decltype(std::get<3>(std::declval<ref_tuple const&&>())), int const&&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&>(std::declval<ref_tuple const&&>())), int&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&>(std::declval<ref_tuple const&&>())), int const&>();
  testing::StaticAssertTypeEq<decltype(std::get<int&&>(std::declval<ref_tuple const&&>())), int&&>();
  testing::StaticAssertTypeEq<decltype(std::get<int const&&>(std::declval<ref_tuple const&&>())), int const&&>();
}

/// @test type-based `std::get<T>` works on the unique part of a tuple with some members repeated
CONSTEXPR_TEST(TupleUntypedTest, GetByTypePartialRepeat) {
  std::tuple<double, int, double> const constant{3.14, 599, 6.022e23};
  std::tuple<double, int, double> non_constant{constant};
  ASSERT_EQ(std::get<int>(constant), 599);
  ASSERT_EQ(std::get<int>(non_constant), 599);
  // NOLINTBEGIN(hicpp-move-const-arg) We need to test that the overload exists for these moves
  ASSERT_EQ(std::get<int>(std::move(constant)), 599);
  ASSERT_EQ(std::get<int>(std::move(non_constant)), 599);
  // NOLINTEND(hicpp-move-const-arg)
}

/// @test a tuple is (nothrow) direct-constructible if and only if all the element types are
/// (nothrow) copy-constructible
TYPED_TEST(Tuple, DirectConstructible) {
  // e.g. is_constructible<std::tuple<int, char>, int const&, char const&>
  //
  // `mutate_t` is using `std::tuple` as the type-list and `concat_t` is using
  // `std::is_constructible` as the type-list

  using constructor_args_t = testing::mutate_t<  //
      testing::mutate_t<                         //
          TypeParam,                             //
          std::add_const>,                       //
      std::add_lvalue_reference>;

  static_assert(
      arene::base::type_lists::concat_t<std::is_constructible<TypeParam>, constructor_args_t>::value ==  //
          test::all_of_v<TypeParam, std::is_copy_constructible>,
      ""
  );
  static_assert(
      arene::base::type_lists::concat_t<std::is_nothrow_constructible<TypeParam>, constructor_args_t>::value ==  //
          test::all_of_v<TypeParam, std::is_nothrow_copy_constructible>,
      ""
  );
}

/// @test the direct constructor of @c tuple is explicit if and only if it is direct-constructible
CONDITIONAL_TYPED_TEST(Tuple, ExplicitDirectConstructor, (!std::is_same<TypeParam, std::tuple<>>::value)) {
  using constructor_args_t = testing::mutate_t<  //
      testing::mutate_t<                         //
          TypeParam,                             //
          std::add_const>,                       //
      std::add_lvalue_reference>;

  static_assert(
      arene::base::type_lists::concat_t<std::is_constructible<TypeParam>, constructor_args_t>::value ==  //
          arene::base::type_lists::
              concat_t<arene::base::is_only_explicitly_constructible<TypeParam>, constructor_args_t>::value,
      ""
  );
}

/// @test the constructor of an empty @c tuple is not explicit
TEST(Tuple, EmptyTupleConstructorShouldNotBeExplicit) {
  static_assert(std::is_default_constructible<std::tuple<>>::value, "Empty tuple should be default constructible");
  static_assert(
      arene::base::is_implicitly_constructible_v<std::tuple<>>,
      "Empty tuple should be implicitly constructible"
  );
  static_assert(
      !arene::base::is_only_explicitly_constructible_v<std::tuple<>>,
      "Empty tuple should not be explicitly constructible"
  );
}

/// @test the direct constructor is usable in a constant expression if and only
/// if all elements are constexpr copy constructible
CONDITIONAL_TYPED_TEST(Tuple, ConstexprDirectConstructible, (test::all_of_v<TypeParam, std::is_copy_constructible>)) {
  // NOTE: This is a simplification based on test types.
  //
  // In order to invoke the copy constructor in a constant expression, we must
  // first have a value to copy. We get that value from construction from an
  // 'int'.
  auto const constexpr_direct_constructible = [](auto... constant)
      -> std::integral_constant<
          bool,
          (TypeParam{test::const_ref(test::convert_to_ith_element<decltype(constant), TypeParam>())...}, true)> {
    return {};
  };

  constexpr auto is_constexpr_direct_constructible = arene::base::is_invocable_v<
      decltype(test::with_constant_sequence_for<TypeParam, int>),
      decltype(constexpr_direct_constructible)>;

  constexpr auto all_elements_are_constexpr_copy_constructible = test::all_of_v<
      TypeParam,
      test::reduce<std::is_copy_constructible, test::from<int>::is_constexpr_constructible>::using_and>;

  ASSERT_EQ(is_constexpr_direct_constructible, all_elements_are_constexpr_copy_constructible);
}

/// @test a direct constructed tuple has elements that are copy constructed from the source values
CONDITIONAL_TYPED_TEST(
    Tuple,
    DirectConstructor,
    (test::all_of_v<TypeParam, test::reduce<std::is_copy_constructible, test::from<int>::is_constructible>::using_and>)
) {
  auto const values = test::with_constant_sequence_for<TypeParam, int>([](auto... constant) {
    return TypeParam{test::const_ref(test::convert_to_ith_element<decltype(constant), TypeParam>())...};
  });

  arene::base::for_each_index(                                                             //
      values,                                                                              //
      [](auto index, auto const& element) {                                                //
        auto const expected = test::convert_to_ith_element<decltype(index), TypeParam>();  //
        ASSERT_EQ(expected, element);                                                      //
      }                                                                                    //
  );
}

/// @test a tuple is (nothrow) converting-constructible if and only if all the element types are
/// (nothrow) converting-constructible
TYPED_TEST(Tuple, ConvertingConstructible) {
  // e.g. is_constructible<std::tuple<int, char>, int, char>

  auto const to_integral_constant_tuple = [](auto... constant) -> std::tuple<decltype(constant)...> { return {}; };

  using constructor_args_t = decltype(test::with_constant_sequence_for<TypeParam, int>(to_integral_constant_tuple));

  static_assert(
      arene::base::type_lists::concat_t<std::is_constructible<TypeParam>, constructor_args_t>::value ==  //
          test::all_of_v<TypeParam, test::from<int>::is_constructible>,
      ""
  );
  static_assert(
      arene::base::type_lists::concat_t<std::is_nothrow_constructible<TypeParam>, constructor_args_t>::value ==  //
          test::all_of_v<TypeParam, test::from<int>::is_nothrow_constructible>,
      ""
  );
}

/// @test the converting constructor of @c tuple is explicit if and only if it is coverting-constructible
CONDITIONAL_TYPED_TEST(Tuple, ExplicitConvertingConstructor, (!std::is_same<TypeParam, std::tuple<>>::value)) {
  auto const to_integral_constant_tuple = [](auto... constant) -> std::tuple<decltype(constant)...> { return {}; };

  using constructor_args_t = decltype(test::with_constant_sequence_for<TypeParam, int>(to_integral_constant_tuple));

  static_assert(
      arene::base::type_lists::concat_t<std::is_constructible<TypeParam>, constructor_args_t>::value ==  //
          arene::base::type_lists::
              concat_t<arene::base::is_only_explicitly_constructible<TypeParam>, constructor_args_t>::value,
      ""
  );
}

/// @test the converting constructor is usable in a constant expression if and only
/// if all elements are constexpr int constructible
TYPED_TEST(Tuple, ConstexprConvertingConstructible) {
  constexpr auto all_elements_are_constexpr_converting_constructible =
      test::all_of_v<TypeParam, test::from<int>::is_constexpr_constructible>;

  auto const constexpr_converting_constructible = [](auto... constant)  //
      -> decltype(std::integral_constant<bool, (TypeParam{decltype(constant)::value...}, true)>{}, void()) {};

  constexpr auto is_constexpr_converting_constructible = arene::base::is_invocable_v<
      decltype(test::with_constant_sequence_for<TypeParam, int>),
      decltype(constexpr_converting_constructible)>;

  static_assert(is_constexpr_converting_constructible == all_elements_are_constexpr_converting_constructible, "");
}

/// @test a converting constructed tuple has elements that are converting-constructed from the source values
CONDITIONAL_TYPED_TEST(Tuple, ConvertingConstructor, (test::all_of_v<TypeParam, test::from<int>::is_constructible>)) {
  auto const& values = test::with_constant_sequence_for<TypeParam, int>([](auto... constant) -> auto& {
    static TypeParam const _{decltype(constant)::value...};
    return _;
  });

  arene::base::for_each_index(                                      //
      values,                                                       //
      [](auto index_constant, auto const& element) {                //
        using T = std::tuple_element_t<index_constant, TypeParam>;  //
        ASSERT_EQ(T(index_constant), element);                      //
      }                                                             //
  );
}

/// @test a tuple is (nothrow) copy-constructible if and only if all the element types are
/// (nothrow) copy-constructible
TYPED_TEST(Tuple, CopyConstructible) {
  static_assert(
      std::is_copy_constructible<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_copy_constructible>,
      ""
  );
  static_assert(
      std::is_nothrow_copy_constructible<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_nothrow_copy_constructible>,
      ""
  );
}

/// @test the copy constructor is usable in a constant expression if and only
/// if all elements are constexpr copy constructible
CONDITIONAL_TYPED_TEST(Tuple, ConstexprCopyConstructible, std::is_copy_constructible<TypeParam>::value) {
  // cast to ensure we invoke the copy constructor
  static constexpr auto const_ref = arene::base::forward_like<int const&>;

  // NOTE: This is a simplification based on test types.
  //
  // In order to invoke the copy constructor in a constant expression, we must
  // first have a value to copy. We get that value from construction from an
  // 'int'.
  auto const constexpr_copy_constructible = [](auto... constant)
      -> std::integral_constant<
          bool,
          (TypeParam{const_ref(TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...})}, true)> {
    return {};
  };

  constexpr auto is_constexpr_copy_constructible = arene::base::is_invocable_v<
      decltype(test::with_constant_sequence_for<TypeParam, int>),
      decltype(constexpr_copy_constructible)>;

  constexpr auto all_elements_are_constexpr_copy_constructible = test::all_of_v<
      TypeParam,
      test::reduce<std::is_copy_constructible, test::from<int>::is_constexpr_constructible>::using_and>;

  ASSERT_EQ(is_constexpr_copy_constructible, all_elements_are_constexpr_copy_constructible);
}

/// @test a copy constructed tuple has elements that are copy constructed from the other tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    CopyConstructor,
    (test::all_of_v<TypeParam, test::reduce<std::is_copy_constructible, test::from<int>::is_constructible>::using_and>)
) {
  auto const source = test::with_constant_sequence_for<TypeParam>([](auto... constant) {
    return TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
  });

  auto const copy = source;

  arene::base::for_each_index(                                        //
      source,                                                         //
      [&copy](auto index_constant, auto const& expected_element) {    //
        auto const& copied_element = std::get<index_constant>(copy);  //
        ASSERT_EQ(expected_element, copied_element);                  //
      }                                                               //
  );
}

/// @test a tuple is (nothrow) move-constructible if and only if all the element types are
/// (nothrow) move-constructible
TYPED_TEST(Tuple, MoveConstructible) {
  static_assert(
      std::is_move_constructible<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_move_constructible>,
      ""
  );
  static_assert(
      std::is_nothrow_move_constructible<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_nothrow_move_constructible>,
      ""
  );
}

/// @test the move constructor is usable in a constant expression if and only
/// if all elements are constexpr move constructible
CONDITIONAL_TYPED_TEST(Tuple, ConstexprMoveConstructible, std::is_move_constructible<TypeParam>::value) {
  // NOTE: This is a simplification based on test types.
  //
  // In order to invoke the move constructor in a constant expression, we must
  // first have a value to move. We get that value from construction from an
  // 'int'.
  auto const constexpr_move_constructible = [](auto... constant)
      -> std::integral_constant<
          bool,
          (TypeParam{TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...}}, true)> {
    return {};
  };

  constexpr auto is_constexpr_move_constructible = arene::base::is_invocable_v<
      decltype(test::with_constant_sequence_for<TypeParam, int>),
      decltype(constexpr_move_constructible)>;

  constexpr auto all_elements_are_constexpr_move_constructible = test::all_of_v<
      TypeParam,
      test::reduce<std::is_move_constructible, test::from<int>::is_constexpr_constructible>::using_and>;

  ASSERT_EQ(is_constexpr_move_constructible, all_elements_are_constexpr_move_constructible);
}

/// @test a move constructed tuple has elements that are move constructed from the other tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    MoveConstructor,
    (test::all_of_v<TypeParam, test::reduce<std::is_move_constructible, test::from<int>::is_constructible>::using_and>)
) {
  auto const make_source = [] {
    return test::with_constant_sequence_for<TypeParam>([](auto... constant) {
      return TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
    });
  };

  // _recent_ GCC versions warn '-Wpessimizing-move' with just 'std::move'
  auto const explicit_move = [](TypeParam&& value) { return std::move(value); };

  auto const move_constructed = explicit_move(make_source());

  auto const values = make_source();

  arene::base::for_each_index(                                                   //
      values,                                                                    //
      [&move_constructed](auto index_constant, auto const& expected_element) {   //
        auto const& moved_element = std::get<index_constant>(move_constructed);  //
        ASSERT_EQ(expected_element, moved_element);                              //
      }                                                                          //
  );
}

/// @test a tuple is (nothrow) constructible from a const-ref of a different tuple
/// type if and only if all the element types are (nothrow) constructible from a
/// const-ref of the types in the other tuple
TYPED_TEST(Tuple, OtherTupleCopyConstructible) {
  using int_constant_1 = std::integral_constant<int, 1>;

  using other_tuple_type = test::repeat_t<std::tuple_size<TypeParam>::value, int_constant_1>;

  static_assert(
      std::is_constructible<TypeParam, other_tuple_type const&>::value ==  //
          test::all_of_v<TypeParam, test::from<int_constant_1>::is_constructible>,
      ""
  );

  static_assert(
      std::is_nothrow_constructible<TypeParam, other_tuple_type const&>::value ==  //
          test::all_of_v<TypeParam, test::from<int_constant_1>::is_nothrow_constructible>,
      ""
  );
}

/// @test a tuple is constexpr constructible from a const-ref of a different tuple
/// type if and only if all the element types are constexpr constructible from a
/// const-ref of the types in the other tuple
TYPED_TEST(Tuple, ConstexprOtherTupleCopyConstructible) {
  using other_tuple_type = test::repeat_t<std::tuple_size<TypeParam>::value, test::reference_constant_of_one>;

  using reference_constant_of_other_tuple = std::integral_constant<  //
      decltype((test::constant_value<other_tuple_type>)),            //
      test::constant_value<other_tuple_type>>;

  auto const constexpr_constructible = [](auto constant)  //
      -> std::integral_constant<bool, (TypeParam{decltype(constant)::value}, true)> { return {}; };

  constexpr auto is_constexpr_constructible =
      arene::base::is_invocable_v<decltype(constexpr_constructible), reference_constant_of_other_tuple>;

  constexpr auto all_elements_are_constexpr_constructible =
      test::all_of_v<TypeParam, test::from<test::reference_constant_of_one>::is_constexpr_constructible>;

  static_assert(is_constexpr_constructible == all_elements_are_constexpr_constructible, "");
}

/// @test a copy-from-other-tuple constructed tuple has elements that are
/// constructed from const-ref's of the values of the other tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    OtherTupleCopyConstructor,
    (test::all_of_v<TypeParam, test::from<int const&>::is_constructible>)
) {
  using int_constant_1 = std::integral_constant<int, 1>;

  auto const other_tuple = test::repeat_t<std::tuple_size<TypeParam>::value, int_constant_1>{};

  auto const& values = TypeParam{other_tuple};

  arene::base::for_each_index(                                        //
      other_tuple,                                                    //
      [&values](auto index_constant, auto const& source_element) {    //
        using T = std::tuple_element_t<index_constant, TypeParam>;    //
        auto const& dest_element = std::get<index_constant>(values);  //
        ASSERT_EQ(T(source_element), dest_element);                   //
      }                                                               //
  );
}

/// @test a tuple is (nothrow) constructible from an rvalue-ref of a different tuple
/// type if and only if all the element types are (nothrow) constructible from
/// forwarded references of the values in the other tuple
TYPED_TEST(Tuple, OtherTupleMoveConstructible) {
  using int_constant_1 = std::integral_constant<int, 1>;

  using other_tuple_type = test::repeat_t<std::tuple_size<TypeParam>::value, int_constant_1>;

  static_assert(
      std::is_constructible<TypeParam, other_tuple_type&&>::value ==  //
          test::all_of_v<TypeParam, test::from<int_constant_1&&>::is_constructible>,
      ""
  );

  static_assert(
      std::is_nothrow_constructible<TypeParam, other_tuple_type&&>::value ==  //
          test::all_of_v<TypeParam, test::from<int_constant_1&&>::is_nothrow_constructible>,
      ""
  );
}

/// @test a tuple is constexpr constructible from an rvalue-ref of a different tuple
/// type if and only if all the element types are constexpr constructible from
/// forwarded references of the values in the other tuple
TYPED_TEST(Tuple, ConstexprOtherTupleMoveConstructible) {
  using int_constant_1 = std::integral_constant<int, 1>;

  using other_tuple_type = test::repeat_t<std::tuple_size<TypeParam>::value, int_constant_1>;

  auto const constexpr_constructible = [](auto other)  //
      -> std::integral_constant<bool, (TypeParam{decltype(other){}}, true)> { return {}; };

  constexpr auto is_constexpr_constructible =
      arene::base::is_invocable_v<decltype(constexpr_constructible), other_tuple_type>;

  constexpr auto all_elements_are_constexpr_constructible =
      test::all_of_v<TypeParam, test::from<int_constant_1>::is_constexpr_constructible>;

  static_assert(is_constexpr_constructible == all_elements_are_constexpr_constructible, "");
}

/// @test a move-from-other-tuple constructed tuple has elements that are
/// constructed from forwarded references of the values in the other tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    OtherTupleMoveConstructor,
    (test::all_of_v<TypeParam, test::reduce<test::from<int&&>::is_constructible>::using_and>)
) {
  using int_constant_1 = std::integral_constant<int, 1>;

  auto other_tuple = test::repeat_t<std::tuple_size<TypeParam>::value, int_constant_1>{};

  auto const& values = TypeParam{std::move(other_tuple)};

  arene::base::for_each_index(                                      //
      values,                                                       //
      [](auto index_constant, auto const& dest_element) {           //
        using T = std::tuple_element_t<index_constant, TypeParam>;  //
        ASSERT_EQ(T(1), dest_element);                              //
      }                                                             //
  );
}

/// @test a tuple is (nothrow) constructible from a const-ref of a pair
/// type if and only if all the element types are (nothrow) constructible from a
/// const-ref of the types in the pair
TYPED_TEST(Tuple, PairCopyConstructible) {
  using int_constant_1 = std::integral_constant<int, 1>;

  using pair_type = std::pair<int_constant_1, int_constant_1>;

  static_assert(
      std::is_constructible<TypeParam, pair_type const&>::value ==  //
          (test::all_of_v<TypeParam, test::from<int_constant_1>::is_constructible> &&
           2U == std::tuple_size<TypeParam>::value),
      ""
  );

  static_assert(
      std::is_nothrow_constructible<TypeParam, pair_type const&>::value ==  //
          (test::all_of_v<TypeParam, test::from<int_constant_1>::is_nothrow_constructible> &&
           2U == std::tuple_size<TypeParam>::value),
      ""
  );
}

/// @test a tuple is constexpr constructible from a const-ref of a pair
/// type if and only if all the element types are constexpr constructible from a
/// const-ref of the types in the pair
TYPED_TEST(Tuple, ConstexprPairCopyConstructible) {
  using pair_type = std::pair<test::reference_constant_of_one, test::reference_constant_of_one>;

  using reference_constant_of_pair =
      std::integral_constant<decltype((test::constant_value<pair_type>)), test::constant_value<pair_type>>;

  auto const constexpr_constructible = [](auto constant)  //
      -> std::integral_constant<bool, (TypeParam{decltype(constant)::value}, true)> { return {}; };

  constexpr auto is_constexpr_constructible =
      arene::base::is_invocable_v<decltype(constexpr_constructible), reference_constant_of_pair>;

  constexpr auto all_elements_are_constexpr_constructible =
      test::all_of_v<TypeParam, test::from<test::reference_constant_of_one>::is_constexpr_constructible>;

  static_assert(
      is_constexpr_constructible ==
          (all_elements_are_constexpr_constructible && 2U == std::tuple_size<TypeParam>::value),
      ""
  );
}

/// @test a copy-from-pair constructed tuple has elements that are
/// constructed from const-ref's of the values of the pair
CONDITIONAL_TYPED_TEST(
    Tuple,
    PairCopyConstructor,
    (test::all_of_v<TypeParam, test::from<int const&>::is_constructible> && 2U == std::tuple_size<TypeParam>::value)
) {
  auto const pair = std::pair<int, int>{1, 1};

  auto const values = TypeParam{pair};

  arene::base::for_each_index(                                      //
      values,                                                       //
      [](auto index_constant, auto const& dest_element) {           //
        using T = std::tuple_element_t<index_constant, TypeParam>;  //
        ASSERT_EQ(T(1), dest_element);                              //
      }                                                             //
  );
}

/// @test a tuple is (nothrow) constructible from an rvalue-ref of a pair
/// type if and only if all the element types are (nothrow) constructible from
/// forwarded references of the values in the pair
TYPED_TEST(Tuple, PairMoveConstructible) {
  using int_constant_1 = std::integral_constant<int, 1>;

  using pair_type = std::pair<int_constant_1, int_constant_1>;

  static_assert(
      std::is_constructible<TypeParam, pair_type&&>::value ==  //
          (test::all_of_v<TypeParam, test::from<int_constant_1&&>::is_constructible> &&
           2U == std::tuple_size<TypeParam>::value),
      ""
  );

  static_assert(
      std::is_nothrow_constructible<TypeParam, pair_type&&>::value ==  //
          (test::all_of_v<TypeParam, test::from<int_constant_1&&>::is_nothrow_constructible> &&
           2U == std::tuple_size<TypeParam>::value),
      ""
  );
}

/// @test a tuple is constexpr constructible from an rvalue-ref of a pair
/// type if and only if all the element types are constexpr constructible from
/// forwarded references of the values in the pair
TYPED_TEST(Tuple, ConstexprPairMoveConstructible) {
  using int_constant_1 = std::integral_constant<int, 1>;

  using pair_type = std::pair<int_constant_1, int_constant_1>;

  auto const constexpr_constructible = [](auto other)  //
      -> std::integral_constant<bool, (TypeParam{decltype(other){}}, true)> { return {}; };

  constexpr auto is_constexpr_constructible =  //
      arene::base::is_invocable_v<decltype(constexpr_constructible), pair_type>;

  constexpr auto all_elements_are_constexpr_constructible =
      test::all_of_v<TypeParam, test::from<int_constant_1>::is_constexpr_constructible>;

  static_assert(
      is_constexpr_constructible ==
          (all_elements_are_constexpr_constructible && 2U == std::tuple_size<TypeParam>::value),
      ""
  );
}

/// @test a move-from-pair constructed tuple has elements that are
/// constructed from forwarded references of the values of the pair
CONDITIONAL_TYPED_TEST(
    Tuple,
    PairMoveConstructor,
    (test::all_of_v<TypeParam, test::from<int&&>::is_constructible> && 2U == std::tuple_size<TypeParam>::value)
) {
  auto pair = std::pair<int, int>{1, 1};

  // NOLINTNEXTLINE(hicpp-move-const-arg) : move and copy have different semantics
  auto const values = TypeParam{std::move(pair)};

  arene::base::for_each_index(                                      //
      values,                                                       //
      [](auto index_constant, auto const& dest_element) {           //
        using T = std::tuple_element_t<index_constant, TypeParam>;  //
        ASSERT_EQ(T(1), dest_element);                              //
      }                                                             //
  );
}

template <class T>
struct constexpr_reference_wrapper {
  T value;

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T() const noexcept { return static_cast<T&&>(value); }
};

/// @test a tuple with an lvalue reference element type can be constructed from
/// a reference or a reference wrapper
CONSTEXPR_TEST(Tuple, UsableWithLvalueReferencesAndReferenceWrapperLike) {
  using Tup = std::tuple<int, int&, int const&>;

  int value = 0;
  Tup tup{value, value, value};
  value = 1;
  ASSERT_EQ(std::get<0>(tup), 0);
  ASSERT_EQ(std::get<1>(tup), 1);
  ASSERT_EQ(std::get<2>(tup), 1);
  ASSERT_EQ(std::get<int>(tup), 0);
  ASSERT_EQ(std::get<int&>(tup), 1);
  ASSERT_EQ(std::get<int const&>(tup), 1);

  Tup tup2{std::get<int>(tup), std::get<int>(tup), std::get<int>(tup)};
  value = 2;
  std::get<int>(tup) = 3;
  ASSERT_EQ(std::get<0>(tup2), 0);
  ASSERT_EQ(std::get<1>(tup2), 3);
  ASSERT_EQ(std::get<2>(tup2), 3);
  ASSERT_EQ(std::get<int>(tup2), 0);
  ASSERT_EQ(std::get<int&>(tup2), 3);
  ASSERT_EQ(std::get<int const&>(tup2), 3);

  Tup tup3{value, constexpr_reference_wrapper<int&>{value}, constexpr_reference_wrapper<int const&>{value}};
  value = 4;
  ASSERT_EQ(std::get<0>(tup3), 2);
  ASSERT_EQ(std::get<1>(tup3), 4);
  ASSERT_EQ(std::get<2>(tup3), 4);
  ASSERT_EQ(std::get<int>(tup3), 2);
  ASSERT_EQ(std::get<int&>(tup3), 4);
  ASSERT_EQ(std::get<int const&>(tup3), 4);
}

/// @test a tuple with an rvalue reference element type can be constructed from
/// a reference or a reference wrapper
CONSTEXPR_TEST(Tuple, UsableWithRvalueReferencesAndReferenceWrapperLike) {
  using Tup = std::tuple<int&&, int const&&>;

  {
    int value1 = 0;
    int value2 = 0;

    Tup tup{std::move(value1), std::move(value2)};  // NOLINT(hicpp-move-const-arg)
    ASSERT_EQ(&value1, &std::get<0>(tup));          // NOLINT(bugprone-use-after-move)
    ASSERT_EQ(&value2, &std::get<1>(tup));          // NOLINT(bugprone-use-after-move)
  }

  {
    int value1 = 0;
    int value2 = 0;

    Tup tup{
        constexpr_reference_wrapper<int&&>{std::move(value1)},       // NOLINT(hicpp-move-const-arg)
        constexpr_reference_wrapper<int const&&>{std::move(value2)}  // NOLINT(hicpp-move-const-arg)
    };

    ASSERT_EQ(&value1, &std::get<0>(tup));
    ASSERT_EQ(&value2, &std::get<1>(tup));
  }
}

/// @test a tuple is (nothrow) copy-assignable if and only if all the element types are
/// (nothrow) copy-assignable
TYPED_TEST(Tuple, CopyAssignable) {
  static_assert(
      std::is_copy_assignable<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_copy_assignable>,
      ""
  );

  static_assert(
      std::is_nothrow_copy_assignable<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_nothrow_copy_assignable>,
      ""
  );
}

template <class T>
struct wrap_with_counting_wrapper {
  using type = testing::counting_wrapper<T>;
};

/// @test a copy assigned tuple has elements that are copy assigned from the other tuple of the same type
CONDITIONAL_TYPED_TEST(
    Tuple,
    CopyAssignment,
    std::is_default_constructible_v<TypeParam>&& std::is_copy_assignable_v<TypeParam> &&
        (test::all_of_v<
            TypeParam,
            test::reduce<std::is_copy_constructible, test::from<int>::is_constructible>::using_and>)
) {
  static_assert(
      std::is_same_v<TypeParam&, decltype(std::declval<TypeParam&>() = std::declval<TypeParam const&>())>,
      ""
  );

  using counting_tuple = ::testing::mutate_t<TypeParam, wrap_with_counting_wrapper>;
  auto const source = test::with_constant_sequence_for<TypeParam>([](auto... constant) {
    return counting_tuple{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
  });

  auto copy = counting_tuple{};
  copy = source;

  arene::base::for_each_index(                                        //
      source,                                                         //
      [&copy](auto index_constant, auto const& expected_element) {    //
        auto const& copied_element = std::get<index_constant>(copy);  //
        ASSERT_EQ(expected_element.value, copied_element.value);      //
        ASSERT_TRUE(copied_element.count.copy_assign >= 1);           //
      }                                                               //
  );
}

/// @test a tuple is (nothrow) move-assignable if and only if all the element types are
/// (nothrow) move-assignable
TYPED_TEST(Tuple, MoveAssignable) {
  static_assert(
      std::is_move_assignable<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_move_assignable>,
      ""
  );
  static_assert(
      std::is_nothrow_move_assignable<TypeParam>::value ==  //
          test::all_of_v<TypeParam, std::is_nothrow_move_assignable>,
      ""
  );
}

/// @test a move assigned tuple has elements that are move assigned from the other tuple of the same type
CONDITIONAL_TYPED_TEST(
    Tuple,
    MoveAssignment,
    std::is_default_constructible_v<TypeParam>&& std::is_move_assignable_v<TypeParam> &&
        (test::all_of_v<
            TypeParam,
            test::reduce<std::is_copy_constructible, test::from<int>::is_constructible>::using_and>)
) {
  static_assert(std::is_same_v<TypeParam&, decltype(std::declval<TypeParam&>() = std::declval<TypeParam&&>())>, "");

  using counting_tuple = ::testing::mutate_t<TypeParam, wrap_with_counting_wrapper>;
  auto const make_source = [] {
    return test::with_constant_sequence_for<TypeParam>([](auto... constant) {
      return counting_tuple{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
    });
  };

  auto move = counting_tuple{};
  move = make_source();

  auto const values = make_source();

  arene::base::for_each_index(                                       //
      values,                                                        //
      [&move](auto index_constant, auto const& expected_element) {   //
        auto const& moved_element = std::get<index_constant>(move);  //
        ASSERT_EQ(expected_element.value, moved_element.value);      //
        ASSERT_EQ(moved_element.count.copy_assign, 0);               //
        ASSERT_TRUE(moved_element.count.move_assign >= 1);           //
      }                                                              //
  );
}

/// @brief Wrapper type that is always assignable from @c T
/// @tparam NoExcept true if assignment should be noexcept
/// @tparam T The type to be assignable from
template <bool NoExcept, class T>
class assignable {
  T val_;

 public:
  assignable() = default;
  auto operator=(T const& other) noexcept(NoExcept) -> assignable& {
    copy_assign_count++;
    val_ = other;
    return *this;
  }
  auto operator=(T&& other) noexcept(NoExcept) -> assignable& {
    move_assign_count++;
    val_ = std::move(other);
    return *this;
  }
  auto val() const -> T const& { return val_; }
  auto val() -> T& { return val_; }

  using type = assignable<NoExcept, T>;

  // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
  static std::size_t copy_assign_count;
  static std::size_t move_assign_count;
};

template <bool NoExcept, class T>
std::size_t assignable<NoExcept, T>::copy_assign_count = {};

template <bool NoExcept, class T>
std::size_t assignable<NoExcept, T>::move_assign_count = {};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/// @brief Alias for a wrapper type that is noexcept assignable from @c T
/// @tparam T The type to be assignable from
template <class T>
using noexcept_assignable = assignable<true, T>;

/// @brief Alias for a wrapper type that is not noexcept assignable from @c T
/// @tparam T The type to be assignable from
template <class T>
using not_noexcept_assignable = assignable<false, T>;

/// @test a tuple is (nothrow) copy-assignable from another tuple if and only if all the element types are
/// (nothrow) assignable
TYPED_TEST(Tuple, OtherTupleCopyAssignable) {
  using assignable_tuple_type = ::testing::mutate_t<TypeParam, noexcept_assignable>;

  static_assert(std::is_nothrow_assignable_v<assignable_tuple_type&, TypeParam const&>, "");

  using not_nothrow_assignable_tuple_type = ::testing::mutate_t<TypeParam, not_noexcept_assignable>;
  static_assert(std::is_assignable_v<not_nothrow_assignable_tuple_type&, TypeParam const&>, "");

  // Empty tuple is always nothrow assignable.
  constexpr auto is_empty = std::tuple_size_v<TypeParam> == 0;
  static_assert(std::is_nothrow_assignable_v<not_nothrow_assignable_tuple_type&, TypeParam const&> == is_empty, "");

  using other_tuple_type = test::repeat_t<std::tuple_size<TypeParam>::value, int&>;
  static_assert(
      std::is_assignable_v<TypeParam, other_tuple_type const&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_assignable>,
      ""
  );

  static_assert(
      std::is_nothrow_assignable_v<TypeParam, other_tuple_type const&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_nothrow_assignable>,
      ""
  );
}

/// @test a copy assigned tuple from another tuple type has elements that are copy assigned from the other tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    OtherTupleCopyAssignment,
    std::is_default_constructible_v<TypeParam>&& std::is_copy_assignable_v<TypeParam>&&
            std::is_copy_constructible_v<TypeParam> &&
        (test::all_of_v<TypeParam, test::from<int>::is_constructible>)
) {
  auto const source = test::with_constant_sequence_for<TypeParam>([](auto... constant) {
    return TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
  });
  static_assert(std::is_same_v<TypeParam&, decltype(std::declval<TypeParam&>() = source)>, "");

  auto assigned = ::testing::mutate_t<TypeParam, noexcept_assignable>{};
  assigned = source;

  arene::base::for_each_index(                                        //
      source,                                                         //
      [&assigned](auto index_constant, auto const& source_element) {  //
        auto& dest_element = std::get<index_constant>(assigned);      //
        ASSERT_EQ(source_element, dest_element.val());                //
        ASSERT_TRUE(dest_element.copy_assign_count >= 1);             //
      }                                                               //
  );
}

/// @test a tuple is (nothrow) move-assignable from another tuple if and only if all the element types are
/// (nothrow) move assignable
TYPED_TEST(Tuple, OtherTupleMoveAssignable) {
  using assignable_tuple_type = ::testing::mutate_t<TypeParam, noexcept_assignable>;
  static_assert(std::is_nothrow_assignable_v<assignable_tuple_type&, TypeParam&&>, "");

  using not_nothrow_assignable_tuple_type = ::testing::mutate_t<TypeParam, not_noexcept_assignable>;
  static_assert(std::is_assignable_v<not_nothrow_assignable_tuple_type&, TypeParam&&>, "");

  // Empty tuple is always nothrow assignable.
  constexpr auto is_empty = std::tuple_size_v<TypeParam> == 0;
  static_assert(std::is_nothrow_assignable_v<not_nothrow_assignable_tuple_type&, TypeParam&&> == is_empty, "");

  using other_tuple_type = test::repeat_t<std::tuple_size<TypeParam>::value, int&>;
  static_assert(
      std::is_assignable_v<TypeParam, other_tuple_type&&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_assignable>,
      ""
  );

  static_assert(
      std::is_nothrow_assignable_v<TypeParam, other_tuple_type&&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_nothrow_assignable>,
      ""
  );
}

/// @test a move assigned tuple from another tuple type has elements that are move assigned from the other tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    OtherTupleMoveAssignment,
    std::is_default_constructible_v<TypeParam>&& std::is_copy_assignable_v<TypeParam>&&
            std::is_copy_constructible_v<TypeParam> &&
        (test::all_of_v<TypeParam, test::from<int>::is_constructible>)
) {
  auto const make_source = [] {
    return test::with_constant_sequence_for<TypeParam>([](auto... constant) {
      return TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
    });
  };
  static_assert(std::is_same_v<TypeParam&, decltype(std::declval<TypeParam&>() = make_source())>, "");

  auto assigned = ::testing::mutate_t<TypeParam, noexcept_assignable>{};
  assigned = make_source();

  auto const values = make_source();

  arene::base::for_each_index(                                        //
      values,                                                         //
      [&assigned](auto index_constant, auto const& source_element) {  //
        auto& dest_element = std::get<index_constant>(assigned);      //
        ASSERT_EQ(source_element, dest_element.val());                //
        ASSERT_TRUE(dest_element.move_assign_count >= 1);             //
      }                                                               //
  );
}

/// @test a tuple is (nothrow) copy-assignable from a pair if and only if all the element types are
/// (nothrow) assignable
CONDITIONAL_TYPED_TEST(Tuple, PairCopyAssignable, std::tuple_size_v<TypeParam> == 2) {
  using assignable_tuple_type = ::testing::mutate_t<TypeParam, noexcept_assignable>;
  using pair_type = std::pair<std::tuple_element_t<0, TypeParam>, std::tuple_element_t<1, TypeParam>>;
  static_assert(std::is_nothrow_assignable_v<assignable_tuple_type&, pair_type const&>, "");

  using not_nothrow_assignable_tuple_type = ::testing::mutate_t<TypeParam, not_noexcept_assignable>;
  static_assert(std::is_assignable_v<not_nothrow_assignable_tuple_type&, pair_type const&>, "");
  static_assert(!std::is_nothrow_assignable_v<not_nothrow_assignable_tuple_type&, pair_type const&>, "");

  using other_pair_type = std::pair<int&, int&>;
  static_assert(
      std::is_assignable_v<TypeParam, other_pair_type const&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_assignable>,
      ""
  );

  static_assert(
      std::is_nothrow_assignable_v<TypeParam, other_pair_type const&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_nothrow_assignable>,
      ""
  );
}

/// @test a copy assigned tuple from a pair type has elements that are copy assigned from the pair
CONDITIONAL_TYPED_TEST(
    Tuple,
    PairCopyAssignment,
    (std::is_default_constructible_v<TypeParam> &&                           //
     std::is_copy_assignable_v<TypeParam> &&                                 //
     test::all_of_v<TypeParam, test::from<int const&>::is_constructible> &&  //
     2U == std::tuple_size<TypeParam>::value)
) {
  auto const pair = std::pair<std::tuple_element_t<0, TypeParam>, std::tuple_element_t<1, TypeParam>>{0, 1};

  auto values = ::testing::mutate_t<TypeParam, noexcept_assignable>{};
  values = pair;

  arene::base::for_each_index(                                      //
      values,                                                       //
      [](auto index_constant, auto const& dest_element) {           //
        using T = std::tuple_element_t<index_constant, TypeParam>;  //
        ASSERT_EQ(T(index_constant), dest_element.val());           //
        ASSERT_TRUE(dest_element.copy_assign_count >= 1);           //
      }                                                             //
  );
}

/// @test a tuple is (nothrow) move-assignable from a pair if and only if all the element types are
/// (nothrow) move assignable
CONDITIONAL_TYPED_TEST(Tuple, PairMoveAssignable, std::tuple_size_v<TypeParam> == 2) {
  using assignable_tuple_type = ::testing::mutate_t<TypeParam, noexcept_assignable>;
  using pair_type = std::pair<std::tuple_element_t<0, TypeParam>, std::tuple_element_t<1, TypeParam>>;
  static_assert(std::is_nothrow_assignable_v<assignable_tuple_type&, pair_type&&>, "");

  using not_nothrow_assignable_tuple_type = ::testing::mutate_t<TypeParam, not_noexcept_assignable>;
  static_assert(std::is_assignable_v<not_nothrow_assignable_tuple_type&, pair_type&&>, "");
  static_assert(!std::is_nothrow_assignable_v<not_nothrow_assignable_tuple_type&, pair_type&&>, "");

  using other_pair_type = std::pair<int&, int&>;
  static_assert(
      std::is_assignable_v<TypeParam, other_pair_type&&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_assignable>,
      ""
  );

  static_assert(
      std::is_nothrow_assignable_v<TypeParam, other_pair_type&&> ==  //
          test::all_of_v<TypeParam, test::from<int&>::is_nothrow_assignable>,
      ""
  );
}

/// @test a move assigned tuple from a pair type has elements that are move assigned from the pair
CONDITIONAL_TYPED_TEST(
    Tuple,
    PairMoveAssignment,
    (std::is_default_constructible_v<TypeParam> && std::is_move_assignable_v<TypeParam> &&
     test::all_of_v<TypeParam, test::from<int const&>::is_constructible> && 2U == std::tuple_size<TypeParam>::value)
) {
  auto const make_source = [] {
    return std::pair<std::tuple_element_t<0, TypeParam>, std::tuple_element_t<1, TypeParam>>{0, 1};
  };

  auto values = ::testing::mutate_t<TypeParam, noexcept_assignable>{};
  values = make_source();

  arene::base::for_each_index(                                      //
      values,                                                       //
      [](auto index_constant, auto const& dest_element) {           //
        using T = std::tuple_element_t<index_constant, TypeParam>;  //
        ASSERT_EQ(T(index_constant), dest_element.val());           //
        ASSERT_TRUE(dest_element.move_assign_count >= 1);           //
      }                                                             //
  );
}

/// @brief @c std::tie lifted into a function object
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
auto const lifted_tie = FUNCTION_LIFT(std::tie);

/// @brief Helper to check that std::tie is nothrow invocable with reference types and returns a tuple of references
/// @tparam Tuple of input types
/// @{
template <class Tuple>
struct nothrow_invocable_with_tuple_args {};

template <class... Ts>
struct nothrow_invocable_with_tuple_args<std::tuple<Ts...>> {
  using expected_return = std::tuple<Ts&...>;
  static constexpr auto value = arene::base::is_nothrow_invocable_r_v<expected_return, decltype(lifted_tie), Ts&...>;
};

/// @}

/// @test @c std::tie is always invocable with any references and unconditionally noexcept
TYPED_TEST(Tuple, TiePropertiesTest) { static_assert(nothrow_invocable_with_tuple_args<TypeParam>::value, ""); }

/// @test @c std::tie can be used to unpack a tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    TieTest,
    std::is_default_constructible_v<TypeParam>&& std::is_copy_assignable_v<TypeParam> &&
        (test::all_of_v<
            TypeParam,
            test::reduce<std::is_copy_constructible, test::from<int>::is_constructible>::using_and>)
) {
  auto const source = test::with_constant_sequence_for<TypeParam>([](auto... constant) {
    return TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
  });

  auto values = TypeParam{};

  arene::base::apply(lifted_tie, values) = source;

  arene::base::for_each_index(                                          //
      source,                                                           //
      [&values](auto index_constant, auto const& expected_element) {    //
        auto const& copied_element = std::get<index_constant>(values);  //
        ASSERT_EQ(expected_element, copied_element);                    //
      }                                                                 //
  );
}

/// @test @c std::tie can be used with std::ignore
TEST(Tuple, TieIgnore) {
  auto const tup = std::tuple<int, double>{1, 3.14};

  auto second = double{};

  std::tie(std::ignore, second) = tup;
  ASSERT_EQ(second, std::get<1>(tup));
}

/// @test @c std::tie can be used to unpack a pair
TEST(Tuple, TiePairTest) {
  auto const pair = std::pair<int, double>{1, 3.14};

  auto first = int{};
  auto second = double{};

  std::tie(first, second) = pair;

  ASSERT_EQ(first, std::get<0>(pair));
  ASSERT_EQ(second, std::get<1>(pair));
}

/// @test calling the @c swap member function on a tuple swaps its contents with another tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    TupleMemberSwapSwapsTheContentsOfTwoTuples,
    std::is_default_constructible_v<TypeParam>&& std::is_copy_constructible_v<TypeParam> &&
        (test::all_of_v<TypeParam, test::from<int>::is_constructible>)
) {
  auto const make_tuple_with_values = [] {
    return test::with_constant_sequence_for<TypeParam>([](auto... constant) {
      return TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
    });
  };

  auto has_values_before_swap = make_tuple_with_values();
  auto has_values_after_swap = TypeParam{};

  ASSERT_TRUE(has_values_before_swap == make_tuple_with_values());
  ASSERT_TRUE(has_values_after_swap == TypeParam{});

  has_values_before_swap.swap(has_values_after_swap);

  ASSERT_TRUE(has_values_before_swap == TypeParam{});
  ASSERT_TRUE(has_values_after_swap == make_tuple_with_values());
}

/// @test the @c swap member function is constrained on the member types being swappable
TEST(Tuple, TupleMemberSwapConstrainedOnTypesBeingSwappable) {
  using not_swappable = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_construct |
       testing::disable::move_assign | testing::disable::swap)>;

  auto const do_member_swap = [](auto& lhs, auto& rhs) -> decltype(lhs.swap(rhs)) { return lhs.swap(rhs); };

  ASSERT_TRUE(arene::base::
                  is_invocable_v<decltype(do_member_swap), std::tuple<int, int, int>&, std::tuple<int, int, int>&>);

  ASSERT_FALSE(arene::base::
                   is_invocable_v<decltype(do_member_swap), std::tuple<not_swappable>&, std::tuple<not_swappable>&>);

  ASSERT_FALSE(arene::base::is_invocable_v<
               decltype(do_member_swap),
               std::tuple<not_swappable, int, int>&,
               std::tuple<not_swappable, int, int>&>);

  ASSERT_FALSE(arene::base::is_invocable_v<
               decltype(do_member_swap),
               std::tuple<int, not_swappable, int, int>&,
               std::tuple<int, not_swappable, int, int>&>);

  ASSERT_FALSE(arene::base::is_invocable_v<
               decltype(do_member_swap),
               std::tuple<int, int, not_swappable>&,
               std::tuple<int, int, not_swappable>&>);
}

struct throwing_swap {};

ARENE_MAYBE_UNUSED auto swap(throwing_swap&, throwing_swap&) noexcept(false) -> void {}

template <class Tuple>
constexpr bool is_member_swap_noexcept_v = noexcept(std::declval<Tuple&>().swap(std::declval<Tuple&>()));

/// @test the @c swap member function is noexcept if swapping each element of the tuple is noexcept
TEST(Tuple, TupleMemberSwapIsNoexceptIfAllElementSwapsAreNoexcept) {
  ASSERT_TRUE(is_member_swap_noexcept_v<std::tuple<int, int, int>>);

  ASSERT_FALSE(is_member_swap_noexcept_v<std::tuple<throwing_swap>>);
  ASSERT_FALSE(is_member_swap_noexcept_v<std::tuple<throwing_swap, int, int>>);
  ASSERT_FALSE(is_member_swap_noexcept_v<std::tuple<int, throwing_swap, int, int>>);
  ASSERT_FALSE(is_member_swap_noexcept_v<std::tuple<int, int, throwing_swap>>);
}

/// @test calling the @c swap free function on a tuple swaps its contents with another tuple
CONDITIONAL_TYPED_TEST(
    Tuple,
    TupleFreeFunctionSwapSwapsTheContentsOfTwoTuples,
    std::is_default_constructible_v<TypeParam>&& std::is_copy_constructible_v<TypeParam> &&
        (test::all_of_v<TypeParam, test::from<int>::is_constructible>)
) {
  auto const make_tuple_with_values = [] {
    return test::with_constant_sequence_for<TypeParam>([](auto... constant) {
      return TypeParam{test::convert_to_ith_element<decltype(constant), TypeParam>()...};
    });
  };

  auto has_values_before_swap = make_tuple_with_values();
  auto has_values_after_swap = TypeParam{};

  ASSERT_TRUE(has_values_before_swap == make_tuple_with_values());
  ASSERT_TRUE(has_values_after_swap == TypeParam{});

  swap(has_values_before_swap, has_values_after_swap);

  ASSERT_TRUE(has_values_before_swap == TypeParam{});
  ASSERT_TRUE(has_values_after_swap == make_tuple_with_values());
}

/// @test the @c swap free function is constrained on the member types being swappable
TEST(Tuple, TupleFreeFunctionSwapConstrainedOnTypesBeingSwappable) {
  using not_swappable = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      (testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_construct |
       testing::disable::move_assign | testing::disable::swap)>;

  auto const lifted_swap = FUNCTION_LIFT(swap);

  ASSERT_TRUE(arene::base::
                  is_invocable_v<decltype(lifted_swap), std::tuple<int, int, int>&, std::tuple<int, int, int>&>);

  ASSERT_FALSE(arene::base::
                   is_invocable_v<decltype(lifted_swap), std::tuple<not_swappable>&, std::tuple<not_swappable>&>);

  ASSERT_FALSE(arene::base::is_invocable_v<
               decltype(lifted_swap),
               std::tuple<not_swappable, int, int>&,
               std::tuple<not_swappable, int, int>&>);

  ASSERT_FALSE(arene::base::is_invocable_v<
               decltype(lifted_swap),
               std::tuple<int, not_swappable, int, int>&,
               std::tuple<int, not_swappable, int, int>&>);

  ASSERT_FALSE(arene::base::is_invocable_v<
               decltype(lifted_swap),
               std::tuple<int, int, not_swappable>&,
               std::tuple<int, int, not_swappable>&>);
}

/// @test the @c swap free function is noexcept if swapping each element of the tuple is noexcept
TEST(Tuple, TupleFreeSwapIsNoexceptIfAllElementSwapsAreNoexcept) {
  ASSERT_TRUE(arene::base::is_nothrow_swappable_v<std::tuple<int, int, int>>);

  ASSERT_FALSE(arene::base::is_nothrow_swappable_v<std::tuple<throwing_swap>>);
  ASSERT_FALSE(arene::base::is_nothrow_swappable_v<std::tuple<throwing_swap, int, int>>);
  ASSERT_FALSE(arene::base::is_nothrow_swappable_v<std::tuple<int, throwing_swap, int, int>>);
  ASSERT_FALSE(arene::base::is_nothrow_swappable_v<std::tuple<int, int, throwing_swap>>);
}

ARENE_IGNORE_END();

/// @test default instances of all parameterized tuple types compare equal
CONDITIONAL_TYPED_TEST(Tuple, SelfComparisonCorrect, std::is_default_constructible_v<TypeParam>) {
  TypeParam tup{};
  ASSERT_TRUE(tup == tup);
  ASSERT_FALSE(tup != tup);
  ASSERT_FALSE(tup < tup);
  ASSERT_TRUE(tup <= tup);
  ASSERT_FALSE(tup > tup);
  ASSERT_TRUE(tup >= tup);
}

/// @test tuples of the same type with different values compare correctly
CONDITIONAL_TYPED_TEST(
    Tuple,
    MismatchedComparisonCorrect,
    ((std::tuple_size<TypeParam>() > 0UL) && (test::all_of_v<TypeParam, test::from<int const&>::is_constructible>))
) {
  using int_constant_1 = std::integral_constant<int, 1>;
  using int_constant_2 = std::integral_constant<int, 2>;
  auto const constant_ones = test::repeat_t<std::tuple_size<TypeParam>::value, int_constant_1>{};
  auto const constant_twos = test::repeat_t<std::tuple_size<TypeParam>::value, int_constant_2>{};

  TypeParam const ones{constant_ones};
  TypeParam const twos{constant_twos};

  ASSERT_FALSE(ones == twos);
  ASSERT_TRUE(ones != twos);
  ASSERT_TRUE(ones < twos);
  ASSERT_TRUE(ones <= twos);
  ASSERT_FALSE(ones > twos);
  ASSERT_FALSE(ones >= twos);
}

/// @test default instances of all parameterized tuple types compare equal
TYPED_TEST(Tuple, SelfComparisonHasRightNoexcept) {
  ASSERT_EQ(
      noexcept(std::declval<TypeParam&>() == std::declval<TypeParam&>()),
      TestFixture::elements_nothrow_equality_comparable
  );
  ASSERT_EQ(
      noexcept(std::declval<TypeParam&>() != std::declval<TypeParam&>()),
      TestFixture::elements_nothrow_equality_comparable
  );
  ASSERT_EQ(
      noexcept(std::declval<TypeParam&>() < std::declval<TypeParam&>()),
      TestFixture::elements_nothrow_less_than_comparable
  );
  ASSERT_EQ(
      noexcept(std::declval<TypeParam&>() <= std::declval<TypeParam&>()),
      TestFixture::elements_nothrow_less_than_comparable
  );
  ASSERT_EQ(
      noexcept(std::declval<TypeParam&>() > std::declval<TypeParam&>()),
      TestFixture::elements_nothrow_less_than_comparable
  );
  ASSERT_EQ(
      noexcept(std::declval<TypeParam&>() >= std::declval<TypeParam&>()),
      TestFixture::elements_nothrow_less_than_comparable
  );
}

/// @test comparisons give the correct order when operands are the same or merely comparable
CONSTEXPR_TEST(TupleUntypedTest, ComparisonGivesCorrectOrder) {
  std::tuple<double, int, double> const small{3.75, -599, 800.125};
  std::tuple<double, int, double> const big{3.75, 599, 1.5};
  std::tuple<float, std::int64_t, float> const bigf{3.75F, 599L, 1.5F};

  ASSERT_FALSE(small == big);
  ASSERT_FALSE(big == small);
  ASSERT_FALSE(small == bigf);
  ASSERT_FALSE(bigf == small);
  ASSERT_TRUE(bigf == big);
  ASSERT_TRUE(big == bigf);

  ASSERT_TRUE(small != big);
  ASSERT_TRUE(big != small);
  ASSERT_TRUE(small != bigf);
  ASSERT_TRUE(bigf != small);
  ASSERT_FALSE(bigf != big);
  ASSERT_FALSE(big != bigf);

  ASSERT_TRUE(small < big);
  ASSERT_FALSE(big < small);
  ASSERT_TRUE(small < bigf);
  ASSERT_FALSE(bigf < small);
  ASSERT_FALSE(bigf < big);
  ASSERT_FALSE(big < bigf);

  ASSERT_TRUE(small <= big);
  ASSERT_FALSE(big <= small);
  ASSERT_TRUE(small <= bigf);
  ASSERT_FALSE(bigf <= small);
  ASSERT_TRUE(bigf <= big);
  ASSERT_TRUE(big <= bigf);

  ASSERT_FALSE(small > big);
  ASSERT_TRUE(big > small);
  ASSERT_FALSE(small > bigf);
  ASSERT_TRUE(bigf > small);
  ASSERT_FALSE(bigf > big);
  ASSERT_FALSE(big > bigf);

  ASSERT_FALSE(small >= big);
  ASSERT_TRUE(big >= small);
  ASSERT_FALSE(small >= bigf);
  ASSERT_TRUE(bigf >= small);
  ASSERT_TRUE(bigf >= big);
  ASSERT_TRUE(big >= bigf);
}

/// @brief A test class that counts the number of times it's been compared without using static variables
struct comp_count {
  /// @brief The number of times equality comparison has happened
  std::size_t equal{0};
  /// @brief The number of times less-than comparison has happened
  std::size_t less{0};
};

/// @brief A test class that counts the number of times it's been compared in various ways
class comparison_counter {
 public:
  // Data has to be public in order to use this class from constexpr
  /// @brief The value of this class; this is the thing that's actually compared
  int value;
  /// @brief A reference to a struct containing the counts of different comparison operations
  comp_count& count;

  constexpr comparison_counter(int arg_value, comp_count& arg_count) noexcept
      : value(arg_value),
        count(arg_count) {}

  friend constexpr auto operator==(comparison_counter const& left, comparison_counter const& right) noexcept {
    ++left.count.equal;
    ++right.count.equal;
    return left.value == right.value;
  }

  friend constexpr auto operator<(comparison_counter const& left, comparison_counter const& right) noexcept {
    ++left.count.less;
    ++right.count.less;
    return left.value < right.value;
  }
};

/// @test equality comparisons short-circuit from left to right after finding the answer they're looking for
CONSTEXPR_TEST(TupleUntypedTest, EqualityShortCircuits) {
  comp_count small_count;
  comp_count big_count;

  auto const small = std::make_tuple(
      comparison_counter(2, small_count),
      comparison_counter(7, small_count),
      comparison_counter(3, small_count)
  );

  auto const big = std::make_tuple(
      comparison_counter(2, big_count),
      comparison_counter(8, big_count),
      comparison_counter(1, big_count)
  );

  // small == small is 3 comparisons, each of which are recorded twice under "small"
  ASSERT_EQ(small, small);
  ASSERT_EQ(small_count.equal, 6UL);
  ASSERT_EQ(small_count.less, 0UL);
  small_count.equal = 0UL;

  // small != big short-circuits after 2 comparisons; these are recorded under both "small" and "big"
  ASSERT_NE(small, big);
  ASSERT_EQ(small_count.equal, 2UL);
  ASSERT_EQ(big_count.equal, 2UL);
  ASSERT_EQ(small_count.less, 0UL);
  ASSERT_EQ(big_count.less, 0UL);
}

/// @test ordered comparisons short-circuit from left to right after finding the answer they're looking for
CONSTEXPR_TEST(TupleUntypedTest, LessThanShortCircuits) {
  comp_count small_count;
  comp_count big_count;

  auto const small = std::make_tuple(
      comparison_counter(2, small_count),
      comparison_counter(7, small_count),
      comparison_counter(3, small_count)
  );

  auto const big = std::make_tuple(
      comparison_counter(2, big_count),
      comparison_counter(8, big_count),
      comparison_counter(1, big_count)
  );

  // small <= small is 2 *less than* comparisons for each element, each of which are recorded twice under "small"
  ASSERT_LE(small, small);
  ASSERT_EQ(small_count.equal, 0UL);
  ASSERT_EQ(small_count.less, 12UL);
  small_count.less = 0UL;

  // small < big short-circuits halfway through the second pair of comparisons; recorded under both "small" and "big"
  ASSERT_LT(small, big);
  ASSERT_EQ(small_count.equal, 0UL);
  ASSERT_EQ(big_count.equal, 0UL);
  ASSERT_EQ(small_count.less, 3UL);
  ASSERT_EQ(big_count.less, 3UL);
  small_count.less = 0UL;
  big_count.less = 0UL;

  // small <= big does the comparison in the other order so it short-circuits at the end of the second pair
  ASSERT_LE(small, big);
  ASSERT_EQ(small_count.equal, 0UL);
  ASSERT_EQ(big_count.equal, 0UL);
  ASSERT_EQ(small_count.less, 4UL);
  ASSERT_EQ(big_count.less, 4UL);
}

/// @test a tuple of pairs with more than 2 elements works with std::get
CONSTEXPR_TEST(TupleUntypedTest, TupleOfPairsGetWorks) {
  auto const tuple_of_pairs = std::make_tuple(std::make_pair(1, 2), std::make_pair(3, 4), std::make_pair(5, 6));

  ASSERT_EQ(std::get<0>(tuple_of_pairs).first, 1);
  ASSERT_EQ(std::get<0>(tuple_of_pairs).second, 2);

  ASSERT_EQ(std::get<1>(tuple_of_pairs).first, 3);
  ASSERT_EQ(std::get<1>(tuple_of_pairs).second, 4);

  ASSERT_EQ(std::get<2>(tuple_of_pairs).first, 5);
  ASSERT_EQ(std::get<2>(tuple_of_pairs).second, 6);
}

/// @test a tuple of tuples works with std::get
CONSTEXPR_TEST(TupleUntypedTest, TupleOfTuplesGetWorks) {
  auto tuple_of_tuples = std::make_tuple(std::make_tuple(1), std::tuple<int*>{});

  ASSERT_EQ(std::make_tuple(1), std::get<0>(tuple_of_tuples));
  ASSERT_EQ(std::tuple<int*>{}, std::get<1>(tuple_of_tuples));
}

}  // namespace

// NOLINTEND(readability-identifier-length,llvm-qualified-auto)
