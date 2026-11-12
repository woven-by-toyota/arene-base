// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_CONSTANT_SEQUENCE_ACCESSOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_CONSTANT_SEQUENCE_ACCESSOR_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_abstract.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"

namespace arene {
namespace base {
namespace constant_sequence_accessor_detail {

/// @brief Helper trait to check if @c Sequence has a static @c values() member. The value is @c true if it does, @c
/// false otherwise
/// @tparam Sequence The type to check
template <typename Sequence, typename = constraints<>>
extern constexpr bool has_values_member_v = false;

/// @brief Helper trait to check if @c Sequence has a static @c values() member. The value is @c true if it does, @c
/// false otherwise
/// @tparam Sequence The type to check
template <typename Sequence>
extern constexpr bool has_values_member_v<Sequence, constraints<decltype(Sequence::values())>> = true;

/// @brief Array of elements for the sequence type @c Sequence
/// @tparam Sequence The type of the sequence
/// @pre Requires that @c Sequence has a type alias @c value_type for the element type, a static member function @c
/// size() for the size of the sequence that is callable in a constant expression, and a static member function @c
/// values() that returns something convertible to an array of @c Sequence::size() elements of type @c
/// Sequence::value_type that is callable in a constant expression
template <typename Sequence, typename = constraints<>>
extern constexpr array<typename Sequence::value_type, Sequence::size()> sequence_values_v{Sequence::values()};

/// @brief Array of elements for a sequence type that is defined as a template with the same parameters as @c
/// std::integer_sequence but not constrained to be integers
/// @tparam SequenceElementType The type of the sequence element
/// @tparam Values The values of the sequence
template <
    template <typename SequenceElementType, SequenceElementType... Values>
    class SequenceTemplate,
    typename SequenceElementType,
    SequenceElementType... Values>
extern constexpr array<SequenceElementType, sizeof...(Values)> sequence_values_v<
    SequenceTemplate<SequenceElementType, Values...>,
    constraints<
        std::enable_if_t<(sizeof...(Values) > 0)>,
        std::enable_if_t<!has_values_member_v<SequenceTemplate<SequenceElementType, Values...>>>>>{Values...};

}  // namespace constant_sequence_accessor_detail

/// @brief Accessor policy for @c mdspan which returns values from a compile time sequence
/// @tparam Sequence A type holding the sequence of values. This must either be:
///
/// - an instance of a class template that has
/// the element type as the first parameter and the elements themselves as subsequence parameters, and a member type
/// alias @c value_type that is the type of the elements, as per @c std::integer_sequence , or
///
/// - it must be a class type that has a member type alias @c value_type that is the type of
/// the elements, a static member function @c size() for the size of the sequence that is callable in a constant
/// expression, and a static member function @c values() that returns something convertible to an array of @c
/// Sequence::size() elements of type @c Sequence::value_type that is callable in a constant expression
/// @tparam ElementType the type of the element. The element type of @c Sequence must be convertible to the @c
/// ElementType , which must be an object type that is not an abstract type.
template <typename Sequence, typename ElementType = typename Sequence::value_type>
class constant_sequence_accessor {
 public:
  static_assert(!std::is_abstract<ElementType>::value, "ElementType must not be abstract");
  static_assert(std::is_object<ElementType>::value, "ElementType must be an object type");
  static_assert(
      std::is_convertible<typename Sequence::value_type, ElementType>::value,
      "Raw sequence value_type must be convertible to ElementType"
  );

  /// @brief The type of the element
  using element_type = ElementType;
  /// @brief The type of the data handle --- this is the index into the sequence from which to start the accesses
  using data_handle_type = std::size_t;
  /// @brief The type of a "reference" to the element --- this is the same as the element type, so no backing data is
  /// required, the value is returned by value
  using reference = ElementType;
  /// @brief The type of the accessor policy for accessing data with an offset --- this is @c constant_sequence_accessor
  /// , since the value is constant
  using offset_policy = constant_sequence_accessor;

  /// @brief Return the value for the given data handle and index. The index is ignored as the value is always constant
  /// @param data_handle The base position into the sequence to use
  /// @param index The index value to the chosen item from the base position
  /// @return The fixed value from the sequence at the position <c>(data_handle+index)</c>
  /// @pre The value of <c>(data_handle+index)</c> must be less than @c Sequence::size()
  static constexpr auto access(data_handle_type data_handle, std::size_t index) noexcept -> reference {
    ARENE_PRECONDITION((data_handle + index) < Sequence::size());
    return static_cast<ElementType>(constant_sequence_accessor_detail::sequence_values_v<Sequence>[data_handle + index]
    );
  }
  /// @brief Return the data handle value to use with the offset policy for the given data handle and index.
  /// @param data_handle The base position into the sequence to use
  /// @param index The index value to the chosen item from the base position
  /// @return <c>(data_handle+index)</c>
  static constexpr auto offset(data_handle_type data_handle, std::size_t index) noexcept -> data_handle_type {
    return data_handle + index;
  }
};
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_CONSTANT_SEQUENCE_ACCESSOR_HPP_
