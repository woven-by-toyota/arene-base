// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "inline_container/map_reference.hpp is clear enough for
// inline_map_reference and const_inline_map_reference"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_REFERENCE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/functional/function_traits.hpp"
#include "arene/base/inline_container/detail/compare.hpp"
#include "arene/base/inline_container/detail/container_base_type.hpp"
#include "arene/base/inline_container/detail/erased_member_function.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/detail/range_interface.hpp"
#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_container/map_reference_iterator.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/type_manipulation/smallest_integer_for.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-b "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace inline_map_reference_detail {

/// @brief determine if a comparison is @c noexcept
/// @tparam Key key type of a map
/// @tparam Compare compare type of a map
///
template <class Key, class Compare>
extern constexpr bool comparison_is_noexcept_v{inline_container::detail::compare_wrapper<Compare, Key>::
                                                   template comparison_is_noexcept<Key>};
/// @brief alias for a non-const iterator
/// @tparam MapBase associated @c inline_map_base type
///
template <class MapBase>
using iterator = inline_map_reference_iterator<MapBase, false>;

/// @brief alias for a const iterator
/// @tparam MapBase associated @c inline_map_base type
///
template <class MapBase>
using const_iterator = inline_map_reference_iterator<MapBase, true>;

/// @brief function pointers to dispatch to const member functions
/// @tparam MapBase associated @c inline_map_base type
///
template <class MapBase>
struct const_operations {
  /// @brief type of the capacity-erased function
  /// @tparam R function return type
  /// @tparam Ts function argument types, excluding the map base
  ///
  template <class R, class... Ts>
  using erased_op_type = auto (*)(MapBase const*, Ts...) -> R;

  /// @brief The function pointer of the key_comp operation
  erased_op_type<typename MapBase::compare_type> key_comp;
  /// @brief The function pointer of the size operation
  erased_op_type<typename MapBase::size_type> size;
  /// @brief The function pointer of the capacity operation
  erased_op_type<typename MapBase::size_type> capacity;
  /// @brief The function pointer of the capacity operation
  erased_op_type<typename MapBase::size_type> max_size;
  /// @brief The function pointer of the find operation
  erased_op_type<const_iterator<MapBase>, typename MapBase::key_type const&> find;
  /// @brief The function pointer of the contains operation
  erased_op_type<bool, typename MapBase::key_type const&> contains;
  /// @brief The function pointer of the count operation
  erased_op_type<typename MapBase::size_type, typename MapBase::key_type const&> count;
  /// @brief The function pointer of the lower_bound operation
  erased_op_type<const_iterator<MapBase>, typename MapBase::key_type const&> lower_bound;
  /// @brief The function pointer of the upper_bound operation
  erased_op_type<const_iterator<MapBase>, typename MapBase::key_type const&> upper_bound;
  /// @brief The function pointer of the equal_range operation
  erased_op_type<std::pair<const_iterator<MapBase>, const_iterator<MapBase>>, typename MapBase::key_type const&>
      equal_range;
  /// @brief The function pointer of the at operation
  erased_op_type<typename MapBase::mapped_type const&, typename MapBase::key_type const&> at;
  /// @brief The function pointer of the begin operation
  erased_op_type<const_iterator<MapBase>> begin;
  /// @brief The function pointer of the end operation
  erased_op_type<const_iterator<MapBase>> end;
};

/// @brief function pointers to dispatch to non-const member functions
/// @tparam MapBase associated @c inline_map_base type
///
template <class MapBase>
struct operations {
  /// @brief type of the capacity-erased function
  /// @tparam R function return type
  /// @tparam Ts function argument types, excluding the map base
  ///
  template <class R, class... Ts>
  using erased_op_type = auto (*)(MapBase*, Ts...) -> R;

  /// @brief pointer to the const operations associated with this set of
  ///   non-const operations
  const_operations<MapBase> const* const_ops;
  /// @brief The function pointer of the find operation
  erased_op_type<iterator<MapBase>, typename MapBase::key_type const&> find;
  /// @brief The function pointer of the lower_bound operation
  erased_op_type<iterator<MapBase>, typename MapBase::key_type const&> lower_bound;
  /// @brief The function pointer of the upper_bound operation
  erased_op_type<iterator<MapBase>, typename MapBase::key_type const&> upper_bound;
  /// @brief The function pointer of the equal_range operation
  erased_op_type<std::pair<iterator<MapBase>, iterator<MapBase>>, typename MapBase::key_type const&> equal_range;
  /// @brief The function pointer of the operator[] operation
  erased_op_type<typename MapBase::mapped_type&, typename MapBase::key_type const&> operator_index;
  /// @brief The function pointer of the at operation
  erased_op_type<typename MapBase::mapped_type&, typename MapBase::key_type const&> at;
  /// @brief The function pointer of the insert operation
  erased_op_type<std::pair<iterator<MapBase>, bool>, typename MapBase::value_type const&> insert;
  /// @brief The function pointer of the insert_or_assign operation
  erased_op_type<
      std::pair<iterator<MapBase>, bool>,
      typename MapBase::key_type const&,
      typename MapBase::mapped_type const&>
      insert_or_assign;
  /// @brief The function pointer of the erase operation, key overload
  erased_op_type<iterator<MapBase>, typename MapBase::key_type const&> erase_key;
  /// @brief The function pointer of the erase operation, iterator overload
  erased_op_type<iterator<MapBase>, const_iterator<MapBase>> erase_pos;
  /// @brief The function pointer of the erase operation, range overload
  erased_op_type<iterator<MapBase>, const_iterator<MapBase>, const_iterator<MapBase>> erase_rng;
  /// @brief The function pointer of the clear operation
  erased_op_type<void> clear;
  /// @brief The function pointer of the begin operation
  erased_op_type<iterator<MapBase>> begin;
  /// @brief The function pointer of the end operation
  erased_op_type<iterator<MapBase>> end;
};

