// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_TRY_CONSTRUCT_INTERFACE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_TRY_CONSTRUCT_INTERFACE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/in_place.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace inline_container_detail {

/// @brief helper type for @c try_construct_interface
/// @tparam Head parameters prepended to common container constructor arguments
///
/// Used to specify @c Head arguments at the front of all non special member
/// constructors, if any.
///
template <class... Head>
struct try_construct_head_args {};

// parasoft-begin-suppress AUTOSAR-A2_7_2-a "False positive: no commented out code, this is documentation"
/// @brief helper to define common @c try_construct static member functions
/// @tparam Derived derived container type
/// @tparam Policy policy type for @c try_construct_interface
/// @tparam Head arguments to prepend to @c try_construct members associated
///   with non special member constructors. Defaults to an empty parameter pack.
///
/// CRTP helper class to define common @c try_construct static member functions.
/// Each @c try_construct overload is associated with a constructor of
/// @c Derived and is conditionally defined. These @c try_construct members
/// remove the size <= capacity precondition, returning an empty @c optional if
/// the size would have exceeded the container capacity.
///
/// For each of the following @c Derived(xs...) constructors, this helper will
/// define the associated @c try_construct(xs...) overload, if the constructor is
/// provided:
///
/// * <tt> Derived() </tt>
/// * <tt> Derived(Derived&&) </tt>
/// * <tt> Derived(Derived const&) </tt>
/// * <tt> Derived(Head...) </tt>
/// * <tt> Derived(Head..., Iterator, Iterator) </tt>
/// * <tt> Derived(Head..., size_type) </tt>
/// * <tt> Derived(Head..., size_type, value_type) </tt>
/// * <tt> Derived(Head..., Derived&&) </tt>
/// * <tt> Derived(Head..., Derived const&) </tt>
/// * <tt> Derived(Head..., std::initializer_list<value_type>) </tt>
///
/// The @c try_construct overloads take the same arguments as the constuctors of
/// @c Derived and return @c optional<Derived>. These overloads inherit all
/// existing exception specifications and preconditions from the associated
/// constructors _except_ they remove the size within capacity precondition.
///
/// @note The <tt> try_construct(Head..., Iterator, Iterator) </tt> overload for
/// InputIterators requires @c Derived to define @c emplace_back.
///
/// Since @c Derived is not fully defined when defining
/// @c try_construct_interface, an additional @c Policy type must contain the
/// following static type aliases and static member functions:
///
/// ~~~{.cpp}
/// struct policy
/// {
///   using size_type = ...;
///   using value_type = ...;
///   static constexpr auto capacity_of(Head const&...) noexcept -> size_type { ... }
/// };
/// ~~~
///
/// where @c size_type and @c value_type are used to define @c try_construct
/// overloads and @c capacity_of specifies a function to obtain the capacity of
/// @c Derived.
///
/// @attention @c Derived *must* be able to provide the container capacity from
///   @c Head.
///
/// @note @c Head may be an empty parameter pack - an example policy for
///   @c inline_vector could be
/// ~~~{.cpp}
/// template <class T, std::size_t N>
/// struct inline_vector_policy
/// {
///   using size_type = std::size_t;
///   using value_type = T;
///   static constexpr auto capacity_of() noexcept -> size_type { return N; }
/// };
/// ~~~
///
/// The @c Head parameter pack is specified as the third template parameter, and
/// must be a specialization of @c try_construct_head_args. @c Head arguments
/// are typically used to perform @c Derived container specific initialization.
///
/// This primary template is empty.
///
// parasoft-end-suppress AUTOSAR-A2_7_2-a
template <class Derived, class Policy, class Head = try_construct_head_args<>>
class try_construct_interface {};

/// @brief helper to define common @c try_construct static member functions
/// @tparam Derived derived container type
/// @tparam Policy policy type for @c try_construct_interface
/// @tparam Head arguments to prepend to @c try_construct members associated
///   with non special member constructors. Defaults to an empty parameter pack.
///
/// Specialization if the last template parameter is a specialization of
/// @c try_construct_head_args.
///
template <class Derived, class Policy, class... Head>
class try_construct_interface<Derived, Policy, try_construct_head_args<Head...>> {
  /// @brief container size type
  using size_type = typename Policy::size_type;
  /// @brief container value type
  using value_type = typename Policy::value_type;

