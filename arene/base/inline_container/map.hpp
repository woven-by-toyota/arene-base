// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 AUTOSAR-A8_4_2-a CERT_C-MSC37-a CERT_CPP-MSC52-a
// "inline_container/map.hpp is clear enough for inline_map and
// ARENE_PRECONDITION terminates, so does not need a return"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/algorithm/lexicographical_compare.hpp"
#include "arene/base/algorithm/rotate.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/inline_container/detail/compare.hpp"
#include "arene/base/inline_container/detail/container_base_type.hpp"
#include "arene/base/inline_container/detail/iterator_interface.hpp"
#include "arene/base/inline_container/detail/lower_bound_index.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/optional/optional_resetter.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_manipulation/smallest_integer_for.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/decays_to.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/forward_like.hpp"
#include "arene/base/utility/in_place.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: A2-10-1 exempts identifiers at class/namespace scope"
// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace inline_map_detail {

// parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: The exception type that can be thrown is documented."
/// @brief Implementation helper for conditional disablement of throwing when compiling with @c -fno-exceptions.
/// @throws @c std::out_of_range if @c detail::are_exceptions_enabled_v is @c true .
ARENE_NORETURN void throw_out_of_range();
// parasoft-end-suppress AUTOSAR-A15_4_5-a "False positive: The exception type that can be thrown is documented."

/// @brief base type for @c inline_map, allowing erasure of the @c Capacity value
/// @tparam Key The key type for the map
/// @tparam Value The mapped type for the map
/// @tparam Compare The comparison function
/// @pre The comparison must provide a strict ordering over the values of @c Key
///
template <typename Key, typename Value, typename Compare = compare_three_way>
class inline_map_base {
 public:
  /// @brief key type of the map
  using key_type = Key;
  /// @brief mapped type of the map
  using mapped_type = Value;
  /// @brief value type of the map
  using value_type = std::pair<Key const, Value>;
  /// @brief compare type of the map
  using compare_type = Compare;
  /// @brief The size type of the map
  using size_type = std::size_t;
};

/// @brief provides private access for the family of types related to an @c inline_map
/// @tparam MapBase common tag across @c inline_map and related types
///
template <class MapBase>
class inline_map_passkey_base {
  static_assert(
      is_instantiation_of_v<MapBase, inline_map_base>,
      "'MapBase' must be an instantiation of 'inline_map_base'."
  );

 protected:
  /// @brief tag type used to limit access to a family of associated types
  ///
  class inline_map_passkey {
   public:
    /// @brief default constructor
    ///
    explicit inline_map_passkey() = default;
  };
};

}  // namespace inline_map_detail

