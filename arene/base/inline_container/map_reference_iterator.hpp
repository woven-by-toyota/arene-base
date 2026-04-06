// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "inline_container/map_reference_iterator.hpp is clear enough for
// inline_map_reference_iterator"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_REFERENCE_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_REFERENCE_ITERATOR_HPP_

// IWYU pragma: private, include "arene/base/inline_container/map_reference.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/inline_container/detail/iterator_interface.hpp"
#include "arene/base/inline_container/map.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace inline_map_reference_iterator_detail {

/// @brief poison pill declaration
auto value_at_index() -> void;

}  // namespace inline_map_reference_iterator_detail

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: template
// constructors do not participate in overload resolution for copy/move
// construction"

// parasoft-begin-suppress AUTOSAR-A10_1_1-a "False positive:
// inline_map_reference_iterator does not inherit from more than one class that
// is not an interface"

// parasoft-begin-suppress AUTOSAR-A12_1_5-a "Delegating constructors would not reduce duplication"

/// @brief iterator type used by @c inline_map_reference and
///   @c const_inline_map_reference
///
/// @tparam MapBase base type of an @c inline_map
/// @tparam IsConst specifies if this iterator provides const or non-const
///     access
///
template <class MapBase, bool IsConst>
class inline_map_reference_iterator
    : public inline_container_detail::iterator_interface<inline_map_reference_iterator<MapBase, IsConst>>
    , inline_map_detail::inline_map_passkey_base<MapBase> {
  /// @brief alias to @c iterator_interface base type
  ///
  using iterator_interface = inline_container_detail::iterator_interface<inline_map_reference_iterator>;

  /// @brief add @c const if @c IsConst is @c true
  /// @tparam T type to possible add @c const to
  /// @tparam B predicate to use to apply @c const, defaults to @c IsConst
  ///
  template <class T, bool B = IsConst>
  using maybe_const_t = std::conditional_t<B, T const, T>;

  /// @brief alias to the @c inline_map passkey type
  using inline_map_passkey = typename inline_map_detail::inline_map_passkey_base<MapBase>::inline_map_passkey;

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Friendship allows conversion
  // from non-const iterator to const iterator without exposing implementation
  // details."
  friend inline_map_reference_iterator<MapBase, true>;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

 public:
  /// @brief iterator category
  using iterator_category = std::bidirectional_iterator_tag;
  /// @brief iterator value type
  using value_type = typename MapBase::value_type;
  /// @brief iterator difference type
  using difference_type = std::ptrdiff_t;
  /// @brief pointer type
  using pointer = maybe_const_t<value_type>*;
  /// @brief reference type
  using reference = maybe_const_t<value_type>&;

  /// @brief @c inline_map base type
  using map_base_type = MapBase;

 private:
  /// @brief determine if an iterator is a non-const iterator
  /// @tparam I iterator type
  ///
  template <class I>
  static constexpr bool is_const_iterator_v{
      std::is_const<std::remove_reference_t<typename std::iterator_traits<I>::reference>>::value
  };

  /// @brief index type of this iterator
  /// @note set to the maximum possible @c index_type of any @c inline_map
  /// @note @c inline_map can never have an @c index_type larger than 16 bits
  ///
  using index_type = std::uint16_t;

  // parasoft-begin-suppress CERT_C-EXP37-b "False positive: The rule does not mention naming all parameters"

  /// @brief function pointer type for indexing into an @c inline_map
  /// @tparam B determines if the function pointer type is assocated with the
  ///   const or non-const iterator
  ///
  template <bool B>
  using erased_index_fn_type = auto (*)(maybe_const_t<map_base_type, B>*, index_type) -> maybe_const_t<value_type, B>&;

  // parasoft-end-suppress CERT_C-EXP37-b

  /// @brief function to index into a non-const @c inline_map
  ///
  /// @note While only one of these functions is used by @c do_deref for a
  ///   specific instance of this type, there must be some way to change the
  ///   function pointer when converting from a non-const iterator to a const
  ///   iterator. This implementation stores both functions.
  ///
  erased_index_fn_type<false> non_const_erased_index_;

  /// @brief function to index into a const @c inline_map
  ///
  /// @note While only one of these functions is used by @c do_deref for a
  ///   specific instance of this type, there must be some way to change the
  ///   function pointer when converting from a non-const iterator to a const
  ///   iterator. This implementation stores both functions.
  ///
  erased_index_fn_type<true> const_erased_index_;

  /// @brief pointer to the capacity-erased base of @c inline_map
  ///
  maybe_const_t<map_base_type>* map_;

  /// @brief iterator position in the map
  ///
  index_type pos_;

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Use of a friend function with
  // passkey improves encapsulation. It allows data members to remain private
  // and does not unnecessarily expose a public member function."
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A7_1_1-a	"Declaring 'iter' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'iter' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a	"Declaring 'iter' as reference to const changes semantics"

  /// @brief basis function for increment
  /// @param iter iterator to increment
  ///
  friend auto
  step_forward(inline_container_detail::iterator_interface_tag, inline_map_reference_iterator& iter) noexcept -> void {
    ++iter.pos_;
  }

  /// @brief basis function for decrement
  /// @param iter iterator to decrement
  ///
  friend auto
  step_backward(inline_container_detail::iterator_interface_tag, inline_map_reference_iterator& iter) noexcept -> void {
    --iter.pos_;
  }

  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A7_1_1-a
  // parasoft-end-suppress AUTOSAR-M7_1_2-c
  // parasoft-end-suppress AUTOSAR-A8_4_9-a

  /// @brief perform iterator dereference
  /// @tparam True deduced from tag argument, this function is disabled if
  ///   @c True is not @c true or @c IsConst is not @c true
  /// @return reference to the indirect value represented by this iterator
  ///
  template <bool True, constraints<std::enable_if_t<IsConst && True>> = nullptr>
  auto do_deref(std::integral_constant<bool, True>) const noexcept -> reference {
    return const_erased_index_(map_, pos_);
  }

  /// @brief perform iterator dereference
  /// @tparam False deduced from tag argument, this function is disabled if
  ///   @c False is not @c false or @c IsConst is not @c false
  /// @return reference to the indirect value represented by this iterator
  ///
  template <bool False, constraints<std::enable_if_t<!IsConst && !False>> = nullptr>
  auto do_deref(std::integral_constant<bool, False>) const noexcept -> reference {
    return non_const_erased_index_(map_, pos_);
  }

  /// @brief obtains a value in a capacity-erased map
  /// @tparam B switches between const and non-const access
  /// @tparam Map the @c const qualified @c inline_map type
  /// @param map pointer to capacity-erased @c inline_map
  /// @param index index in @c inline_map
  /// @return reference to the value at @c index in @c map
  ///
  template <bool B, class Map>
  static auto value_in_map(maybe_const_t<MapBase, B>* map, index_type index) noexcept -> maybe_const_t<value_type, B>& {
    using ::arene::base::inline_map_reference_iterator_detail::value_at_index;
    return value_at_index(inline_map_passkey{}, *static_cast<maybe_const_t<Map, B>*>(map), index);
  }

 public:
  /// @brief default construct an invalid iterator
  ///
  inline_map_reference_iterator() noexcept
      : iterator_interface{},
        non_const_erased_index_{nullptr},
        const_erased_index_{nullptr},
        map_{nullptr},
        pos_{} {}

  /// @brief construct from an @c inline_map iterator type
  /// @tparam I @c inline_map iterator type
  /// @param itr base iterator
  ///
  template <
      class I,
      constraints<std::enable_if_t<
          std::is_base_of<MapBase, typename I::inline_map_type>::value && (IsConst || (!is_const_iterator_v<I>))>> =
          nullptr>
  explicit inline_map_reference_iterator(I const& itr) noexcept
      : iterator_interface{},
        non_const_erased_index_{&value_in_map<false, typename I::inline_map_type>},
        const_erased_index_{&value_in_map<true, typename I::inline_map_type>},
        map_{itr.get_map(inline_map_passkey{})},
        pos_{itr.get_index(inline_map_passkey{})} {
    constexpr auto capacity = I::inline_map_type::capacity;

    // parasoft-begin-suppress CERT_C-PRE31-c "False positive: numeric_limits::max() is constexpr and has no side
    // effects"
    static_assert(
        capacity <= std::numeric_limits<std::uint16_t>::max(),
        "'index_type' is defined based on an upper limit for 'Capacity' which is no longer valid"
    );
    // parasoft-end-suppress CERT_C-PRE31-c
  }

  /// @brief implictly construct from a non-const @c inline_map_reference_iterator
  /// @tparam OtherConst specifies if the other iterator is @c const
  /// @param other iterator
  ///
  template <bool OtherConst, constraints<std::enable_if_t<IsConst && !OtherConst>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  inline_map_reference_iterator(inline_map_reference_iterator<MapBase, OtherConst> const& other) noexcept
      : iterator_interface{},
        non_const_erased_index_{other.non_const_erased_index_},
        const_erased_index_{other.const_erased_index_},
        map_{other.map_},
        pos_{other.pos_} {}

  /// @brief dereference
  /// @return value in the associated @c inline_map
  ///
  auto operator*() const noexcept -> reference { return do_deref(std::integral_constant<bool, IsConst>{}); }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
  // translation units"

  /// @brief equality
  /// @param lhs left iterator
  /// @param rhs right iterator
  /// @return @c true if iterators are equal, otherwise @c false
  ///
  friend auto operator==(inline_map_reference_iterator const& lhs, inline_map_reference_iterator const& rhs) noexcept
      -> bool {
    return lhs.pos_ == rhs.pos_;
  }

  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A0_1_3-a

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not
  // mention naming all parameters"

  /// @brief obtain a pointer to the map that this iterator references
  /// @return pointer to the referenced map
  //
  auto get_map(inline_map_passkey) const noexcept -> maybe_const_t<map_base_type>* { return map_; }

  /// @brief obtain the position that this iterator references within the map
  /// @return index position of this iterator
  ///
  auto get_index(inline_map_passkey) const noexcept -> index_type { return pos_; }

  // parasoft-end-suppress CERT_C-EXP37-a
};

// parasoft-end-suppress AUTOSAR-A14_5_1-a
// parasoft-end-suppress AUTOSAR-A10_1_1-a
// parasoft-end-suppress AUTOSAR-A12_1_5-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_REFERENCE_ITERATOR_HPP_