  /// @brief determines if an iterator range constructor is within the container capacity
  /// @tparam I forward iterator type
  /// @param head @c Derived container specific arguments
  /// @param first beginning of the range
  /// @param last end of the range
  /// @return @c true if <tt> std::distance(first, last) <= capacity </tt>;
  ///   otherwise @c false
  ///
  template <class I, constraints<std::enable_if_t<is_forward_iterator_v<I>>> = nullptr>
  static constexpr auto within_capacity(Head const&... head, I first, I last) noexcept(noexcept(
      (arene::base::distance(std::declval<I&>(), std::declval<I&>()) <=
       Policy::capacity_of(std::declval<Head const&>()...))
  )) -> bool {
    return static_cast<size_type>(arene::base::distance(first, last)) <= Policy::capacity_of(head...);
  }

  /// @brief determines if a size constructor is within the container capacity
  /// @tparam Value potential value type for the size-value constructor, unused
  /// @param head @c Derived container specific arguments
  /// @param count size of the @c Derived container
  /// @return @c true if <tt> count <= capacity </tt>;
  ///   otherwise @c false
  ///
  template <class... Value>
  static constexpr auto within_capacity(Head const&... head, size_type count, Value const&...) noexcept(
      noexcept(std::declval<size_type&>() <= Policy::capacity_of(std::declval<Head const&>()...))
  ) -> bool {
    return count <= Policy::capacity_of(head...);
  }

  /// @brief determines if a range constructor is within the container capacity
  /// @tparam Range range type that defines a @c size() member
  /// @param head @c Derived container specific arguments
  /// @param range reference to @c Range
  /// @return @c true if <tt> range.size() <= capacity </tt>;
  ///   otherwise @c false
  ///
  template <class Range>
  static constexpr auto within_capacity(Head const&... head, Range const& range) noexcept(
      noexcept(std::declval<Range&>().size() <= Policy::capacity_of(std::declval<Head const&>()...))
  ) -> bool {
    return range.size() <= Policy::capacity_of(head...);
  }

  /// @brief determine if @c within_capcity is nothrow invocable
  /// @tparam Args parameter pack of head and tail constructor argument types
  ///
  template <class... Args>
  static constexpr auto nothrow_within_capacity(type_list<Args...>) noexcept(
      noexcept(within_capacity(std::declval<Args>()...))
  ) -> void {}

  /// @brief determine if @c within_capcity is nothrow invocable
  /// @tparam Tail parameter pack of common constructor argument types
  ///
  template <class... Tail>
  static constexpr bool is_nothrow_within_capacity_v{
      noexcept(nothrow_within_capacity(type_lists::concat_t<type_list<Head const&...>, type_list<Tail...>>{}))
  };

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive: 'Head' arguments are not always rvalue reference types
  // due to reference collapsing"
  //
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive: 'Head' arguments are not always rvalue reference types
  // due to reference collapsing"
  //
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: 'Head' arguments are not always rvalue reference types
  // due to reference collapsing"

  /// @brief constructs the @c Derived container or @c nullopt based on arguments
  /// @tparam Tail common container argument types
  /// @param head @c Derived container specific arguments
  /// @param tail common container arguments
  /// @return @c Derived container if within capacity; otherwise @c nullopt
  ///
  template <class... Tail>
  static constexpr auto construct_if_within_capacity(Head&&... head, Tail&&... tail) noexcept(
      std::is_nothrow_constructible<Derived, Head&&..., Tail&&...>::value
  ) -> optional<Derived> {
    return within_capacity(head..., tail...)
               ? optional<Derived>{in_place, static_cast<Head&&>(head)..., std::forward<Tail>(tail)...}
               : optional<Derived>{};
  }

  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

  /// @brief determines if the @c Derived container is nothrow constructible
  /// @tparam Self template parameter that is always intended to be @c Derived
  /// @tparam Tail common container argument types
  ///
  template <class Self, class... Tail>
  static constexpr bool is_nothrow_head_constructible_v{std::is_nothrow_constructible<Self, Head&&..., Tail...>::value};

