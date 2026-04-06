// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_HPP_

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// parasoft-begin-suppress AUTOSAR-M17_0_3-a "Reserved names are used as part of a standard library implementation"
// parasoft-begin-suppress CERT_CPP-DCL51-d "Reserved names are used as part of a standard library implementation"

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

#include "arene/base/constraints.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/index_of.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/integer_sequence.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_copy_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_default_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_final.hpp"
#include "stdlib/include/stdlib_detail/is_move_assignable.hpp"
#include "stdlib/include/stdlib_detail/pair.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"
#include "stdlib/include/stdlib_detail/tuple_element.hpp"
#include "stdlib/include/stdlib_detail/tuple_fwd.hpp"
#include "stdlib/include/stdlib_detail/tuple_size.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "False positive: tuple is defined in the current file"

/// @brief obtain the number of elements in a tuple-like type
/// @tparam Types parameter pack of element types of a tuple
template <class... Types>
class tuple_size<tuple<Types...>> : public integral_constant<size_t, sizeof...(Types)> {};

/// @brief obtain the element type at a given index
/// @tparam I index to obtain the element type of
/// @tparam Types parameter pack of element types of a tuple
/// @note ill-formed if @c I is out of bounds
///
template <size_t I, class... Types>
class tuple_element<I, tuple<Types...>> {
 public:
  static_assert(I < sizeof...(Types), "index out of range for the given tuple");

  /// @brief the element type at index @c I
  using type = arene::base::type_lists::at_t<arene::base::type_list<Types...>, I>;
};

// parasoft-end-suppress AUTOSAR-A14_7_2-a

/// @brief Constructs a tuple of references to the given arguments.
/// @tparam Types Types of the arguments.
/// @param args References to the values to include in the tuple.
/// @return A tuple containing references to the input arguments.
template <class... Types>
constexpr auto tie(Types&... args) noexcept -> tuple<Types&...> {
  return tuple<Types&...>{args...};
}

namespace tuple_detail {
/// @brief poison pill declaration of @c __get_tuple_impl
///
/// This is to ensure that @c __get_tuple_impl only looks for a function
/// declaration and does not pick up some other global object named
///  @c __get_tuple_impl .
///
template <class>
// NOLINTNEXTLINE(readability-identifier-naming): detail within a standard library implemantation
auto __get_tuple_impl() -> void = delete;
}  // namespace tuple_detail

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief access an element of a mutable lvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
template <size_t I, class... Types>
constexpr auto get(tuple<Types...>& tup) noexcept -> tuple_element_t<I, tuple<Types...>>& {
  using tuple_detail::__get_tuple_impl;

  return __get_tuple_impl<I>(tup).value();
}

/// @brief access an element of a const lvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
template <size_t I, class... Types>
constexpr auto get(tuple<Types...> const& tup) noexcept -> tuple_element_t<I, tuple<Types...>> const& {
  using tuple_detail::__get_tuple_impl;

  // parasoft-begin-suppress AUTOSAR-A5_2_3-a "tup data not accessed, and const added back before returning reference"
  // parasoft-begin-suppress CERT_CPP-EXP55-a "tup data not accessed, and const added back before returning reference"
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  return __get_tuple_impl<I>(const_cast<tuple<Types...>&>(tup)).value();
  // parasoft-end-suppress CERT_CPP-EXP55-a
  // parasoft-end-suppress AUTOSAR-A5_2_3-a
}

/// @brief access an element of a mutable rvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
// parasoft-begin-suppress AUTOSAR-A8_4_6-a "tup variable only used to deduce tuple member to return a reference to"
// parasoft-begin-suppress AUTOSAR-A8_4_5-a "tup variable only used to deduce tuple member to return a reference to"
// parasoft-begin-suppress AUTOSAR-A12_8_4-a "tup variable only used to deduce tuple member to return a reference to"
template <size_t I, class... Types>
constexpr auto get(tuple<Types...>&& tup) noexcept -> tuple_element_t<I, tuple<Types...>>&& {
  using tuple_detail::__get_tuple_impl;
  using return_type = tuple_element_t<I, tuple<Types...>>&&;

  return static_cast<return_type>(__get_tuple_impl<I>(tup).value());
}
// parasoft-end-suppress AUTOSAR-A12_8_4-a
// parasoft-end-suppress AUTOSAR-A8_4_5-a
// parasoft-end-suppress AUTOSAR-A8_4_6-a

