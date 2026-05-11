// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BASIC_INSERT_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BASIC_INSERT_ITERATOR_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <iterator>

#include "stdlib/include/stdlib_detail/addressof.hpp"
#include "stdlib/include/stdlib_detail/basic_iterator.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/is_move_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"
#include "stdlib/include/stdlib_detail/iterator_tags.hpp"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a "False positive: This header *does* contain declarations necessary to
// compile this file."
#include "stdlib/include/stdlib_detail/move.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a
#include "stdlib/include/stdlib_detail/remove_cv.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"
#include "stdlib/include/stdlib_detail/void_t.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

namespace std {

namespace basic_insert_iterator_detail {

/// @brief helper variable template to check if a type defines member @c value_type
/// @tparam T type to check
///
/// This primary template is always @c false.
///
template <class T, class = void>
extern constexpr bool has_value_type_member_impl{false};

/// @brief helper variable template to check if a type defines member @c value_type
/// @tparam T type to check
///
/// This template specialization is always @c true if @c T has member @c value_type.
///
template <class T>
extern constexpr bool has_value_type_member_impl<T, std::void_t<typename T::value_type>>{true};

/// @brief helper variable template to check if a type defines member @c value_type
/// @tparam T type to check
///
/// This variable template invokes the variable template above in order to avoid
/// displaying the second template parameter in compiler error messages.
///
template <class T>
extern constexpr bool has_value_type_member_v = has_value_type_member_impl<T>;

/// @brief helper variable template to check if a type defines member @c iterator
/// @tparam T type to check
///
/// This primary template is always @c false.
///
template <class T, class = void>
extern constexpr bool has_iterator_member_impl{false};

/// @brief helper variable template to check if a type defines member @c iterator
/// @tparam T type to check
///
/// This template specialization is always @c true if @c T has member @c iterator.
///
template <class T>
extern constexpr bool has_iterator_member_impl<T, std::void_t<typename T::iterator>>{true};

/// @brief helper variable template to check if a type defines member @c iterator
/// @tparam T type to check
///
/// This variable template invokes the variable template above in order to avoid
/// displaying the second template parameter in compiler error messages.
///
template <class T>
extern constexpr bool has_iterator_member_v = has_iterator_member_impl<T>;

// parasoft-begin-suppress AUTOSAR-M14_5_3-a "False positive: There is no template assignment operator with generic
// parameter"

/// @brief iterator adaptor for insertion into a container
/// @tparam Container associated container type
/// @tparam AdaptorMixin mixin specifying the container insert operation
///
/// The class template @c basic_insert_iterator is used to define the insert
/// iterator adaptors:
/// * @c back_insert_iterator
/// * @c insert_iterator
///
/// @c AdaptorMixin specifies the container insert operation (e.g. @c push_back)
/// and provides storage for the container pointer and other necessary members.
///
template <class Container, class AdaptorMixin>
class basic_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    , protected AdaptorMixin {
  /// @brief determine if container insert operation is noexcept
  /// @tparam T type to insert
  ///
  template <class T>
  static constexpr bool is_nothrow_insertable_v{noexcept(std::declval<basic_insert_iterator&>().insert(std::declval<T>()
  ))};

 public:
  /// @brief container type to insert into
  ///
  using container_type = Container;

  /// @brief @c basic_insert_iterator constructor
  ///
  /// This type inherits constructors from the adaptor mixin.
  ///
  using AdaptorMixin::AdaptorMixin;

  // parasoft-begin-suppress AUTOSAR-A12_8_6-a "False positive: these are
  // neither copy assignment nor move assignment operators"

  /// @brief inserts a value into the associated container
  /// @param value value to insert
  ///
  /// Inserts @c value into the associated container with a call to the mixin's
  /// @c insert member function.
  ///
  /// @returns @c *this
  ///
  constexpr auto operator=(typename Container::value_type const& value
  ) noexcept(is_nothrow_insertable_v<typename Container::value_type const&>) -> basic_insert_iterator& {
    this->insert(value);
    return *this;
  }