 protected:
  /// @brief constructor
  /// @tparam Actual tag type, intended to always be @c Derived
  /// @tparam Expected deduced template parameter, defined to improve compiler
  ///   error messages
  ///
  /// Construct a 'try_construct_interface' CRTP helper type. Intended to be invoked as:
  /// ~~~{.cpp}
  /// try_construct_interface{this}
  /// ~~~
  ///
  /// This constructor exists to prevent use by a type that does _not_ define
  /// 'try_construct_interface' as a CRTP base class or to detect
  /// mismatch between the actual derived type and the @c Derived template
  /// parameter.
  ///
  template <class Actual, class Expected = Derived>
  constexpr explicit try_construct_interface(Actual*) noexcept {
    static_assert(
        std::is_same<Actual, Expected>::value,
        "'Derived' type mismatch.\n"  //
        "Please verify that 'Derived' is defined correctly when using 'try_construct_interface' as a CRTP "
        "base."
    );
  }
  /// @brief default destructor
  ~try_construct_interface() = default;
  /// @brief default copy constructor
  constexpr try_construct_interface(try_construct_interface const&) = default;
  /// @brief default move constructor
  constexpr try_construct_interface(try_construct_interface&&) = default;
  /// @brief default copy assignment operator
  constexpr auto operator=(try_construct_interface const&) -> try_construct_interface& = default;
  /// @brief default move assignment operator
  constexpr auto operator=(try_construct_interface&&) -> try_construct_interface& = default;

 public:
  /// @brief default constructs @c Derived
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are default constructible
  /// @return @c optional<Derived> containing a default constructed value
  ///
  template <class Self = Derived, constraints<std::enable_if_t<std::is_default_constructible<Self>::value>> = nullptr>
  static constexpr auto try_construct() noexcept(std::is_nothrow_default_constructible<Self>::value)
      -> optional<Derived> {
    return {in_place};
  }

  /// @brief move constructs @c Derived
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are move constructible
  /// @param other @c Derived container to move
  /// @return @c optional<Derived> containing a move constructed value
  ///
  template <class Self = Derived, constraints<std::enable_if_t<std::is_move_constructible<Self>::value>> = nullptr>
  static constexpr auto try_construct(Derived&& other) noexcept(std::is_nothrow_move_constructible<Self>::value)
      -> optional<Derived> {
    return {in_place, std::move(other)};
  }

  /// @brief copy constructs @c Derived
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are copy constructible
  /// @param other @c Derived container to copy
  /// @return @c optional<Derived> containing a copy constructed value
  ///
  template <class Self = Derived, constraints<std::enable_if_t<std::is_copy_constructible<Self>::value>> = nullptr>
  static constexpr auto try_construct(Derived const& other) noexcept(std::is_nothrow_copy_constructible<Self>::value)
      -> optional<Derived> {
    return {in_place, other};
  }

  /// @brief constructs @c Derived from @c Head args
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args
  /// @param head @c Derived container specific arguments
  ///
  /// Invokes <tt> Derived{head...} </tt>.
  ///
  /// @return @c optional<Derived> containing a @c Derived value
  ///
  template <
      class Self = Derived,
      std::size_t N = sizeof...(Head),
      constraints<std::enable_if_t<N != 0>, std::enable_if_t<std::is_constructible<Self, Head...>::value>> = nullptr>
  static constexpr auto try_construct(Head... head) noexcept(is_nothrow_head_constructible_v<Self>)
      -> optional<Derived> {
    // forward references, move values
    return {in_place, static_cast<Head&&>(head)...};
  }