/// @brief "alias" to a capacity-erased member function helper
/// @tparam MemFn member function pointer of the non-erased container type
/// @tparam Func pointer to the member function
/// @tparam Return return type of this helper, defaults to @c member_function_return_t<MemFn>
///
template <class MemFn, MemFn Func, class Return = member_function_return_t<MemFn>>
extern constexpr auto erased_member_function = inline_container::detail::erased_member_function<MemFn, Func, Return>;

/// @brief capacity-erased function that returns a compile-time constant
/// @tparam Map The associated @c inline_map type (used to deduce @c MapBase)
/// @tparam T the type of the constant
/// @tparam Value the compile-time constant to return
/// @tparam MapBase deduced template param used to simplify definition
/// @return Value
template <class Map, class T, T Value, class MapBase = inline_container::detail::container_base_type_t<Map>>
auto erased_integral_constant_function(MapBase const*) noexcept -> T {
  return Value;
}

/// @brief function pointers to dispatch to const member functions
/// @tparam Map The associated @c inline_map type
/// @tparam MapBase deduced template param used to simplify definition
/// @tparam ComparisonIsNoexcept deduced template param used to simplify definition
///
/// Collection of member function pointers for a given instance of @c
/// inline_map. Member function pointers are transformed to replace the implicit
/// @c this parameter with a pointer to @c MapBase. Member function pointers
/// that return an @c inline_map specific iterator type are also transformed to
/// return a @c map_reference_iterator instead.
///
/// @note Older versions of GCC (e.g. GCC8) require the exact @c noexcept
///   qualifier for the pointer to member function type. More recent versions
///   may omit the @c noexcept qualifier and rely on an implicit conversion of
///   a @c noexcept(true) function to a @c noexcept(false) function.
///
template <
    class Map,
    class MapBase = inline_container::detail::container_base_type_t<Map>,
    bool ComparisonIsNoexcept = comparison_is_noexcept_v<typename Map::key_type, typename Map::compare_type>>
extern constexpr auto const_map_operations_with_at = const_operations<MapBase>{
    erased_member_function<decltype(&Map::key_comp), &Map::key_comp>,
    erased_member_function<decltype(&Map::size), &Map::size>,
    &erased_integral_constant_function<Map, std::size_t, Map::capacity>,
    &erased_integral_constant_function<Map, std::size_t, Map::max_size>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::const_iterator,
        &Map::find,
        const_iterator<MapBase>>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->bool,
        &Map::contains>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::size_type,
        &Map::count>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::const_iterator,
        &Map::lower_bound,
        const_iterator<MapBase>>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::const_iterator,
        &Map::upper_bound,
        const_iterator<MapBase>>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)
            ->std::pair<typename Map::const_iterator, typename Map::const_iterator>,
        &Map::equal_range,
        std::pair<const_iterator<MapBase>, const_iterator<MapBase>>>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(false)->typename Map::mapped_type const&,
        &Map::at>,
    erased_member_function<decltype(&Map::cbegin), &Map::begin, const_iterator<MapBase>>,
    erased_member_function<decltype(&Map::cend), &Map::end, const_iterator<MapBase>>
};

/// @brief function pointers to dispatch to const member functions
/// @tparam Map The associated @c inline_map type
/// @tparam MapBase deduced template param used to simplify definition
/// @tparam ComparisonIsNoexcept deduced template param used to simplify definition
///
/// Collection of member function pointers for a given instance of @c
/// inline_map. Member function pointers are transformed to replace the implicit
/// @c this parameter with a pointer to @c MapBase. Member function pointers
/// that return an @c inline_map specific iterator type are also transformed to
/// return a @c map_reference_iterator instead.
///
/// @note Older versions of GCC (e.g. GCC8) require the exact @c noexcept
///   qualifier for the pointer to member function type. More recent versions
///   may omit the @c noexcept qualifier and rely on an implicit conversion of
///   a @c noexcept(true) function to a @c noexcept(false) function.
///
template <
    class Map,
    class MapBase = inline_container::detail::container_base_type_t<Map>,
    bool ComparisonIsNoexcept = comparison_is_noexcept_v<typename Map::key_type, typename Map::compare_type>>
extern constexpr auto const_map_operations_without_at = const_operations<MapBase>{
    erased_member_function<decltype(&Map::key_comp), &Map::key_comp>,
    erased_member_function<decltype(&Map::size), &Map::size>,
    &erased_integral_constant_function<Map, std::size_t, Map::capacity>,
    &erased_integral_constant_function<Map, std::size_t, Map::max_size>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::const_iterator,
        &Map::find,
        const_iterator<MapBase>>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->bool,
        &Map::contains>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::size_type,
        &Map::count>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::const_iterator,
        &Map::lower_bound,
        const_iterator<MapBase>>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)->typename Map::const_iterator,
        &Map::upper_bound,
        const_iterator<MapBase>>,
    erased_member_function<
        auto (Map::*)(typename Map::key_type const&) const noexcept(ComparisonIsNoexcept)
            ->std::pair<typename Map::const_iterator, typename Map::const_iterator>,
        &Map::equal_range,
        std::pair<const_iterator<MapBase>, const_iterator<MapBase>>>,
    nullptr,  // this is &Map::at, which does not exist (and is removed from map_reference by SFINAE)
    erased_member_function<decltype(&Map::cbegin), &Map::begin, const_iterator<MapBase>>,
    erased_member_function<decltype(&Map::cend), &Map::end, const_iterator<MapBase>>
};