  /// @brief inserts a value into the associated container
  /// @param value value to insert
  ///
  /// Inserts @c std::move(value) into the associated container with a call to
  /// the mixin's @c insert member function.
  ///
  /// @returns @c *this
  ///
  constexpr auto operator=(typename Container::value_type&& value
  ) noexcept(is_nothrow_insertable_v<typename Container::value_type&&>) -> basic_insert_iterator& {
    this->insert(std::move(value));
    return *this;
  }

  // parasoft-end-suppress AUTOSAR-A12_8_6-a

  /// @brief no-op
  /// @returns @c *this
  ///
  constexpr auto operator*() noexcept -> basic_insert_iterator& { return *this; }

  /// @brief no-op
  /// @returns @c *this
  ///
  constexpr auto operator++() noexcept -> basic_insert_iterator& { return *this; }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "False positive: postincrement requires an int parameter"

  /// @brief no-op
  /// @returns @c *this
  ///
  constexpr auto operator++(int) noexcept -> basic_insert_iterator& { return *this; }

  // parasoft-end-suppress AUTOSAR-A3_9_1-b
};

// parasoft-end-suppress AUTOSAR-M14_5_3-a

}  // namespace basic_insert_iterator_detail

namespace back_insert_iterator_detail {

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: There is no template constructor"

/// @brief mixin used to define @c back_insert_iterator
/// @tparam Container associated container type
///
template <class Container>
class adaptor_mixin {
  static_assert(
      basic_insert_iterator_detail::has_value_type_member_v<Container>,
      "'Container' does not satisfy syntactic requirements"
  );

 protected:
  // Naming and access control for these members are specified by the standard.
  //
  // NOLINTBEGIN(misc-non-private-member-variables-in-classes,readability-identifier-naming)

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "Identifier 'container' must be
  // declared 'protected' to be standard compliant"

  /// @brief associated container to insert into
  ///
  Container* container;

  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  // NOLINTEND(misc-non-private-member-variables-in-classes,readability-identifier-naming)

  /// @brief insert an element into the associated container
  /// @tparam T cv-ref qualified container value type
  /// @param value value to insert
  ///
  template <class T>
  constexpr auto insert(T&& value) noexcept(noexcept(container->push_back(std::forward<T>(value)))) -> void {
    static_assert(
        std::is_same_v<typename Container::value_type, std::remove_cv_t<std::remove_reference_t<T>>>,
        "this internal function is only intended for use with cv-ref qualified container value types."
    );
    static_cast<void>(container->push_back(std::forward<T>(value)));
  }