/// @brief access an element of a const rvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
template <size_t I, class... Types>
constexpr auto get(tuple<Types...> const&& tup) noexcept -> tuple_element_t<I, tuple<Types...>> const&& {
  using tuple_detail::__get_tuple_impl;
  using return_type = tuple_element_t<I, tuple<Types...>> const&&;

  // parasoft-begin-suppress AUTOSAR-A5_2_3-a "tup data not accessed, and const added back before returning reference"
  // parasoft-begin-suppress CERT_CPP-EXP55-a "tup data not accessed, and const added back before returning reference"
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  return static_cast<return_type>(__get_tuple_impl<I>(const_cast<tuple<Types...>&>(tup)).value());
  // parasoft-end-suppress CERT_CPP-EXP55-a
  // parasoft-end-suppress AUTOSAR-A5_2_3-a
}

/// @brief access an element of an lvalue reference to a @c tuple
/// @tparam T type of the element to access; must occur exactly once within @c tuple
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the only occurrence of @c T within @c tup
template <class T, class... Types>
constexpr auto get(tuple<Types...>& tup) noexcept -> T& {
  constexpr size_t first_idx{::arene::base::index_of_v<T, Types...>};
  constexpr size_t last_idx{::arene::base::last_index_of_v<T, Types...>};
  static_assert(
      first_idx == last_idx,
      "Type-based std::get<T>(std::tuple<Types...>) can only be used if T appears exactly once in Types, but it "
      "appears multiple times"
  );

  return get<first_idx>(tup);
}

/// @brief access an element of a const lvalue reference to a @c tuple
/// @tparam T type of the element to access; must occur exactly once within @c tuple
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the only occurrence of @c T within @c tup
template <class T, class... Types>
constexpr auto get(tuple<Types...> const& tup) noexcept -> T const& {
  constexpr size_t first_idx{::arene::base::index_of_v<T, Types...>};
  constexpr size_t last_idx{::arene::base::last_index_of_v<T, Types...>};
  static_assert(
      first_idx == last_idx,
      "Type-based std::get<T>(std::tuple<Types...>) can only be used if T appears exactly once in Types, but it "
      "appears multiple times"
  );

  return get<first_idx>(tup);
}

/// @brief access an element of an rvalue reference to a @c tuple
/// @tparam T type of the element to access; must occur exactly once within @c tuple
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the only occurrence of @c T within @c tup
template <class T, class... Types>
constexpr auto get(tuple<Types...>&& tup) noexcept -> T&& {
  constexpr size_t first_idx{::arene::base::index_of_v<T, Types...>};
  constexpr size_t last_idx{::arene::base::last_index_of_v<T, Types...>};
  static_assert(
      first_idx == last_idx,
      "Type-based std::get<T>(std::tuple<Types...>) can only be used if T appears exactly once in Types, but it "
      "appears multiple times"
  );

  return get<first_idx>(std::move(tup));
}