  /// @brief constructs @c Derived from @c Head args and an iterator-pair range
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and an iterator-pair
  /// @tparam I input iterator type
  /// @param head @c Derived container specific arguments
  /// @param first beginning of the range
  /// @param last end of the range
  ///
  /// If within capacity, constructs @c Derived by repeatedly calling
  /// @c emplace_back.
  ///
  /// @return @c optional<Derived> containing a @c Derived value if the range
  ///   <tt> [first, last) </tt> is within capacity; otherwise @c nullopt
  ///
  template <
      class Self = Derived,
      class I,
      constraints<
          std::enable_if_t<is_input_iterator_v<I>>,
          std::enable_if_t<!is_forward_iterator_v<I>>,
          std::enable_if_t<std::is_constructible<value_type, typename std::iterator_traits<I>::reference>::value>,
          std::enable_if_t<std::is_constructible<Self, Head..., I, I>::value>,
          decltype(std::declval<Self&>().emplace_back(*std::declval<I>()))> = nullptr>
  static constexpr auto try_construct(
      Head... head,
      I first,
      I last
  ) noexcept(noexcept(Policy::capacity_of(std::declval<Head&&>()...)) && is_nothrow_head_constructible_v<Self, I, I>)
      -> optional<Derived> {
    auto const capacity = Policy::capacity_of(head...);

    auto result = optional<Derived>{in_place, static_cast<Head&&>(head)...};

    while (first != last) {
      if (result->size() == capacity) {
        return {};
      }

      std::ignore = result->emplace_back(*first);
      arene::base::advance(first, typename std::iterator_traits<I>::difference_type{1});
    }

    return result;
  }

  /// @brief constructs @c Derived from @c Head args and an iterator-pair range
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and an iterator-pair
  /// @tparam I forward iterator type
  /// @param head @c Derived container specific arguments
  /// @param first beginning of the range
  /// @param last end of the range
  ///
  /// If within capacity, invokes <tt> Derived{head..., first, last} </tt>.
  ///
  /// @return @c optional<Derived> containing a @c Derived value if
  ///   <tt> std::distance(first, last) <= capacity </tt>; otherwise @c nullopt
  ///
  template <
      class Self = Derived,
      class I,
      constraints<
          std::enable_if_t<is_forward_iterator_v<I>>,
          std::enable_if_t<std::is_constructible<value_type, typename std::iterator_traits<I>::reference>::value>,
          std::enable_if_t<std::is_constructible<Self, Head..., I, I>::value>> = nullptr>
  static constexpr auto try_construct(
      Head... head,
      I first,
      I last
  ) noexcept(is_nothrow_within_capacity_v<I, I> && is_nothrow_head_constructible_v<Self, I, I>) -> optional<Derived> {
    return construct_if_within_capacity(static_cast<Head&&>(head)..., first, last);
  }

  /// @brief constructs @c Derived from @c Head args and a count
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and a count
  /// @tparam U dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and a count
  /// @param head @c Derived container specific arguments
  /// @param count size of the constructed container
  ///
  /// If within capacity, invokes <tt> Derived{head..., count} </tt>.
  ///
  /// @return @c optional<Derived> containing a @c Derived value if
  ///   <tt> count <= capacity </tt>; otherwise @c nullopt
  ///
  template <
      class Self = Derived,
      class U = value_type,
      constraints<
          std::enable_if_t<std::is_default_constructible<U>::value>,
          std::enable_if_t<std::is_constructible<Self, Head..., size_type>::value>> = nullptr>
  static constexpr auto try_construct(
      Head... head,
      size_type count
  ) noexcept(is_nothrow_within_capacity_v<size_type> && is_nothrow_head_constructible_v<Self, size_type>)
      -> optional<Derived> {
    return construct_if_within_capacity(static_cast<Head&&>(head)..., count);
  }

  /// @brief constructs @c Derived from @c Head args, count, and value to copy
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args, count, and a value to copy
  /// @tparam U dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args, count, and a value to copy
  /// @param count size of the constructed container
  /// @param head @c Derived container specific arguments
  /// @param value value to copy into the constructed container
  ///
  /// If within capacity, invokes <tt> Derived{head..., count, value} </tt>.
  ///
  /// @return @c optional<Derived> containing a @c Derived value if
  ///   <tt> count <= capacity </tt>; otherwise @c nullopt
  ///
  template <
      class Self = Derived,
      class U = value_type,
      constraints<
          std::enable_if_t<std::is_copy_constructible<U>::value>,
          std::enable_if_t<std::is_constructible<Self, Head..., size_type, value_type>::value>> = nullptr>
  static constexpr auto try_construct(
      Head... head,
      size_type count,
      value_type const& value
  ) noexcept(is_nothrow_within_capacity_v<size_type, value_type const&> && is_nothrow_head_constructible_v<Self, size_type, value_type const&>)
      -> optional<Derived> {
    return construct_if_within_capacity(static_cast<Head&&>(head)..., count, value);
  }