// parasoft-begin-suppress AUTOSAR-A12_0_1-b-2 "False positive: copy assignment defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False positive: copy assignment defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False positive: copy assignment defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: the ordering base class is empty"
// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: This follows the std::map interface. The non-const
// overload inserts a new element in the map if not present; the const overload cannot do that so is not provided"
/// @brief A container similar to @c std::map that has a fixed capacity.
///
/// The storage is held internally in the object. Any attempt to store more than @c Capacity elements will either result
/// in a precondition failure (and thus process termination), or an error being returned.  The comparison function
/// specified with the @c Compare template parameter can either return @c bool, in which case it is assumed to be a
/// simple ordering comparison like @c std::less, or it can return @c strong_ordering, in which case it is assumed to be
/// a three-way comparison operator. The default comparison is @c three_way_compare.
/// @tparam Key The key type for the map
/// @tparam Value The mapped type for the map
/// @tparam Capacity The maximum number of elements that can be stored in the map
/// @tparam Compare The comparison function
/// @pre The comparison must provide a strict ordering over the values of @c Key
template <typename Key, typename Value, std::size_t Capacity, typename Compare = compare_three_way>
// NOLINTNEXTLINE(hicpp-special-member-functions,cppcoreguidelines-special-member-functions)
class inline_map
    : public inline_map_detail::inline_map_base<Key, Value, Compare>
    , inline_map_detail::inline_map_passkey_base<inline_map_detail::inline_map_base<Key, Value, Compare>>
    , inline_container::detail::compare_wrapper<Compare, Key>
    , generic_ordering_from_three_way_compare_and_equals<inline_map<Key, Value, Capacity, Compare>> {
  // AUTOSAR exceptions:
  // A11-3-1: Friend declarations shall not be used
  // The inline_map class template declares other instantiations of the same
  // template to be friends, so that copy and move operations between maps with
  // different capacities work nicely.
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Friendship allows copy and move between maps with different
  // capacities"
  template <typename OtherKey, typename OtherValue, std::size_t OtherCapacity, typename OtherCompare>
  friend class inline_map;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  /// @brief helper trait for determining if transparent comparison is supported in a type-dependent context
  /// @tparam OtherKey Dummy parameter to make this type-dependent as it's used for SFINAE.
  /// @return bool Equivalent to @c ::arene::base::is_transparent_comparator_v<Compare>
  template <typename OtherKey>
  static constexpr bool transparent_comparison_supported{::arene::base::is_transparent_comparator_v<Compare>};

  /// @brief helper trait for determining if transparent insertion is supported in a type-dependent context
  /// @tparam OtherKey Dummy parameter to make this type-dependent as it's used for SFINAE.
  /// @return bool Equivalent to @c transparent_comparison_supported<Compare>&&!std::is_integral<Key>::value
  template <typename OtherKey>
  static constexpr bool transparent_insertion_supported{
      transparent_comparison_supported<OtherKey> && !std::is_integral<Key>::value
  };

  /// @brief helper trait for determining if the comparator supports transparent comparisons.
  /// @tparam OtherKey the type of the other operand to test for validity against.
  /// @return bool @c true If @c Compare satisfies either @c is_transparent_comparator_for_v or
  ///         @c is_transparent_three_way_comparator_for_v with @c Key and @c OtherKey , else @c false .
  template <typename OtherKey>
  static constexpr bool
      transparent_comparison_supported_for{::arene::base::is_transparent_comparator_for_v<Compare, Key const&, OtherKey const&> || ::arene::base::is_transparent_three_way_comparator_for_v<Compare, Key const&, OtherKey const&>};

  /// @brief The base class that holds the comparator
  using comparator_base = inline_container::detail::compare_wrapper<Compare, Key>;

  /// @brief The base class for ordering
  using ordering_base = generic_ordering_from_three_way_compare_and_equals<inline_map<Key, Value, Capacity, Compare>>;

  /// @brief Can comparisons throw?
  /// @tparam OtherKey The type of the rhs operand to a comparison.
  /// @return bool alias for @c comparator_base::template comparison_is_noexcept<OtherKey>
  template <typename OtherKey = Key>
  static constexpr bool comparison_is_noexcept{comparator_base::template comparison_is_noexcept<OtherKey>};

  /// @brief Base type of this @c inline_map type
  using map_base_type = inline_map_detail::inline_map_base<Key, Value, Compare>;

  /// @brief The type of an index which is the smallest that can fit @c Capacity .
  using index_type = smallest_unsigned_integer_for<Capacity>;

  /// @brief alias to the passkey type
  using inline_map_passkey = typename inline_map_detail::inline_map_passkey_base<map_base_type>::inline_map_passkey;

  /// @brief An iterator for a map; if @c IsConst is @c true this is a @c
  /// const_iterator, otherwise an @c iterator.
  /// @tparam IsConst Is this a const iterator?
  template <bool IsConst>
  class iterator_impl
      : public inline_container_detail::iterator_interface<iterator_impl<IsConst>>
      , inline_map_detail::inline_map_passkey_base<map_base_type> {
    /// @brief Alias for interface base
    using iterator_interface = inline_container_detail::iterator_interface<iterator_impl>;

    /// @brief alias to the passkey type
    using inline_map_passkey = typename inline_map_detail::inline_map_passkey_base<map_base_type>::inline_map_passkey;

    /// @brief The type of the associated map: if this is a const iterator, we can use
    /// a const map
    using map_type = conditional_t<IsConst, inline_map const, inline_map>;
    /// @brief Pointer to the referenced map
    map_type* map_;
    /// @brief The index of the pointed-to element
    index_type pos_;

    /// @brief The corresponding non-const iterator
    using non_const_iterator = iterator_impl<false>;
    /// @brief The source of an implicit conversion: const iterators can be implicitly
    /// constructed from non-const iterators
    using implicit_conversion_type = conditional_t<IsConst, non_const_iterator, non_constructible_dummy>;

    // parasoft-begin-suppress AUTOSAR-A7_1_1-a	"Declaring 'iter' as reference to const changes semantics"
    // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'iter' as reference to const changes semantics"
    // parasoft-begin-suppress AUTOSAR-A8_4_9-a	"Declaring 'iter' as reference to const changes semantics"

    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Use of a friend function with
    // passkey improves encapsulation. It allows data members to remain private
    // and does not unnecessarily expose a public member function."

    // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
    // translation units"

    /// @brief basis function for increment
    /// @param iter iterator to increment
    ///
    friend auto step_forward(inline_container_detail::iterator_interface_tag, iterator_impl& iter) noexcept -> void {
      ++iter.pos_;
    }

    /// @brief basis function for decrement
    /// @param iter iterator to increment
    ///
    friend auto step_backward(inline_container_detail::iterator_interface_tag, iterator_impl& iter) noexcept -> void {
      --iter.pos_;
    }

    // parasoft-end-suppress AUTOSAR-A11_3_1-a
    // parasoft-end-suppress AUTOSAR-A0_1_3-a
    // parasoft-end-suppress AUTOSAR-A7_1_1-a
    // parasoft-end-suppress AUTOSAR-M7_1_2-c
    // parasoft-end-suppress AUTOSAR-A8_4_9-a

   public:
    /// @brief Default construct an iterator that does not refer to any map element
    iterator_impl() noexcept
        : map_(nullptr),
          pos_(0) {}

    /// @brief Default copy constructor
    /// @param other The source
    iterator_impl(iterator_impl const& other) = default;
    /// @brief Default move constructor
    /// @param other The source
    iterator_impl(iterator_impl&& other) = default;
    /// @brief Default copy assignment
    /// @param other The source
    auto operator=(iterator_impl const& other) -> iterator_impl& = default;
    /// @brief Default move assignment
    /// @param other The source
    auto operator=(iterator_impl&& other) -> iterator_impl& = default;

    /// @brief Default destructor
    ~iterator_impl() = default;

    /// @brief Construct an iterator from a map and a position; only callable from @c
    /// inline_map members.
    /// @param map The map
    /// @param pos The position
    iterator_impl(inline_map_passkey, map_type* map, index_type pos) noexcept
        : iterator_interface{},
          map_(map),
          pos_(pos) {}

    // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
    // initialize the base class"
    /// @brief Implicitly convert from a non-const iterator, if this
    /// is a const iterator.
    /// @param other The source iterator
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    iterator_impl(implicit_conversion_type other) noexcept
        : iterator_impl(
              inline_map_passkey{},
              other.get_map(inline_map_passkey{}),
              other.get_index(inline_map_passkey{})
          ) {}
    // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

    /// @brief The iterator category
    using iterator_category = std::bidirectional_iterator_tag;
    /// @brief The iterator value type
    using value_type = std::pair<Key const, Value>;
    /// @brief The iterator difference type
    using difference_type = std::ptrdiff_t;
    /// @brief The pointer type
    using pointer = conditional_t<IsConst, value_type const*, value_type*>;
    /// @brief The reference type
    using reference = conditional_t<IsConst, value_type const&, value_type&>;

    /// @brief The associated @c inline_map type
    using inline_map_type = inline_map;

    // parasoft-begin-suppress AUTOSAR-M9_3_1-a-2 "False positive: It returns a reference to the map element"
    /// @brief Dereference the iterator
    /// @return A reference to the referenced element
    /// @pre The iterator must have been constructed referring to a map, otherwise the behaviour is undefined
    auto operator*() const noexcept -> reference { return *map_->entry_at_index(pos_); }
    // parasoft-end-suppress AUTOSAR-M9_3_1-a-2

    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
    // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
    // translation units"

    /// @brief equality
    /// @param lhs The first iterator to compare
    /// @param rhs The second iterator to compare
    /// @return @c true if @c lhs is equal to @c rhs, otherwise @c false
    /// @pre The iterators must refer to the same map, otherwise the result is meaningless
    friend auto operator==(iterator_impl const& lhs, iterator_impl const& rhs) noexcept -> bool {
      return lhs.pos_ == rhs.pos_;
    }

    // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
    // parasoft-end-suppress AUTOSAR-A0_1_3-a

    /// @brief Check if this is an iterator for a given map.
    /// @param map A pointer to the map we are looking for
    /// @return true If @c this was produced from @c map
    /// @return false Otherwise.
    auto is_iterator_for(map_type* map) const noexcept -> bool { return map == map_; }

    /// @brief Get the map this iterator references. Can only be called by members of
    /// @c inline_map
    /// @return A pointer to the referenced map
    auto get_map(inline_map_passkey) const noexcept -> map_type* { return map_; }

    /// @brief Get the position this iterator references within a map. Can only be
    /// called by members of @c inline_map
    /// @return The index position of the referenced element
    auto get_index(inline_map_passkey) const noexcept -> index_type { return pos_; }
  };

 public:
  /// @brief The value type of the map
  using value_type = typename map_base_type::value_type;
  /// @brief The key type of the map
  using key_type = typename map_base_type::key_type;
  /// @brief The mapped type of the map
  using mapped_type = typename map_base_type::mapped_type;
  /// @brief The comparator of the map
  using key_compare = typename map_base_type::compare_type;
  /// @brief The size type of the map
  using size_type = typename map_base_type::size_type;
  /// @brief The type of a pointer to the value type of the map
  using pointer = value_type*;
  /// @brief The type of a @c const pointer to the value type of the map
  using const_pointer = value_type const*;
  /// @brief The type of a reference to the value type of the map
  using reference = value_type&;
  /// @brief The type of a @c const reference to the value type of the map
  using const_reference = value_type const&;

  /// @brief The type of an iterator that provides non- @c const access to the elements
  using iterator = iterator_impl<false>;
  /// @brief The type of an iterator that provides @c const access to the elements
  using const_iterator = iterator_impl<true>;

  /// @brief The type of an iterator that provides non- @c const access to the elements
  /// and iterates in reverse through the elements
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  /// @brief The type of an iterator that provides @c const access to the elements
  /// and iterates in reverse through the elements
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;

  /// @brief The difference type of the map
  using difference_type = typename iterator::difference_type;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief map capacity
  static constexpr std::integral_constant<size_type, Capacity> capacity{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief @c inline_map_base type
  using inline_map_base_type = map_base_type;

 private:
  /// @brief A type for the source of the copy-assignment operator: if the comparator
  /// is copy assignable and the values are copy-constructible, then this the
  /// map is copy-assignable, so this is @c inline_map, otherwise it is @c
  /// non_constructible_dummy so the copy-assignment operator is deleted.
  using copy_assign_source = conditional_t<
      std::is_copy_assignable<Compare>::value && std::is_copy_constructible<value_type>::value,
      inline_map,
      non_constructible_dummy>;

  /// @brief A type to use for a deleted assignment operator: if the map should not be
  /// copy-assignable, this is @c inline_map, otherwise @c
  /// non_constructible_dummy
  using dummy_copy_assign_source = conditional_t<
      !std::is_copy_assignable<Compare>::value || !std::is_copy_constructible<value_type>::value,
      inline_map,
      non_constructible_dummy>;

  /// @brief A type for the source of the move-assignment operator: if the comparator
  /// is move assignable and the values are move-constructible, then this the
  /// map is move-assignable, so this is @c inline_map, otherwise it is @c
  /// non_constructible_dummy so the move-assignment operator is deleted.
  using move_assign_source = conditional_t<
      std::is_move_assignable<Compare>::value && std::is_move_constructible<value_type>::value,
      inline_map,
      non_constructible_dummy>;
  /// @brief A type to use for a deleted assignment operator: if the map should not be
  /// move-assignable, this is @c inline_map, otherwise @c
  /// non_constructible_dummy
  using dummy_move_assign_source = conditional_t<
      !std::is_move_assignable<Compare>::value || !std::is_move_constructible<value_type>::value,
      inline_map,
      non_constructible_dummy>;

  /// @brief A type for the source of the move constructor: if the comparator
  /// is move constructible and the values are move-constructible, then this the
  /// map is move-constructible, so this is @c inline_map, otherwise it is @c
  /// non_constructible_dummy so the move constructor is deleted.
  using move_construct_source = conditional_t<
      std::is_move_constructible<Compare>::value && std::is_move_constructible<value_type>::value,
      inline_map,
      non_constructible_dummy>;

 public:
  /// @brief Construct an empty map with a default-constructed comparator
  /// @tparam C The comparator type of the map, used for constraints
  /// @throws Any exception thrown by the default constructor of the comparator,
  /// @return optional<inline_map> holding a default-constructed map
  template <typename C = Compare, constraints<std::enable_if_t<std::is_default_constructible<C>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct() noexcept(std::is_nothrow_default_constructible<Compare>::value)
      -> optional<inline_map> {
    return optional<inline_map>{in_place};
  }

  /// @brief Construct a map with a comparator copied from the supplied value
  /// @tparam C The comparator type of the map, used so this function is not viable if @c Compare cannot be copy
  /// constructed.
  /// @param comparator A comparator instance to copy
  /// @throws Any exception thrown by the copy constructor of the comparator
  /// @return optional<inline_map> holding a map constructed from the comparator
  template <typename C = Compare, constraints<std::enable_if_t<std::is_copy_constructible<C>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(Compare const& comparator
  ) noexcept(std::is_nothrow_copy_constructible<Compare>::value) -> optional<inline_map> {
    return optional<inline_map>{in_place, comparator};
  }

  /// @brief Copy construct a map
  /// @tparam V The value type of the map, used for constraints
  /// @tparam C The comparator type of the map, used for constraints
  /// @param other The source container
  /// @throws Any exception thrown by the copy constructor of the comparator, or the values
  /// @return optional<inline_map> holding a copy-constructed map
  template <
      typename V = value_type,
      typename C = Compare,
      constraints<
          std::enable_if_t<std::is_copy_constructible<C>::value>,
          std::enable_if_t<std::is_copy_constructible<V>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_map const& other
  ) noexcept(std::is_nothrow_copy_constructible<C>::value && std::is_nothrow_copy_constructible<V>::value)
      -> optional<inline_map> {
    return optional<inline_map>{in_place, other};
  }

  /// @brief Copy construct a map with a different size
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam V The value_type
  /// @tparam C The comparator type
  /// @param other The source map
  /// @throws Any exception thrown by the copy constructors of the comparator, keys or values
  /// @return optional<inline_map> holding the copied map, or @c nullopt if there were too many elements
  template <
      std::size_t OtherCapacity,
      typename V = value_type,
      typename C = Compare,
      constraints<
          std::enable_if_t<OtherCapacity != Capacity>,
          std::enable_if_t<std::is_copy_constructible<V>::value>,
          std::enable_if_t<std::is_copy_constructible<C>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_map<Key, Value, OtherCapacity, Compare> const& other
  ) noexcept(std::is_nothrow_copy_constructible<C>::value && std::is_nothrow_copy_constructible<V>::value)
      -> optional<inline_map> {
    if (other.size() > Capacity) {
      return {};
    }
    return optional<inline_map>{in_place, other};
  }

  /// @brief Move construct a map
  /// @tparam K The key type
  /// @tparam V The value type
  /// @tparam C The comparator type of the map, used for constraints
  /// @param other The source container
  /// @throws Any exception thrown by the move constructor of the comparator, or the values
  /// @return optional<inline_map> holding a move-constructed map
  template <
      typename K = Key,
      typename V = Value,
      typename C = Compare,
      constraints<
          std::enable_if_t<std::is_move_constructible<C>::value>,
          std::enable_if_t<std::is_copy_constructible<K>::value>,
          std::enable_if_t<std::is_move_constructible<V>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_map&& other) noexcept(
      std::is_nothrow_move_constructible<C>::value && std::is_nothrow_copy_constructible<K>::value &&
      std::is_nothrow_move_constructible<V>::value
  ) -> optional<inline_map> {
    return optional<inline_map>{in_place, std::move(other)};
  }

  /// @brief Move construct a map with a different size
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam K The key type
  /// @tparam V The value type
  /// @tparam C The comparator type
  /// @param other The source map
  /// @throws Any exception thrown by the copy constructor of the comparator, or the move constructor of the keys or
  /// values
  /// @return optional<inline_map> holding the copied map, or @c nullopt if there were too many elements
  template <
      std::size_t OtherCapacity,
      typename K = Key,
      typename V = Value,
      typename C = Compare,
      constraints<
          std::enable_if_t<OtherCapacity != Capacity>,
          std::enable_if_t<std::is_copy_constructible<K>::value>,
          std::enable_if_t<std::is_move_constructible<V>::value>,
          std::enable_if_t<std::is_copy_constructible<C>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_map<Key, Value, OtherCapacity, Compare>&& other) noexcept(
      std::is_nothrow_copy_constructible<C>::value && std::is_nothrow_copy_constructible<K>::value &&
      std::is_nothrow_move_constructible<V>::value
  ) -> optional<inline_map> {
    if (other.size() > Capacity) {
      return {};
    }
    return optional<inline_map>{in_place, std::move(other)};
  }

  /// @brief Construct a map with a default-constructed comparator and copies of the elements from the provided
  /// initializer list. If there are too many elements, return an empty @c optional
  /// @tparam T The value type of the map, used so this function is not viable if @c value_type cannot be copy
  /// constructed.
  /// @tparam C The comparator type of the map, used so this function is not viable if @c Compare cannot be default
  /// constructed.
  /// @param init_list The initializer list
  /// @throws Any exception thrown by the default constructor of the comparator,
  /// the copy constructor of the keys or values, or the comparisons
  /// @return optional<inline_map> holding a map constructed with the elements from the initializer list, or @c
  /// nullopt if there were too many elements.
  template <
      typename T = value_type,
      typename C = Compare,
      constraints<
          std::enable_if_t<std::is_default_constructible<C>::value>,
          std::enable_if_t<std::is_copy_constructible<T>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(std::initializer_list<value_type> init_list) noexcept(
      std::is_nothrow_default_constructible<Compare>::value && comparison_is_noexcept<> &&
      std::is_nothrow_copy_constructible<Key>::value && std::is_nothrow_copy_constructible<Value>::value
  ) -> optional<inline_map> {
    optional<inline_map> res{in_place};
    if (!res->insert_elements_if_they_fit(init_list)) {
      res.reset();
    }
    return res;
  }

  /// @brief Construct a map with a comparator copied from the supplied value and copies of the elements from the
  /// provided initializer list. If there are too many elements, return an empty @c optional
  /// @tparam T The value type of the map, used so this function is not viable if @c value_type cannot be copy
  /// constructed.
  /// @tparam C The comparator type of the map, used so this function is not viable if @c Compare cannot be default
  /// constructed.
  /// @param init_list The initializer list
  /// @param comparator A comparator instance to copy
  /// @throws Any exception thrown by the copy constructor of the comparator,
  /// the copy constructor of the keys or values, or the comparisons
  /// @return optional<inline_map> holding a map constructed with the elements from the initializer list, or @c
  /// nullopt if there were too many elements.
  template <
      typename T = value_type,
      typename C = Compare,
      constraints<
          std::enable_if_t<std::is_copy_constructible<C>::value>,
          std::enable_if_t<std::is_copy_constructible<T>::value>> = nullptr>
  ARENE_NODISCARD static auto
  try_construct(std::initializer_list<value_type> init_list, Compare const& comparator) noexcept(
      std::is_nothrow_copy_constructible<Compare>::value && comparison_is_noexcept<> &&
      std::is_nothrow_copy_constructible<Key>::value && std::is_nothrow_copy_constructible<Value>::value
  ) -> optional<inline_map> {
    optional<inline_map> res{in_place, comparator};
    if (!res->insert_elements_if_they_fit(init_list)) {
      res.reset();
    }
    return res;
  }

  /// @brief Construct the map with a default-constructed comparator and no elements
  /// @throws Any exception thrown by the default constructor of the comparator
  inline_map() = default;

  /// @brief Default destructor
  ~inline_map() = default;

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct the map with a default-constructed comparator and copies of the elements from the provided
  /// initializer list
  /// @tparam T The value type of the map, used so this constructor  is not viable if @c value_type cannot be copy
  /// constructed.
  /// @tparam C The comparator type of the map, used so this constructor is not viable if @c Compare cannot be default
  /// constructed.
  /// @param init_list The initializer list
  /// @throws Any exception thrown by the default constructor of the comparator,
  /// the copy constructor of the keys or values, or the comparisons
  /// @pre There are no more than @c Capacity elements in @c init_list with unique keys, else @c ARENE_PRECONDITION
  /// violation.
  template <
      typename T = value_type,
      typename C = Compare,
      constraints<
          std::enable_if_t<std::is_default_constructible<C>::value>,
          std::enable_if_t<std::is_copy_constructible<T>::value>> = nullptr>
  inline_map(std::initializer_list<value_type> init_list)
      : inline_map() {
    for (auto const& element : init_list) {
      std::ignore = try_emplace(element.first, element.second);
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct the map with a copy of the specified comparator and copies of
  /// the elements from the provided initializer list
  /// @tparam T The value type of the map, used so this constructor is not
  /// viable if @c value_type cannot be copy constructed.
  /// @tparam C The comparator type of the map, used so this constructor is not
  /// viable if @c Compare cannot be copy constructed.
  /// @param init_list The initializer list
  /// @param compare The comparator to copy
  /// @throws Any exception thrown by the copy-constructor of the comparator,
  /// the copy constructor of the keys or values, or the comparisons
  /// @pre There are no more than @c Capacity elements in @c init_list with unique keys, else @c ARENE_PRECONDITION
  /// violation.
  template <
      typename T = Value,
      typename C = Compare,
      constraints<
          std::enable_if_t<std::is_copy_constructible<T>::value>,
          std::enable_if_t<std::is_copy_constructible<C>::value>> = nullptr>
  inline_map(std::initializer_list<value_type> init_list, Compare const& compare)
      : inline_map(compare) {
    for (auto const& element : init_list) {
      std::ignore = try_emplace(element.first, element.second);
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief Construct the map with a copy of the specified comparator and no elements.
  /// @param compare The comparator to copy#
  /// @throws Any exception thrown by the copy-constructor of the comparator
  explicit inline_map(Compare const& compare) noexcept(std::is_nothrow_copy_constructible<Compare>::value)
      : comparator_base{compare},
        ordering_base{} {}

  /// @brief Copy the source map, so the new map has the same size, comparator and
  /// elements as the source.
  /// @param other The instance to be copied.
  /// @throws Any exception thrown by the copy constructor of the comparator,
  /// the keys, or values
  inline_map(inline_map const& other) = default;

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  /// @brief Move the source map into the new instance, so the new map has the same
  /// size, comparator and elements as the source did before-hand.
  ///
  /// All the keys from the source map are copy-constructed, and all
  /// the mapped values are move-constructed.
  /// @param other The instance to be copied.
  /// @throws Any exception thrown by the copy constructor of the comparator or
  /// the keys, or the move-constructor of the values
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  inline_map(move_construct_source&& other) noexcept(
      std::is_nothrow_move_constructible<Compare>::value && std::is_nothrow_copy_constructible<Key>::value &&
      std::is_nothrow_move_constructible<Value>::value
  )
      : comparator_base(std::move(static_cast<comparator_base&>(other))),
        ordering_base{},
        size_(other.size_),
        indices_(other.indices_),
        values_(move_values(other, std::make_index_sequence<Capacity>())) {}
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Copy from a map with a different capacity
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam SfinaeKey Dummy key type for SFINAE
  /// @tparam SfinaeValue Dummy value type for SFINAE
  /// @tparam SfinaeCompare Dummy compare type for SFINAE
  /// @param other The source map
  /// @throws Any exception thrown by the copy constructors of the comparator, keys or values
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation.
  template <
      std::size_t OtherCapacity,
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      typename SfinaeCompare = Compare,
      constraints<
          std::enable_if_t<OtherCapacity != Capacity>,
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_copy_constructible<SfinaeValue>::value>,
          std::enable_if_t<std::is_copy_constructible<SfinaeCompare>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  inline_map(inline_map<Key, Value, OtherCapacity, Compare> const& other)
      : inline_map(other.key_comp()) {
    ARENE_PRECONDITION(other.size() <= Capacity);
    for (auto const& element : other) {
      std::ignore = try_emplace(element.first, element.second);
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Move from a map with a different capacity
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam SfinaeKey Dummy key type for SFINAE
  /// @tparam SfinaeValue Dummy value type for SFINAE
  /// @tparam SfinaeCompare Dummy compare type for SFINAE
  /// @param other The source map
  /// @throws Any exception thrown by the copy constructors of the comparator or keys or the move constructor of the
  /// values
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation.
  template <
      std::size_t OtherCapacity,
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      typename SfinaeCompare = Compare,
      constraints<
          std::enable_if_t<OtherCapacity != Capacity>,
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_move_constructible<SfinaeValue>::value>,
          std::enable_if_t<std::is_copy_constructible<SfinaeCompare>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  inline_map(inline_map<Key, Value, OtherCapacity, Compare>&& other)
      : inline_map(other.key_comp()) {
    ARENE_PRECONDITION(other.size() <= Capacity);
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a-2 "False positive: RAII class"
    auto const clear_on_exit = clear_on_scope_exit(other);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a-2
    for (auto& element : other) {
      std::ignore = try_emplace(element.first, std::move(element.second));
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  /// @brief Deleted copy-assignment if the map should not be copy-assignable
  auto operator=(dummy_copy_assign_source const&) -> inline_map& = delete;

  /// @brief Deleted move-assignment if the map should not be move-assignable
  auto operator=(dummy_move_assign_source&&) -> inline_map& = delete;

  /// @brief copy-assignment of a map with the same capacity.
  ///
  /// If an exception is thrown then @c *this is left in a valid state containing the subset of elements copied from the
  /// source prior to the exception being thrown.
  /// @param other The instance to be copied.
  /// @return inline_map& A reference to @c *this
  /// @throws Any exception thrown by the copy assignment of the comparator, or
  /// the copy constructors of the keys, or values
  auto operator=(copy_assign_source const& other) noexcept(
      std::is_nothrow_copy_assignable<Compare>::value && std::is_nothrow_copy_constructible<Key>::value &&
      std::is_nothrow_copy_constructible<Value>::value
  ) -> inline_map& {
    if (this != &other) {
      do_copy(other);
    }
    return *this;
  }

  /// @brief copy-assignment of a map with a greater capacity.
  ///
  /// If an exception is thrown then @c *this is left in a valid state containing the subset of elements copied from the
  /// source prior to the exception being thrown.
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam SfinaeCompare Dummy key type for SFINAE
  /// @tparam SfinaeValue Dummy value type for SFINAE
  /// @tparam SfinaeCompare Dummy compare type for SFINAE
  /// @param other The instance to be copied.
  /// @return inline_map& A reference to @c *this
  /// @throws Any exception thrown by the copy assignment of the comparator, or
  /// the copy constructors of the keys, or values.
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation.
  template <
      std::size_t OtherCapacity,
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      typename SfinaeCompare = Compare,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_copy_constructible<SfinaeValue>::value>,
          std::enable_if_t<std::is_move_assignable<SfinaeCompare>::value>> = nullptr>
  auto operator=(inline_map<Key, Value, OtherCapacity, Compare> const& other) -> inline_map& {
    ARENE_PRECONDITION(other.size() <= Capacity);
    do_copy(other);
    return *this;
  }

  /// @brief copy-assignment of a map with a smaller capacity.
  ///
  /// If an exception is thrown then @c *this is left in a valid state containing the subset of elements copied from the
  /// source prior to the exception being thrown.
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam SfinaeKey Dummy key type for SFINAE
  /// @tparam SfinaeValue Dummy value type for SFINAE
  /// @tparam SfinaeCompare Dummy comparator type for SFINAE
  /// @param other The instance to be copied.
  /// @return inline_map& A reference to @c *this
  /// @throws Any exception thrown by the copy assignment of the comparator, or the copy constructors of the keys, or
  /// values
  template <
      std::size_t OtherCapacity,
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      typename SfinaeCompare = Compare,
      constraints<
          std::enable_if_t<(OtherCapacity < Capacity)>,
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_copy_constructible<SfinaeValue>::value>,
          std::enable_if_t<std::is_move_assignable<SfinaeCompare>::value>> = nullptr>
  auto operator=(inline_map<Key, Value, OtherCapacity, Compare> const& other) -> inline_map& {
    do_copy(other);
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  /// @brief Move-assignment of a map with the same capacity.
  ///
  /// All the keys from the source map are copy-constructed, and all the mapped values are move-constructed. The source
  /// map is cleared, so all elements are destroyed and this size is zero. If an exception is thrown then @c *this is
  /// left in a valid state containing the subset of elements moved from the source prior to the exception being thrown,
  /// and the source is left empty.
  /// @param other The instance to be moved.
  /// @return inline_map& A reference to @c *this
  /// @throws Any exception thrown by the move-assignment of the comparator or the copy-constructor of the keys, or the
  /// move-constructor of the values
  auto operator=(move_assign_source&& other
  ) noexcept(std::is_nothrow_move_assignable<Compare>::value && std::is_nothrow_move_constructible<value_type>::value)
      -> inline_map& {
    if (this != &other) {
      do_move(std::move(other));
    }
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'other' is used"

  /// @brief Move-assignment of a map with a greater capacity.
  ///
  /// All the keys from the source map are copy-constructed, and all the mapped values are move-constructed. The source
  /// map is cleared, so all elements are destroyed and this size is zero. If an exception is thrown then @c *this is
  /// left in a valid state containing the subset of elements moved from the source prior to the exception being thrown,
  /// and the source is left empty.
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam SfinaeKey Dummy key type for SFINAE
  /// @tparam SfinaeValue Dummy value type for SFINAE
  /// @tparam SfinaeCompare Dummy comparator type for SFINAE
  /// @param other The instance to be moved.
  /// @return inline_map& A reference to @c *this
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the move-assignment of the comparator or the copy-constructor of the keys, or the
  /// move-constructor of the values
  template <
      std::size_t OtherCapacity,
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      typename SfinaeCompare = Compare,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_move_constructible<SfinaeValue>::value>,
          std::enable_if_t<std::is_move_assignable<SfinaeCompare>::value>> = nullptr>
  auto operator=(inline_map<Key, Value, OtherCapacity, Compare>&& other) -> inline_map& {
    ARENE_PRECONDITION(other.size() <= Capacity);
    do_move(std::move(other));
    return *this;
  }

  // parasoft-end-suppress AUTOSAR-A0_1_4-a

  /// @brief Move-assignment of a map with a lesser capacity.
  ///
  /// All the keys from the source map are copy-constructed, and all the mapped values are move-constructed. The source
  /// map is cleared, so all elements are destroyed and this size is zero. If an exception is thrown then @c *this is
  /// left in a valid state containing the subset of elements moved from the source prior to the exception being thrown,
  /// and the source is left empty.
  /// @tparam OtherCapacity The capacity of the other map
  /// @tparam SfinaeKey Dummy key type for SFINAE
  /// @tparam SfinaeValue Dummy value type for SFINAE
  /// @tparam SfinaeCompare Dummy comparator type for SFINAE
  /// @param other The instance to be moved.
  /// @return inline_map& A reference to @c *this
  /// @throws Any exception thrown by the move-assignment of the comparator or the copy-constructor of the keys, or the
  /// move-constructor of the values
  template <
      std::size_t OtherCapacity,
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      typename SfinaeCompare = Compare,
      constraints<
          std::enable_if_t<(OtherCapacity < Capacity)>,
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_move_constructible<SfinaeValue>::value>,
          std::enable_if_t<std::is_move_assignable<SfinaeCompare>::value>> = nullptr>
  auto operator=(inline_map<Key, Value, OtherCapacity, Compare>&& other) -> inline_map& {
    do_move(std::move(other));
    return *this;
  }

  /// @brief Obtain a copy of the comparator
  /// @return The comparator
  /// @throws Any exception thrown by the copy-constructor of the comparator
  ARENE_NODISCARD auto key_comp() const noexcept(std::is_nothrow_copy_constructible<Compare>::value) -> Compare {
    return comparator_base::get_comparator();
  }

  /// @brief Check if the map is empty
  /// @return @c true if the map is empty, @c false otherwise
  ARENE_NODISCARD auto empty() const noexcept -> bool { return size_ == 0U; }
  /// @brief Get the number of elements in the map
  /// @return The number of elements
  ARENE_NODISCARD auto size() const noexcept -> size_type { return size_; }
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  /// @brief Get the maximum number of elements in the map, @c Capacity
  static constexpr std::integral_constant<size_type, Capacity> max_size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Obtain an iterator referring to the element equivalent to the supplied key, if there is one.
  ///
  /// @param key The key to find
  /// @return iterator to the equivalent element, or @c end() if the element is not found
  auto find(Key const& key) noexcept(comparison_is_noexcept<>) -> iterator { return do_find(*this, key); }
  /// @brief Obtain an iterator referring to the element equivalent to the supplied key, if there is one.
  ///
  /// @param key The key to find
  /// @return const_iterator to the equivalent element, or @c end() if the element is not found
  auto find(Key const& key) const noexcept(comparison_is_noexcept<>) -> const_iterator { return do_find(*this, key); }
  /// @brief Obtain an iterator referring to the element equivalent to the supplied key, if there is one.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to find
  /// @return iterator to the equivalent element, or @c end() if the element is not found
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto find(OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_find(*this, key);
  }
  /// @brief Obtain an iterator referring to the element equivalent to the supplied key, if there is one.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to find
  /// @return const_iterator to the equivalent element, or @c end() if the element is not found
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto find(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>) -> const_iterator {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_find(*this, key);
  }

  /// @brief Check if the set contains a specific key
  ///
  /// @param key The key to search for
  /// @return true if @c find(key)!=end() .
  /// @return false if @c find(key)==end() .
  auto contains(Key const& key) const noexcept(comparison_is_noexcept<>) -> bool { return do_contains(key); }
  /// @brief Check if the set contains a key equivalent to a specific value.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to search for
  /// @return true if @c find(key)!=end() .
  /// @return false if @c find(key)==end() .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto contains(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>) -> bool {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_contains(key);
  }

  /// @brief Return the number of elements in the map equivalent to a specific key.
  ///
  /// @param key The key to search for.
  /// @return std::size_t @c 0 if the element was not in the map, else @c 1 .
  auto count(Key const& key) const noexcept(comparison_is_noexcept<>) -> size_type { return do_count(key); }
  /// @brief Return the number of elements in the map equivalent to a specific key.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to search for.
  /// @return std::size_t @c 0 if the element was not in the map, else @c 1 .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto count(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>) -> size_type {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_count(key);
  }

  /// @brief Find the first element which is _not less than_ a given key.
  ///
  /// @param key The key to find the lower bound for.
  /// @return iterator to the first element which is _not less than_ @c key , or @c end() if there is no
  ///         such element.
  auto lower_bound(Key const& key) noexcept(comparison_is_noexcept<>) -> iterator { return do_lower_bound(*this, key); }
  /// @brief Find the first element which is _not less than_ a given key.
  ///
  /// @param key The key to find the lower bound for.
  /// @return const_iterator to the first element which is _not less than_ @c key , or @c end() if there is no
  ///         such element.
  auto lower_bound(Key const& key) const noexcept(comparison_is_noexcept<>) -> const_iterator {
    return do_lower_bound(*this, key);
  }
  /// @brief Find the first element which is _not less than_ a given key.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to find the lower bound for.
  /// @return iterator to the first element which is _not less than_ @c key , or @c end() if there is no such element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto lower_bound(OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_lower_bound(*this, key);
  }
  /// @brief Find the first element which is _not less than_ a given key.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to find the lower bound for.
  /// @return const_iterator to the first element which is _not less than_ @c key , or @c end() if there is no such
  /// element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto lower_bound(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>) -> const_iterator {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_lower_bound(*this, key);
  }

  /// @brief Find the first element which is _greater than_ a given key.
  ///
  /// @param key The key to search for.
  /// @return iterator to the first element which is _greater than_ @c key , or @c end() if there is no such element.
  auto upper_bound(Key const& key) noexcept(comparison_is_noexcept<>) -> iterator { return do_upper_bound(*this, key); }
  /// @brief Find the first element which is _greater than_ a given key.
  ///
  /// @param key The key to search for.
  /// @return const_iterator to the first element which is _greater than_ @c key , or @c end() if there is no such
  /// element.
  auto upper_bound(Key const& key) const noexcept(comparison_is_noexcept<>) -> const_iterator {
    return do_upper_bound(*this, key);
  }
  /// @brief Find the first element which is _greater than_ a given key.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to search for.
  /// @return iterator to the first element which is _greater than_ @c key , or @c end() if there is no such element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto upper_bound(OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_upper_bound(*this, key);
  }
  /// @brief Find the first element which is _greater than_ a given key.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to search for.
  /// @return const_iterator to the first element which is _greater than_ @c key , or @c end() if there is no such
  /// element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto upper_bound(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>) -> const_iterator {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_upper_bound(*this, key);
  }

  /// @brief Finds the sequence of elements whose keys compare equivalent to a given key.
  ///
  /// @param key The key to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all elements in the range @c [first,second)
  ///         have keys which compare equal to @c key if there was a key equivalent to @c key in the map. Otherwise both
  ///         iterators will be @c end() .
  auto equal_range(Key const& key) noexcept(comparison_is_noexcept<>) -> std::pair<iterator, iterator> {
    return do_equal_range(*this, key);
  }
  /// @brief Finds the sequence of elements whose keys compare equivalent to a given key.
  ///
  /// @param key The key to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all elements in the range @c [first,second)
  ///         have keys which compare equal to @c key if there was a key equivalent to @c key in the map. Otherwise both
  ///         iterators will be @c end() .
  auto equal_range(Key const& key) const noexcept(comparison_is_noexcept<>)
      -> std::pair<const_iterator, const_iterator> {
    return do_equal_range(*this, key);
  }
  /// @brief Finds the sequence of elements whose keys compare equivalent to a given key.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all elements in the range @c [first,second)
  ///         have keys which compare equal to @c key if there was a key equivalent to @c key in the map. Otherwise both
  ///         iterators will be @c end() .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto equal_range(OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>) -> std::pair<iterator, iterator> {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_equal_range(*this, key);
  }
  /// @brief Finds the sequence of elements whose keys compare equivalent to a given key.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to search for
  /// @return std::pair<const_iterator, const_iterator> A pair of iterators such that all elements in the range @c
  ///         [first,second) have keys which compare equal to @c key if there was a key equivalent to @c key in the map.
  ///         Otherwise both iterators will be @c end() .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto equal_range(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>)
      -> std::pair<const_iterator, const_iterator> {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_equal_range(*this, key);
  }

  /// @brief Obtain a reference to the mapped value for the specified key
  ///
  /// @param key The key to obtain the value for
  /// @return A reference to the mapped value. If @c key did not exist in the map, then a default constructed @c Value
  ///          is inserted into the map at @c key and a reference to it is returned.
  /// @throws Any exception thrown by the comparisons, and exception throw by the copy-constructor of @c Key, or the
  ///         default-constructor of @c Value
  /// @pre @c size()<Capacity if @c key is not in @c *this
  auto operator[](Key const& key) -> Value& { return do_operator_index(key); }
  /// @brief Obtain a reference to the mapped value for the specified key
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> , and
  ///               <c>std::is_constructible<Key,const K&></c> .
  /// @param key The key to obtain the value for
  /// @return A reference to the mapped value. If @c key did not exist in the map, then a default constructed @c Value
  ///          is inserted into the map at @c key and a reference to it is returned.
  /// @throws Any exception thrown by the comparisons, and exception throw by the copy-constructor of @c Key, or the
  ///         default-constructor of @c Value, or
  /// @pre @c size()<Capacity if @c key is not in @c *this
  template <
      typename OtherKey,
      constraints<
          std::enable_if_t<transparent_insertion_supported<OtherKey>>,
          std::enable_if_t<std::is_constructible<Key, OtherKey const&>::value>> = nullptr>
  auto operator[](OtherKey const& key) -> Value& {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_operator_index(key);
  }

  /// @brief Obtain a reference to the mapped value for the specified key or throw.
  ///
  /// @param key The key to obtain the value for
  /// @return Value& A reference to the mapped value
  /// @throws std::out_of_range if @c find(key)==end() .
  /// @throws Any exception thrown by the comparisons
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  ARENE_NODISCARD auto at(Key const& key) -> Value& {
    return do_at(*this, key);
  }
  /// @brief Obtain a reference to the mapped value for the specified key or throw.
  ///
  /// @param key The key to obtain the value for
  /// @return Value const& A reference to the mapped value
  /// @throws std::out_of_range if @c find(key)==end() .
  /// @throws Any exception thrown by the comparisons
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  ARENE_NODISCARD auto at(Key const& key) const -> Value const& {
    return do_at(*this, key);
  }

  /// @brief Obtain a reference to the mapped value for the specified key or throw.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to obtain the value for
  /// @return Value& A reference to the mapped value
  /// @throws std::out_of_range if @c find(key)==end() .
  /// @throws Any exception thrown by the comparisons
  template <
      typename OtherKey,
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<
          std::enable_if_t<AreExceptionsEnabled>,
          std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  ARENE_NODISCARD auto at(OtherKey const& key) -> Value& {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_at(*this, key);
  }
  /// @brief Obtain a reference to the mapped value for the specified key or throw.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to obtain the value for
  /// @return Value& A reference to the mapped value
  /// @throws std::out_of_range if @c find(key)==end() .
  /// @throws Any exception thrown by the comparisons
  template <
      typename OtherKey,
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<
          std::enable_if_t<AreExceptionsEnabled>,
          std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  ARENE_NODISCARD auto at(OtherKey const& key) const -> Value const& {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_at(*this, key);
  }

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  /// @brief Insert a new element in the map if there is not already one with the corresponding key.
  ///
  /// Copy constructs the provided value into the new element if an appropriate element does not already exist.
  /// @tparam SfinaeKey Dummy key type for SFINAE.
  /// @tparam SfinaeValue Dummy value type for SFINAE.
  /// @param value The value to insert
  /// @return std::pair<iterator, bool> whose first element is the key equivalent to @c v.first, and whose second
  ///         element is a boolean with a value of @c true if the element was inserted, or @c false otherwise.
  /// @pre @c contains(value.first) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons, or the copy constructor of @c value_type
  template <
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      constraints<
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_copy_constructible<SfinaeValue>::value>> = nullptr>
  auto insert(value_type const& value) -> std::pair<iterator, bool> {
    return try_emplace(value.first, value.second);
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Insert a new element in the map if there is not already one with the corresponding key.
  ///
  /// If an element does not already exist with the given key, the key is copy-constructed into the map while the value
  /// is move-constructed.
  /// @tparam SfinaeKey Dummy key type for SFINAE.
  /// @tparam SfinaeValue Dummy value type for SFINAE.
  /// @param value The value to insert
  /// @return std::pair<iterator, bool> whose first element is the key equivalent to @c v.first, and whose second
  ///         element is a boolean with a value of @c true if the element was inserted, or @c false otherwise.
  /// @pre @c contains(value.first) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons, or the copy constructor of @c value_type
  template <
      typename SfinaeKey = Key,
      typename SfinaeValue = Value,
      constraints<
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_move_constructible<SfinaeValue>::value>> = nullptr>
  auto insert(value_type&& value) -> std::pair<iterator, bool> {
    return try_emplace(value.first, std::move(value.second));
  }

  /// @brief Insert a new element in the map if there is not already one with the corresponding key.
  ///
  /// If an element does not already exist with the given key, the key is copy-constructed into the map while the value
  /// is move-constructed.
  /// @tparam P The type of the provided initializer
  /// @param value The value to insert
  /// @return std::pair<iterator, bool> whose first element is the key equivalent to @c  v.first, and whose second
  ///         element is a boolean with a value of @c true if the element was inserted, or @c false otherwise.
  /// @pre @c contains(value.first) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons, or the copy constructor of @c value_type
  template <
      typename P,
      constraints<
          std::enable_if_t<!std::is_same<value_type, P>::value>,
          std::enable_if_t<std::is_constructible<value_type, P&&>::value>> = nullptr>
  auto insert(P&& value) -> std::pair<iterator, bool> {
    return emplace(std::forward<P>(value));
  }

  /// @brief Insert a new element in the map if there is not already one with the corresponding key, otherwise assigns
  /// the new value to the existing one.
  ///
  /// Perfectly forwards the provided key and value into the constructor of the new element if an appropriate element
  /// does not already exist; perfectly forwards into the assignment operator for the existing element if there is one.
  /// @tparam OtherMapped The type of the initializer for the mapped type
  /// @tparam SfinaeKey The key type, used to delete this overload if the key is not copy constructible
  /// @param key The key for which to insert or assign an element
  /// @param mapped_value The value with which to construct or assign the mapped value
  /// @return A pair of an iterator to the element with a key equivalent to @c key, and a boolean with a value of
  ///         @c true if the element was inserted, or @c false otherwise.
  /// @pre @c contains(key) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons, the move constructor of @c Key or the constructor or assignment
  /// operator of @c Value .
  template <
      typename OtherMapped,
      typename SfinaeKey = Key,
      constraints<
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_constructible<Value, OtherMapped&&>::value>,
          std::enable_if_t<std::is_assignable<Value&, OtherMapped&&>::value>> = nullptr>
  auto insert_or_assign(Key const& key, OtherMapped&& mapped_value) -> std::pair<iterator, bool> {
    return do_insert_or_assign(key, std::forward<OtherMapped>(mapped_value));
  }
  /// @brief Insert a new element in the map if there is not already one with the corresponding key, otherwise assigns
  /// the new value to the existing one.
  ///
  /// Perfectly forwards the provided key and value into the constructor of the new element if an appropriate element
  /// does not already exist; perfectly forwards into the assignment operator for the existing element if there is one.
  /// @tparam OtherMapped The type of the initializer for the mapped type
  /// @tparam SfinaeKey The key type, used to delete this overload if the key is not move constructible
  /// @param key The key for which to insert or assign an element
  /// @param mapped_value The value with which to construct or assign the mapped value
  /// @return A pair of an iterator to the element with a key equivalent to @c key, and a boolean with a value of
  ///         @c true if the element was inserted, or @c false otherwise.
  /// @pre @c contains(key) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons, the move constructor of @c Key or the constructor or assignment
  /// operator of @c Value .
  template <
      typename OtherMapped,
      typename SfinaeKey = Key,
      constraints<
          std::enable_if_t<std::is_move_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_constructible<Value, OtherMapped&&>::value>,
          std::enable_if_t<std::is_assignable<Value&, OtherMapped&&>::value>> = nullptr>
  auto insert_or_assign(Key&& key, OtherMapped&& mapped_value) -> std::pair<iterator, bool> {
    return do_insert_or_assign(std::move(key), std::forward<OtherMapped>(mapped_value));
  }
  /// @brief Insert a new element in the map if there is not already one with an equivalent key, otherwise assigns the
  /// new value to the existing one.
  ///
  /// Perfectly forwards the provided key and value into the constructor of the new element if an appropriate element
  /// does not already exist; perfectly forwards into the assignment operator for the existing element if there is one.
  /// @tparam OtherKey The key type. Must satisfy <c>decays_to_v<K, Key> || transparent_comparison_supported<K></c> and
  ///         <c>std::is_constructible<Key, K&&></c>
  /// @tparam OtherMapped The type of the initializer for the mapped type
  /// @param key The key for which to insert or assign an element
  /// @param mapped_value The value with which to construct or assign the mapped value
  /// @return A pair of an iterator to the element with a key equivalent to @c key, and a boolean with a value of
  ///         @c true if the element was inserted, or @c false otherwise.
  /// @pre @c contains(key) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons, the move constructor of @c Key or the constructor or assignment
  /// operator of @c Value .
  template <
      typename OtherKey,
      typename OtherMapped,
      constraints<
          std::enable_if_t<transparent_insertion_supported<OtherKey>>,
          std::enable_if_t<std::is_constructible<Key, OtherKey&&>::value>,
          std::enable_if_t<std::is_constructible<Value, OtherMapped&&>::value>,
          std::enable_if_t<std::is_assignable<Value&, OtherMapped&&>::value>> = nullptr>
  auto insert_or_assign(OtherKey&& key, OtherMapped&& mapped_value) -> std::pair<iterator, bool> {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_insert_or_assign(std::forward<OtherKey>(key), std::forward<OtherMapped>(mapped_value));
  }

  /// @brief Try to emplace a new element in the map if there is not already one with the corresponding key.
  ///
  /// Perfectly forwards the provided arguments into the constructor of the key and value if an appropriate element does
  /// not already exist.
  /// @tparam Args The types of the initializers for the mapped type. Must satisfy <c>std::is_constructible<Value,
  ///         Args&&...></c>.
  /// @tparam SfinaeKey The key type, used to delete this overload if the key is not copy/move constructible
  /// @param key The key for which to insert or assign an element
  /// @param args The values with which to construct the mapped value
  /// @return std::pair<iterator, bool> A pair whose second element will be @c true if the emplace happened. The first
  ///         element will be an iterator pointing to the element corresponding to @c key : the existing element if
  ///         the insert did not happen, or the new element if it did.
  /// @pre @c contains(key) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons or constructors of @c Key and @c Value .
  template <
      typename... Args,
      typename SfinaeKey = Key,
      constraints<
          std::enable_if_t<std::is_copy_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_constructible<Value, Args&&...>::value>> = nullptr>
  auto try_emplace(Key const& key, Args&&... args) -> std::pair<iterator, bool> {
    return do_try_emplace(key, std::forward<Args>(args)...);
  }
  /// @brief Try to emplace a new element in the map if there is not already one with the corresponding key.
  ///
  /// Perfectly forwards the provided arguments into the constructor of the key and value if an appropriate element does
  /// not already exist.
  /// @tparam Args The types of the initializers for the mapped type. Must satisfy <c>std::is_constructible<Value,
  ///         Args&&...></c>.
  /// @tparam SfinaeKey The key type, used to delete this overload if the key is not copy/move constructible
  /// @param key The key for which to insert or assign an element
  /// @param args The values with which to construct the mapped value
  /// @return std::pair<iterator, bool> A pair whose second element will be @c true if the emplace happened. The first
  ///         element will be an iterator pointing to the element corresponding to @c key : the existing element if
  ///         the insert did not happen, or the new element if it did.
  /// @pre @c contains(key) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons or constructors of @c Key and @c Value .
  template <
      typename... Args,
      typename SfinaeKey = Key,
      constraints<
          std::enable_if_t<std::is_move_constructible<SfinaeKey>::value>,
          std::enable_if_t<std::is_constructible<Value, Args&&...>::value>> = nullptr>
  auto try_emplace(Key&& key, Args&&... args) -> std::pair<iterator, bool> {
    return do_try_emplace(std::move(key), std::forward<Args>(args)...);
  }
  /// @brief Try to emplace a new element in the map if there is not already one with an equivalent key.
  ///
  /// Perfectly forwards the provided arguments into the constructor of the key and value if an appropriate element does
  /// not already exist.
  /// @tparam Args The types of the initializers for the mapped type. Must satisfy <c>std::is_constructible<Value,
  ///         Args&&...></c>.
  /// @tparam OtherKey The key type. Must satisfy <c>decays_to_v<K, Key> || transparent_comparison_supported<K></c> and
  ///         <c>std::is_constructible<Key, K&&></c>
  /// @param key The key for which to insert or assign an element
  /// @param args The values with which to construct the mapped value
  /// @return std::pair<iterator, bool> A pair whose second element will be @c true if the emplace happened. The first
  ///         element will be an iterator pointing to the element corresponding to @c key : the existing element if
  ///         the insert did not happen, or the new element if it did.
  /// @pre @c contains(key) is @c true, or @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  /// @throws Any exception thrown by the comparisons or constructors of @c Key and @c Value .
  template <
      typename OtherKey,
      typename... Args,
      constraints<
          std::enable_if_t<transparent_insertion_supported<OtherKey>>,
          std::enable_if_t<!decays_to_v<OtherKey, Key>>,
          std::enable_if_t<std::is_constructible<Key, OtherKey&&>::value>,
          std::enable_if_t<std::is_constructible<Value, Args&&...>::value>> = nullptr>
  auto try_emplace(OtherKey&& key, Args&&... args) -> std::pair<iterator, bool> {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_try_emplace(std::forward<OtherKey>(key), std::forward<Args>(args)...);
  }

  /// @brief Construct a new @c value_type object and try to insert it in the map if
  /// there is not already one with the corresponding key.
  ///
  /// Perfectly forwards the provided arguments into the constructor
  /// of the new element if an appropriate element does not already
  /// exist.
  /// @tparam Args The types of the initializers for the value type
  /// @param args The values with which to construct the value value
  /// @return A pair of an iterator to the element with a key equivalent to @c
  /// key, and a boolean with a value of @c true if the element was
  /// inserted, or @c false otherwise.
  /// @throws Any exception thrown by the comparisons, the constructor
  /// of @c value_type
  /// @pre @c size()<Capacity or there is already a corresponding element in the map, else @c ARENE_PRECONDITION
  /// violation.
  template <
      typename... Args,
      constraints<std::enable_if_t<std::is_constructible<value_type, Args&&...>::value>> = nullptr>
  auto emplace(Args&&... args) -> std::pair<iterator, bool> {
    if (size_ == Capacity) {
      inline_map::value_type temp{std::forward<Args>(args)...};
      auto const existing_element = find(temp.first);
      ARENE_PRECONDITION(existing_element != end());
      return {existing_element, false};
    }
    auto const new_index = indices_[size_];
    auto& potentially_inserted_element = values_[new_index];
    potentially_inserted_element.emplace(std::forward<Args>(args)...);
    optional_resetter<value_type> destroy_if_not_inserted{potentially_inserted_element};
    auto const pos_info = lower_bound_index(potentially_inserted_element->first);
    bool const do_insert{pos_info.order != strong_ordering::equal};
    if (do_insert) {
      insert_index_at(pos_info.index, new_index);
      destroy_if_not_inserted.dismiss();
    }
    return {make_iterator(*this, pos_info.index), do_insert};
  }

  /// @brief Erase the element with the corresponding key, if there is one.
  ///
  /// @param key The key to erase.
  /// @return iterator An iterator which points to the position after the removed element.
  auto erase(Key const& key) noexcept(comparison_is_noexcept<>) -> iterator { return do_erase(key); }
  /// @brief Erase the element with the corresponding key, if there is one.
  ///
  /// @tparam OtherKey the type of the key to find. Must satisfy @c is_transparent_comparator_for<Compare,Key,K> or
  ///         @c is_transparent_three_way_comparator_for<Compare,Key,K> .
  /// @param key The key to erase.
  /// @return iterator An iterator which points to the position after the removed element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported<OtherKey>>> = nullptr>
  auto erase(OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    static_assert(
        transparent_comparison_supported_for<OtherKey>,
        "Compare must provide a valid comparison for (const Key&, const K&)"
    );
    return do_erase(key);
  }

  /// @brief Erase the element at the specified position.
  /// @param pos An iterator referring to the element to erase
  /// @return An iterator referring to the element after the erased element, or
  /// @c end() if there is no such element.
  /// @pre @c pos must be a valid dereferencable iterator referring to an element in @c *this, else @c
  /// ARENE_PRECONDITION violation.
  auto erase(iterator pos) noexcept -> iterator {
    ARENE_PRECONDITION(pos.is_iterator_for(this));
    ARENE_PRECONDITION(pos != end());
    return erase_at_index(pos.get_index(inline_map_passkey{}));
  }

  /// @brief Erase the element at the specified position.
  /// @param pos An iterator referring to the element to erase
  /// @return An iterator referring to the element after the erased element, or
  /// @c end() if there is no such element.
  /// @pre @c pos must be a valid dereferencable iterator referring to an element in @c *this, else @c
  /// ARENE_PRECONDITION violation.
  auto erase(const_iterator pos) noexcept -> iterator {
    ARENE_PRECONDITION(pos.is_iterator_for(this));
    ARENE_PRECONDITION(pos != end());
    return erase_at_index(pos.get_index(inline_map_passkey{}));
  }

  /// @brief Erase all the elements in the specified iterator range
  /// @param first An iterator referring to the start of the range to erase
  /// @param last An iterator referring to the end of the range to erase
  /// @return An iterator referring to the element after the erased elements,
  /// or @c end() if there is no such element.
  /// @pre @c [first,last) must be a valid iterator range referring to @c *this, else @c ARENE_PRECONDITION violation.
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  auto erase(const_iterator first, const_iterator last) noexcept -> iterator {
    ARENE_PRECONDITION(first.is_iterator_for(this));
    ARENE_PRECONDITION(last.is_iterator_for(this));
    ARENE_PRECONDITION(arene::base::distance(first, last) >= difference_type{});
    auto const first_index = first.get_index(inline_map_passkey{});
    auto const last_index = last.get_index(inline_map_passkey{});
    return erase_index_range(first_index, static_cast<index_type>(last_index - first_index));
  }

  /// @brief Destroy all elements in @c *this and set the size to zero.
  /// @post @c size()==0 .
  /// @post The destructors of all elements in the range @c [begin(),end()) will have been called once.
  void clear() noexcept {
    for (index_type index{0U}; index < size_; ++index) {
      entry_at_index(index).reset();
    }
    size_ = 0U;
  }

  /// @brief Obtain an iterator referring to the beginning of the sorted range of elements.
  /// @return iterator Points to the first element in the sequence, or @c end() if @c empty() .
  ARENE_NODISCARD auto begin() noexcept -> iterator { return make_iterator(*this, 0U); }
  /// @brief Obtain an iterator referring to the beginning of the sorted range of elements.
  /// @return const_iterator Points to the first element in the sequence, or @c end() if @c empty() .
  ARENE_NODISCARD auto begin() const noexcept -> const_iterator { return make_iterator(*this, 0U); }
  /// @brief Obtain an iterator referring to the beginning of the sorted range of elements.
  /// @return const_iterator Points to the first element in the sequence, or @c end() if @c empty() .
  ARENE_NODISCARD auto cbegin() const noexcept -> const_iterator { return make_iterator(*this, 0U); }

  /// @brief Obtain an iterator referring to one past the last element in the sorted range of elements.
  /// @return iterator An iterator pointing to one past the last element in the sequence.
  ARENE_NODISCARD auto end() noexcept -> iterator { return make_iterator(*this, size_); }
  /// @brief Obtain an iterator referring to one past the last element in the sorted range of elements.
  /// @return const_iterator Points to one past the last element in the sequence.
  ARENE_NODISCARD auto end() const noexcept -> const_iterator { return make_iterator(*this, size_); }
  /// @brief Obtain an iterator referring to one past the last element in the sorted range of elements.
  /// @return const_iterator Points to one past the last element in the sequence.
  ARENE_NODISCARD auto cend() const noexcept -> const_iterator { return make_iterator(*this, size_); }

  /// @brief Obtain an iterator referring to the beginning of a reversed traversal of the sorted range of elements.
  /// @return reverse_iterator Points to the last element in the sequence, or @c rend() if @c empty() .
  ARENE_NODISCARD auto rbegin() noexcept -> reverse_iterator { return reverse_iterator(end()); }
  /// @brief Obtain an iterator referring to the beginning of a reversed traversal of the sorted range of elements.
  /// @return const_reverse_iterator Points to the last element in the sequence, or @c rend() if @c empty() .
  ARENE_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(end()); }
  /// @brief Obtain an iterator referring to the beginning of a reversed traversal of the sorted range of elements.
  /// @return const_reverse_iterator Points to the last element in the sequence, or @c rend() if @c empty() .
  ARENE_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(end()); }

  /// @brief Obtain an iterator referring to one past the last element in a reversed traversal of the sorted range of
  ///        elements.
  /// @return reverse_iterator An iterator pointing to one before the first element in the sequence.
  ARENE_NODISCARD auto rend() noexcept -> reverse_iterator { return reverse_iterator(begin()); }
  /// @brief Obtain an iterator referring to one past the last element in a reversed traversal of the sorted range of
  ///        elements.
  /// @return const_reverse_iterator An iterator pointing to one before the first element in the sequence.
  ARENE_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator(begin()); }
  /// @brief Obtain an iterator referring to one past the last element in a reversed traversal of the sorted range of
  ///        elements.
  /// @return const_reverse_iterator An iterator pointing to one before the first element in the sequence.
  ARENE_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return const_reverse_iterator(begin()); }

  /// @brief Fast inequality heuristic to shortcut on different sized containers.
  ///
  /// @tparam OtherCapacity the capacity of @c rhs
  /// @param lhs The left-hand operand to the check
  /// @param rhs The right-hand operand to the check
  /// @return inequality_heuristic::may_be_equal_or_not_equal if @c lhs.size()==rhs.size()
  /// @return inequality_heuristic::definitely_not_equal if @c lhs.size()!=rhs.size()
  template <size_type OtherCapacity>
  ARENE_NODISCARD static auto
  fast_inequality_check(inline_map const& lhs, inline_map<Key, Value, OtherCapacity, Compare> const& rhs) noexcept
      -> inequality_heuristic {
    return lhs.size() == rhs.size() ? inequality_heuristic::may_be_equal_or_not_equal
                                    : inequality_heuristic::definitely_not_equal;
  }

  /// @brief Compares two maps lexicographically .
  ///
  /// @note Lexicographic comparison between maps ignores the order of keys as defined by @c Compare . That is, it is
  /// a lexicographic comparison between the @c value_type elements of each container, in iteration order, using the
  /// standard @c compare_three_way facility. This means that for all possible maps which result in element sequences
  /// <c>{{1,"a"}, {2, "b"}, {3,"c"}}</c> and <c>{{1,"a"}, {2, "b"}, {5,"d"}}</c>, when compared against each other
  /// the result of the comparison will always be the same, @c strong_ordering::less.
  ///
  /// @tparam OtherCapacity the capacity of @c rhs
  /// @tparam SfinaeKey dummy template parameter to disable the operator if @c key_Type is not three way comparable.
  /// @tparam SfinaeMappedType dummy template parameter to disable the operator if @c mapped_type is not three way
  /// comparable.
  /// @param lhs The left hand operand to the comparison.
  /// @param rhs The right hand operand to the comparison.
  /// @return strong_ordering Equivalent to
  ///         @c arene::base::lexicographical_compare_three_way(lhs.begin(),lhs.end(),rhs.begin(),rhs.end())
  template <
      size_type OtherCapacity,
      typename SfinaeKey = key_type,
      typename SfinaeMappedType = mapped_type,
      constraints<
          std::enable_if_t<compare_three_way_supported_v<SfinaeKey>>,
          std::enable_if_t<compare_three_way_supported_v<SfinaeMappedType>>> = nullptr>
  ARENE_NODISCARD static auto
  three_way_compare(inline_map const& lhs, inline_map<Key, Value, OtherCapacity, Compare> const& rhs) noexcept
      -> strong_ordering {
    return ::arene::base::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Compares two maps for equality of _elements_ when @c mapped_value is not three-way-comparable.
  ///
  ///
  /// @tparam OtherCapacity the capacity of @c rhs
  /// @tparam SfinaeValue dummy template parameter to disable the operator if @c value_type is not equality comparable.
  /// @tparam SfinaeKey dummy template parameter to enable the operator if @c key_type is not three way comparable.
  /// @tparam SfinaeMappedType dummy template parameter to enable the operator if @c mapped_type is not three way
  /// comparable.
  /// @param lhs The left hand operand to the comparison.
  /// @param rhs The right hand operand to the comparison.
  /// @return bool Equivalent to @c arene::base::equal(lhs.begin(),lhs.end(),rhs.begin(),rhs.end())
  template <
      size_type OtherCapacity,
      typename SfinaeValue = value_type,
      constraints<std::enable_if_t<is_equality_comparable_v<SfinaeValue>>> = nullptr>
  ARENE_NODISCARD friend auto
  operator==(inline_map const& lhs, inline_map<Key, Value, OtherCapacity, Compare> const& rhs) noexcept -> bool {
    return (inline_map::fast_inequality_check(lhs, rhs) == inequality_heuristic::may_be_equal_or_not_equal) &&
           ::arene::base::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Compares two maps for inequality of _elements_ when @c mapped_value is not three-way-comparable.
  ///
  /// @tparam OtherCapacity the capacity of @c rhs
  /// @tparam SfinaeValue dummy template parameter to disable the operator if @c value_type is not equality comparable.
  /// @tparam SfinaeKey dummy template parameter to enable the operator if @c key_Type is not three way comparable.
  /// @tparam SfinaeMappedType dummy template parameter to enable the operator if @c mapped_type is not three way
  /// comparable.
  /// @param lhs The left hand operand to the comparison.
  /// @param rhs The right hand operand to the comparison.
  /// @return bool Equivalent to @c !arene::base::equal(lhs.begin(),lhs.end(),rhs.begin(),rhs.end())
  template <
      size_type OtherCapacity,
      typename SfinaeValue = value_type,
      constraints<std::enable_if_t<is_equality_comparable_v<SfinaeValue>>> = nullptr>
  ARENE_NODISCARD friend auto
  operator!=(inline_map const& lhs, inline_map<Key, Value, OtherCapacity, Compare> const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

 private:
  /// @brief The type of the storage for each entry
  using entry_type = optional<value_type>;

  /// @brief Insert the values from @c init_list into the map.
  /// @param init_list The elements to insert
  /// @throws Any exception thrown by the copy constructor of the keys or values, or the comparisons
  /// @return bool @c true if the values fit, @c false if there is insufficient capacity
  auto insert_elements_if_they_fit(std::initializer_list<value_type> const& init_list) noexcept(
      comparison_is_noexcept<> && std::is_nothrow_copy_constructible<Key>::value &&
      std::is_nothrow_copy_constructible<Value>::value
  ) -> bool {
    for (auto const& element : init_list) {
      auto pos_info = lower_bound_index(element.first);
      if (pos_info.order == strong_ordering::equal) {
        continue;
      }
      if (size() == Capacity) {
        return false;
      }
      std::ignore = insert_at(pos_info.index, element.first, element.second);
    }
    return true;
  }

  /// @brief Gets a reference to the entry corrsponding to a given index.
  /// @param index Index of the element in iteration order.
  /// @return A reference to the element for the given index.
  /// @pre @c index<indicies.size()
  auto entry_at_index(index_type index) noexcept -> entry_type& { return values_[indices_[index]]; }
  /// @brief Gets a reference to the entry corrsponding to a given index.
  /// @param index Index of the element in iteration order.
  /// @return A reference to the element for the given index.
  /// @pre @c index<indicies.size()
  auto entry_at_index(index_type index) const noexcept -> entry_type const& { return values_[indices_[index]]; }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Use of a friend function with
  // passkey improves encapsulation. It allows data members to remain private
  // and does not unnecessarily expose a public member function."

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

  /// @brief Gets the value at an index
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param index the index in the map
  /// @return reference to the value at the given index
  /// @note this is primarily used to provide minimal access to @c inline_map_reference_iterator
  ///
  template <typename Self, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  friend auto value_at_index(inline_map_passkey, Self& self, std::uint16_t index)
      -> conditional_t<std::is_const<std::remove_reference_t<Self>>::value, value_type const&, value_type&> {
    return *self.entry_at_index(static_cast<index_type>(index));
  }

  // parasoft-end-suppress AUTOSAR-M3_3_2-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  /// @brief Insert a new element at the specified position, constructed by perfectly forwarding the key and
  /// initialization arguments.
  ///
  /// @tparam OtherKey The type of the key to insert
  /// @tparam Args The types of the initialization arguments for the mapped type
  /// @param pos The position of the new element
  /// @param key The key for the element to insert
  /// @param args The initialization arguments for the mapped type
  /// @return A reference to the newly constructed element
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation.
  template <typename OtherKey, typename... Args>
  auto insert_at(index_type pos, OtherKey&& key, Args&&... args) -> entry_type& {
    ARENE_PRECONDITION(size_ < Capacity);
    index_type const new_index{indices_[size_]};
    auto& entry = values_[new_index];
    entry.emplace(
        std::piecewise_construct,
        std::tuple<OtherKey&&>(std::forward<OtherKey>(key)),
        std::tuple<Args&&...>(std::forward<Args&&>(args)...)
    );
    insert_index_at(pos, new_index);
    return entry;
  }

  /// @brief Insert a new index at the specified position
  /// @param pos The position for the new index
  /// @param new_index The new index value to insert
  void insert_index_at(index_type pos, index_type new_index) noexcept {
    for (index_type i{size_}; i > pos; --i) {
      indices_[i] = indices_[static_cast<index_type>(i - 1U)];
    }
    indices_[pos] = new_index;
    ++size_;
  }

  /// @brief Obtain the index of the first entry such that @c entry.first < @c key
  ///
  /// @tparam OtherKey the type of the key to search for
  /// @param key The key to search for
  /// @return inline_container::detail::lower_bound_index_info<index_type> The index of the element, and the
  ///         strong_ordering between that element and @c key . If @c key is greater than all elements, then the index
  ///         is @c size() and the ordering is @c strong_ordering::greater .
  /// @throws Any exception thrown by the comparisons
  template <typename OtherKey>
  auto lower_bound_index(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>)
      -> inline_container::detail::lower_bound_index_info<index_type> {
    auto const compare = [this](key_type const& lhs, OtherKey const& rhs) noexcept(comparison_is_noexcept<OtherKey>) {
      return this->key_ordering(lhs, rhs);
    };
    auto const accessor = [this](index_type const index) noexcept -> key_type const& {
      return this->entry_at_index(index)->first;
    };
    return inline_container::detail::lower_bound_index<key_type, OtherKey, index_type>(key, size_, compare, accessor);
  }

  /// @brief A type alias for the appropriate iterator for @c Self
  /// @tparam Self the type of the container
  template <typename Self>
  using self_iterator = iterator_impl<std::is_const<std::remove_reference_t<Self>>::value>;

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "The universal reference is used to detect the value category"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Self is a template parameter"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: this is not a move constructor"
  /// @brief Deduced-this helper for universally constructing iterators.
  ///
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param index The index to construct the iterator pointing to.
  /// @return auto An iterator of the correct constness for @c Self which points to @c index .
  template <typename Self, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto make_iterator(Self&& self, index_type index) noexcept -> self_iterator<Self> {
    return {inline_map_passkey{}, &self, index};
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  /// @brief Deduced-this helper that detemplitizes find iterator generation from key type.
  ///
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param lb_info An instance of the return from @c lower_bound_index()
  /// @return auto If @c lb_info.order==strong_ordering::equal , then an iterator constructed as if via @c
  /// make_iterator(self,lb_info.index) , otherwise @c self.end() .
  template <typename Self, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto make_find_iterator(Self&& self, inline_container::detail::lower_bound_index_info<index_type> lb_info)
      -> self_iterator<Self> {
    if (lb_info.order == strong_ordering::equal) {
      return make_iterator(std::forward<Self>(self), lb_info.index);
    }
    return self.end();
  }

  /// @brief Deduced-this helper implementation for @c find() which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the key to search for.
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param key The key to search for.
  /// @return The iterator at the position of @c value if it is found, else @c end() .
  template <typename Self, typename OtherKey, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto do_find(Self&& self, OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>)
      -> self_iterator<Self> {
    return make_find_iterator(std::forward<Self>(self), self.lower_bound_index(key));
  }

  /// @brief Helper implementation for @c contains which works for all comparators.
  ///
  /// @tparam OtherKey the type of the key to find
  /// @param key The value to search for
  /// @return true if @c find(key)!=end() .
  /// @return false if @c find(key)==end() .
  template <typename OtherKey>
  auto do_contains(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>) -> bool {
    return lower_bound_index(key).order == strong_ordering::equal;
  }

  /// @brief Helper implementation for @c count which works for all comparators.
  ///
  /// @tparam OtherKey the type of the key to find.
  /// @param key The value to search for.
  /// @return std::size_t @c 0 if the element was not in the map, else @c 1 .
  template <typename OtherKey>
  auto do_count(OtherKey const& key) const noexcept(comparison_is_noexcept<OtherKey>) -> std::size_t {
    return static_cast<std::size_t>(do_contains(key));
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "The universal reference is used to detect the value category"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False Positive: Self is a template parameter"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: this is not a move constructor"
  /// @brief Helper implementation for @c lower_bound which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the key to find.
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param key The key to find the lower bound for.
  /// @return iterator An iterator to the first element which is _not less than_ @c value.
  template <typename Self, typename OtherKey, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto do_lower_bound(Self&& self, OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>)
      -> self_iterator<Self> {
    return make_iterator(self, self.lower_bound_index(key).index);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  /// @brief Deduced-this helper that detemplitizes upper_bound iterator generation from key type.
  ///
  /// @tparam Self Deduced-this type of the map.
  /// @param self The instance of the map to operate on.
  /// @param lb_info An instance of the return from @c lower_bound_index()
  /// @return auto If @c lb_info.order==strong_ordering::equal , then an iterator constructed as if via @c
  /// make_iterator(self,lb_info.index + 1) , otherwise @c make_iterator(self,lb_info.index) .
  template <typename Self, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto
  make_upper_bound_iterator(Self&& self, inline_container::detail::lower_bound_index_info<index_type> lb_info) noexcept
      -> self_iterator<Self> {
    if (lb_info.order == strong_ordering::equal) {
      ++lb_info.index;
    }
    return make_iterator(std::forward<Self>(self), lb_info.index);
  }

  /// @brief Helper implementation for @c upper_bound which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the key to find
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param key The key to search for.
  /// @return An iterator to the first element which is _greater than_ @c key , or @c end() if there is no such
  ///         element.
  template <typename Self, typename OtherKey, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto do_upper_bound(Self&& self, OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>)
      -> self_iterator<Self> {
    return make_upper_bound_iterator(std::forward<Self>(self), self.lower_bound_index(key));
  }

  /// @brief Deduced-this helper that detemplitizes equal_range iterator generation from key type.
  ///
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param lb_info An instance of the return from @c lower_bound_index()
  /// @return auto A pair of iterators where the first element is @c make_find_iterator(self,lb_info) , and the second
  /// element is either that iterator incremented by one if the iterator was not @c self.end() , otherwise @c self.end()
  template <typename Self, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto
  make_equal_range_iterator(Self&& self, inline_container::detail::lower_bound_index_info<index_type> lb_info)
      -> std::pair<self_iterator<Self>, self_iterator<Self>> {
    auto first = make_find_iterator(std::forward<Self>(self), lb_info);
    auto last = first == self.end() ? first : ::arene::base::next(first);
    return std::make_pair(first, last);
  }

  /// @brief Helper implementation for @c equal_range which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the value to find
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param key The value to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all values in the range @c [first,second)
  ///         compare equal to @c value if there was an element equivalent to @c value in the set. Otherwise both
  ///         iterators will be @c end() .
  template <typename Self, typename OtherKey, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  static auto do_equal_range(Self&& self, OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>)
      -> std::pair<self_iterator<Self>, self_iterator<Self>> {
    return make_equal_range_iterator(std::forward<Self>(self), self.lower_bound_index(key));
  }

  // parasoft-begin-suppress AUTOSAR-A7_5_1-a-2 "False positive: Returns reference to element"
  /// @brief Helper implementation for @c operator[] which works for all comparators.
  ///
  /// @tparam OtherKey the type of the key to find.
  /// @param key The key to obtain the value for
  /// @return Value& A reference to the mapped value. If @c key did not exist in the map, then a default constructed
  /// @c Value is inserted into the map at @c key and a reference to it is returned.
  template <typename OtherKey>
  auto do_operator_index(OtherKey const& key) -> Value& {
    auto pos_info = lower_bound_index(key);
    if (pos_info.order == strong_ordering::equal) {
      return entry_at_index(pos_info.index)->second;
    }
    return insert_at(pos_info.index, key)->second;
  }
  // parasoft-end-suppress AUTOSAR-A7_5_1-a-2

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "The universal reference is used to detect the value category"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "The universal reference is used to detect the value category"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Self is a template parameter"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: this is not a move constructor"
  /// @brief Helper implementation for @c at which works for all comparators and const-ness.
  ///
  /// @tparam OtherKey the type of the key to find
  /// @tparam Self Deduced-this type of the map.
  /// @param self The map
  /// @param key The key to obtain the value for
  /// @return A reference to the mapped value
  /// @throws std::out_of_range if @c find(key)==end() .
  /// @throws Any exception thrown by the comparisons
  template <typename Self, typename OtherKey, constraints<std::enable_if_t<decays_to_v<Self, inline_map>>> = nullptr>
  ARENE_NODISCARD static auto do_at(Self&& self, OtherKey const& key)
      -> conditional_t<std::is_const<remove_reference_t<Self>>::value, mapped_type const&, mapped_type&> {
    auto maybe_element = do_find(self, key);
    if (maybe_element == self.end()) {
      inline_map_detail::throw_out_of_range();
    }
    return maybe_element->second;
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  /// @brief Helper for @c insert_or_assign which works for all comparators and key types.
  ///
  /// Perfectly forwards the provided key and value into the constructor of the new element if an appropriate element
  /// does not already exist; perfectly forwards into the assignment operator for the existing element if there is one.
  /// @tparam OtherKey The key type.
  /// @tparam OtherMapped The type of the initializer for the mapped type
  /// @param key The key for which to insert or assign an element
  /// @param mapped_value The value with which to construct or assign the mapped value
  /// @return A pair of an iterator to the element with a key equivalent to @c key, and a boolean with a value of
  ///         @c true if the element was inserted, or @c false otherwise.
  /// @throws Any exception thrown by the comparisons, the move constructor of @c Key or the constructor or assignment
  /// operator of @c Value .
  /// @throws std::length_error if there is insufficient space to insert a new element.
  template <typename OtherKey, typename OtherMapped>
  auto do_insert_or_assign(OtherKey&& key, OtherMapped&& mapped_value) -> std::pair<iterator, bool> {
    auto res = do_try_emplace(std::forward<OtherKey>(key), std::forward<OtherMapped>(mapped_value));
    if (!res.second) {
      res.first->second = std::forward<OtherMapped>(mapped_value);
    }
    return res;
  }

  /// @brief Helper for @c try_emplace which works for all comparators and keys.
  ///
  /// Perfectly forwards the provided arguments into the constructor of the key and value if an appropriate element does
  /// not already exist.
  /// @tparam Args The types of the initializers for the mapped type.
  /// @tparam OtherKey The key type.
  /// @param key The key for which to insert or assign an element
  /// @param args The values with which to construct the mapped value
  /// @return std::pair<iterator, bool> A pair whose second element will be @c true if the emplace happened. The first
  ///         element will be an iterator pointing to the element corresponding to @c key : the existing element if
  ///         the insert did not happen, or the new element if it did.
  /// @throws std::length_error if there is insufficient space to insert a new element.
  /// @throws Any exception thrown by the comparisons or constructors of @c Key and @c Value .
  template <typename OtherKey, typename... Args>
  auto do_try_emplace(OtherKey&& key, Args&&... args) -> std::pair<iterator, bool> {
    auto const pos_info = lower_bound_index(key);
    bool const needs_insert{pos_info.order != strong_ordering::equal};
    if (needs_insert) {
      std::ignore = insert_at(pos_info.index, std::forward<OtherKey>(key), std::forward<Args>(args)...);
    }
    // parasoft-begin-suppress AUTOSAR-A8_4_2-a CERT_C-MSC37-a CERT_CPP-MSC52-a "False positive: returns a value"
    return {make_iterator(*this, pos_info.index), needs_insert};
    // parasoft-end-suppress AUTOSAR-A8_4_2-a CERT_C-MSC37-a CERT_CPP-MSC52-a
  }

  /// @brief helper for @c erase(key) which works for all comparators.
  ///
  /// @tparam OtherKey The type of the key to find
  /// @param key The key to erase.
  /// @return iterator An iterator which points to the position after the removed element.
  template <typename OtherKey>
  auto do_erase(OtherKey const& key) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    return erase_at_index(do_find(*this, key).get_index(inline_map_passkey{}));
  }

  /// @brief Erase the element at the specified index.
  ///
  /// @param index The index of the element to erase
  /// @return An iterator to the element after the erased element, or @c end() if @c index>=size_.
  auto erase_at_index(index_type index) noexcept -> iterator {
    return index < size_ ? erase_index_range(index, 1U) : end();
  }

  /// @brief Erase the elements in the specified index range.
  ///
  /// @param from The position in the index of the first element to erase
  /// @param erase_count The number of elements to erase
  /// @return An iterator to the element after the specified elements, or @c end() if there is no such element.
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  auto erase_index_range(index_type from, index_type erase_count) noexcept -> iterator {
    // We used this odd syntax because GCC8/9 are convinced that from + erase_count results in integer promotion, even
    // though they're guaranteed to be the same type.
    auto const last_index = static_cast<index_type>(from + erase_count);
    for (auto index = from; index < last_index; ++index) {
      entry_at_index(index).reset();
    }
    std::ignore = arene::base::rotate(indices_.begin() + from, indices_.begin() + last_index, indices_.begin() + size_);
    size_ = static_cast<index_type>(size_ - (last_index - from));

    return make_iterator(*this, from);
  }

  /// @brief Move-construct a new element from the specified element, and clear the old
  /// element.
  /// @param other The element to move from
  /// @return The new element
  /// @throws Any exception thrown by the copy-constructor of @c Key or the
  /// move-constructor of @c Value
  static auto move_value(entry_type& other
  ) noexcept(std::is_nothrow_copy_constructible<Key>::value && std::is_nothrow_move_constructible<Value>::value)
      -> entry_type {
    entry_type res{std::move(other)};
    other.reset();
    return res;
  }

  /// @brief Create a new array of elements which is a move-constructed copy of the
  /// elements from the provided map.
  /// @tparam Indices The indices of the elements from @c 0 to @c Capacity-1
  /// @param other The source map
  /// @return A new value array
  /// @throws Any exception thrown by the copy-constructor of @c Key or the
  /// move-constructor of @c Value
  template <std::size_t... Indices>
  static auto move_values(inline_map& other, std::index_sequence<Indices...>) noexcept(
      std::is_nothrow_move_constructible<value_type>::value
  ) -> array<entry_type, Capacity> {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a-2 "False positive: RAII class"
    auto const clear_on_exit = clear_on_scope_exit(other);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a-2
    array<entry_type, Capacity> res{{move_value(other.values_[Indices])...}};
    return res;
  }

  // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Return type cannot be named"
  /// @brief Helper to create a scope-guard which clears the input source on scope exit.
  ///
  /// @param source The container to clear on scope exit.
  /// @return A @c scope_guard which will call @c clear() on @c source .
  template <std::size_t OtherCapacity>
  static auto clear_on_scope_exit(inline_map<Key, Value, OtherCapacity, Compare>& source) noexcept {
    return on_scope_exit([&source]() noexcept {  //
      source.clear();
    });
  }
  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: Uses this"
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Return type cannot be named"
  /// @brief Helper to create a scope-guard which resets all values between the current size_ and an index.
  ///
  /// @param end_index One past the last index to reset to.
  /// @return A @c scope_guard which will call @c reset() on every element fetched via @c entry_at_index() in the range
  /// @c [size_,end) .
  auto reset_trailing_values_on_scope_exit(index_type const end_index) noexcept {
    return on_scope_exit([this, end_index]() noexcept {
      for (index_type idx = this->size_; idx < end_index; ++idx) {
        this->entry_at_index(idx).reset();
      }
    });
  }
  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  /// @brief Copy the elements from the other map over those in the current set.
  ///
  /// @tparam OtherMap The type of the map to copy from
  /// @param other The source map to copy from.
  /// @throws Any exception thrown by the copy of the elements or comparator
  /// @post @c size()==other.size() , or the index of the first element which throws during copy construction if a throw
  ///       occurs.
  /// @post @c The elements from @c other will have been copy-emplaced into @c this , up to the first element which
  ///       threw during move-construction, if any.
  /// @post @c The elements in the range @c [size(),other.size()) will have been reset if @c other.size() was smaller
  ///       than the original size of @c this .
  template <typename OtherMap>
  void do_copy(OtherMap const& other
  ) noexcept(std::is_nothrow_copy_constructible<OtherMap>::value && std::is_nothrow_copy_assignable<OtherMap>::value) {
    do_assignment(other);
  }

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'other' is used"
  // parasoft-begin-suppress AUTOSAR-M0_1_8-b "False positive: function 'do_move' is not empty"

  /// @brief Move the elements from the other map over those in the current map.
  ///
  /// @tparam OtherMap The type of the map to move from,
  /// @param other The source map to move from.
  /// @throws Any exception thrown by the move of the elements or comparator.
  /// @post @c size()==other.size() , or the index of the first element which throws during move construction if a throw
  ///       occurs.
  /// @post @c other.clear() will have been executed.
  /// @post @c The elements from @c other will have been move-emplaced into @c this , up to the first element which
  ///       threw during move-construction, if any.
  /// @post @c The elements in the range @c [size(),other.size()) will have been reset if @c other.size() was smaller
  ///       than the original size of @c this .
  template <typename OtherMap>
  void do_move(OtherMap&& other
  ) noexcept(std::is_nothrow_move_constructible<OtherMap>::value && std::is_nothrow_move_assignable<OtherMap>::value) {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a-2 "False positive: RAII class"
    auto const clear_on_exit = clear_on_scope_exit(other);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a-2
    do_assignment(std::forward<OtherMap>(other));
  }

  // parasoft-end-suppress AUTOSAR-A0_1_4-a
  // parasoft-end-suppress AUTOSAR-M0_1_8-b

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'other' is used"
  // parasoft-begin-suppress AUTOSAR-M0_1_8-b "False positive: function 'do_assignment' is not empty"

  /// @brief Helper for @c do_copy and @c do_move with non-copy/move dependent logic.
  ///
  /// @tparam OtherMap The type of the map to copy/move from
  /// @param other The source map top copy/move from.
  /// @throws Any exception thrown by the copy/move of the elements or comparator.
  /// @post @c size()==other.size() , or the index of the first element which throws during move construction if a throw
  ///       occurs.
  /// @post @c The elements from @c other will have been move-emplaced into @c this , up to the first element which
  ///       threw during copy/move-construction, if any.
  /// @post @c The elements in the range @c [size(),other.size()) will have been reset if @c other.size() was smaller
  ///       than the original size of @c this .
  template <typename OtherMap>
  void do_assignment(OtherMap&& other) noexcept(
      std::is_nothrow_constructible<OtherMap, OtherMap&&>::value &&
      std::is_nothrow_assignable<OtherMap&, OtherMap&&>::value
  ) {
    static_cast<comparator_base&>(*this) = std::forward<OtherMap>(other);
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a-2 "False positive: RAII class"
    auto const reset_trailing_values = reset_trailing_values_on_scope_exit(size_);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a-2
    size_ = 0U;
    while (size_ != other.size_) {
      this->entry_at_index(size_).emplace(forward_like<OtherMap>(*other.entry_at_index(size_)));
      ++size_;
    }
  }

  // parasoft-end-suppress AUTOSAR-A0_1_4-a
  // parasoft-end-suppress AUTOSAR-M0_1_8-b

  /// @brief The current number of elements in the map
  index_type size_{0};
  /// @brief The indices of the elements in sorted order
  array<index_type, Capacity> indices_{sequential_values<index_type, Capacity>};
  // parasoft-begin-suppress AUTOSAR-M8_5_2-b-2 "False Positive: all elements default-initialized"
  /// @brief The storage of the actual elements themselves
  array<entry_type, Capacity> values_{};
  // parasoft-end-suppress AUTOSAR-M8_5_2-b-2
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2
// parasoft-end-suppress AUTOSAR-A12_0_1-b-2
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-A1_1_1-b-2

template <typename Key, typename Value, std::size_t Capacity, typename Compare>
constexpr std::integral_constant<typename inline_map<Key, Value, Capacity, Compare>::size_type, Capacity>
    inline_map<Key, Value, Capacity, Compare>::capacity;

template <typename Key, typename Value, std::size_t Capacity, typename Compare>
constexpr std::integral_constant<typename inline_map<Key, Value, Capacity, Compare>::size_type, Capacity>
    inline_map<Key, Value, Capacity, Compare>::max_size;

namespace inline_container {
namespace detail {

/// @brief specialization for @c inline_map
/// @tparam Key key type of @c inline_map
/// @tparam Value mapped type of @c inline_map
/// @tparam Capacity capacity of @c inline_map
/// @tparam Compare compare type of @c inline_map
///
template <class Key, class Value, std::size_t Capacity, class Compare>
struct container_base_type<inline_map<Key, Value, Capacity, Compare>> {
  /// @brief container capacity-erased base type
  ///
  using type = inline_map_detail::inline_map_base<Key, Value, Compare>;
};

}  // namespace detail
}  // namespace inline_container

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_MAP_HPP_