/// @brief capacity-erased helper for @c inline_map::erase overloads
/// @tparam Map The associated @c inline_map type
/// @tparam MapBase deduced template param used to simplify definition
///
/// Helper type that provides functions to call @c inline_map::erase with
/// iterator arguments.
///
template <class Map, class MapBase = inline_container::detail::container_base_type_t<Map>>
class erase_fn_iterator_argument_adaptor : inline_map_detail::inline_map_passkey_base<MapBase> {
  /// @brief convert a capacity-erased iterator to an @c inline_map::const_iterator
  /// @param iter capacity-erased iterator
  /// @return @c inline_map::const_iterator to the same position in the map
  ///
  static auto restored(const_iterator<MapBase> iter) noexcept -> typename Map::const_iterator {
    using index_type = smallest_unsigned_integer_for<Map::capacity>;

    constexpr auto passkey = typename inline_map_detail::inline_map_passkey_base<MapBase>::inline_map_passkey{};

    return typename Map::const_iterator{
        passkey,
        static_cast<Map const*>(iter.get_map(passkey)),
        static_cast<index_type>(iter.get_index(passkey))
    };
  }

  /// @brief obtain the associated @c inline_map
  /// @param map pointer to a map base
  /// @return reference to an @c inline_map
  ///
  static auto derived(MapBase* map) noexcept -> Map& { return *static_cast<Map*>(map); }

 public:
  /// @brief erase an element in a map
  /// @param map pointer to a map base
  /// @param pos capacity-erased iterator specifying a position in @c map
  /// @pre @c pos is a dereferencable iterator of @c map
  /// @return capacity-erased iterator specifying the position after the removed
  ///   element
  ///
  static auto one_argument_overload(MapBase* map, const_iterator<MapBase> pos) noexcept -> iterator<MapBase> {
    return iterator<MapBase>{derived(map).erase(restored(pos))};
  }

  /// @brief erase elements in a map
  /// @param map pointer to a map base
  /// @param first beginning of the range of elements to remove
  /// @param last end of the range of elements to remove
  /// @pre <tt> [first, last) </tt> is a valid range in @c map
  /// @return capacity-erased iterator specifying the position after the removed
  ///   elements
  ///
  static auto two_argument_overload(MapBase* map, const_iterator<MapBase> first, const_iterator<MapBase> last) noexcept
      -> iterator<MapBase> {
    return iterator<MapBase>{derived(map).erase(restored(first), restored(last))};
  }
};

/// @brief function pointers to dispatch to non-const member functions
/// @tparam Map The associated @c inline_map type
/// @tparam MapBase deduced template param used to simplify definition
/// @tparam ComparisonIsNoexcept deduced template param used to simplify definition
///
/// Collection of member function pointers for a given instance of @c
/// inline_map. Member function pointers are transformed to replace the implicit
/// @c this parameter with a pointer to @c MapBase. Member function pointers
/// that return an @c inline_map specific iterator type are also transformed to
/// return a @c map_reference_iterator instead.
///
/// @note Older versions of GCC (e.g. GCC8) require the exact @c noexcept
///   qualifier for the pointer to member function type. More recent versions
///   may omit the @c noexcept qualifier and rely on an implicit conversion of
///   a @c noexcept(true) function to a @c noexcept(false) function.
///
template <
    class Map,
    class MapBase = inline_container::detail::container_base_type_t<Map>,
    bool ComparisonIsNoexcept = comparison_is_noexcept_v<typename Map::key_type, typename Map::compare_type>>
extern constexpr auto map_operations_with_at = operations<MapBase> {
  &const_map_operations_with_at<Map>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::find,
      iterator<MapBase>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::lower_bound,
      iterator<MapBase>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::upper_bound,
      iterator<MapBase>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)
          ->std::pair<typename Map::iterator, typename Map::iterator>,
      &Map::equal_range,
      std::pair<iterator<MapBase>, iterator<MapBase>>>,
  erased_member_function < auto (Map::*)(typename Map::key_type const&) noexcept(false)->typename Map::mapped_type&,
  &Map::operator[]>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(false)->typename Map::mapped_type&,
      &Map::at>,
  erased_member_function<
      auto (Map::*)(typename Map::value_type const&) noexcept(false)->std::pair<typename Map::iterator, bool>,
      &Map::insert,
      std::pair<iterator<MapBase>, bool>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&, typename Map::mapped_type const&) noexcept(false)
          ->std::pair<typename Map::iterator, bool>,
      &Map::insert_or_assign,
      std::pair<iterator<MapBase>, bool>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::erase,
      iterator<MapBase>>,
  &erase_fn_iterator_argument_adaptor<Map>::one_argument_overload,
  &erase_fn_iterator_argument_adaptor<Map>::two_argument_overload,
  erased_member_function<decltype(&Map::clear), &Map::clear>,
  erased_member_function<auto (Map::*)() noexcept -> typename Map::iterator, &Map::begin, iterator<MapBase>>,
  erased_member_function<auto (Map::*)() noexcept -> typename Map::iterator, &Map::end, iterator<MapBase>>
};

