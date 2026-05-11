// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file set.hpp
/// @brief Provides the implementation of @c arene::base::inline_set .
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_SET_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_SET_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/lexicographical_compare.hpp"
#include "arene/base/algorithm/rotate.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/inline_container/detail/compare.hpp"
#include "arene/base/inline_container/detail/lower_bound_index.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/optional/optional_resetter.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_manipulation/smallest_integer_for.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/decays_to.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/forward_like.hpp"
#include "arene/base/utility/in_place.hpp"
#include "arene/base/utility/make_subrange.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// forward declaration
// IWYU pragma: begin_keep
template <typename Value, std::size_t Capacity, typename Compare>
class inline_set;
// IWYU pragma: end_keep

namespace inline_set_detail {
/// @brief Tag type to indicate the comparator for an inline_set
struct inline_set_comparator_tag {};

/// @brief Passkey type for iterator access.
class set_passkey {
 private:
  /// @brief Prevent construction from @c {}
  explicit set_passkey() = default;

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Passkey idiom permitted by A11-3-1 Permit #1"
  /// @brief friend declaration to allow inline_set to construct the passkey.
  template <typename Value, std::size_t Capacity, typename Compare>
  friend class ::arene::base::inline_set;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
};
}  // namespace inline_set_detail

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: delegating constructors are used"
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: 'generic_ordering_from_three_way_compare' is an
// interface"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False Positive: Copy operations are defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False Positive: Copy operations are defined or deleted as appropriate"
/// @brief A non-allocating fixed-capacity associative container storing elements of type @c Value .
///
/// Any attempt to store more than @c Capacity elements will result in an @c ARENE_PRECONDITION violation. The
/// comparison function specified with the @c Compare template parameter can either return @c bool, in which case it is
/// assumed to be a simple ordering comparison like @c std::less, or it can return @c strong_ordering, in which case it
/// is assumed to be a three-way comparison operator. The default comparison is @c three_way_compare.
/// @tparam Value The type of values stored in the set. Must satisfy @c std::is_nothrow_destructible .
/// @tparam Capacity The maximum number of elements that can be stored in the set .
/// @tparam Compare The comparison function used to impose a sort order on the elements. Must satisfy
///         @c std::is_nothrow_destructible as well as either
///         <c>is_invocable_r_v<bool, const Compare&, const Value&, const Value&></c> or
///         <c>is_invocable_r_v<strong_ordering, const Compare&, const Value&, const Value&></c>. The comparison must
///         provide a strict ordering over the values of @c Value , and copies or moves of the comparator must yield the
///         same ordering. Defaults to @c compare_three_way .
template <typename Value, std::size_t Capacity, typename Compare = compare_three_way>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class inline_set
    : inline_container::detail::compare_wrapper<Compare, Value>
    , generic_ordering_from_three_way_compare<inline_set<Value, Capacity, Compare>> {
  static_assert(std::is_nothrow_destructible<Value>::value, "Destructors must not throw");
  static_assert(std::is_nothrow_destructible<Compare>::value, "Destructors must not throw");
  static_assert(
      is_invocable_r_v<bool, Compare const&, Value const&, Value const&> ||
          is_invocable_r_v<strong_ordering, Compare const&, Value const&, Value const&>,
      "Compare must be invocable with two Values, and yield either a bool or a strong_ordering"
  );

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief helper trait for determining if the comparator supports transparent comparisons.
  /// @tparam OtherKey the type of the other operand to test for validity against.
  /// @return bool @c true If @c Compare satisfies either @c is_transparent_comparator_for_v or
  ///         @c is_transparent_three_way_comparator_for_v with @c Value and @c OtherKey , else @c false .
  template <typename OtherKey>
  static constexpr bool
      transparent_comparison_supported_for{::arene::base::is_transparent_comparator_for_v<Compare, Value const&, OtherKey const&> || ::arene::base::is_transparent_three_way_comparator_for_v<Compare, Value const&, OtherKey const&>};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The base class that holds the comparator
  using comparator_base = inline_container::detail::compare_wrapper<Compare, Value>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The base class that injects operator overloads.
  using operator_base = generic_ordering_from_three_way_compare<inline_set<Value, Capacity, Compare>>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Can comparisons throw?
  /// @tparam OtherKey The type of the rhs operand to a comparison.
  /// @return bool alias for @c comparator_base::template comparison_is_noexcept<OtherKey>
  template <typename OtherKey = Value>
  static constexpr bool comparison_is_noexcept{comparator_base::template comparison_is_noexcept<OtherKey>};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of each element in the index
  using index_type = smallest_unsigned_integer_for<Capacity>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of the storage for an individual element
  using entry_type = optional<Value>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The argument to the "move constructor": @c inline_set if the move constructor is supported, @c
  /// non_constructible_dummy otherwise
  using move_construct_source = conditional_t<
      std::is_move_constructible<Compare>::value && std::is_move_constructible<Value>::value,
      inline_set,
      non_constructible_dummy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The argument to the "move assignment operator": @c inline_set if the move assignment operator is supported,
  /// @c non_constructible_dummy otherwise
  using move_assign_source = conditional_t<
      std::is_move_assignable<Compare>::value && std::is_move_constructible<Value>::value,
      inline_set,
      non_constructible_dummy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The argument to the deleted "move assignment operator": @c inline_set if the move assignment operator is
  /// NOT supported, @c non_constructible_dummy otherwise
  using deleted_move_assign_source = conditional_t<
      std::is_move_assignable<Compare>::value && std::is_move_constructible<Value>::value,
      non_constructible_dummy,
      inline_set>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The argument to the "copy assignment operator": @c inline_set if the copy assignment operator is supported,
  /// @c non_constructible_dummy otherwise
  using copy_assign_source = conditional_t<
      std::is_copy_assignable<Compare>::value && std::is_copy_constructible<Value>::value,
      inline_set,
      non_constructible_dummy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The argument to the deleted "copy assignment operator": @c inline_set if the copy assignment operator is
  /// NOT supported, @c non_constructible_dummy otherwise
  using deleted_copy_assign_source = conditional_t<
      std::is_copy_assignable<Compare>::value && std::is_copy_constructible<Value>::value,
      non_constructible_dummy,
      inline_set>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief An iterator into an @c inline_set
  class iterator_impl : generic_ordering_from_three_way_compare<iterator_impl> {
    // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
    /// @brief The set
    inline_set const* set_{nullptr};
    /// @brief The index to the target element
    index_type element_index_{0};

   public:
    /// @brief The category of this iterator
    using iterator_category = std::bidirectional_iterator_tag;

    // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "Required type alias to meet iterator requirements"
    /// @brief The value type
    using value_type = Value;
    // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

    // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "Required type alias to meet iterator requirements"
    /// @brief The return type of @c operator->
    using pointer = Value const*;
    // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

    // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "Required type alias to meet iterator requirements"
    /// @brief The return type of @c operator*
    using reference = Value const&;
    // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

    // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "Required type alias to meet iterator requirements"
    /// @brief The type of the difference between two iterators
    using difference_type = std::ptrdiff_t;
    // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

    // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
    // initialize the base class"
    /// @brief A default constructed iterator has no associated set
    /// @post @c is_for(/*non-null*/) will return @c false .
    /// @post @c index()  will return @c 0 .
    iterator_impl() noexcept
        : iterator_impl(inline_set_detail::set_passkey{}, nullptr, 0U) {}
    // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

    // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
    /// @brief Construct an iterator for the specified set and element index
    ///
    /// @param set_to_iterate The set to refer to
    /// @param element The index into @c inline_set::indices_ of the element the iterator currently points to.
    /// @post @c is_for(set_to_iterate) will return @c true .
    /// @post @c index() will return @c element .
    iterator_impl(inline_set_detail::set_passkey, inline_set const* set_to_iterate, index_type element) noexcept
        : generic_ordering_from_three_way_compare<iterator>(),
          set_(set_to_iterate),
          element_index_(element) {}
    // parasoft-end-suppress CERT_C-EXP37-a-3

    /// @brief Get a reference to the target element
    ///
    /// @return reference A reference to the element at the current position.
    /// @pre @c *this!=iterator{} , else @c ARENE_PRECONDITION violation.
    /// @pre The iterator is in the valid range of the set, else @c ARENE_PRECONDITION violation.
    auto operator*() const noexcept -> reference {
      ARENE_PRECONDITION(set_ != nullptr);
      return *(set_->values_[set_->indices_[element_index_]]);
    }
    /// @brief Get a pointer to the target element
    ///
    /// @return pointer A pointer to the element at the current position.
    /// @pre @c *this!=iterator{} , else @c ARENE_PRECONDITION violation.
    /// @pre The iterator is in the valid range of the set, else @c ARENE_PRECONDITION violation.
    auto operator->() const noexcept -> pointer { return &**this; }

    /// @brief Pre-increment the iterator
    ///
    /// @return iterator& The iterator post-modification
    /// @post The iterator points to the next position in the set.
    auto operator++() noexcept -> iterator_impl& {
      ++element_index_;
      return *this;
    }

    // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
    // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
    /// @brief Post-increment the iterator
    ///
    /// @return iterator A copy of the iterator pre-modification
    /// @post The iterator points to the next position in the set.
    auto operator++(int) noexcept -> iterator_impl {
      iterator temp{*this};
      ++(*this);
      return temp;
    }
    // parasoft-end-suppress CERT_C-EXP37-a-3
    // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

    /// @brief Pre-decrement the iterator
    ///
    /// @return iterator& The iterator post-modification
    /// @post The iterator points to the previous position in the set.
    auto operator--() noexcept -> iterator_impl& {
      ARENE_PRECONDITION(element_index_ > 0U);
      --element_index_;
      return *this;
    }

    // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postdecrement requires an int parameter"
    // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
    /// @brief Post-increment the iterator
    ///
    /// @return iterator A copy of the iterator pre-modification
    /// @post The iterator points to the previous position in the set.
    auto operator--(int) noexcept -> iterator_impl {
      iterator temp{*this};
      --(*this);
      return temp;
    }
    // parasoft-end-suppress CERT_C-EXP37-a-3
    // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

    // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
    /// @brief Do three-way comparison on the provided iterators.
    ///
    /// @param lhs The first iterator to compare
    /// @param rhs The second iterator to compare
    /// @return @c strong_ordering::less if @c lhs comes before @c rhs in iteration order.
    /// @return @c strong_ordering::equal if @c lhs is the same as @c rhs in iteration order.
    /// @return @c strong_ordering::greater if @c lhs comes after @c rhs in iteration order.
    /// @pre The iterators must be constructed from the same @c inline_set instance, else @c ARENE_PRECONDITION
    /// violation.
    static auto three_way_compare(iterator_impl const lhs, iterator_impl const rhs) noexcept -> strong_ordering {
      ARENE_PRECONDITION(lhs.is_for(rhs.set_));
      return compare_three_way{}(lhs.element_index_, rhs.element_index_);
    }
    // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

    /// @brief check if iterators are from the same set.
    ///
    /// @param set_to_check The set to check for
    /// @return true if @c this->set_==set_to_check
    /// @return false otherwise
    auto is_for(inline_set const* set_to_check) const noexcept -> bool { return set_ == set_to_check; }

    // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
    /// @brief Get the position this iterator references within a set.
    /// Can only be called by members of @c inline_set
    /// @return index_type The index position of this iterator.
    auto index(inline_set_detail::set_passkey) const noexcept -> index_type { return element_index_; }
    // parasoft-end-suppress CERT_C-EXP37-a-3
  };

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of the value in the set
  using value_type = Value;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of the comparator for the value
  using value_compare = Compare;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of the key in the set, which is the same as the value
  using key_type = Value;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of the comparator for the key
  using key_compare = Compare;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief A pointer to the value
  using pointer = value_type*;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief A pointer to a const value
  using const_pointer = value_type const*;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief A reference to a value
  using reference = value_type&;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief A reference to a const value
  using const_reference = value_type const&;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of the size of the container
  using size_type = std::size_t;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The type of the difference between two iterators
  using difference_type = std::ptrdiff_t;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The iterator type for the set.
  using iterator = iterator_impl;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief A const iterator for the set: the same as @c iterator, since elements are @c const
  using const_iterator = iterator;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief A reverse iterator for the set
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief A const reverse iterator for the set.
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  /// @brief Construct a set with no elements and a default-constructed comparator
  inline_set() = default;

  /// @brief Default destructor; destroys all stored elements
  ~inline_set() = default;

  /// @brief Construct a set with no elements, using the specified comparator
  ///
  /// @param comp The comparator to use
  /// @post @c size()==0
  explicit inline_set(Compare const& comp) noexcept(std::is_nothrow_copy_constructible<Compare>::value)
      : comparator_base(comp),
        operator_base() {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  /// @brief Construct a set from the specified list of values.
  ///
  /// @param init The list of elements to store in the set
  /// @post @c size()==init.size()
  /// @post The elements in the set will be the unique elements of @c init , sorted via a default-constructed instance
  ///       of @c Compare .
  /// @pre @c init.size()<=max_size() else @c ARENE_PRECONDITION violation .
  inline_set(std::initializer_list<Value> init
  ) noexcept(std::is_nothrow_copy_constructible<Value>::value && std::is_nothrow_constructible<Compare>::value)
      : inline_set() {
    insert(init);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  /// @brief Construct a set from the specified list of values and a specified comparator instance .
  ///
  /// @param init The list of elements to store in the set .
  /// @param comp The comparator to use.
  /// @post @c size()==init.size()
  /// @post The elements in the set will be the unique elements of @c init , sorted via @c comp .
  /// @pre @c init.size()<=max_size() else @c ARENE_PRECONDITION violation .
  inline_set(std::initializer_list<Value> init, Compare const& comp) noexcept(
      std::is_nothrow_copy_constructible<Value>::value && std::is_nothrow_copy_constructible<Compare>::value
  )
      : inline_set(comp) {
    insert(init);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempt to construct a set from the specified list of values.
  ///
  /// @param init The list of elements to store in the set
  /// @return optional<inline_set> If the number of unique elements in @c init is less than or equal to @c Capacity ,
  /// then will return a populated optional equivalent to @c inline_set{init} . Otherwise returns a null optional.
  ARENE_NODISCARD static auto try_construct(std::initializer_list<Value> init
  ) noexcept(noexcept(inline_set{init}) && std::is_nothrow_move_constructible<inline_set>::value)
      -> optional<inline_set> {
    optional<inline_set> res{in_place};
    if (!res->insert_elements_if_they_fit(init.begin(), init.end())) {
      res.reset();
    }
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempt to construct a set from the specified list of values and a comparator.
  ///
  /// @param init The list of elements to store in the set
  /// @param comp The comparator to use.
  /// @return optional<inline_set> If the number of unique elements in @c init is less than or equal to @c Capacity ,
  /// then will return a populated optional equivalent to @c inline_set{init,comp} . Otherwise returns a null optional.
  ARENE_NODISCARD static auto try_construct(std::initializer_list<Value> init, Compare const& comp) noexcept(
      noexcept(inline_set{init, comp}) && std::is_nothrow_move_constructible<inline_set>::value
  ) -> optional<inline_set> {
    optional<inline_set> res{in_place, comp};
    if (!res->insert_elements_if_they_fit(init.begin(), init.end())) {
      res.reset();
    }
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  /// @brief Construct a set from the elements in the provided iterator range.
  ///
  /// @tparam Iterator The type of the iterators. Must satisfy
  ///         @c std::is_constructible<Value,std::iterator_traits<Iterator>::reference> .
  /// @param first The iterator to the first element in the range
  /// @param last The iterator to the one-past-the-end element in the range
  /// @post @c size() will be equal to the number of unique elements in the range @c [first,last)
  /// @post The elements in the set will be the unique elements in the range @c [first,last) , sorted via a
  ///       default-constructed instance of @c Compare .
  /// @pre The number of unique elements in the range @c [first,last) is less than or equal to @c max_size() , else @c
  /// ARENE_PRECONDITION violation.
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<std::is_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  inline_set(Iterator first, Iterator last) noexcept(
      std::is_nothrow_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value &&
      std::is_nothrow_constructible<Compare>::value
  )
      : inline_set() {
    insert(first, last);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  /// @brief Construct a set from the elements in the provided iterator range and comparator.
  ///
  /// @tparam Iterator The type of the iterators. Must satisfy
  ///         @c std::is_constructible<Value,std::iterator_traits<Iterator>::reference> .
  /// @param first The iterator to the first element in the range
  /// @param last The iterator to the one-past-the-end element in the range
  /// @param comp The comparator to use .
  /// @post @c size() will be equal to the number of unique elements in the range @c [first,last)
  /// @post The elements in the set will be the unique elements in the range @c [first,last) , sorted via @c comp .
  /// @pre The number of unique elements in the range @c [first,last) is less than or equal to @c max_size() , else @c
  /// ARENE_PRECONDITION violation.
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<std::is_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  inline_set(Iterator first, Iterator last, Compare const& comp) noexcept(
      std::is_nothrow_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value &&
      std::is_nothrow_copy_constructible<Compare>::value
  )
      : inline_set(comp) {
    insert(first, last);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempt to construct a set from the specified sequence of values.
  ///
  /// @tparam Iterator The type of the iterators. Must satisfy
  ///         @c std::is_constructible<Value,std::iterator_traits<Iterator>::reference> .
  /// @param first The iterator to the first element in the range
  /// @param last The iterator to the one-past-the-end element in the range
  /// @return optional<inline_set> If the number of unique elements in the range @c [first,last) is less than or equal
  /// to @c Capacity , then will return a populated optional equivalent to @c inline_set{first,last} . Otherwise returns
  /// a null optional.
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<std::is_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  ARENE_NODISCARD static auto try_construct(Iterator first, Iterator last) noexcept(
      noexcept(inline_set{first, last}) && std::is_nothrow_move_constructible<inline_set>::value
  ) -> optional<inline_set> {
    optional<inline_set> res{in_place};
    if (!res->insert_elements_if_they_fit(first, last)) {
      res.reset();
    }
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempt to construct a set from the specified sequence of values and a comparator.
  ///
  /// @tparam Iterator The type of the iterators. Must satisfy
  ///         @c std::is_constructible<Value,std::iterator_traits<Iterator>::reference> .
  /// @param first The iterator to the first element in the range
  /// @param last The iterator to the one-past-the-end element in the range
  /// @param comp The comparator to use .
  /// @return optional<inline_set> If the number of unique elements in the range @c [first,last) is less than or equal
  /// to @c Capacity , then will return a populated optional equivalent to @c inline_set{first,last} . Otherwise returns
  /// a null optional.
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<std::is_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  ARENE_NODISCARD static auto try_construct(Iterator first, Iterator last, Compare const& comp) noexcept(
      noexcept(inline_set{first, last, comp}) && std::is_nothrow_move_constructible<inline_set>::value
  ) -> optional<inline_set> {
    optional<inline_set> res{in_place, comp};
    if (!res->insert_elements_if_they_fit(first, last)) {
      res.reset();
    }
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief copy ctor.
  // NOLINTBEGIN(google-explicit-constructor,hicpp-explicit-conversions) These are copy ctors.
  inline_set(inline_set const&) = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief copy construct from a set with a smaller capacity
  ///
  /// Participates in overload resolution if @c Value and @c Compare satisfy @c is_copy_constructible
  ///
  /// @tparam OtherCapacity The capacity of the other set
  /// @param other The set to copy from.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       copy-construction.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity < Capacity)>,
          std::enable_if_t<std::is_copy_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  inline_set(inline_set<Value, OtherCapacity, Compare> const& other
  ) noexcept(std::is_nothrow_copy_constructible<Value>::value && std::is_nothrow_copy_constructible<Compare>::value)
      : comparator_base(other.get_comparator_base(inline_set_detail::set_passkey{})),
        operator_base() {
    do_copy(other);
  }

  /// @brief copy construct from a set with a larger capacity
  ///
  /// Participates in overload resolution if @c Value and @c Compare satisfy @c is_copy_constructible
  ///
  /// @tparam OtherCapacity The capacity of the other set
  /// @param other The set to copy from.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       copy-construction.
  /// @pre @c other.size()<=maximum_size() else @c ARENE_PRECONDITION violation.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_copy_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  inline_set(inline_set<Value, OtherCapacity, Compare> const& other
  ) noexcept(std::is_nothrow_copy_constructible<Value>::value && std::is_nothrow_copy_constructible<Compare>::value)
      : comparator_base(other.get_comparator_base(inline_set_detail::set_passkey{})),
        operator_base() {
    ARENE_PRECONDITION(other.size() <= max_size());
    do_copy(other);
  }
  // NOLINTEND(google-explicit-constructor,hicpp-explicit-conversions)

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempts to construct a set via copy-construction from a set with the same or smaller capacity.
  ///
  /// @tparam OtherCapacity The capacity of the other set
  /// @param other The set to copy from.
  /// @return A populated optional equivalent to @c inline_set{other}
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity <= Capacity)>,
          std::enable_if_t<std::is_copy_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_set<Value, OtherCapacity, Compare> const& other
  ) noexcept(noexcept(inline_set{other})) -> optional<inline_set> {
    return optional<inline_set>{in_place, other};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempts to construct a set from a set with a larger capacity.
  ///
  /// @tparam OtherCapacity The capacity of the other set
  /// @param other The set to copy from.
  /// @return optional<inline_set> If @c other.size()<=Capacity , then will return a populated optional equivalent to
  /// @c inline_set{other} . Otherwise returns a null optional.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_copy_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_set<Value, OtherCapacity, Compare> const& other
  ) noexcept(noexcept(inline_set{other})) -> optional<inline_set> {
    if (other.size() <= Capacity) {
      return optional<inline_set>{in_place, other};
    }
    return nullopt;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // NOLINTBEGIN(google-explicit-constructor,hicpp-explicit-conversions) clang-tidy doesn't see these as move ctors.
  /// @brief move ctor
  ///
  /// Participates in overload resolution if @c Value satisfies @c is_move_constructible
  ///
  /// @tparam OtherCapacity The maximum capacity of the other set to move from.
  /// @param other The set to move from
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       move-construction.
  inline_set(move_construct_source&& other
  ) noexcept(std::is_nothrow_move_constructible<Value>::value && std::is_nothrow_constructible<Compare>::value)
      : comparator_base(std::move(static_cast<comparator_base&>(other))),
        operator_base(),
        number_of_active_elements_(other.number_of_active_elements_),
        values_(move_values(other, std::make_index_sequence<Capacity>())),
        indices_(std::move(other.indices_)) {}
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  /// @brief move construct from a set with a larger capacity
  ///
  /// Participates in overload resolution if @c Value satisfies @c is_move_constructible
  ///
  /// @tparam OtherCapacity The maximum capacity of the other set to move from.
  /// @param other The set to move from
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       move-construction.
  /// @pre @c other.size()<=maximum_size() else @c ARENE_PRECONDITION violation.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_move_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  inline_set(inline_set<Value, OtherCapacity, Compare>&& other)
      : comparator_base(std::move(other.get_comparator_base(inline_set_detail::set_passkey{}))),
        operator_base() {
    ARENE_PRECONDITION(other.size() <= max_size());
    do_move(std::move(other));
  }
  /// @brief move construct from a set with a smaller capacity
  ///
  /// Participates in overload resolution if @c Value satisfies @c is_move_constructible
  ///
  /// @tparam OtherCapacity The maximum capacity of the other set to move from.
  /// @param other The set to move from
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       move-construction.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity < Capacity)>,
          std::enable_if_t<std::is_move_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  inline_set(inline_set<Value, OtherCapacity, Compare>&& other
  ) noexcept(std::is_nothrow_move_constructible<Value>::value && std::is_nothrow_move_constructible<Compare>::value)
      : comparator_base(std::move(other.get_comparator_base(inline_set_detail::set_passkey{}))),
        operator_base() {
    do_move(std::move(other));
  }
  /// @}
  // NOLINTEND(google-explicit-constructor,hicpp-explicit-conversions) clang-tidy doesn't see these as move ctors.

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempts to construct a set via move-construction from a set with a capacity that is less than or equal to
  /// the target capacity.
  ///
  /// @tparam OtherCapacity The capacity of the other set
  /// @param other The set to move from.
  /// @return A populated optional equivalent to @c inline_set{other} .
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity <= Capacity)>,
          std::enable_if_t<std::is_copy_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_set<Value, OtherCapacity, Compare>&& other
  ) noexcept(noexcept(inline_set{std::move(other)})) -> optional<inline_set> {
    return optional<inline_set>{in_place, std::move(other)};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Attempts to construct a set via move-construction from a set with a larger capacity.
  ///
  /// @tparam OtherCapacity The capacity of the other set
  /// @param other The set to move from.
  /// @return If @c other.size()<=Capacity , then will return a populated optional equivalent to @c inline_set{other} .
  /// Otherwise returns a null optional.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_copy_constructible<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  ARENE_NODISCARD static auto try_construct(inline_set<Value, OtherCapacity, Compare>&& other
  ) noexcept(noexcept(inline_set{other})) -> optional<inline_set> {
    if (other.size() <= Capacity) {
      return optional<inline_set>{in_place, std::move(other)};
    }
    return nullopt;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  /// @brief Deleted copy-assignment operator where the set is not copy-assignable
  auto operator=(deleted_copy_assign_source const& other) -> inline_set& = delete;
  /// @brief copy-assignment operator.
  ///
  /// Participates in overload resolution if @c Value satisfies @c std::is_copy_constructible and @c Compare satisfies
  /// @c is_copy_assignable
  ///
  /// @tparam OtherCapacity the capacity of the other set.
  /// @param other The set to copy-assign from.
  /// @return inline_set& The set post-assignment.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       copy-assignment/construction.
  /// @pre @c other.size()<=maximum_size() else @c ARENE_PRECONDITION violation.
  auto operator=(copy_assign_source const& other
  ) noexcept(std::is_nothrow_copy_constructible<Value>::value && std::is_nothrow_copy_assignable<Compare>::value)
      -> inline_set& {
    if (this != &other) {
      static_cast<comparator_base&>(*this) = other;
      do_copy(other);
    }
    return *this;
  }
  /// @brief copy-assignment operator for copying from a set with a larger capacity
  ///
  /// Participates in overload resolution if @c Value satisfies @c std::is_copy_constructible and @c Compare satisfies
  /// @c is_copy_assignable
  ///
  /// @tparam OtherCapacity the capacity of the other set.
  /// @param other The set to copy-assign from.
  /// @return inline_set& The set post-assignment.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       copy-assignment/construction.
  /// @pre @c other.size()<=maximum_size() else @c ARENE_PRECONDITION violation.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_copy_assignable<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  auto operator=(inline_set<Value, OtherCapacity, Compare> const& other
  ) noexcept(std::is_nothrow_copy_constructible<Value>::value && std::is_nothrow_copy_assignable<Compare>::value)
      -> inline_set& {
    ARENE_PRECONDITION(other.size() <= max_size());
    static_cast<comparator_base&>(*this) = other.get_comparator_base(inline_set_detail::set_passkey{});
    do_copy(other);
    return *this;
  }
  /// @brief copy-assignment operator for copying from a set with a smaller capacity
  ///
  /// Participates in overload resolution if @c Value satisfies @c std::is_copy_constructible and @c Compare satisfies
  /// @c is_copy_assignable
  ///
  /// @tparam OtherCapacity the capacity of the other set.
  /// @param other The set to copy-assign from.
  /// @return inline_set& The set post-assignment.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       copy-assignment/construction.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity < Capacity)>,
          std::enable_if_t<std::is_copy_assignable<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  auto operator=(inline_set<Value, OtherCapacity, Compare> const& other
  ) noexcept(std::is_nothrow_copy_assignable<inline_set<Value, OtherCapacity, Compare>>::value) -> inline_set& {
    static_cast<comparator_base&>(*this) = other.get_comparator_base(inline_set_detail::set_passkey{});
    do_copy(other);
    return *this;
  }
  /// @}

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief The move assignment operator is deleted if move assignment is not supported
  auto operator=(deleted_move_assign_source&&) -> inline_set& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  /// @brief move-assignment operator
  ///
  /// Participates in overload resolution if @c Value satisfies @c is_move_constructible, and @c Compare satisfies @c
  /// std::is_move_assignable
  ///
  /// @param other The set to move-assign from.
  /// @return inline_set& The set post-assignment.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       move-assignment/construction.
  /// @pre @c other.size()<=maximum_size() else @c ARENE_PRECONDITION violation.
  auto operator=(move_assign_source&& other
  ) noexcept(std::is_nothrow_move_constructible<Value>::value && std::is_nothrow_move_assignable<Compare>::value)
      -> inline_set& {
    if (this != &other) {
      static_cast<comparator_base&>(*this) = std::move(other);
      do_move(std::move(other));
    }
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  /// @brief move-assignment operator from an @c inline_set with a larger capacity
  ///
  /// Participates in overload resolution if @c Value satisfies @c is_move_constructible, and @c Compare satisfies @c
  /// std::is_move_assignable
  ///
  /// @tparam OtherCapacity The capacity of the source set
  /// @param other The set to move-assign from.
  /// @return inline_set& The set post-assignment.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       move-assignment/construction.
  /// @pre @c other.size()<=maximum_size() else @c ARENE_PRECONDITION violation.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity > Capacity)>,
          std::enable_if_t<std::is_move_assignable<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  auto operator=(inline_set<Value, OtherCapacity, Compare>&& other) -> inline_set& {
    ARENE_PRECONDITION(other.size() <= max_size());
    static_cast<comparator_base&>(*this) = std::move(other.get_comparator_base(inline_set_detail::set_passkey{}));
    do_move(std::move(other));
    return *this;
  }

  /// @brief move-assignment operator from an @c inline_set with a smaller capacity
  ///
  /// Participates in overload resolution if @c Value satisfies @c is_move_constructible, and @c Compare satisfies @c
  /// std::is_move_assignable
  ///
  /// @tparam OtherCapacity The capacity of the source set
  /// @param other The set to move-assign from.
  /// @return inline_set& The set post-assignment.
  /// @post @c size()==other.size()
  /// @post The elements in the set will be equivalent to the elements that were in @c other , produced as if via
  ///       move-assignment/construction.
  template <
      std::size_t OtherCapacity,
      constraints<
          std::enable_if_t<(OtherCapacity < Capacity)>,
          std::enable_if_t<std::is_move_assignable<inline_set<Value, OtherCapacity, Compare>>::value>> = nullptr>
  auto operator=(inline_set<Value, OtherCapacity, Compare>&& other
  ) noexcept(std::is_nothrow_move_assignable<inline_set<Value, OtherCapacity, Compare>>::value) -> inline_set& {
    static_cast<comparator_base&>(*this) = std::move(other.get_comparator_base(inline_set_detail::set_passkey{}));
    do_move(std::move(other));
    return *this;
  }
  /// @}

  /// @brief Check if the set is empty
  /// @return @c true if the set does not hold any elements, @c false otherwise
  ARENE_NODISCARD auto empty() const noexcept -> bool { return number_of_active_elements_ == 0U; }

  /// @brief Get the number of elements in the set
  /// @return The number of elements in the set
  auto size() const noexcept -> std::size_t { return number_of_active_elements_; }

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief The maximum number of elements that can be held in the set.
  ///
  /// @return size_t Equivalent to @c Capacity .
  static constexpr std::integral_constant<size_type, Capacity> capacity{};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Get the maximum number of elements in the set
  ///
  /// @return size_t Equivalent to @c Capacity .
  static constexpr std::integral_constant<size_type, Capacity> max_size{};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Get the comparator
  /// @return A copy of the comparator
  auto key_comp() const noexcept(std::is_nothrow_copy_constructible<Compare>::value) -> key_compare {
    return comparator_base::get_comparator();
  }

  /// @brief Get the comparator
  /// @return A copy of the comparator
  auto value_comp() const noexcept(std::is_nothrow_copy_constructible<Compare>::value) -> value_compare {
    return comparator_base::get_comparator();
  }

  /// @brief Obtain an iterator referring to the element equivalent to the supplied value, if there is one.
  ///
  /// @param value_to_find The value to find
  /// @return iterator to the equivalent element, or @c end() if the element is not found
  auto find(Value const& value_to_find) noexcept(comparison_is_noexcept<>) -> iterator {
    return do_find(value_to_find);
  }
  /// @brief Obtain an iterator referring to the element equivalent to the supplied value, if there is one.
  ///
  /// @param value_to_find The value to find
  /// @return const_iterator to the equivalent element, or @c end() if the element is not found
  auto find(Value const& value_to_find) const noexcept(comparison_is_noexcept<>) -> const_iterator {
    return do_find(value_to_find);
  }
  /// @brief Obtain an iterator referring to the element equivalent to the supplied value, if there is one.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to find
  /// @return iterator to the equivalent element, or @c end() if the element is not found
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto find(OtherKey const& value) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    return do_find(value);
  }
  /// @brief Obtain an iterator referring to the element equivalent to the supplied value, if there is one.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to find
  /// @return const_iterator to the equivalent element, or @c end() if the element is not found
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto find(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> const_iterator {
    return do_find(value);
  }

  /// @brief Check if the set contains a specific value
  ///
  /// @param value_to_check The value to search for
  /// @return true if @c find(value_to_check)!=end() .
  /// @return false if @c find(value_to_check)==end() .
  auto contains(Value const& value_to_check) const noexcept(comparison_is_noexcept<>) -> bool {
    return do_contains(value_to_check);
  }
  /// @brief Check if the set contains a value equivalent to a specific value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to search for
  /// @return true if @c find(value)!=end() .
  /// @return false if @c find(value)==end() .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto contains(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> bool {
    return do_contains(value);
  }

  /// @brief Return the number of elements in the set equivalent to a specific value.
  ///
  /// @param value_to_count The value to search for.
  /// @return std::size_t @c 0 if the element was not in the set, else @c 1 .
  auto count(Value const& value_to_count) const noexcept(comparison_is_noexcept<>) -> std::size_t {
    return do_count(value_to_count);
  }
  /// @brief Return the number of elements in the set equivalent to a specific value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to search for.
  /// @return std::size_t @c 0 if the element was not in the set, else @c 1 .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto count(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> std::size_t {
    return do_count(value);
  }

  /// @brief Find the first element which is _not less than_ a given value.
  ///
  /// @param value_to_find The value to find the lower bound for.
  /// @return iterator to the first element which is _not less than_ @c value_to_find, or @c end() if there is no
  ///         such element.
  auto lower_bound(Value const& value_to_find) noexcept(comparison_is_noexcept<>) -> iterator {
    return do_lower_bound(value_to_find);
  }
  /// @brief Find the first element which is _not less than_ a given value.
  ///
  /// @param value_to_find The value to find the lower bound for.
  /// @return const_iterator to the first element which is _not less than_ @c value_to_find, or @c end() if there is no
  ///         such element.
  auto lower_bound(Value const& value_to_find) const noexcept(comparison_is_noexcept<>) -> const_iterator {
    return do_lower_bound(value_to_find);
  }
  /// @brief Find the first element which is _not less than_ a given value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to find the lower bound for.
  /// @return iterator to the first element which is _not less than_ @c value , or @c end() if there is no
  ///         such element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto lower_bound(OtherKey const& value) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    return do_lower_bound(value);
  }
  /// @brief Find the first element which is _not less than_ a given value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to find the lower bound for.
  /// @return const_iterator to the first element which is _not less than_ @c value , or @c end() if there is no
  ///         such element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto lower_bound(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> const_iterator {
    return do_lower_bound(value);
  }

  /// @brief Find the first element which is _greater than_ a given value.
  ///
  /// @param value_to_find The value to search for.
  /// @return iterator to the first element which is _greater than_ @c value_to_find , or @c end() if there is no such
  ///         element.
  auto upper_bound(Value const& value_to_find) noexcept(comparison_is_noexcept<>) -> iterator {
    return do_upper_bound(value_to_find);
  }
  /// @brief Find the first element which is _greater than_ a given value.
  ///
  /// @param value_to_find The value to search for.
  /// @return const_iterator to the first element which is _greater than_ @c value_to_find , or @c end() if there is no
  /// such
  ///         element.
  auto upper_bound(Value const& value_to_find) const noexcept(comparison_is_noexcept<>) -> const_iterator {
    return do_upper_bound(value_to_find);
  }
  /// @brief Find the first element which is _greater than_ a given value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to search for.
  /// @return iterator to the first element which is _greater than_ @c value , or @c end() if there is no such
  ///         element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto upper_bound(OtherKey const& value) noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    return do_upper_bound(value);
  }
  /// @brief Find the first element which is _greater than_ a given value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to search for.
  /// @return const_iterator to the first element which is _greater than_ @c value , or @c end() if there is no such
  ///         element.
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto upper_bound(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> const_iterator {
    return do_upper_bound(value);
  }

  /// @brief Finds the sequence of values which compare equivalent to a given value.
  ///
  /// @param value_to_find The value to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all values in the range @c [first,second)
  ///         compare equal to @c value_to_find if there was an element equivalent to @c value_to_find in the set.
  ///         Otherwise both iterators will be @c end() .
  auto equal_range(Value const& value_to_find) noexcept(comparison_is_noexcept<>) -> std::pair<iterator, iterator> {
    return do_equal_range(value_to_find);
  }
  /// @brief Finds the sequence of values which compare equivalent to a given value.
  ///
  /// @param value_to_find The value to search for
  /// @return std::pair<const_iterator, const_iterator> A pair of iterators such that all values in the range
  ///         @c [first,second) compare equal to @c value_to_find if there was an element equivalent to @c value_to_find
  ///         in the set. Otherwise both iterators will be @c end() .
  auto equal_range(Value const& value_to_find) const noexcept(comparison_is_noexcept<>)
      -> std::pair<const_iterator, const_iterator> {
    return do_equal_range(value_to_find);
  }
  /// @brief Finds the sequence of values which compare equivalent to a given value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all values in the range @c [first,second)
  ///         compare equal to @c value if there was an element equivalent to @c value in the set. Otherwise both
  ///         iterators will be @c end() .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto equal_range(OtherKey const& value) noexcept(comparison_is_noexcept<OtherKey>) -> std::pair<iterator, iterator> {
    return do_equal_range(value);
  }
  /// @brief Finds the sequence of values which compare equivalent to a given value.
  ///
  /// @tparam OtherKey the type of the value to find. Must satisfy @c is_transparent_comparator_for<Compare,Value,K> or
  ///               @c is_transparent_three_way_comparator_for<Compare,Value,K> .
  /// @param value The value to search for
  /// @return std::pair<const_iterator, const_iterator> A pair of iterators such that all values in the range
  ///         @c [first,second) compare equal to @c value if there was an element equivalent to @c value in the set.
  ///         Otherwise both iterators will be @c end() .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto equal_range(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>)
      -> std::pair<const_iterator, const_iterator> {
    return do_equal_range(value);
  }

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  /// @brief Attempt to insert an element into the set.
  ///
  /// @param value_to_insert The element to insert via copy-construction
  /// @return std::pair<iterator,bool> A pair where the first element is an iterator pointing to either the newly
  ///         inserted element at its sort-order position in the set, or the existing element in the set if the set
  ///         already contained @c value_to_insert , and the second element is a boolean which is @c true if the
  ///         insertion happened, or @c false otherwise.
  /// @post If the insertion happened, @c size() increases by 1.
  /// @post If the insertion happened, there is an element equivalent to @c value_to_insert in the set at the position
  /// referenced
  ///       by the returned iterator.
  /// @pre @c size()+1<=max_size() , else @c ARENE_PRECONDITION violation .
  auto insert(Value const& value_to_insert
  ) noexcept(comparison_is_noexcept<> && std::is_nothrow_copy_constructible<Value>::value)
      -> std::pair<iterator, bool> {
    return internal_insert(value_to_insert);
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Attempt to insert an element into the set.
  ///
  /// @param value_to_insert The element to insert via move-construction
  /// @return std::pair<iterator,bool> A pair where the first element is an iterator pointing to either the newly
  ///         inserted element at its sort-order position in the set, or the existing element in the set if the set
  ///         already contained @c value_to_insert , and the second element is a boolean which is @c true if the
  ///         insertion happened, or @c false otherwise.
  /// @post If the insertion happened, @c size() increases by 1.
  /// @post If the insertion happened, there is an element equivalent to @c value_to_insert in the set at the position
  /// referenced
  ///       by the returned iterator.
  /// @pre @c size()+1<=max_size() , else @c ARENE_PRECONDITION violation .
  auto insert(Value&& value_to_insert
  ) noexcept(comparison_is_noexcept<> && std::is_nothrow_move_constructible<Value>::value)
      -> std::pair<iterator, bool> {
    return internal_insert(std::move(value_to_insert));
  }

  /// @brief Attempt to insert an element into the set.
  ///
  /// @tparam OtherKey The type of the element to attempt to insert. Must satisfy
  ///         @c std::is_constructible<Value,OtherKey&&>
  /// @param value The element to insert via perfect forwarding.
  /// @return std::pair<iterator,bool> A pair where the first element is an iterator pointing to either the newly
  ///         inserted element at its sort-order position in the set, or the existing element in the set if the set
  ///         already contained @c value , and the second element is a boolean which is @c true if the insertion
  ///         happened, or @c false otherwise.
  /// @post If the insertion happened, @c size() increases by 1.
  /// @post If the insertion happened, there is an element equivalent to @c value in the set at the position referenced
  ///       by the returned iterator.
  /// @pre @c size()+1<=max_size() , else @c ARENE_PRECONDITION violation .
  template <
      typename OtherKey,
      typename SfinaeKey = Value,
      constraints<
          std::enable_if_t<transparent_comparison_supported_for<OtherKey>>,
          std::enable_if_t<!std::is_integral<SfinaeKey>::value>,
          std::enable_if_t<std::is_constructible<Value, OtherKey&&>::value>> = nullptr>
  auto insert(OtherKey&& value
  ) noexcept(comparison_is_noexcept<OtherKey> && std::is_nothrow_constructible<Value, OtherKey&&>::value)
      -> std::pair<iterator, bool> {
    return internal_insert(std::forward<OtherKey>(value));
  }

  /// @brief Insert all the elements in the provided iterator range into the set, if there are not already equivalent
  /// elements.
  /// @tparam Iterator The type of the iterators
  /// @param first The iterator to the first element in the range
  /// @param last The iterator to the one-past-the-end element in the range
  /// @post @c size() increases by the number of elements in @c [first,last) which did not already exist in the set.
  /// @post @c find(*itr) will return an iterator other than @c end() for all elements in the range @c [first,last)
  /// @pre Let @c unique_elements equal the number of elements in the range @c [first,last) for which @c contains(*itr)
  ///      would return @c false ; @c size()+unique_elements<=max_size() , else @c ARENE_PRECONDITION violation .
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<std::is_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  void insert(Iterator first, Iterator last) noexcept(std::is_nothrow_constructible<Value, decltype(*first)>::value) {
    for (decltype(auto) element : arene::base::make_subrange(first, last)) {
      std::ignore = insert(std::forward<decltype(element)>(element));
    }
  }

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  /// @brief Attempt to insert the elements in the provided initializer list into the set.
  ///
  /// @param init The list of elements to insert
  /// @post @c size() increases by the number of elements in @c init which did not already exist in the set.
  /// @post @c find(*itr) wil return an iterator other than @c end() for all elements in @c init
  /// @pre Let @c unique_elements equal the number of elements in @c init for which @c contains()
  ///      would return @c false ; @c size()+unique_elements<=max_size() , else @c ARENE_PRECONDITION violation .
  void insert(std::initializer_list<Value> init) { insert(init.begin(), init.end()); }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Obtain an iterator referring to the beginning of the sorted range of elements.
  /// @return const_iterator Points to the first element in the sequence, or @c end() if @c empty() .
  auto begin() const noexcept -> const_iterator { return make_iterator(*this, 0U); }
  /// @brief Obtain an iterator referring to the beginning of the sorted range of elements.
  /// @return const_iterator Points to the first element in the sequence, or @c end() if @c empty() .
  auto cbegin() const noexcept -> const_iterator { return make_iterator(*this, 0U); }

  /// @brief Obtain an iterator referring to one past the last element in the sorted range of elements.
  /// @return const_iterator Points to one past the last element in the sequence.
  auto end() const noexcept -> const_iterator { return make_iterator(*this, number_of_active_elements_); }
  /// @brief Obtain an iterator referring to one past the last element in the sorted range of elements.
  /// @return const_iterator Points to one past the last element in the sequence.
  auto cend() const noexcept -> const_iterator { return make_iterator(*this, number_of_active_elements_); }

  /// @brief Obtain an iterator referring to the beginning of a reversed traversal of the sorted range of elements.
  /// @return const_reverse_iterator Points to the last element in the sequence, or @c rend() if @c empty() .
  auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(end()); }
  /// @brief Obtain an iterator referring to the beginning of a reversed traversal of the sorted range of elements.
  /// @return const_reverse_iterator Points to the last element in the sequence, or @c rend() if @c empty() .
  auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(end()); }

  /// @brief Obtain an iterator referring to one past the last element in a reversed traversal of the sorted range of
  ///        elements.
  /// @return const_reverse_iterator An iterator pointing to one before the first element in the sequence.
  auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator(begin()); }
  /// @brief Obtain an iterator referring to one past the last element in a reversed traversal of the sorted range of
  ///        elements.
  /// @return const_reverse_iterator An iterator pointing to one before the first element in the sequence.
  auto crend() const noexcept -> const_reverse_iterator { return const_reverse_iterator(begin()); }

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does have a return"
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: Parameter is used"
  /// @brief Construct a new element from the specified initialization arguments, and then attempt to insert it.
  ///
  /// @tparam ArgTypes The types of the initialization arguments
  /// @param args The initialization arguments of the new element
  /// @return std::pair<iterator, bool> Equivalent to calling @c insert(Value{ArgTypes...}) .
  template <
      typename... ArgTypes,
      constraints<std::enable_if_t<std::is_constructible<Value, ArgTypes&&...>::value>> = nullptr>
  auto emplace(ArgTypes&&... args) -> std::pair<iterator, bool> {
    if (number_of_active_elements_ == Capacity) {
      Value const value{std::forward<ArgTypes>(args)...};
      auto const pos_info = lower_bound_index(value);
      ARENE_PRECONDITION(pos_info.order == strong_ordering::equal);
      return {make_iterator(*this, pos_info.index), false};
    }
    auto const new_index = indices_[number_of_active_elements_];
    auto& potentially_inserted_element = values_[new_index];
    potentially_inserted_element.emplace(std::forward<ArgTypes>(args)...);
    optional_resetter<Value> destroy_if_not_inserted{potentially_inserted_element};
    auto const pos_info = lower_bound_index(*potentially_inserted_element);
    bool const do_insert{pos_info.order != strong_ordering::equal};
    if (do_insert) {
      insert_index_at(pos_info.index, new_index);
      destroy_if_not_inserted.dismiss();
    }
    return {make_iterator(*this, pos_info.index), do_insert};
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
  // parasoft-end-suppress CERT_CPP-MSC52-a
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Erase any elements with a value equivalent to the supplied element.
  ///
  /// @param value_to_erase The value of elements to erase
  /// @return The number of elements erased, which will be @c 1 if the element was in the set, else @c 0 .
  auto erase(Value const& value_to_erase) noexcept(comparison_is_noexcept<>) -> size_type {
    return do_erase(value_to_erase);
  }
  /// @brief Erase any elements with a value equivalent to the supplied element.
  ///
  /// @tparam OtherKey The type of the element to erase.
  /// @param value The value of elements to erase
  /// @return The number of elements erased, which will be @c 1 if the element was in the set, else @c 0 .
  template <typename OtherKey, constraints<std::enable_if_t<transparent_comparison_supported_for<OtherKey>>> = nullptr>
  auto erase(OtherKey const& value) noexcept(comparison_is_noexcept<OtherKey>) -> size_type {
    return do_erase(value);
  }

  /// @brief Erase the element referred to be the specified iterator.
  /// @param pos The iterator referring to the element to erase
  /// @return An iterator referring to the element after the erased element if there is one, or @c end() otherwise.
  /// @pre @c pos Must be a deferencable iterator into @c *this
  auto erase(iterator pos) noexcept -> iterator {
    ARENE_PRECONDITION(pos.is_for(this));
    auto const index = pos.index(inline_set_detail::set_passkey{});
    ARENE_PRECONDITION(index <= number_of_active_elements_);
    std::ignore = erase_at_index(index);
    return pos;
  }

  /// @brief Erase all the elements referred to be the specified iterator range.
  /// @param first The iterator for the start of the range
  /// @param last The iterator for one-past-the-end of the range
  /// @return An iterator referring to the element after the erased elements if there is one, or @c end() otherwise.
  /// @pre @c (first,last] Must be a valid iterator range into @c *this
  auto erase(iterator first, iterator last) noexcept -> iterator {
    ARENE_PRECONDITION(first.is_for(this) && last.is_for(this));
    index_type const first_index{first.index(inline_set_detail::set_passkey{})};
    index_type const last_index{last.index(inline_set_detail::set_passkey{})};
    ARENE_PRECONDITION((first_index <= last_index) && (last_index <= size()));
    std::ignore = erase_index_range(first_index, static_cast<index_type>(last_index - first_index));
    return first;
  }

  /// @brief Erase all the elements in @c *this
  /// @post @c size()==0 .
  /// @post The destructors of all elements in the range @c [begin(),end()) will have been called once.
  void clear() noexcept { std::ignore = erase_index_range(0U, number_of_active_elements_); }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Fast inequality heuristic to shortcut on different sized containers.
  ///
  /// @tparam OtherCapacity the capacity of @c rhs
  /// @param lhs The left-hand operand to the check
  /// @param rhs The right-hand operand to the check
  /// @return inequality_heuristic::may_be_equal_or_not_equal if @c lhs.size()==rhs.size()
  /// @return inequality_heuristic::definitely_not_equal if @c lhs.size()!=rhs.size()
  template <size_type OtherCapacity>
  ARENE_NODISCARD static auto
  fast_inequality_check(inline_set const& lhs, inline_set<Value, OtherCapacity, Compare> const& rhs) noexcept
      -> inequality_heuristic {
    return lhs.size() == rhs.size() ? inequality_heuristic::may_be_equal_or_not_equal
                                    : inequality_heuristic::definitely_not_equal;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Compares two sets lexicographically .
  ///
  /// @note Lexicographic comparison between sets ignores the order of keys as defined by @c Compare . That is, it is
  /// a lexicographic comparison between the @c value_type elements of each container, in iteration order, using the
  /// standard @c compare_three_way facility. This means that for all possible sets which result in element sequences
  /// <c>{1, 2, 3}</c> and <c>{1, 2, 5}</c>, when compared against each other
  /// the result of the comparison will always be the same, @c strong_ordering::less.
  ///
  /// @tparam OtherCapacity the capacity of @c rhs
  /// @tparam SfinaeKey dummy template parameter to disable the operator if @c key_Type is not three way comparable.
  /// @param lhs The left hand operand to the comparison.
  /// @param rhs The right hand operand to the comparison.
  /// @return strong_ordering Equivalent to
  ///         @c arene::base::lexicographical_compare_three_way(lhs.begin(),lhs.end(),rhs.begin(),rhs.end())
  template <
      size_type OtherCapacity,
      typename SfinaeKey = key_type,
      constraints<std::enable_if_t<compare_three_way_supported_v<SfinaeKey>>> = nullptr>
  ARENE_NODISCARD static auto
  three_way_compare(inline_set const& lhs, inline_set<Value, OtherCapacity, Compare> const& rhs) noexcept
      -> strong_ordering {
    return ::arene::base::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Get the comparator base.
  /// @note This can only be called from @c inline_set internals due to the passkey
  /// @return A reference to the base
  auto get_comparator_base(inline_set_detail::set_passkey) noexcept -> comparator_base& { return *this; }
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Get the comparator base.
  /// @note This can only be called from @c inline_set internals due to the passkey
  /// @return A reference to the base
  auto get_comparator_base(inline_set_detail::set_passkey) const noexcept -> comparator_base const& { return *this; }
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Gets a reference to the element corrsponding to a given index.
  /// @param index Index of the element in iteration order.
  /// @return A reference to the element for the given index.
  /// @note This can only be called from @c inline_set internals due to the passkey
  /// @pre @c index<indicies.size()
  auto entry_at_index(inline_set_detail::set_passkey, index_type index) noexcept -> entry_type& {
    return values_[indices_[index]];
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Gets a reference to the element corrsponding to a given index.
  /// @param index Index of the element in iteration order.
  /// @return A reference to the element for the given index.
  /// @note This can only be called from @c inline_set internals due to the passkey
  /// @pre @c index<indicies.size()
  auto entry_at_index(inline_set_detail::set_passkey, index_type index) const noexcept -> entry_type const& {
    return values_[indices_[index]];
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3

 private:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  ///  @brief The type of the storage for the whole set of elements
  using storage_type = array<entry_type, Capacity>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  ///  @brief The current size of set
  index_type number_of_active_elements_{0};
  // parasoft-begin-suppress AUTOSAR-M8_5_2-b-2 "False Positive: all elements default-initialized"
  ///  @brief The storage of the elements
  storage_type values_{};
  // parasoft-end-suppress AUTOSAR-M8_5_2-b-2

  ///  @brief The indices for the elements. Initially @c indices_[i] is @c i, so each element sets to the corresponding
  ///  element in @c values_.
  array<index_type, Capacity> indices_{sequential_values<index_type, Capacity>};

  /// @brief Implementation helper for @c try_construct
  ///
  /// @param first The first iterator in the sequence of elements to attempt to insert.
  /// @param last The last iterator in the sequence of elements to attempt to insert.
  /// @return true If all elements were successfully inserted without exceeding capacity.
  /// @return false If an element needed to be inserted, but there was not enough room.
  /// @post Equivalent to having called @c insert on each element in the range @c [first,last) until either all elements
  /// were successfully inserted or @c Capacity was reached an an element needed to be inserted still.
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<std::is_constructible<Value, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  auto insert_elements_if_they_fit(Iterator first, Iterator last) noexcept(
      comparison_is_noexcept<> && std::is_nothrow_copy_constructible<Value>::value
  ) -> bool {
    while (first != last) {
      if (size() < Capacity) {
        std::ignore = insert(*first);
      } else if (!contains(*first)) {
        return false;
      } else {
        // do nothing, element already present
      }
      arene::base::advance(first, 1);
    }
    return true;
  }

  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: this is not a move constructor"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "The universal reference is used to detect the value category"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Self is a template parameter"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Deduced-this helper for universally constructing iterators.
  ///
  /// @tparam SelfType Deduced-this type of the set.
  /// @param self The instance of the set to operate on.
  /// @param index The index to create the iterator from
  /// @return auto An iterator constructed as if via @c self and @c index .
  template <typename SelfType, constraints<std::enable_if_t<decays_to_v<SelfType, inline_set>>> = nullptr>
  static auto make_iterator(SelfType&& self, index_type index) noexcept -> iterator {
    return iterator_impl{inline_set_detail::set_passkey{}, &self, index};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Deduced-this helper that detemplitizes find iterator generation from key type.
  ///
  /// @tparam SelfType Deduced-this type of the set.
  /// @param self The instance of the set to operate on.
  /// @param lb_info An instance of the return from @c lower_bound_index()
  /// @return auto If @c lb_info.order==strong_ordering::equal , then an iterator constructed as if via @c
  /// make_iterator(self,lb_info.index) , otherwise @c self.end() .
  template <typename SelfType, constraints<std::enable_if_t<decays_to_v<SelfType, inline_set>>> = nullptr>
  static auto
  make_find_iterator(SelfType&& self, inline_container::detail::lower_bound_index_info<index_type> lb_info) noexcept
      -> iterator {
    if (lb_info.order == strong_ordering::equal) {
      return make_iterator(std::forward<SelfType>(self), lb_info.index);
    }
    return self.end();
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  /// @brief Helper implementation for @c find() which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the key to search for.
  /// @param value The key to search for.
  /// @return iterator The iterator at the position of @c value if it is found, else @c end() .
  template <typename OtherKey>
  auto do_find(OtherKey const& value) const -> iterator {
    return make_find_iterator(*this, lower_bound_index(value));
  }

  /// @brief Helper implementation for @c contains which works for all comparators.
  ///
  /// @tparam OtherKey the type of the value to find
  /// @param value The value to search for
  /// @return true if @c find(value)!=end() .
  /// @return false if @c find(value)==end() .
  template <typename OtherKey>
  auto do_contains(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> bool {
    return do_find(value) != end();
  }

  /// @brief Helper implementation for @c count which works for all comparators.
  ///
  /// @tparam OtherKey the type of the value to find.
  /// @param value The value to search for.
  /// @return std::size_t @c 0 if the element was not in the set, else @c 1 .
  template <typename OtherKey>
  auto do_count(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> std::size_t {
    return static_cast<std::size_t>(do_contains(value));
  }

  /// @brief Helper implementation for @c lower_bound which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the value to find.
  /// @param value The value to find the lower bound for.
  /// @return iterator An iterator to the first element which is _not less than_ @c value.
  template <typename OtherKey>
  auto do_lower_bound(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    return make_iterator(*this, lower_bound_index(value).index);
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Deduced-this helper that detemplitizes upper_bound iterator generation from key type.
  ///
  /// @tparam SelfType Deduced-this type of the set.
  /// @param self The instance of the set to operate on.
  /// @param lb_info An instance of the return from @c lower_bound_index()
  /// @return auto If @c lb_info.order==strong_ordering::equal , then an iterator constructed as if via @c
  /// make_iterator(self,lb_info.index + 1) , otherwise @c make_iterator(self,lb_info.index) .
  template <typename SelfType, constraints<std::enable_if_t<decays_to_v<SelfType, inline_set>>> = nullptr>
  static auto make_upper_bound_iterator(
      SelfType&& self,
      inline_container::detail::lower_bound_index_info<index_type> lb_info
  ) noexcept -> iterator {
    if (lb_info.order == strong_ordering::equal) {
      ++lb_info.index;
    }
    return make_iterator(std::forward<SelfType>(self), lb_info.index);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  /// @brief Helper implementation for @c upper_bound which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the value to find
  /// @param value The value to search for.
  /// @return An iterator to the first element which is _greater than_ @c value , or @c end() if there is no such
  ///         element.
  template <typename OtherKey>
  auto do_upper_bound(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>) -> iterator {
    return make_upper_bound_iterator(*this, lower_bound_index(value));
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  // parasoft-begin-suppress AUTOSAR-M0_1_3-a "False Positive: All local variables are used"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does have a return"
  /// @brief Deduced-this helper that detemplitizes equal_range iterator generation from key type.
  ///
  /// @tparam SelfType Deduced-this type of the set.
  /// @param self The instance of the set to operate on.
  /// @param lb_info An instance of the return from @c lower_bound_index()
  /// @return auto A pair of iterators where the first element is @c make_find_iterator(self,lb_info) , and the second
  /// element is either that iterator incremented by one if the iterator was not @c self.end() , otherwise @c self.end()
  template <typename SelfType, constraints<std::enable_if_t<decays_to_v<SelfType, inline_set>>> = nullptr>
  static auto
  make_equal_range_iterator(SelfType&& self, inline_container::detail::lower_bound_index_info<index_type> lb_info)
      -> std::pair<iterator, iterator> {
    auto first = make_find_iterator(std::forward<SelfType>(self), lb_info);
    auto last = first == self.end() ? first : ::arene::base::next(first);
    return std::make_pair(first, last);
  }
  // parasoft-end-suppress CERT_CPP-MSC52-a
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress CERT_C-EXP37-a
  // parasoft-end-suppress AUTOSAR-M0_1_3-a
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  /// @brief Helper implementation for @c equal_range which works for all comparators and iterator types.
  ///
  /// @tparam OtherKey the type of the value to find
  /// @param value The value to search for
  /// @return std::pair<iterator, iterator> A pair of iterators such that all values in the range @c [first,second)
  ///         compare equal to @c value if there was an element equivalent to @c value in the set. Otherwise both
  ///         iterators will be @c end() .
  template <typename OtherKey>
  auto do_equal_range(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>)
      -> std::pair<iterator, iterator> {
    return make_equal_range_iterator(*this, lower_bound_index(value));
  }

  /// @brief Obtain the index of the first element such that @c element < @c value
  /// @param value The value to search for
  /// @return inline_container::detail::lower_bound_index_info<index_type> The index of the element, and the
  /// strong_ordering between that
  ///         element and @c value . If @c value is greater than all elements, then the index is @c size() and the
  ///         ordering is @c strong_ordering::greater .
  /// @throws Any exception thrown by the comparisons
  template <typename OtherKey>
  auto lower_bound_index(OtherKey const& value) const noexcept(comparison_is_noexcept<OtherKey>)
      -> inline_container::detail::lower_bound_index_info<index_type> {
    auto const compare = [this](key_type const& lhs, OtherKey const& rhs) noexcept(comparison_is_noexcept<OtherKey>) {
      return this->key_ordering(lhs, rhs);
    };
    auto const accessor = [this](index_type const index) noexcept -> key_type const& {
      return *(this->entry_at_index(inline_set_detail::set_passkey{}, index));
    };
    return inline_container::detail::lower_bound_index<key_type, OtherKey, index_type>(
        value,
        number_of_active_elements_,
        compare,
        accessor
    );
  }

  /// @brief Helper implementation for @c erase which works for all comparators
  ///
  /// @tparam OtherKey The type of the element to erase.
  /// @param value The value of elements to erase
  /// @return The number of elements erased, which will be @c 1 if the element was in the set, else @c 0 .
  template <typename OtherKey>
  auto do_erase(OtherKey const& value) noexcept(comparison_is_noexcept<OtherKey>) -> size_type {
    return static_cast<size_type>(erase_at_index(do_find(value).index(inline_set_detail::set_passkey{})) != end());
  }

  /// @brief Insert a new element at the specified index, constructed from the supplied value. This uses perfect
  /// forwarding so the same function handles copies and moves.
  /// @tparam V The type of the source value
  /// @param pos The position at which to insert the element
  /// @param value The new value
  /// @throws std::length_error if there is insufficient space to insert a new element
  template <typename V>
  void insert_at(index_type pos, V&& value) noexcept(std::is_nothrow_constructible<Value, V&&>::value) {
    auto const new_index = indices_[number_of_active_elements_];
    values_[new_index].emplace(std::forward<V>(value));
    insert_index_at(pos, new_index);
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: function does have a return"
  /// @brief Insert a new entry into the index at the specified position.
  /// @param pos The position at which to insert the new value
  /// @param new_index The index into the data array of the element being inserted
  void insert_index_at(index_type pos, index_type new_index) noexcept {
    for (index_type i{number_of_active_elements_}; i != pos; --i) {
      indices_[i] = indices_[static_cast<index_type>(i - 1U)];
    }
    indices_[pos] = new_index;
    ++number_of_active_elements_;
  }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress CERT_C-EXP37-a

  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does have a return"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
  /// @brief Insert a new element constructed from the specified value if there is not already an equivalent element in
  /// the map. Uses perfect forwarding to handle copying/moving as appropriate
  /// @tparam V The type of the source value
  /// @param value The new value
  /// @return A pair holding an iterator to the inserted element, or the existing element with equivalent value, and a
  /// boolean which is @c true if the value was inserted, and @c false otherwise.
  /// @pre If insertion is required, @c size<Capacity else @c ARENE_PRECONDITION violation.
  template <typename V>
  auto internal_insert(V&& value
  ) noexcept(comparison_is_noexcept<V> && std::is_nothrow_constructible<Value, V&&>::value)
      -> std::pair<iterator, bool> {
    auto const pos_info = lower_bound_index(value);
    auto const do_insert = pos_info.order != strong_ordering::equal;
    if (do_insert) {
      insert_at(pos_info.index, std::forward<V>(value));
    }
    return {make_iterator(*this, pos_info.index), do_insert};
  }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress CERT_CPP-MSC52-a
  // parasoft-end-suppress CERT_C-MSC37-a

  /// @brief Erase the element at the specified index.
  /// @param index The index of the element to erase
  /// @return An iterator to the element after the erased element, or @c end() if @c index>=number_of_active_elements_.
  auto erase_at_index(index_type index) noexcept -> iterator {
    return index < number_of_active_elements_ ? erase_index_range(index, 1U) : end();
  }

  /// @brief Erase the specified number of elements starting at the specified position.
  /// @param from The position in the index of the first element to erase
  /// @param count_to_erase The number of elements to erase
  /// @return An iterator to the element after the specified elements, or @c end() if there is no such element.
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  auto erase_index_range(index_type from, index_type count_to_erase) noexcept -> iterator {
    index_type last_index{static_cast<index_type>(from + count_to_erase)};
    for (index_type index{from}; index < last_index; ++index) {
      entry_at_index(inline_set_detail::set_passkey{}, index).reset();
    }
    std::ignore = arene::base::rotate(
        indices_.begin() + from,
        indices_.begin() + last_index,
        indices_.begin() + number_of_active_elements_
    );
    number_of_active_elements_ = static_cast<index_type>(number_of_active_elements_ - (last_index - from));

    return make_iterator(*this, from);
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Move-construct a new element from the specified element, and clear the old element.
  /// @param other The element to move from
  /// @return The new element
  /// @throws Any exception thrown by the copy-constructor of @c Key or the
  /// move-constructor of @c Value
  static auto move_value(entry_type& other) noexcept(std::is_nothrow_move_constructible<value_type>::value)
      -> entry_type {
    entry_type res{std::move(other)};
    other.reset();
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Move the values from the source set into a new array. Clears the source set.
  /// @tparam Indices The indices into the source set
  /// @param source The source set
  /// @return The new data array
  template <std::size_t... Indices>
  static auto move_values(inline_set& source, std::index_sequence<Indices...>) -> storage_type {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a-2 "False positive: RAII class"
    auto const clear_on_exit = clear_on_scope_exit(source);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a-2
    return {{move_value(source.values_[Indices])...}};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Move the values from a source set with a different capacity into a new array. Clears the source set.
  /// @tparam OtherCapacity The capacity of the other set
  /// @tparam Indices The indices into the source set
  /// @param source The source set
  /// @return The new data array
  /// @pre The size of the source set must be less than @c Capacity
  template <std::size_t OtherCapacity, std::size_t... Indices>
  static auto move_values(inline_set<Value, OtherCapacity, Compare>& source, std::index_sequence<Indices...>)
      -> storage_type {
    auto const clear_on_exit = clear_on_scope_exit(source);
    return {{move_value(source.entry_at_index(inline_set_detail::set_passkey{}, Indices))...}};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False Positive: This identifier does not hide anything"
  /// @brief Helper to create a scope-guard which clears the input source on scope exit.
  ///
  /// @param source The container to clear on scope exit.
  /// @return A @c scope_guard which will call @c clear() on @c source .
  template <std::size_t OtherCapacity>
  static auto clear_on_scope_exit(inline_set<Value, OtherCapacity, Compare>& source) noexcept -> decltype(auto) {
    return on_scope_exit(  //
        [&source]() noexcept { source.clear(); }
    );
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: Uses this"
  /// @brief Helper to create a scope-guard which resets all values between the current number_of_active_elements_ and
  /// an index.
  ///
  /// @param end_index One past the last index to reset to.
  /// @return A @c scope_guard which will call @c reset() on every element fetched via @c entry_at_index() in the range
  /// @c [number_of_active_elements_,end_index) .
  auto reset_trailing_values_on_scope_exit(index_type end_index) noexcept -> decltype(auto) {
    return on_scope_exit([this, end_index]() noexcept {
      for (index_type idx = this->number_of_active_elements_; idx < end_index; ++idx) {
        this->entry_at_index(inline_set_detail::set_passkey{}, idx).reset();
      }
    });
  }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  /// @brief Copy the elements from the other set over those in the current set.
  ///
  /// @tparam OtherSet The type of the set to copy from
  /// @param other The source set
  /// @throws Any exception thrown by the copy of the elements or comparator
  /// @post @c size()==other.size() , or the index of the first element which throws during copy construction if a throw
  ///       occurs.
  /// @post @c The elements from @c other will have been copy-emplaced into @c this , up to the first element which
  ///       threw during move-construction, if any.
  /// @post @c The elements in the range @c [size(),other.size()) will have been reset if @c other.size() was smaller
  ///       than the original size of @c this .
  template <typename OtherSet>
  void do_copy(OtherSet const& other
  ) noexcept(std::is_nothrow_copy_constructible<OtherSet>::value && std::is_nothrow_copy_assignable<OtherSet>::value) {
    do_assignment(other);
  }

  /// @brief Move the elements from the other set over those in the current set.
  ///
  /// @tparam OtherSet The type of the set to move from
  /// @param other The source set
  /// @throws Any exception thrown by the move of the elements or comparator
  /// @post @c size()==other.size() , or the index of the first element which throws during move construction if a throw
  ///       occurs.
  /// @post @c other.clear() will have been executed
  /// @post @c The elements from @c other will have been move-emplaced into @c this , up to the first element which
  ///       threw during move-construction, if any.
  /// @post @c The elements in the range @c [size(),other.size()) will have been reset if @c other.size() was smaller
  ///       than the original size of @c this .
  template <typename OtherSet>
  void do_move(OtherSet&& other
  ) noexcept(std::is_nothrow_move_constructible<OtherSet>::value && std::is_nothrow_move_assignable<OtherSet>::value) {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a-2 "False positive: RAII class"
    auto const clear_on_exit = clear_on_scope_exit(other);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a-2
    do_assignment(std::forward<OtherSet>(other));
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are forwarded via 'forward_like'"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are forwarded via 'forward_like'"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are forwarded via 'forward_like'"
  /// @brief Helper for @c do_copy and @c do_move with non-copy/move dependent logic.
  ///
  /// @tparam OtherSet The type of the set to copy/move from
  /// @param other The source set top copy/move from.
  /// @throws Any exception thrown by the copy/move of the elements or comparator
  /// @post @c size()==other.size() , or the index of the first element which throws during move construction if a throw
  ///       occurs.
  /// @post @c The elements from @c other will have been move-emplaced into @c this , up to the first element which
  ///       threw during copy/move-construction, if any.
  /// @post @c The elements in the range @c [size(),other.size()) will have been reset if @c other.size() was smaller
  ///       than the original size of @c this .
  template <typename OtherSet>
  void do_assignment(OtherSet&& other) noexcept(
      std::is_nothrow_constructible<OtherSet, OtherSet&&>::value &&
      std::is_nothrow_assignable<OtherSet&, OtherSet&&>::value
  ) {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a-2 "False positive: RAII class"
    auto const reset_trailing_values = reset_trailing_values_on_scope_exit(number_of_active_elements_);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a-2
    number_of_active_elements_ = 0U;
    while (number_of_active_elements_ != other.size()) {
      this->entry_at_index(inline_set_detail::set_passkey{}, number_of_active_elements_)
          .emplace(forward_like<OtherSet>(
              *other.entry_at_index(inline_set_detail::set_passkey{}, number_of_active_elements_)
          ));
      ++number_of_active_elements_;
    }
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
};  // namespace base
// parasoft-end-suppress AUTOSAR-A1_1_1-c-2
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

/// @brief Externalized initialization of max_size is needed pre C++17.
template <typename T, std::size_t Capacity, typename Compare>
constexpr std::integral_constant<std::size_t, Capacity> inline_set<T, Capacity, Compare>::max_size;

/// @brief Externalized initialization of capacity is needed pre C++17.
template <typename T, std::size_t Capacity, typename Compare>
constexpr std::integral_constant<std::size_t, Capacity> inline_set<T, Capacity, Compare>::capacity;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_SET_HPP_
