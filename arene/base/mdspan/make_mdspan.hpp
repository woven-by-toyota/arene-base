// parasoft-begin-suppress AUTOSAR-A2_8_1-a "This header defines the make_mdspan callable object"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_MAKE_MDSPAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_MAKE_MDSPAN_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/detail/deduced_static_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/is_accessor_policy.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/extent.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/rank.hpp"
#include "arene/base/stdlib_choice/remove_all_extents.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_pointer.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace make_mdspan_detail {
/// @brief Class to define the @c make_mdspan callable object. Consists entirely of overloads of the function call
/// operator.
class make_mdspan_impl {
 public:
  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  /// @brief Helper function to create an @c mdspan from a C array, with automatically deduced extents and default
  /// accessor
  /// @tparam CArray The type of the array
  /// @param array A reference to the array
  /// @return An @c mdspan referencing the elements of the array as a single-dimensional span with the single extent
  /// equal to the extent of the array
  /// @pre The array must be a single-dimensional array with known bounds
  template <
      typename CArray,
      constraints<std::enable_if_t<std::is_array<CArray>::value>, std::enable_if_t<std::rank<CArray>::value == 1>> =
          nullptr>
  constexpr auto operator()(CArray& array) const noexcept
      -> mdspan<std::remove_all_extents_t<CArray>, extents<std::size_t, std::extent<CArray, 0>::value>> {
    return {array, extents<std::size_t, std::extent<CArray, 0>::value>{}};
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Helper function to create an @c mdspan from a pointer to a single object, with zero-dimensional extents and
  /// a default accessor
  /// @tparam Pointer The pointer type
  /// @param ptr The pointer to the object
  /// @return An @c mdspan referencing the pointed-to object as a zero-dimensional span
  template <
      typename Pointer,
      constraints<std::enable_if_t<std::is_pointer<std::remove_reference_t<Pointer>>::value>> = nullptr>
  constexpr auto operator()(Pointer&& ptr) const noexcept
      -> mdspan<std::remove_pointer_t<std::remove_reference_t<Pointer>>, extents<std::size_t>> {
    return {std::forward<Pointer>(ptr), extents<std::size_t>{}};
  }

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: All arguments are named"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Helper function to create an @c mdspan with a default accessor from a pointer to an array of objects and
  /// the extents of the dimensions
  /// @tparam ElementType The type of the pointed-to objects
  /// @tparam Integrals The types of the extents values
  /// @param ptr The pointer to the objects
  /// @param extent_values The extents of each of the dimensions
  /// @return An @c mdspan referencing the pointed-to objects with a number of dimensions equal to the number of
  /// supplied extents values, where each dimension has a static extent of @c extent_values::value where the supplied
  /// extent value is an integral-constant-like or a dynamic extent otherwise, and the runtime extent values
  /// are equal to the supplied values converted to @c std::size_t
  /// @pre Each type in @c Integrals must be convertible to @c std::size_t
  template <
      typename ElementType,
      typename... Integrals,
      constraints<
          std::enable_if_t<sizeof...(Integrals) != 0>,
          std::enable_if_t<all_of_v<std::is_convertible<Integrals, std::size_t>::value...>>> = nullptr>
  constexpr auto operator()(ElementType* ptr, Integrals... extent_values) const noexcept -> mdspan<
      ElementType,
      extents<std::size_t, mdspan_detail::deduced_static_extent_v<remove_cvref_t<Integrals>>...>> {
    return {
        ptr,
        extents<std::size_t, mdspan_detail::deduced_static_extent_v<remove_cvref_t<Integrals>>...>{
            std::move(extent_values)...
        }
    };
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-begin-suppress CERT_C-EXP37-a

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: extent_values used in return expression"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Helper function to create an @c mdspan with a default accessor from a pointer to an array of objects and a
  /// @c span holding the extents of the dimensions
  /// @tparam ElementType The type of the pointed-to objects
  /// @tparam DimensionType The type of the supplied dimensions
  /// @tparam Rank The number of elements in the span
  /// @param ptr The pointer to the objects
  /// @param extent_values The extents of each of the dimensions
  /// @return An @c mdspan referencing the pointed-to objects with a number of dimensions equal to the number of
  /// supplied extents values, where each dimension has a dynamic extent, and the runtime extent values are equal to the
  /// supplied values converted to @c std::size_t
  /// @pre The @c DimensionType must be convertible to @c std::size_t
  /// @pre The supplied @c Rank must not be @c dynamic_extent --- the rank of the resulting extents must be a fixed
  /// number, so a dynamic-sized @c span cannot be used
  template <
      typename ElementType,
      typename DimensionType,
      std::size_t Rank,
      constraints<
          std::enable_if_t<std::is_convertible<DimensionType, std::size_t>::value>,
          std::enable_if_t<Rank != dynamic_extent>> = nullptr>
  constexpr auto operator()(ElementType* ptr, span<DimensionType, Rank> extent_values) const noexcept
      -> mdspan<ElementType, dextents<std::size_t, Rank>> {
    return {ptr, dextents<std::size_t, Rank>{extent_values}};
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: extent_values used in return expression"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Helper function to create an @c mdspan with a default accessor from a pointer to an array of objects and a
  /// @c array holding the extents of the dimensions
  /// @tparam ElementType The type of the pointed-to objects
  /// @tparam DimensionType The type of the supplied dimensions
  /// @tparam Rank The number of elements in the span
  /// @param ptr The pointer to the objects
  /// @param extent_values The extents of each of the dimensions
  /// @return An @c mdspan referencing the pointed-to objects with a number of dimensions equal to the number of
  /// supplied extents values, where each dimension has a dynamic extent, and the runtime extent values are equal to the
  /// supplied values converted to @c std::size_t
  /// @pre The @c DimensionType must be convertible to @c std::size_t
  template <
      typename ElementType,
      typename DimensionType,
      std::size_t Rank,
      constraints<std::enable_if_t<std::is_convertible<DimensionType, std::size_t>::value>> = nullptr>
  constexpr auto operator()(ElementType* ptr, array<DimensionType, Rank> const& extent_values) const noexcept
      -> mdspan<ElementType, dextents<std::size_t, Rank>> {
    return {ptr, dextents<std::size_t, Rank>{extent_values}};
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

  /// @brief Helper function to create an @c mdspan with a default accessor from a pointer to an array of objects and an
  /// @c extents object
  /// @tparam ElementType The type of the pointed-to objects
  /// @tparam ExtentsType The type of the extents object
  /// @param ptr The pointer to the objects
  /// @param source_extents The extents
  /// @return An @c mdspan referencing the pointed-to objects with dimensions specified by the source extents
  /// @pre The @c ExtentsType must be an instance of @c arene::base::extents
  template <
      typename ElementType,
      typename ExtentsType,
      constraints<std::enable_if_t<is_extents_v<ExtentsType>>> = nullptr>
  constexpr auto operator()(ElementType* ptr, ExtentsType const& source_extents) const noexcept
      -> mdspan<ElementType, ExtentsType> {
    return {ptr, source_extents};
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: There is no identifier 'mapping' being hidden"
  /// @brief Helper function to create an @c mdspan with a default accessor from a pointer to an array of objects and a
  /// layout mapping
  /// @tparam ElementType The type of the pointed-to objects
  /// @tparam MappingType The type of the mapping
  /// @param ptr The pointer to the objects
  /// @param mapping The mapping
  /// @return An @c mdspan referencing the pointed-to objects with dimensions and layout specified by the mapping
  /// @pre The @c MappingType must be a valid layout mapping
  template <
      typename ElementType,
      typename MappingType,
      constraints<std::enable_if_t<is_layout_mapping_v<MappingType>>> = nullptr>
  constexpr auto operator()(ElementType* ptr, MappingType const& mapping) const noexcept
      -> mdspan<ElementType, typename MappingType::extents_type, typename MappingType::layout_type> {
    return {ptr, mapping};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: There is no identifier 'mapping' being hidden"
  /// @brief Helper function to create an @c mdspan from a handle to an array of objects, a layout mapping and an
  /// accessor
  /// @tparam MappingType The type of the mapping
  /// @tparam AccessorType The type of the accessor
  /// @param ptr The handle to the objects
  /// @param mapping The mapping
  /// @param accessor The accessor
  /// @return An @c mdspan referencing the specified objects with dimensions and layout specified by the mapping, and
  /// access policy specified by the accessor
  /// @pre The @c MappingType must be a valid layout mapping
  /// @pre The @c AccessorType must be a valid accessor policy
  template <
      typename MappingType,
      typename AccessorType,
      constraints<
          std::enable_if_t<is_layout_mapping_v<MappingType>>,
          std::enable_if_t<is_accessor_policy_v<AccessorType>>> = nullptr>
  constexpr auto operator()(
      typename AccessorType::data_handle_type const& ptr,
      MappingType const& mapping,
      AccessorType const& accessor
  ) const noexcept
      -> mdspan<
          typename AccessorType::element_type,
          typename MappingType::extents_type,
          typename MappingType::layout_type,
          AccessorType> {
    return {ptr, mapping, accessor};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a
};
}  // namespace make_mdspan_detail

/// @def arene::base::make_mdspan
/// @brief Helper function to create an @c mdspan from a C array, or pointer to data and a set of extents for the
/// dimensions. See the documentation for the function call operator overloads for the details.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(make_mdspan_detail::make_mdspan_impl, make_mdspan);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_MAKE_MDSPAN_HPP_