/// @brief function pointers to dispatch to non-const member functions
/// @tparam Map The associated @c inline_map type
/// @tparam MapBase deduced template param used to simplify definition
/// @tparam ComparisonIsNoexcept deduced template param used to simplify definition
///
/// Collection of member function pointers for a given instance of @c
/// inline_map. Member function pointers are transformed to replace the implicit
/// @c this parameter with a pointer to @c MapBase. Member function pointers
/// that return an @c inline_map specific iterator type are also transformed to
/// return a @c map_reference_iterator instead.
///
/// @note Older versions of GCC (e.g. GCC8) require the exact @c noexcept
///   qualifier for the pointer to member function type. More recent versions
///   may omit the @c noexcept qualifier and rely on an implicit conversion of
///   a @c noexcept(true) function to a @c noexcept(false) function.
///
template <
    class Map,
    class MapBase = inline_container::detail::container_base_type_t<Map>,
    bool ComparisonIsNoexcept = comparison_is_noexcept_v<typename Map::key_type, typename Map::compare_type>>
extern constexpr auto map_operations_without_at = operations<MapBase> {
  &const_map_operations_without_at<Map>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::find,
      iterator<MapBase>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::lower_bound,
      iterator<MapBase>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::upper_bound,
      iterator<MapBase>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)
          ->std::pair<typename Map::iterator, typename Map::iterator>,
      &Map::equal_range,
      std::pair<iterator<MapBase>, iterator<MapBase>>>,
  erased_member_function < auto (Map::*)(typename Map::key_type const&) noexcept(false)->typename Map::mapped_type&,
  &Map::operator[]>,
  nullptr,  // this is &Map::at, which does not exist (and is removed from map_reference by SFINAE)
  erased_member_function<
      auto (Map::*)(typename Map::value_type const&) noexcept(false)->std::pair<typename Map::iterator, bool>,
      &Map::insert,
      std::pair<iterator<MapBase>, bool>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&, typename Map::mapped_type const&) noexcept(false)
          ->std::pair<typename Map::iterator, bool>,
      &Map::insert_or_assign,
      std::pair<iterator<MapBase>, bool>>,
  erased_member_function<
      auto (Map::*)(typename Map::key_type const&) noexcept(ComparisonIsNoexcept)->typename Map::iterator,
      &Map::erase,
      iterator<MapBase>>,
  &erase_fn_iterator_argument_adaptor<Map>::one_argument_overload,
  &erase_fn_iterator_argument_adaptor<Map>::two_argument_overload,
  erased_member_function<decltype(&Map::clear), &Map::clear>,
  erased_member_function<auto (Map::*)() noexcept -> typename Map::iterator, &Map::begin, iterator<MapBase>>,
  erased_member_function<auto (Map::*)() noexcept -> typename Map::iterator, &Map::end, iterator<MapBase>>
};

/// @brief CRTP helper providing const member functions
/// @tparam Derived @c const_inline_map_reference or @c inline_map_reference
///
template <class Derived>
class const_inline_map_reference_interface {
  /// @brief downcast
  /// @return reference to @c**this as the derived type
  ///
  auto derived() const noexcept -> Derived const& { return static_cast<Derived const&>(*this); }

  /// @brief obtain a pointer to the capacity-erased map
  /// @tparam D type that is always intended to be @c Derived
  /// @return map base type pointer
  ///
  template <class D>
  auto map() const noexcept -> typename D::map_base_type const* {
    return derived().map_ptr();
  }

  /// @brief obtain the capacity-erased operations
  /// @tparam D type that is always intended to be @c Derived
  /// @return reference to capacity-erased operations
  ///
  template <class D>
  auto ops() const noexcept -> const_operations<typename D::map_base_type> const& {
    return *derived().const_ops_ptr();
  }

  /// @brief determine if comparison is @c noexcept
  /// @tparam D type that is always intended to be @c Derived
  ///
  template <class D>
  static constexpr bool comparison_is_noexcept{inline_map_reference_detail::comparison_is_noexcept_v<
      typename D::key_type,
      typename D::compare_type>};

 protected:
  /// @brief constructor
  /// @tparam Actual tag type, intended to always be @c Derived
  /// @tparam Expected deduced template parameter, defined to improve compiler
  ///   error messages
  ///
  /// Construct a 'const_inline_map_reference_interface' CRTP helper type. Intended to be invoked as:
  /// ~~~{.cpp}
  /// const_inline_map_reference_interface{this}
  /// ~~~
  ///
  /// This constructor exists to prevent use by a type that does _not_ define
  /// 'const_inline_map_reference_interface' as a CRTP base class or to detect
  /// mismatch between the actual derived type and the @c Derived template
  /// parameter.
  ///
  template <class Actual, class Expected = Derived>
  explicit const_inline_map_reference_interface(Actual*) noexcept {
    static_assert(
        std::is_same<Actual, Expected>::value,
        "'Derived' type mismatch.\n"  //
        "Please verify that 'Derived' is defined correctly when using 'const_inline_map_reference_interface' as a CRTP "
        "base."
    );
  }
  /// @brief default destructor
  ~const_inline_map_reference_interface() = default;
  /// @brief default copy constructor
  const_inline_map_reference_interface(const_inline_map_reference_interface const&) = default;
  /// @brief default move constructor
  const_inline_map_reference_interface(const_inline_map_reference_interface&&) = default;
  /// @brief default copy assignment operator
  auto operator=(const_inline_map_reference_interface const&) -> const_inline_map_reference_interface& = default;
  /// @brief default move assignment operator
  auto operator=(const_inline_map_reference_interface&&) -> const_inline_map_reference_interface& = default;

 public:
  /// @brief Obtain a copy of the comparator
  /// @return The comparator
  template <class D = Derived>
  auto key_comp() const noexcept(std::is_nothrow_copy_constructible<typename D::compare_type>::value)
      -> decltype(ops<D>().key_comp(map<D>())) {
    return ops<D>().key_comp(map<D>());
  }