  /// @brief move constructs @c Derived from @c Head args and another @c Derived
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and @c Derived&&
  /// @tparam N dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and @c Derived&&
  /// @tparam U dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and @c Derived&&
  /// @param head @c Derived container specific arguments
  /// @param other @c Derived container to move values from
  ///
  /// If within capacity, invokes <tt> Derived{head..., std::move(other)} </tt>.
  ///
  /// @return @c optional<Derived> containing a @c Derived value if
  ///   <tt> other.size() <= capacity </tt>; otherwise @c nullopt
  ///
  template <
      class Self = Derived,
      std::size_t N = sizeof...(Head),
      class U = value_type,
      constraints<
          std::enable_if_t<N != 0>,
          std::enable_if_t<std::is_move_constructible<U>::value>,
          std::enable_if_t<std::is_constructible<Self, Head..., Self&&>::value>> = nullptr>
  static constexpr auto try_construct(
      Head... head,
      Derived&& other
  ) noexcept(is_nothrow_within_capacity_v<Self&&> && is_nothrow_head_constructible_v<Self, Self&&>)
      -> optional<Derived> {
    return construct_if_within_capacity(static_cast<Head&&>(head)..., std::move(other));
  }

  /// @brief copy constructs @c Derived from @c Head args and another @c Derived
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and <tt> Derived const& </tt>
  /// @tparam N dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and <tt> Derived const& </tt>
  /// @tparam U dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and <tt> Derived const& </tt>
  /// @param head @c Derived container specific arguments
  /// @param other @c Derived container to copy values from
  ///
  /// If within capacity, invokes <tt> Derived{head..., other} </tt>.
  ///
  /// @return @c optional<Derived> containing a @c Derived value if
  ///   <tt> other.size() <= capacity </tt>; otherwise @c nullopt
  ///
  template <
      class Self = Derived,
      std::size_t N = sizeof...(Head),
      class U = value_type,
      constraints<
          std::enable_if_t<N != 0>,
          std::enable_if_t<std::is_copy_constructible<U>::value>,
          std::enable_if_t<std::is_constructible<Self, Head..., Self const&>::value>> = nullptr>
  static constexpr auto try_construct(
      Head... head,
      Derived const& other
  ) noexcept(is_nothrow_within_capacity_v<Self const&> && is_nothrow_head_constructible_v<Self, Self const&>)
      -> optional<Derived> {
    return construct_if_within_capacity(static_cast<Head&&>(head)..., other);
  }

  /// @brief constructs @c Derived from @c Head args and an @c initialier_list
  /// @tparam Self dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and @c std::initializer_list
  /// @tparam N dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and @c std::initializer_list
  /// @tparam U dummy used to constrain this constructor to @c Derived types
  ///   that are constructible from @c Head args and @c std::initializer_list
  /// @param head @c Derived container specific arguments
  /// @param init_list @c std::initialier_list to copy values from
  ///
  /// If within capacity, invokes <tt> Derived{head..., init_list} </tt>.
  ///
  /// @return @c optional<Derived> containing a @c Derived value if
  ///   <tt> other.size() <= capacity </tt>; otherwise @c nullopt
  ///
  template <
      class Self = Derived,
      class U = value_type,
      constraints<
          std::enable_if_t<std::is_copy_constructible<U>::value>,
          std::enable_if_t<std::is_constructible<Self, Head..., std::initializer_list<value_type>>::value>> = nullptr>
  static constexpr auto try_construct(
      Head... head,
      std::initializer_list<value_type> init_list
  ) noexcept(is_nothrow_within_capacity_v<std::initializer_list<value_type>> && is_nothrow_head_constructible_v<Self, std::initializer_list<value_type>>)
      -> optional<Derived> {
    return construct_if_within_capacity(static_cast<Head&&>(head)..., init_list);
  }
};

}  // namespace inline_container_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_TRY_CONSTRUCT_INTERFACE_HPP_