 public:
  /// @brief constructs an @c adaptor_mixin
  /// @param cont container to insert into
  ///
  constexpr explicit adaptor_mixin(Container& cont) noexcept
      : container{std::addressof(cont)} {}
};

// parasoft-end-suppress AUTOSAR-A14_5_1-a

}  // namespace back_insert_iterator_detail

/// @brief iterator adaptor for insertion at the end of a container
/// @tparam Container associated container type
///
/// @c std::back_insert_iterator is an OutputIterator that appends elements to a
/// container for which it was constructed. The container's @c push_back()
/// member function is called whenever the iterator (whether dereferenced or
/// not) is assigned to. Incrementing the @c std::back_insert_iterator is a
/// no-op.
///
template <class Container>
using back_insert_iterator = basic_insert_iterator_detail::
    basic_insert_iterator<Container, back_insert_iterator_detail::adaptor_mixin<Container>>;

// parasoft-end-suppress AUTOSAR-M14_5_3-a
// parasoft-end-suppress AUTOSAR-A14_5_1-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief creates a @c std::back_insert_iterator of type inferred from the
///     argument
/// @tparam Container container type to insert into
/// @param container container to insert into
///
/// @c std::back_inserter is a convenience function template that constructs a
/// @c std::back_insert_iterator for the container @c container with the type
/// deduced from the type of the argument.
///
/// @return a @c std::back_insert_iterator which can be used to add elements to
///     the end of the container @c container.
///
template <class Container>
constexpr auto back_inserter(Container& container) noexcept -> back_insert_iterator<Container> {
  return back_insert_iterator<Container>{container};
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

namespace insert_iterator_detail {

/// @brief mixin used to define @c insert_iterator
/// @tparam Container associated container type
///
template <class Container>
class adaptor_mixin {
  static_assert(
      basic_insert_iterator_detail::has_value_type_member_v<Container>,
      "'Container' does not satisfy syntactic requirements"
  );

  static_assert(
      basic_insert_iterator_detail::has_iterator_member_v<Container>,
      "'Container' does not satisfy syntactic requirements"
  );

  /// @brief base iterator type
  ///
  using iterator_type = typename Container::iterator;

 protected:
  // Naming and access control for these members are specified by the standard.
  //
  // NOLINTBEGIN(misc-non-private-member-variables-in-classes,readability-identifier-naming)

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "Identifiers must be declared
  // 'protected' to be standard compliant"

  /// @brief associated container to insert into
  ///
  Container* container;

  /// @brief location to insert into
  ///
  iterator_type iter;

  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  // NOLINTEND(misc-non-private-member-variables-in-classes,readability-identifier-naming)

  /// @brief insert an element into the associated container
  /// @tparam T cv-ref qualified container value type
  /// @param value value to insert
  ///
  template <class T>
  constexpr auto insert(T&& value) noexcept(noexcept(
      std::declval<iterator_type&>() = container->insert(std::declval<iterator_type&>(), std::forward<T>(value)),
      ++std::declval<iterator_type&>()
  )) -> void {
    static_assert(
        std::is_same_v<typename Container::value_type, std::remove_cv_t<std::remove_reference_t<T>>>,
        "this internal function is only intended for use with cv-ref qualified container value types."
    );
    iter = container->insert(iter, std::forward<T>(value));
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
    static_cast<void>(++iter);
    // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
  }

 public:
  /// @brief constructs an @c adaptor_mixin
  /// @param cont container to insert into
  /// @param pos iterator in @c cont specifying the insertion position
  ///
  constexpr adaptor_mixin(Container& cont, typename Container::iterator pos) noexcept
      : container{std::addressof(cont)},
        iter{std::move(pos)} {}
};

}  // namespace insert_iterator_detail

/// @brief iterator adaptor for insertion into a container
/// @tparam Container associated container type
///
/// @c std::insert_iterator is an OutputIterator that inserts elements into a
/// container for which it was constructed, at the position pointed to by the
/// supplied iterator. The container's @c insert() member function is called
/// whenever the iterator (whether dereferenced or not) is assigned to.
/// Incrementing the @c std::insert_iterator is a no-op.
///
/// @note Inserting multiple elements with @c std::insert_iterator places the
///     earliest elements first.
///
template <class Container>
using insert_iterator =
    basic_insert_iterator_detail::basic_insert_iterator<Container, insert_iterator_detail::adaptor_mixin<Container>>;

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief creates an @c std::insert_iterator of type inferred from the
///     argument
/// @tparam Container container type to insert into
/// @param container container to insert into
/// @param iter iterator in @c container specifying the insertion position
///
/// @c std::inserter is a convenience function template that constructs an @c
/// std::insert_iterator for the container @c container and its iterator @c iter
/// with the type deduced from the type of the @c container argument.
///
/// @return a @c std::insert_iterator which can be used to insert elements into
///     the container @c container at the position specified by @c iter.
///
template <class Container>
constexpr auto inserter(
    Container& container,
    typename Container::iterator iter
) noexcept(std::is_nothrow_move_constructible_v<typename Container::iterator>) -> insert_iterator<Container> {
  return {container, std::move(iter)};
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BASIC_INSERT_ITERATOR_HPP_