  /// @brief Get the current number of elements in the map
  /// @return the size
  template <class D = Derived>
  auto size() const noexcept -> decltype(ops<D>().size(map<D>())) {
    return ops<D>().size(map<D>());
  }

  /// @brief Get the element capacity of the map
  /// @return the capacity
  template <class D = Derived>
  auto capacity() const noexcept -> decltype(ops<D>().capacity(map<D>())) {
    return ops<D>().capacity(map<D>());
  }

  /// @brief Get the max size of the map
  /// @return the max size
  template <class D = Derived>
  auto max_size() const noexcept -> decltype(ops<D>().max_size(map<D>())) {
    return ops<D>().max_size(map<D>());
  }

  /// @brief Obtain an iterator referring to the element equivalent to the supplied key, if there is one.
  /// @param key The key to find
  /// @return iterator to the equivalent element, or @c end() if the element is not found
  template <class D = Derived>
  auto find(typename D::key_type const& key) const noexcept(comparison_is_noexcept<D>)
      -> decltype(ops<D>().find(map<D>(), key)) {
    return ops<D>().find(map<D>(), key);
  }

  /// @brief Check if the set contains a specific key
  /// @param key The key to search for
  /// @return true if @c find(key)!=end() .
  /// @return false if @c find(key)==end() .
  template <class D = Derived>
  auto contains(typename D::key_type const& key) const noexcept(comparison_is_noexcept<D>)
      -> decltype(ops<D>().contains(map<D>(), key)) {
    return ops<D>().contains(map<D>(), key);
  }

  /// @brief Return the number of elements in the map equivalent to a specific key.
  ///
  /// @param key The key to search for.
  /// @return std::size_t @c 0 if the element was not in the map, else @c 1 .
  template <class D = Derived>
  auto count(typename D::key_type const& key) const noexcept(comparison_is_noexcept<D>)
      -> decltype(ops<D>().count(map<D>(), key)) {
    return ops<D>().count(map<D>(), key);
  }

  /// @brief Find the first element which is _not less than_ a given key.
  ///
  /// @param key The key to find the lower bound for.
  /// @return iterator to the first element which is _not less than_ @c key , or @c end() if there is no
  ///         such element.
  template <class D = Derived>
  auto lower_bound(typename D::key_type const& key) const noexcept(comparison_is_noexcept<D>)
      -> decltype(ops<D>().lower_bound(map<D>(), key)) {
    return ops<D>().lower_bound(map<D>(), key);
  }

  /// @brief Find the first element which is _not less than_ a given key.
  ///
  /// @param key The key to find the lower bound for.
  /// @return const_iterator to the first element which is _not less than_ @c key , or @c end() if there is no
  ///         such element.
  template <class D = Derived>
  auto upper_bound(typename D::key_type const& key) const noexcept(comparison_is_noexcept<D>)
      -> decltype(ops<D>().upper_bound(map<D>(), key)) {
    return ops<D>().upper_bound(map<D>(), key);
  }

  /// @brief Finds the sequence of elements whose keys compare equivalent to a given key.
  ///
  /// @param key The key to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all elements in the range @c [first,second)
  ///         have keys which compare equal to @c key if there was a key equivalent to @c key in the map. Otherwise both
  ///         iterators will be @c end() .
  template <class D = Derived>
  auto equal_range(typename D::key_type const& key) const noexcept(comparison_is_noexcept<D>)
      -> decltype(ops<D>().equal_range(map<D>(), key)) {
    return ops<D>().equal_range(map<D>(), key);
  }

  /// @brief Obtain a reference to the mapped value for the specified key or throw.
  ///
  /// @param key The key to obtain the value for
  /// @return Value& A reference to the mapped value
  /// @throws std::out_of_range if @c find(key)==end() .
  /// @throws Any exception thrown by the comparisons
  template <
      class D = Derived,
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  auto at(typename D::key_type const& key) const noexcept(false) -> decltype(ops<D>().at(map<D>(), key)) {
    return ops<D>().at(map<D>(), key);
  }

  /// @brief Obtain an iterator referring to the beginning of the sorted range of elements.
  /// @return iterator Points to the first element in the sequence, or @c end() if @c empty() .
  template <class D = Derived>
  auto begin() const noexcept -> decltype(ops<D>().begin(map<D>())) {
    return ops<D>().begin(map<D>());
  }

  /// @brief Obtain an iterator referring to one past the last element in the sorted range of elements.
  /// @return iterator An iterator pointing to one past the last element in the sequence.
  template <class D = Derived>
  auto end() const noexcept -> decltype(ops<D>().end(map<D>())) {
    return ops<D>().end(map<D>());
  }
};

}  // namespace inline_map_reference_detail

// parasoft-begin-suppress AUTOSAR-A10_2_1-a "Hiding members of the interface
// base (e.g. 'begin') does not 'defeat polymorphism by causing an object to
// behave differently depending on which interface is used to manipulate it.' No
// functions are virtual and this is no different than selecting a const or
// non-const member function based on the const-ness of an object."

// parasoft-begin-suppress AUTOSAR-A10_2_1-b "Hiding members of the interface
// base (e.g. 'begin') does not 'defeat polymorphism by causing an object to
// behave differently depending on which interface is used to manipulate it.' No
// functions are virtual and this is no different than selecting a const or
// non-const member function based on the const-ness of an object."

// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: there is no
// common class initialization betweeen two constructors where one is deleted"

