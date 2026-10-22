// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_MAKE_RANGE_FROM_INDEXABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_MAKE_RANGE_FROM_INDEXABLE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/iterator_interface.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/ranges/size.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/make_subrange.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace detail {

/// @brief tag identifying the stored indexable within @c indexable_iterator
///
/// Used to disambiguate the @c ebo_holder base that stores the viewed value.
struct indexable_storage_tag {};

/// @brief tag identifying the stored index policy within @c indexable_iterator
///
/// Used to disambiguate the @c ebo_holder base that stores the index policy.
struct index_policy_storage_tag {};

/// @brief index policy that reads an element via @c operator()
class call_operator_policy {
 public:
  /// @brief read the element of @c indexable at @c index
  /// @tparam Indexable indexable type providing @c operator()
  /// @tparam Index index type accepted by @c Indexable::operator()
  /// @param indexable the indexable value to read from
  /// @param index the index of the element to read
  /// @return the result of @c indexable(index)
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
  template <class Indexable, class Index>
  constexpr auto operator()(Indexable&& indexable, Index index) const
      noexcept(noexcept(std::forward<Indexable>(indexable)(index)))
          -> decltype(std::forward<Indexable>(indexable)(index)) {
    return std::forward<Indexable>(indexable)(index);
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a
};

/// @brief default end policy reporting the one-past-last index via @c arene::base::ranges::size
///
/// This is the type of the general @c arene::base::ranges::size function object, so any type it supports (a @c size()
/// member, an ADL @c size() free function, or a sized-sentinel iterator pair) can be used as the @c EndPolicy without a
/// bespoke policy.
using default_end_policy = arene::base::remove_cvref_t<decltype(arene::base::ranges::size)>;

/// @brief a random-access or input iterator over the elements of an indexable value
/// @tparam Indexable the indexable value type to view
/// @tparam IndexPolicy function object reading the element at a given index
///
/// The @c indexable_iterator class template provides an iterator over any type that is generally "indexable". The exact
/// meaning of "indexable" depends on @c IndexPolicy but defaults to calling @c operator() on the type @c Indexable.
/// This iterator type is intended to be used to extend types which do not already provide iterators.
///
/// The iterator is a @c RandomAccessIterator when @c IndexPolicy yields a true reference to the element; when it yields
/// a prvalue the iterator can only be an @c InputIterator (a @c ForwardIterator requires @c reference to be a real
/// reference). See @c iterator_category below.
///
/// The iterator stores the indexable value by value together with an offset, so it is self-contained: an iterator
/// remains valid independently of any range object it came from.
///
/// @note Each iterator holds its own copy of @c Indexable - intended for cheap-to-copy @c Indexable types such as
///   lightweight handles or non-owning views. Using it with an @c Indexable whose copy is expensive will make
///   iteration expensive.
/// @warning Holding a copy of @c Indexable makes the iterator independent of the range object, but not of whatever
///   storage @c Indexable refers to. When @c Indexable has reference semantics (a non-owning view such as
///   @c arene::base::span), dereferencing the iterator reads through that external storage, which must outlive every
///   iterator that views it.
/// @note This iterator declares @c iterator_concept as @c std::random_access_iterator_tag in addition to the
///   classic @c iterator_category, so it classifies as random-access under the C++20 ranges iterator model even
///   when @c reference is a prvalue.
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple base classes needed for empty-base-class optimizations."
template <class Indexable, class IndexPolicy = call_operator_policy>
class indexable_iterator
    : public iterator_interface<indexable_iterator<Indexable, IndexPolicy>>
    , private arene::base::ebo_holder<indexable_storage_tag, Indexable>
    , private arene::base::ebo_holder<index_policy_storage_tag, IndexPolicy> {
  // parasoft-end-suppress AUTOSAR-A10_1_1-a
  /// @brief the base holding the viewed value
  using indexable_holder = arene::base::ebo_holder<indexable_storage_tag, Indexable>;

  /// @brief the base holding the index policy
  using index_policy_holder = arene::base::ebo_holder<index_policy_storage_tag, IndexPolicy>;

  /// @brief the CRTP iterator-interface base
  using iterator_interface_base = iterator_interface<indexable_iterator>;

 public:
  // Prioritize operator[] from iterator_interface_base
  using iterator_interface_base::operator[];

  /// @brief the signed distance type between two iterators
  using difference_type = std::ptrdiff_t;
  /// @brief the type returned by dereferencing the iterator
  using reference = decltype(arene::base::invoke(
      std::declval<IndexPolicy const&>(),
      std::declval<Indexable const&>(),
      std::declval<difference_type>()
  ));
  /// @brief the value type of the iterator
  using value_type = arene::base::remove_cvref_t<reference>;

  /// @brief the iterator category
  ///
  /// A @c ForwardIterator (and therefore a @c RandomAccessIterator) requires @c reference to be a true reference to
  /// @c value_type. When @c IndexPolicy yields a prvalue this iterator can only be an @c InputIterator.
  using iterator_category =
      std::conditional_t<std::is_reference<reference>::value, std::random_access_iterator_tag, std::input_iterator_tag>;

  /// @brief the iterator concept
  ///
  /// With the @c ranges classification of iterators, forward and stronger do not require @c reference to be a true
  /// reference, so this iterator is always classified as random-access.
  using iterator_concept = std::random_access_iterator_tag;

  /// @brief the pointer type
  ///
  /// Only meaningful when @c reference is a true reference; otherwise there is no object to point to.
  ///
  /// @note When @c reference is a prvalue, @c operator-> could return an @c arrow_proxy wrapping the element instead of
  ///   @c void; this is deferred to a separate reusable @c arrow_proxy class in a follow-up.
  using pointer = std::conditional_t<std::is_reference<reference>::value, std::remove_reference_t<reference>*, void>;

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: delegates to a constructor which does call the base
  // class constructors"
  /// @brief default constructor, yields a singular iterator
  constexpr indexable_iterator() noexcept  // NOLINT(hicpp-member-init) delegates to (Indexable, difference_type) ctor
      : indexable_iterator{Indexable{}, difference_type{}} {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief construct an iterator into @c indexable at @c offset
  /// @param indexable the viewed value
  /// @param offset the index of the referenced element
  /// @param policy the index policy used to read elements
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
  constexpr explicit indexable_iterator(
      Indexable indexable,
      // parasoft-begin-suppress AUTOSAR-A2_10_1-c "False positive: identifier 'offset' does not hide anything"
      difference_type const offset,
      // parasoft-end-suppress AUTOSAR-A2_10_1-c
      IndexPolicy policy = {}
  ) noexcept(std::is_nothrow_move_constructible<Indexable>::value && std::is_nothrow_move_constructible<IndexPolicy>::value)
      : iterator_interface_base{},
        indexable_holder{std::move(indexable)},
        index_policy_holder{std::move(policy)},
        offset_{offset} {}
  // parasoft-end-suppress AUTOSAR-M2_10_1-a

  /// @brief dereference
  /// @return the element at the current index
  /// @pre @c *this is dereferenceable
  constexpr auto operator*() const
      noexcept(arene::base::is_nothrow_invocable_v<IndexPolicy const&, Indexable const&, difference_type>)
          -> reference {
    return arene::base::invoke(
        index_policy_holder::get_value(index_policy_storage_tag{}),
        indexable_holder::get_value(indexable_storage_tag{}),
        offset_
    );
  }

  /// @brief shift the iterator by a signed distance
  /// @param difference the signed distance by which to shift
  /// @return @c *this after shifting
  constexpr auto operator+=(difference_type const difference) noexcept -> indexable_iterator& {
    offset_ += difference;
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: This is a namespace scope function"
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
  /// @brief distance between two iterators
  /// @param lhs left argument
  /// @param rhs right argument
  /// @return the signed distance from @c rhs to @c lhs
  /// @pre @c lhs and @c rhs view the same value
  friend constexpr auto operator-(indexable_iterator const& lhs, indexable_iterator const& rhs) noexcept
      -> difference_type {
    return lhs.offset_ - rhs.offset_;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_5-a
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

 private:
  /// @brief the index of the referenced element
  std::ptrdiff_t offset_;
};

/// @brief function object that constructs a range over the elements of an indexable value
class make_range_from_indexable_fn {
 public:
  /// @brief construct a range over the elements of an indexable value
  ///
  /// The @c make_range_from_indexable_fn function object constructs a range over any type that is generally
  /// "indexable". The exact meaning of "indexable" depends on @c IndexPolicy but defaults to calling @c operator() on
  /// the type @c Indexable. The range consists of two iterators, the begin iterator is always at offset 0 and the end
  /// iterator is determined by @c EndPolicy. The range is random-access when @c IndexPolicy yields a true reference and
  /// input-only when it yields a prvalue (see @c indexable_iterator).
  ///
  /// This is intended to be used to extend types which do not already provide iterators, so that they can be used with
  /// range-based algorithms. Being a function object rather than a plain function template, it can be partially applied
  /// (e.g. via @c arene::base::bind_back) to name a reusable range factory for a specific policy pair.
  ///
  /// @tparam Indexable the indexable value type to view, must be copy constructible
  /// @tparam EndPolicy move constructible function object returning the one-past-last index, must be invocable with
  /// <c> Indexable const& </c>
  /// @tparam IndexPolicy move constructible function object reading the element at a given index, must be invocable
  /// with <c> Indexable const&, std::ptrdiff_t </c>
  /// @param indexable the value to view
  /// @param end_policy the policy computing the one-past-last index
  /// @param index_policy the policy reading the element at a given index
  /// @return a range viewing @c indexable, suitable for range-based algorithms
  ///
  /// @note Each iterator holds its own copy of @c Indexable - intended for cheap-to-copy @c Indexable types such as
  ///   lightweight handles or non-owning views. Using it with an @c Indexable whose copy is expensive will make
  ///   iteration expensive.
  /// @warning While the returned range is a borrowed range as the iterators hold a copy of @c Indexable, when @c
  ///   Indexable has reference semantics (a non-owning view such as @c arene::base::span), the elements reached through
  ///   the range are valid only while that storage lives - the caller must keep it alive for as long as the range, or
  ///   any iterator taken from it, is used. This cannot be enforced here, since @c Indexable is not required to be a
  ///   range and so cannot be constrained on @c arene::base::ranges::is_borrowed_range.
  template <
      class Indexable,
      class EndPolicy = default_end_policy,
      class IndexPolicy = call_operator_policy,
      arene::base::constraints<
          std::enable_if_t<std::is_copy_constructible<Indexable>::value>,
          std::enable_if_t<std::is_move_constructible<IndexPolicy>::value>,
          std::enable_if_t<std::is_move_constructible<EndPolicy>::value>,
          std::enable_if_t<arene::base::is_invocable_v<IndexPolicy const&, Indexable const&, std::ptrdiff_t>>,
          std::enable_if_t<arene::base::is_invocable_v<EndPolicy const&, Indexable const&>>> = nullptr>
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'end_policy' is used"
  constexpr auto operator()(Indexable const indexable, EndPolicy end_policy = {}, IndexPolicy index_policy = {}) const
      noexcept(
          arene::base::is_nothrow_invocable_v<EndPolicy const&, Indexable const&>&& noexcept(arene::base::make_subrange(
              std::declval<indexable_iterator<Indexable, IndexPolicy>>(),
              std::declval<indexable_iterator<Indexable, IndexPolicy>>()
          ))
      )
          -> decltype(arene::base::make_subrange(
              std::declval<indexable_iterator<Indexable, IndexPolicy>>(),
              std::declval<indexable_iterator<Indexable, IndexPolicy>>()
          )) {
    using iterator = indexable_iterator<Indexable, IndexPolicy>;
    using difference_type = typename iterator::difference_type;

    auto const end_offset = static_cast<difference_type>(arene::base::invoke(end_policy, indexable));
    return arene::base::make_subrange(
        iterator{indexable, difference_type{}, index_policy},
        iterator{indexable, end_offset, std::move(index_policy)}
    );
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
};

/// @def arene::base::detail::make_range_from_indexable
/// @copydoc arene::base::detail::make_range_from_indexable_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(detail::make_range_from_indexable_fn, make_range_from_indexable);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_MAKE_RANGE_FROM_INDEXABLE_HPP_