/// @brief access an element of a const rvalue reference to a @c tuple
/// @tparam T type of the element to access; must occur exactly once within @c tuple
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the only occurrence of @c T within @c tup
template <class T, class... Types>
constexpr auto get(tuple<Types...> const&& tup) noexcept -> T const&& {
  constexpr size_t first_idx{::arene::base::index_of_v<T, Types...>};
  constexpr size_t last_idx{::arene::base::last_index_of_v<T, Types...>};
  static_assert(
      first_idx == last_idx,
      "Type-based std::get<T>(std::tuple<Types...>) can only be used if T appears exactly once in Types, but it "
      "appears multiple times"
  );

  return get<first_idx>(std::move(tup));
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

namespace tuple_detail {

/// @brief contains a single element of a tuple
/// @tparam I element index
/// @tparam T element type
///
template <size_t I, class T, class = arene::base::constraints<>>
class tuple_leaf : arene::base::ebo_holder<integral_constant<size_t, I>, T> {
  /// @brief tag type used to access the value within 'ebo_holder'
  using base_tag_type = integral_constant<size_t, I>;

 public:
  using arene::base::ebo_holder<base_tag_type, T>::ebo_holder;

  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

  /// @brief obtain the value contained in this leaf
  /// @return reference to the value
  ///
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a "This class doesn't contain/own the data, it wraps it with type info"
  constexpr auto value() & noexcept -> T& { return this->get_value(base_tag_type{}); }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a

  /// @brief obtain the value contained in this leaf
  /// @return reference to the value
  ///
  constexpr auto value() const& noexcept -> T const& { return this->get_value(base_tag_type{}); }

  // parasoft-end-suppress AUTOSAR-M2_10_1-a
};

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief access the @c tuple_leaf associated with an index
/// @tparam I element index
/// @tparam T element type
/// @param leaf reference to a @c tuple_leaf
/// @return @c leaf
///
/// This function is intended to be used with @c tuple_impl, which publicly
/// inherits from @c tuple_leaf types.
///
/// When the argument type publicly inherits from multiple specializations of
/// @c tuple_leaf and @c I is explicitly specified, this function can be used a
/// mapping from @c I to @c T, which calling code accesses using the return type
/// of this function.
template <size_t I, class T>
constexpr auto get(tuple_leaf<I, T>& leaf) noexcept -> tuple_leaf<I, T>& {
  return leaf;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief helper used in the implementation of a tuple
/// @tparam Indices type containing the indices of a tuple
/// @tparam Types type containing the types of a tuple
///
/// This primary template is intentionally not defined.
///
template <class Indices, class Types>
class tuple_impl;

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

/// @brief helper used to implement move assignment
/// @tparam Dest The tuple to copy assign to
/// @tparam Source The tuple to copy assign from
/// @tparam Indices Parameter pack of indices for indexing the tuples
/// @param dest The destination tuple
/// @param source The source tuple
template <class Dest, class Source, std::size_t... Indices>
void memberwise_forward_assign(Dest& dest, Source&& source, index_sequence<Indices...>) {
  ::arene::base::consume_values({std::get<Indices>(dest) = std::get<Indices>(std::forward<Source>(source))...});
}

/// @brief helper used to implement swap
/// @tparam Lhs The type of the first tuple to swap
/// @tparam Rhs The type of the second tuple to swap
/// @tparam Indices Parameter pack of indices for indexing the tuples
/// @param lhs The first tupe to swap
/// @param rhs The second tupe to swap
template <class Lhs, class Rhs, std::size_t... Indices>
void memberwise_swap(Lhs& lhs, Rhs&& rhs, index_sequence<Indices...>) {
  ::arene::base::consume_values({(swap(std::get<Indices>(lhs), std::get<Indices>(std::forward<Rhs>(rhs))), true)...});
}

// parasoft-end-suppress AUTOSAR-M2_10_1-a

/// @brief determine if a trait predicate is @c true for all pairs of @c T...
///   and @c U...
/// @tparam Trait binary predicate type trait to transform @c T... with @c U...
/// @tparam TupleTypeList type-list containing T types
/// @tparam List type-list containing U types
/// @note This is a workaround for pack length expand errors with older GCC
///   versions
///
template <template <class...> class Trait, class TupleTypeList, class List, class = void>
class are_all_arguments : public false_type {};

/// @brief determine if a trait predicate is @c true for all pairs of @c T...
///   and @c U...
/// @tparam Trait binary predicate type trait to transform @c T... with @c U...
/// @tparam T types in the destination tuple
/// @tparam U types paired with @c T...
/// @note This is a workaround for pack length expand errors with older GCC
///   versions
///
template <template <class...> class Trait, class... T, class... U>
class are_all_arguments<
    Trait,
    arene::base::type_list<T...>,
    arene::base::type_list<U...>,
    enable_if_t<sizeof...(T) == sizeof...(U)>> : public bool_constant<arene::base::all_of_v<Trait<T, U>::value...>> {};

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: There is no single
// argument template constructor for 'tuple_impl'"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: Delegating constructors are used everywhere they can be"

/// @brief helper used in the implementation of a tuple
/// @tparam I parameter pack of indices of a tuple
/// @tparam T parameter pack of types of a tuple
///
template <size_t... I, class... T>
class tuple_impl<index_sequence<I...>, arene::base::type_list<T...>>
    // this flat implementation is faster to compile than a recursive implementation
    : public tuple_leaf<I, T>... {
  /// @brief determine if a trait predicate is @c true for all pairs of @c T...
  ///   and @c U...
  /// @tparam Trait binary predicate type trait to transform @c T... with @c U...
  /// @tparam U types paired with @c T...
  /// @note This is a workaround for pack length expand errors with older GCC
  ///   versions
  ///
  template <template <class...> class Trait, class... U>
  static constexpr bool are_all_arguments_v{
      are_all_arguments<Trait, arene::base::type_list<T...>, arene::base::type_list<U...>>::value
  };

 public:
  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "False positive: =default would have different semantics"

  /// @brief default constructor
  /// @tparam B template parameter used to check constraints
  ///
  /// Value-initializes each element of the tuple
  ///
  /// @note Requires:
  /// * @c is_default_constructible<Ti>::value is @c true for all @c Ti in @c Types...
  ///
  template <
      bool B = arene::base::all_of_v<is_default_constructible_v<T>...>,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<B>>
  constexpr tuple_impl()  //
      noexcept(arene::base::all_of_v<is_nothrow_default_constructible_v<T>...>)
      : tuple_leaf<I, T>{}... {}
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "This overload, and the overload
  // with types 'UTypes&&...', are implemented as specified by the
  // standard."

  /// @brief direct constructor
  /// @tparam B template parameter used to check constraints
  /// @tparam N template parameter used to check constraints
  /// @param args values to initialize with
  ///
  /// Initializes each element of the tuple with the corresponding parameter
  ///
  /// @note Requires:
  /// * @c is_copy_constructible<Ti>::value is @c true for all @c Ti in @c Types...
  /// * <c> sizeof...(Types) != 0 </c> is @c true
  ///
  template <
      bool B = arene::base::all_of_v<is_copy_constructible_v<T>...>,
      size_t N = sizeof...(T),
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<(N != 0U) && B>>
  constexpr explicit tuple_impl(T const&... args)  //
      noexcept(arene::base::all_of_v<is_nothrow_copy_constructible_v<T>...>)
      : tuple_leaf<I, T>{args}... {}

  // parasoft-end-suppress AUTOSAR-A13_3_1-a

  /// @brief converting constructor
  /// @tparam UTypes parameter pack of types to construct from
  /// @param args values to initialize with
  ///
  /// Initializes each element of the tuple with the corresponding parameter
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == sizeof...(UTypes) </c> is @c true
  /// * @c is_constructible<Ti, Ui&&>::value is @c true for all @c Ti in @c
  ///    Types... and @c Ui in @c UTypes...
  /// * <c> sizeof...(Types) != 0 </c> is @c true
  ///
  template <
      class... UTypes,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<
          (sizeof...(UTypes) != 0U) &&                        //
          are_all_arguments_v<is_constructible, UTypes&&...>  //
          >>
  constexpr explicit tuple_impl(UTypes&&... args)  //
      noexcept(are_all_arguments_v<is_nothrow_constructible, UTypes&&...>)
      : tuple_leaf<I, T>{std::forward<UTypes>(args)}... {}

  // NOLINTBEGIN(hicpp-explicit-conversions)

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "False positive:
  // 'tuple<Utypes...>&&' in the other constructor overload is not a forwarding
  // reference"

  /// @brief copy-from-other-tuple constructor
  /// @tparam UTypes parameter pack of types to construct from
  /// @param other @c tuple containing values to initialize with
  ///
  /// Initializes each element of the tuple from the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == sizeof...(UTypes) </c> is @c true
  /// * @c is_constructible<Ti, Ui const&>::value is @c true for all @c Ti in @c
  ///    Types... and @c Ui in @c UTypes...
  /// * <c> sizeof...(Types) != 0 </c> is @c true
  ///
  template <
      class... UTypes,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<
          (sizeof...(UTypes) != 0U) &&                             //
          are_all_arguments_v<is_constructible, UTypes const&...>  //
          >>
  constexpr tuple_impl(tuple<UTypes...> const& other)  //
      noexcept(are_all_arguments_v<is_nothrow_constructible, UTypes const&...>)
      : tuple_impl{get<I>(other)...} {}

  // parasoft-end-suppress AUTOSAR-A13_3_1-a

  /// @brief move-from-other-tuple constructor
  /// @tparam UTypes parameter pack of types to construct from
  /// @param other @c tuple containing values to initialize with
  ///
  /// Initializes each element of the tuple by forwarding the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == sizeof...(UTypes) </c> is @c true
  /// * @c is_constructible<Ti, Ui&&>::value is @c true for all @c Ti in @c
  ///    Types... and @c Ui in @c UTypes...
  /// * <c> sizeof...(Types) != 0 </c> is @c true
  ///
  template <
      class... UTypes,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<
          (sizeof...(UTypes) != 0U) &&                        //
          are_all_arguments_v<is_constructible, UTypes&&...>  //
          >>
  constexpr tuple_impl(tuple<UTypes...>&& other)  //
      noexcept(are_all_arguments_v<is_nothrow_constructible, UTypes&&...>)
      : tuple_impl{get<I>(std::move(other))...} {}

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "False positive:
  // 'pair<Utypes...>&&' in the other constructor overload is not a forwarding
  // reference"

  /// @brief copy-from-pair constructor
  /// @tparam UTypes parameter pack of types to construct from
  /// @param other @c pair containing values to initialize with
  ///
  /// Initializes each element of the tuple with a copy of the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == sizeof...(UTypes) </c> is @c true
  /// * @c is_constructible<Ti, Ui const&>::value is @c true for all @c Ti in @c
  ///    Types... and @c Ui in @c UTypes...
  ///
  template <
      class... UTypes,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<are_all_arguments_v<is_constructible, UTypes const&...>  //
                          >>
  constexpr tuple_impl(pair<UTypes...> const& other)  //
      noexcept(are_all_arguments_v<is_nothrow_constructible, UTypes const&...>)
      : tuple_impl{get<0>(other), get<1>(other)} {}

  // parasoft-end-suppress AUTOSAR-A13_3_1-a

  /// @brief move-from-pair constructor
  /// @tparam UTypes parameter pack of types to construct from
  /// @param other @c pair containing values to initialize with
  ///
  /// Initializes each element of the tuple with a copy of the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == sizeof...(UTypes) </c> is @c true
  /// * @c is_constructible<Ti, Ui&&>::value is @c true for all @c Ti in @c
  ///    Types... and @c Ui in @c UTypes...
  ///
  template <
      class... UTypes,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<are_all_arguments_v<is_constructible, UTypes&&...>  //
                          >>
  constexpr tuple_impl(pair<UTypes...>&& other)  //
      noexcept(are_all_arguments_v<is_nothrow_constructible, UTypes&&...>)
      : tuple_impl{
            std::forward<tuple_element_t<0, pair<UTypes...>>>(get<0>(other)),
            std::forward<tuple_element_t<1, pair<UTypes...>>>(get<1>(other))
        } {}

  // NOLINTEND(hicpp-explicit-conversions)
};

// parasoft-end-suppress AUTOSAR-A12_1_5-a
// parasoft-end-suppress AUTOSAR-A14_5_1-a

/// @brief A type trait to determine the noexcept specification of elementwise equality comparison
/// @tparam LTuple The left tuple to elementwise compare
/// @tparam RTuple The right tuple to elementwise compare
template <class LTuple, class RTuple>
constexpr bool all_elements_nothrow_equality_comparable_v{false};

/// @brief A type trait to determine the noexcept specification of elementwise equality comparison
/// @tparam LTypes The element types of the left tuple to elementwise compare
/// @tparam RTypes The element types of the right tuple to elementwise compare
template <class... LTypes, class... RTypes>
constexpr bool all_elements_nothrow_equality_comparable_v<tuple<LTypes...>, tuple<RTypes...>>{::arene::base::all_of_v<
    ::arene::base::is_nothrow_equality_comparable_v<LTypes, RTypes>...>};

/// @brief A type trait to determine the noexcept specification of elementwise less-than comparison
/// @tparam LTuple The left tuple to elementwise compare
/// @tparam RTuple The right tuple to elementwise compare
template <class LTuple, class RTuple>
constexpr bool all_elements_nothrow_less_than_comparable_v{false};

/// @brief A type trait to determine the noexcept specification of elementwise less-than comparison
/// @tparam LTypes The element types of the left tuple to elementwise compare
/// @tparam RTypes The element types of the right tuple to elementwise compare
template <class... LTypes, class... RTypes>
constexpr bool all_elements_nothrow_less_than_comparable_v<
    tuple<LTypes...>,
    tuple<
        RTypes...>>{::arene::base::all_of_v<::arene::base::is_nothrow_less_than_comparable_v<LTypes, RTypes>...> && ::arene::base::all_of_v<::arene::base::is_nothrow_less_than_comparable_v<RTypes, LTypes>...>};

// We have to disable clang-format here because it can't parse the constraints with the "<" in them.
// clang-format off
/// @brief Check if element @c Idx and all subsequent elements of the two tuples are equal
/// @tparam Idx The Idx to begin checking at
/// @tparam LTypes The parameter types of the left tuple
/// @tparam RTypes The parameter types of the right tuple
/// @return @c true if element @c Idx and all subsequent elements of the two tuples are equal
template <std::size_t Idx, class... LTypes, class... RTypes,
          ::arene::base::constraints<enable_if_t<Idx == sizeof...(LTypes)>> = nullptr>
constexpr auto elements_equal(tuple<LTypes...> const&, tuple<RTypes...> const&) noexcept -> bool {
  return true;
}

/// @brief Check if element @c Idx and all subsequent elements of the two tuples are equal
/// @tparam Idx The Idx to begin checking at
/// @tparam LTypes The parameter types of the left tuple
/// @tparam RTypes The parameter types of the right tuple
/// @param left The left tuple
/// @param right The right tuple
/// @return @c true if element @c Idx and all subsequent elements of the two tuples are equal
template <std::size_t Idx, class... LTypes, class... RTypes,
          ::arene::base::constraints<enable_if_t<Idx < sizeof...(LTypes)>> = nullptr>
constexpr auto elements_equal(tuple<LTypes...> const& left, tuple<RTypes...> const& right)
    noexcept(all_elements_nothrow_equality_comparable_v<tuple<LTypes...>, tuple<RTypes...>>) -> bool {
  return (get<Idx>(left) == get<Idx>(right)) && ::std::tuple_detail::elements_equal<Idx + 1UL>(left, right);
}

/// @brief Check if the left tuple is less than the right tuple, starting at @c Idx and counting up
/// @tparam Idx The Idx to begin checking at
/// @tparam LTypes The parameter types of the left tuple
/// @tparam RTypes The parameter types of the right tuple
/// @return @c true if <c>get<Idx>(left) < get<Idx>(right)</c> or if they're equal and
/// <c>elements_less<Idx + 1>(left, right)</c>
template <std::size_t Idx, class... LTypes, class... RTypes,
          ::arene::base::constraints<enable_if_t<Idx == sizeof...(LTypes)>> = nullptr>
constexpr auto elements_less(tuple<LTypes...> const&, tuple<RTypes...> const&) noexcept -> bool {
  return false;
}

/// @brief Check if the left tuple is less than the right tuple, starting at @c Idx and counting up
/// @tparam Idx The Idx to begin checking at
/// @tparam LTypes The parameter types of the left tuple
/// @tparam RTypes The parameter types of the right tuple
/// @param left The left tuple
/// @param right The right tuple
/// @return @c true if <c>get<Idx>(left) < get<Idx>(right)</c> or if they're equal and
/// <c>elements_less<Idx + 1>(left, right)</c>
template <std::size_t Idx, class... LTypes, class... RTypes,
          ::arene::base::constraints<enable_if_t<Idx < sizeof...(LTypes)>> = nullptr>
constexpr auto elements_less(tuple<LTypes...> const& left, tuple<RTypes...> const& right)
    noexcept(all_elements_nothrow_less_than_comparable_v<tuple<LTypes...>, tuple<RTypes...>>) -> bool {
  if (get<Idx>(left) < get<Idx>(right)) {
    return true;
  }
  if (get<Idx>(right) < get<Idx>(left)) {
    return false;
  }
  return ::std::tuple_detail::elements_less<Idx + 1UL>(left, right);
}
// clang-format on

}  // namespace tuple_detail

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
/// @brief a heterogeneous, fixed-size collection of values
/// @tparam Types the types of elements that the tuple stores
///
template <class... Types>
class tuple : tuple_detail::tuple_impl<index_sequence_for<Types...>, arene::base::type_list<Types...>> {
  /// @brief implementation type that publicly inherits from the individual tuple elements
  using impl_type = tuple_detail::tuple_impl<index_sequence_for<Types...>, arene::base::type_list<Types...>>;

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2"
  // NOLINTBEGIN(readability-identifier-naming): detail within a standard library implemantation

  /// @brief obtain the @c tuple_leaf element at a specified index
  /// @tparam I tuple element index
  /// @param tup reference to a @c tuple
  /// @return reference to the value containing the @c I 'th element
  template <size_t I>
  friend constexpr auto __get_tuple_impl(tuple& tup) noexcept -> decltype(tuple_detail::get<I>(declval<impl_type&>())) {
    return tuple_detail::get<I>(static_cast<impl_type&>(tup));
  }

  // NOLINTEND(readability-identifier-naming)
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  using impl_type::impl_type;

  /// @brief determine if a trait predicate is @c true for all pairs of @c T...
  ///   and @c U...
  /// @tparam Trait binary predicate type trait to transform @c T... with @c U...
  /// @tparam U types paired with @c T...
  /// @note This is a workaround for pack length expand errors with older GCC
  ///   versions
  ///
  template <template <class...> class Trait, class... U>
  static constexpr bool are_all_arguments_v{
      tuple_detail::are_all_arguments<Trait, arene::base::type_list<Types...>, arene::base::type_list<U...>>::value
  };

  /// @brief Helper index sequence for @c Types...
  static constexpr auto type_index_sequence = std::index_sequence_for<Types...>{};

 public:
  // parasoft-begin-suppress AUTOSAR-A12_8_6-a "False Positive: this is not a base class."
  /// @brief copy-from-other-tuple assignment
  /// @tparam UTypes parameter pack of types to assign from
  /// @param other @c tuple containing values to assign from
  ///
  /// Initializes each element of the tuple from the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == sizeof...(UTypes) </c> is @c true
  /// * @c is_assignable<Ti, Ui const&>::value is @c true for all @c Ti in @c
  ///    Types... and @c Ui in @c UTypes...
  /// * <c> sizeof...(Types) != 0 </c> is @c true
  ///
  template <
      class... UTypes,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<
          (sizeof...(UTypes) != 0U) &&                          //
          are_all_arguments_v<is_assignable, UTypes const&...>  //
          >>
  auto operator=(tuple<UTypes...> const& other) noexcept(are_all_arguments_v<is_nothrow_assignable, UTypes const&...>)
      -> tuple& {
    tuple_detail::memberwise_forward_assign(*this, other, type_index_sequence);
    return *this;
  }

  /// @brief move-from-other-tuple assignment
  /// @tparam UTypes parameter pack of types to move assign from
  /// @param other @c tuple containing values to move assign from
  ///
  /// Initializes each element of the tuple from the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == sizeof...(UTypes) </c> is @c true
  /// * @c is_assignable<Ti, Ui const&>::value is @c true for all @c Ti in @c
  ///    Types... and @c Ui in @c UTypes...
  /// * <c> sizeof...(Types) != 0 </c> is @c true
  ///
  template <
      class... UTypes,
      // avoid GCC9 SFINAE error with 'constraints'
      class = enable_if_t<
          (sizeof...(UTypes) != 0U) &&                     //
          are_all_arguments_v<is_assignable, UTypes&&...>  //
          >>
  auto operator=(tuple<UTypes...>&& other) noexcept(are_all_arguments_v<is_nothrow_assignable, UTypes&&...>) -> tuple& {
    tuple_detail::memberwise_forward_assign(*this, std::move(other), type_index_sequence);
    return *this;
  }

  /// @brief copy-assign-from-pair constructor
  /// @tparam U1 first parameter to assign from
  /// @tparam U2 second parameter to assign from
  /// @param other @c pair containing values to initialize with
  ///
  /// Initializes each element of the tuple with a copy of the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == 2 </c> is @c true
  /// * @c is_assignable<T1, U1 const&>::value  and @c is_assignable<T2, U2 const&>::value is @c true
  ///
  template <
      class U1,
      class U2,
      class = enable_if_t<
          (sizeof...(Types) == 2U) &&                               //
          are_all_arguments_v<is_assignable, U1 const&, U2 const&>  //
          >>
  auto operator=(pair<U1, U2> const& other) noexcept(are_all_arguments_v<is_nothrow_assignable, U1 const&, U2 const&>)
      -> tuple& {
    tuple_detail::memberwise_forward_assign(*this, other, type_index_sequence);
    return *this;
  }

  /// @brief move-assign-from-pair constructor
  /// @tparam U1 first parameter to assign from
  /// @tparam U2 second parameter to assign from
  /// @param other @c pair containing values to initialize with
  ///
  /// Initializes each element of the tuple by moving the values in @c other
  ///
  /// @note Requires:
  /// * <c> sizeof...(Types) == 2 </c> is @c true
  /// * @c is_assignable<T1, U1&&>::value  and @c is_assignable<T2, U2&&>::value is @c true
  ///
  template <
      class U1,
      class U2,
      class = enable_if_t<
          (sizeof...(Types) == 2U) &&                     //
          are_all_arguments_v<is_assignable, U1&&, U2&&>  //
          >>
  auto operator=(pair<U1, U2>&& other) noexcept(are_all_arguments_v<is_nothrow_assignable, U1&&, U2&&>) -> tuple& {
    tuple_detail::memberwise_forward_assign(*this, std::move(other), type_index_sequence);
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A12_8_6-a "False Positive: this is not a base class."
  // parasoft-end-suppress AUTOSAR-M2_10_1-a

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False Positive: 'swap' does not hide anything"
  /// @brief swap with another tuple
  /// @param rhs the other tuple to swap @c this with
  /// @post Equivalent to having invoked @c swap(get<Idx>(*this),get<Idx>(rhs)) for every element.
  ///
  /// @note Requires:
  /// * <c> arene::base::all_of_v<arene::base::is_swappable_v<Types>...> </c> is @c true
  template <
      bool B = arene::base::all_of_v<arene::base::is_swappable_v<Types>...>,
      arene::base::constraints<enable_if_t<B>> = nullptr>
  auto swap(tuple& rhs) noexcept(arene::base::all_of_v<arene::base::is_nothrow_swappable_v<Types>...>) -> void {
    tuple_detail::memberwise_swap(*this, rhs, type_index_sequence);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
};

/// @brief swap two tuples
/// @tparam Types The types in the two tuples to be swapped
/// @param lhs the first tuple to swap
/// @param rhs the second tuple to swap
/// @post Equivalent to having invoked @c swap(get<Idx>(lhs),get<Idx>(rhs)) for every element.
///
/// @note Requires:
/// * <c> arene::base::all_of_v<arene::base::is_swappable_v<Types>...> </c> is @c true
template <
    class... Types,
    arene::base::constraints<enable_if_t<arene::base::all_of_v<arene::base::is_swappable_v<Types>...>>> = nullptr>
auto swap(tuple<Types...>& lhs, tuple<Types...>& rhs) noexcept(
    noexcept(std::declval<tuple<Types...>&>().swap(std::declval<tuple<Types...>&>()))
) -> void {
  lhs.swap(rhs);
}

// parasoft-begin-suppress AUTOSAR-A13_5_5-b "This is how the comparison operators are specified in C++14"
/// @brief Compare two tuples by doing an equality comparison on their elements starting from 0
/// @tparam LTypes The element types of the left tuple
/// @tparam RTypes The element types of the right tuple
/// @param left The left tuple to compare
/// @param right The right tuple to compare
/// @return @c true if the elements of @c left all compare equal to those of <c>right</c>, otherwise @c false
template <class... LTypes, class... RTypes>
constexpr auto operator==(tuple<LTypes...> const& left, tuple<RTypes...> const& right) noexcept(
    noexcept(::std::tuple_detail::elements_equal<0UL>(left, right))
) -> bool {
  // This is a "Requires" clause in the standard, so it's implemented as a static_assert rather than a constraint.
  static_assert(sizeof...(LTypes) == sizeof...(RTypes), "Only tuples of the same size can be compared");
  return ::std::tuple_detail::elements_equal<0UL>(left, right);
}

/// @brief Compare two tuples by doing an equality comparison on their elements starting from 0
/// @tparam LTypes The element types of the left tuple
/// @tparam RTypes The element types of the right tuple
/// @param left The left tuple to compare
/// @param right The right tuple to compare
/// @return @c true if any of the elements of @c left compare unequal to those of <c>right</c>, otherwise @c false
template <class... LTypes, class... RTypes>
constexpr auto operator!=(tuple<LTypes...> const& left, tuple<RTypes...> const& right) noexcept(noexcept(left == right))
    -> bool {
  return !(left == right);
}

/// @brief Compare two tuples by doing a lexicographic comparison on their elements starting from 0
/// @tparam LTypes The element types of the left tuple
/// @tparam RTypes The element types of the right tuple
/// @param left The left tuple to compare
/// @param right The right tuple to compare
/// @return @c true if a lexicographic comparison of the elements gives "less", otherwise @c false
template <class... LTypes, class... RTypes>
constexpr auto operator<(tuple<LTypes...> const& left, tuple<RTypes...> const& right) noexcept(
    noexcept(::std::tuple_detail::elements_less<0UL>(left, right))
) -> bool {
  // This is a "Requires" clause in the standard, so it's implemented as a static_assert rather than a constraint.
  static_assert(sizeof...(LTypes) == sizeof...(RTypes), "Only tuples of the same size can be compared");
  return ::std::tuple_detail::elements_less<0UL>(left, right);
}

/// @brief Compare two tuples by doing a lexicographic comparison on their elements starting from 0
/// @tparam LTypes The element types of the left tuple
/// @tparam RTypes The element types of the right tuple
/// @param left The left tuple to compare
/// @param right The right tuple to compare
/// @return @c true if a lexicographic comparison of the elements gives "less" or "equal", otherwise @c false
template <class... LTypes, class... RTypes>
constexpr auto operator<=(tuple<LTypes...> const& left, tuple<RTypes...> const& right) noexcept(noexcept(right < left))
    -> bool {
  return !(right < left);
}

/// @brief Compare two tuples by doing a lexicographic comparison on their elements starting from 0
/// @tparam LTypes The element types of the left tuple
/// @tparam RTypes The element types of the right tuple
/// @param left The left tuple to compare
/// @param right The right tuple to compare
/// @return @c true if a lexicographic comparison of the elements gives "greater", otherwise @c false
template <class... LTypes, class... RTypes>
constexpr auto operator>(tuple<LTypes...> const& left, tuple<RTypes...> const& right) noexcept(noexcept(right < left))
    -> bool {
  return right < left;
}

/// @brief Compare two tuples by doing a lexicographic comparison on their elements starting from 0
/// @tparam LTypes The element types of the left tuple
/// @tparam RTypes The element types of the right tuple
/// @param left The left tuple to compare
/// @param right The right tuple to compare
/// @return @c true if a lexicographic comparison of the elements gives "greater" or "equal", otherwise @c false
template <class... LTypes, class... RTypes>
constexpr auto operator>=(tuple<LTypes...> const& left, tuple<RTypes...> const& right) noexcept(noexcept(left < right))
    -> bool {
  return !(left < right);
}
// parasoft-end-suppress AUTOSAR-A13_5_5-b

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_HPP_