// parasoft-begin-suppress AUTOSAR-A13_5_1-a "False positive:" rule specifies
// that a const overload of 'operator[]' is required if a non-const overload of
// 'operator[]' is defined. This class only defines a const overload."

// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: Only inherits from interface base classes"

/// @brief A reference class to an inline_map object, with size-erased type
/// @tparam Key The key type for the map
/// @tparam Value The mapped type for the map
/// @tparam Compare The comparison function
/// @pre The comparison must provide a strict ordering over the values of @c Key
///
template <class Key, class Value, class Compare>
class inline_map_reference
    : public inline_map_reference_detail::const_inline_map_reference_interface<
          inline_map_reference<Key, Value, Compare>>
    , public inline_container::detail::range_interface<inline_map_reference<Key, Value, Compare>> {
 public:
  /// @brief alias to the map base class
  using map_base_type = inline_map_detail::inline_map_base<Key, Value, Compare>;

  /// @brief alias to const operations type
  using const_ops_type = inline_map_reference_detail::const_operations<map_base_type>;

  /// @brief alias to non-const operations type
  using ops_type = inline_map_reference_detail::operations<map_base_type>;

  /// @brief key type of the map
  using key_type = Key;
  /// @brief mapped type of the map
  using mapped_type = Value;
  /// @brief value type of the map
  using value_type = std::pair<Key const, Value>;
  /// @brief size type of the map
  using size_type = typename map_base_type::size_type;
  /// @brief iterator type of a reference to the map
  using iterator = inline_map_reference_iterator<map_base_type, false>;
  /// @brief const iterator type of the underlying map
  using const_iterator = inline_map_reference_iterator<map_base_type, true>;
  /// @brief The type of an iterator that provides non- @c const access to the elements
  /// and iterates in reverse through the elements
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  /// @brief The type of an iterator that provides @c const access to the elements
  /// and iterates in reverse through the elements
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;
  /// @brief compare type of the map
  using compare_type = Compare;

 private:
  /// @brief alias to the interface class providing const member functions
  using interface_base = inline_map_reference_detail::const_inline_map_reference_interface<inline_map_reference>;

  /// @brief alias to the range interface class providing range member functions
  using range_base = inline_container::detail::range_interface<inline_map_reference<Key, Value, Compare>>;

  /// @brief pointer to the base class
  map_base_type* map_ptr_;

  /// @brief pointer to const operations
  ops_type const* ops_ptr_;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: identifier
  // 'comparison_is_noexcept' in the base type is not hidden, it is private"

  /// @brief determine if comparison is @c noexcept
  static constexpr bool comparison_is_noexcept{inline_container::detail::compare_wrapper<Compare, Key>::
                                                   template comparison_is_noexcept<Key>};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

 public:
  /// @brief obtain a pointer to the map base
  /// @return pointer to the map base
  ///
  auto map_ptr() const noexcept -> map_base_type* { return map_ptr_; }

  /// @brief obtain a pointer to the non-const operations
  /// @return pointer to the non-const operations
  ///
  auto ops_ptr() const noexcept -> ops_type const* { return ops_ptr_; }

  /// @brief obtain a pointer to the const operations
  /// @return pointer to the const operations
  ///
  auto const_ops_ptr() const noexcept -> const_ops_type const* { return ops_ptr()->const_ops; }

  /// @brief construct an @c inline_map_reference of a map
  /// @tparam Capacity capacity of the @c inline_map
  /// @param map @c inline_map to reference
  ///
  template <
      std::size_t Capacity,
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  explicit inline_map_reference(inline_map<Key, Value, Capacity, Compare>& map) noexcept
      : interface_base{this},
        range_base{this},
        map_ptr_{&map},
        ops_ptr_{&inline_map_reference_detail::map_operations_with_at<inline_map<Key, Value, Capacity, Compare>>} {}

  /// @brief construct an @c inline_map_reference of a map
  /// @tparam Capacity capacity of the @c inline_map
  /// @param map @c inline_map to reference
  ///
  template <
      std::size_t Capacity,
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<!AreExceptionsEnabled>> = nullptr>
  explicit inline_map_reference(inline_map<Key, Value, Capacity, Compare>& map) noexcept
      : interface_base{this},
        range_base{this},
        map_ptr_{&map},
        ops_ptr_{&inline_map_reference_detail::map_operations_without_at<inline_map<Key, Value, Capacity, Compare>>} {}

  /// @brief construct an @c inline_map_reference of a map
  /// @tparam Capacity capacity of the @c inline_map
  ///
  /// Deleted overload for rvalue references.
  ///
  template <std::size_t Capacity>
  explicit inline_map_reference(inline_map<Key, Value, Capacity, Compare> const&&) = delete;

  /// @brief obtain an iterator to the first element with a given key
  /// @param key the key to search for
  /// @return iterator to the first element equal to @c key, or @c end() if the
  ///   element is not found
  ///
  auto find(key_type const& key) const noexcept(comparison_is_noexcept) -> decltype(ops_ptr_->find(map_ptr_, key)) {
    return ops_ptr_->find(map_ptr_, key);
  }

  /// @brief find the first element which is _not less than_ a given key
  /// @param key the key to search for
  /// @return iterator to the first element which is _not less than_ @c key, or
  ///   @c end() if there is no such element
  ///
  auto lower_bound(key_type const& key) const noexcept(comparison_is_noexcept)
      -> decltype(ops_ptr_->lower_bound(map_ptr_, key)) {
    return ops_ptr_->lower_bound(map_ptr_, key);
  }

  /// @brief find the first element which is _greater than_ a given key
  /// @param key the key to search for
  /// @return iterator to the first element which is _greater than_ @c key, or
  ///   @c end() if there is no such element
  ///
  auto upper_bound(key_type const& key) const noexcept(comparison_is_noexcept)
      -> decltype(ops_ptr_->upper_bound(map_ptr_, key)) {
    return ops_ptr_->upper_bound(map_ptr_, key);
  }

  /// @brief find the sequence of elements whose keys compare equivalent to a given key
  /// @param key the key to search for
  /// @return pair of iterators such that all elements in the range <tt> [first, second) </tt>
  ///   have keys which compare equal to @c key if there was a key equivalent to
  ///   @c key in the map. Otherwise both iterators will be @c end().
  ///
  auto equal_range(key_type const& key) const noexcept(comparison_is_noexcept)
      -> decltype(ops_ptr_->equal_range(map_ptr_, key)) {
    return ops_ptr_->equal_range(map_ptr_, key);
  }

  /// @brief obtain a reference to the mapped value for the specified key
  /// @param key the key to search for
  /// @return reference to the mapped value. If @c key did not exist in the map,
  ///   then a default constructed @c mapped_type is inserted into the map at @c
  ///   key and a reference to it is returned.
  ///
  /// @throws anny exception thrown by the comparisons, and exception throw by
  ///   the copy-constructor of @c key_type, or the default-constructor of
  ///   @c mapped_type
  ///
  /// @pre <tt> size() < Capacity </tt> if @c contains(key) is @c false
  ///
  auto operator[](key_type const& key) const noexcept(false) -> decltype(ops_ptr_->operator_index(map_ptr_, key)) {
    return ops_ptr_->operator_index(map_ptr_, key);
  }

  /// @brief obtain a reference to the mapped value for the specified key
  /// @param key the key to search for
  /// @return reference to the mapped value
  /// @throws std::out_of_range if @c contains(key) is @c false
  /// @throws any exception thrown by the comparisons
  ///
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  auto at(key_type const& key) const noexcept(false) -> decltype(ops_ptr_->at(map_ptr_, key)) {
    return ops_ptr_->at(map_ptr_, key);
  }

  /// @brief insert an element in the map
  /// @param value the value to insert
  ///
  /// Copy constructs the provided value into the new element if an appropriate
  /// element does not already exist.
  ///
  /// @return std::pair<iterator, bool> whose first element is the key
  ///   equivalent to @c value.first, and whose second element is a boolean with
  ///   a value of @c true if the element was inserted or @c false if not.
  ///
  /// @pre @c contains(value.first) is @c true, or <tt> size() < Capacity </tt>
  ///   of the @c inline_map
  ///
  /// @throws any exception thrown by the comparisons, or the copy constructor
  ///   of @c value_type
  ///
  auto insert(value_type const& value) const noexcept(false) -> decltype(ops_ptr_->insert(map_ptr_, value)) {
    return ops_ptr_->insert(map_ptr_, value);
  }

  /// @brief insert an element in the map or update an existing element
  /// @param key the key for which to insert or assign an element
  /// @param mapped the value to associate with @c key
  ///
  /// If @c key is contained in the @c inline_map, updates the associated value
  /// by copy-assigning @c mapped. If @c key is not contained in the @c
  /// inline_map, constructs a new element in the @c inline_map by copying @c
  /// key and @c mapped.
  ///
  /// @return pair containing an iterator to the element in the map and boolean
  ///   that is @c true if the element was inserted and @c false if not.
  ///
  /// @pre @c contains(key) is @c true, or <tt> size() < Capacity </tt> of the
  ///   @c inline_map
  ///
  /// @throws any exception thrown by the comparisons, the move constructor of
  ///   @c key_type or the constructor or assignment operator of @c value_type
  ///
  auto insert_or_assign(key_type const& key, mapped_type const& mapped) const noexcept(false)
      -> decltype(ops_ptr_->insert_or_assign(map_ptr_, key, mapped)) {
    return ops_ptr_->insert_or_assign(map_ptr_, key, mapped);
  }

  /// @brief remove element with the corresponding key, if there is one
  /// @param key The key to erase
  /// @return iterator after the removed element, or @c end if there is no
  ///   element was removed
  ///
  auto erase(key_type const& key) const noexcept(comparison_is_noexcept)
      -> decltype(ops_ptr_->erase_key(map_ptr_, key)) {
    return ops_ptr_->erase_key(map_ptr_, key);
  }

  /// @brief remove the element at the specified position
  /// @param pos iterator referring to the element to remove
  /// @pre @c pos must be a dereferencable iterator referring to an element in @c map
  /// @return iterator after the removed element
  ///
  auto erase(const_iterator pos) const noexcept -> decltype(ops_ptr_->erase_pos(map_ptr_, pos)) {
    return ops_ptr_->erase_pos(map_ptr_, pos);
  }

  /// @brief remove elements in the specified range
  /// @param first beginning of the range of elements to remove
  /// @param last end of the range of elements to remove
  /// @pre <tt> [first, last) </tt> is a valid range in @c map
  /// @return iterator after the removed element
  ///
  auto erase(const_iterator first, const_iterator last) const noexcept
      -> decltype(ops_ptr_->erase_rng(map_ptr_, first, last)) {
    return ops_ptr_->erase_rng(map_ptr_, first, last);
  }

  /// @brief destroy all elements in the assocaited @c inline_map
  /// @post <tt> size() == 0 </tt>
  /// @return @c void
  ///
  auto clear() const noexcept -> decltype(ops_ptr_->clear(map_ptr_)) { return ops_ptr_->clear(map_ptr_); }

  /// @brief obtain an iterator referring to the beginning of the sorted range of elements
  /// @return iterator to fhe first element in the sequence, or @c end() if @c empty()
  ///
  auto begin() const noexcept -> decltype(ops_ptr_->begin(map_ptr_)) { return ops_ptr_->begin(map_ptr_); }

  /// @brief obtain an iterator referring to one past the last element in the sorted range of elements
  /// @return iterator one past the last element in the sequence
  ///
  auto end() const noexcept -> decltype(ops_ptr_->end(map_ptr_)) { return ops_ptr_->end(map_ptr_); }
};

// parasoft-end-suppress AUTOSAR-A10_1_1-a-2
// parasoft-end-suppress AUTOSAR-A10_2_1-a
// parasoft-end-suppress AUTOSAR-A10_2_1-b
// parasoft-end-suppress AUTOSAR-A12_1_5-a
// parasoft-end-suppress AUTOSAR-A13_5_1-a

// parasoft-begin-suppress AUTOSAR-A12_1_5-a "Delegating constructors would not reduce duplication"
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: Only inherits from interface base classes"

/// @brief A const reference class to an inline_map object, with size-erased type
/// @tparam Key The key type for the map
/// @tparam Value The mapped type for the map
/// @tparam Compare The comparison function
/// @pre The comparison must provide a strict ordering over the values of @c Key
///
template <class Key, class Value, class Compare>
class const_inline_map_reference
    : public inline_map_reference_detail::const_inline_map_reference_interface<
          const_inline_map_reference<Key, Value, Compare>>
    , public inline_container::detail::range_interface<const_inline_map_reference<Key, Value, Compare>> {
 public:
  /// @brief alias to the capacity-erased map base class
  using map_base_type = inline_map_detail::inline_map_base<Key, Value, Compare>;

  /// @brief alias to const operations type
  using const_ops_type = inline_map_reference_detail::const_operations<map_base_type>;

  /// @brief key type of the map
  using key_type = Key;
  /// @brief mapped type of the map
  using mapped_type = Value;
  /// @brief value type of the map
  using value_type = std::pair<Key const, Value>;
  /// @brief size type of the map
  using size_type = typename map_base_type::size_type;
  /// @brief iterator type of a const reference to the map
  using iterator = inline_map_reference_iterator<map_base_type, true>;
  /// @brief const iterator type of the underlying map
  using const_iterator = iterator;
  /// @brief The type of an iterator that provides non- @c const access to the elements
  /// and iterates in reverse through the elements
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  /// @brief The type of an iterator that provides @c const access to the elements
  /// and iterates in reverse through the elements
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;
  /// @brief compare type of the map
  using compare_type = Compare;

 private:
  /// @brief alias to the interface class providing const member functions
  using interface_base = inline_map_reference_detail::const_inline_map_reference_interface<const_inline_map_reference>;

  /// @brief alias to the range interface class providing range member functions
  using range_base = inline_container::detail::range_interface<const_inline_map_reference<Key, Value, Compare>>;

  /// @brief pointer to the base class
  map_base_type const* map_ptr_;

  /// @brief pointer to const operations
  const_ops_type const* const_ops_ptr_;

 public:
  /// @brief obtain a pointer to the map base
  /// @return pointer to the map base
  ///
  auto map_ptr() const noexcept -> map_base_type const* { return map_ptr_; }

  /// @brief obtain a pointer to the const operations
  /// @return pointer to the const operations
  ///
  auto const_ops_ptr() const noexcept -> const_ops_type const* { return const_ops_ptr_; }

  /// @brief construct a @c const_inline_map_reference of a map
  /// @tparam Capacity capacity of the @c inline_map
  /// @param map @c inline_map to reference
  ///
  template <
      std::size_t Capacity,
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  explicit const_inline_map_reference(inline_map<Key, Value, Capacity, Compare> const& map) noexcept
      : interface_base{this},
        range_base{this},
        map_ptr_{&map},
        const_ops_ptr_{&inline_map_reference_detail::const_map_operations_with_at<
            inline_map<Key, Value, Capacity, Compare>>} {}

  /// @brief construct a @c const_inline_map_reference of a map
  /// @tparam Capacity capacity of the @c inline_map
  /// @param map @c inline_map to reference
  ///
  template <
      std::size_t Capacity,
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<!AreExceptionsEnabled>> = nullptr>
  explicit const_inline_map_reference(inline_map<Key, Value, Capacity, Compare> const& map) noexcept
      : interface_base{this},
        range_base{this},
        map_ptr_{&map},
        const_ops_ptr_{&inline_map_reference_detail::const_map_operations_without_at<
            inline_map<Key, Value, Capacity, Compare>>} {}

  /// @brief construct a @c const_inline_map_reference of a map
  /// @tparam Capacity capacity of the @c inline_map
  ///
  /// Deleted overload for rvalue references.
  ///
  template <std::size_t Capacity>
  explicit const_inline_map_reference(inline_map<Key, Value, Capacity, Compare> const&&) = delete;

  /// @brief construct a @c const_inline_map_reference from an @c inline_map_reference
  /// @param other @c inline_map_reference value
  ///
  explicit const_inline_map_reference(inline_map_reference<Key, Value, Compare> other) noexcept
      : interface_base{this},
        range_base{this},
        map_ptr_{other.map_ptr()},
        const_ops_ptr_{other.const_ops_ptr()} {}
};

// parasoft-end-suppress AUTOSAR-A10_1_1-a-2
// parasoft-end-suppress AUTOSAR-A12_1_5-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_REFERENCE_HPP_
