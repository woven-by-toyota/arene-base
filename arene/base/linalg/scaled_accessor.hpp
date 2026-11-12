// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SCALED_ACCESSOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SCALED_ACCESSOR_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/mdspan/is_accessor_policy.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace linalg {

/// @brief Accessor wrapper to automatically apply a constant scaling factor to all accessed elements
/// @tparam ScalingFactor Type of the scaling factor to use; must be a scalar in the linalg sense
/// @tparam NestedAccessor Type of the underlying data accessor to use; must be an mdspan accessor policy
///
/// Backport of @c std::linalg::scaled_accessor from C++26.
/// Provides access to an underlying data array such that the elements of the array are automatically scaled by a
/// constant factor before being returned.
///
/// @note C++26 only requires @c ScalingFactor to be semiregular, but we require it to be a linalg scalar since this
/// class represents a combination of data access with linalg scalar multiplication.
template <class ScalingFactor, class NestedAccessor>
class scaled_accessor : private NestedAccessor {
 private:
  /// @brief Specific scaling factor used by an accessor instance
  ScalingFactor scaling_factor_;

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Identifiers do not hide anything"
  /// @brief Element type of the element space mapped by the @c access function (no elements of this type are stored)
  using element_type =
      decltype(std::declval<ScalingFactor>() * std::declval<typename NestedAccessor::element_type>()) const;
  /// @brief Reference type yielded by calls to the @c access function
  /// @note Despite the name, this type is not actually a reference, because @c access returns temporary values instead.
  using reference = std::remove_const_t<element_type>;
  /// @brief Data handle type
  using data_handle_type = typename NestedAccessor::data_handle_type;
  /// @brief Offset policy type
  using offset_policy = scaled_accessor<ScalingFactor, typename NestedAccessor::offset_policy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // These first two assertions are equivalent to the ones for a general accessor policy (element_type must be a
  // complete object type, not an abstract class, and not an array). Due to the decltype-based definition of
  // element_type and C++'s rules about what can be returned from a function, the only illegal types that can be written
  // at all are references (which fail the first assertion) and void (which fails the second).
  static_assert(!std::is_reference<element_type>::value, "Element type must not be a reference");
  static_assert(std::is_copy_constructible<reference>::value, "Reference type must be copy-constructible");
  static_assert(arene::base::linalg::is_scalar_v<ScalingFactor>, "ScalingFactor must be a linalg scalar");
  static_assert(arene::base::is_accessor_policy_v<NestedAccessor>, "NestedAccessor must be a valid accessor policy");

  /// @brief Default constructor
  constexpr scaled_accessor() = default;

  /// @brief Converting constructor; implicit SFINAE overload for when the nested accessors are implicitly convertible
  /// @tparam OtherNestedAccessor Accessor type of the other @c scaled_accessor
  /// @param other A @c scaled_accessor with the same @c ScalingFactor type but possibly a different @c NestedAccessor
  template <
      class OtherNestedAccessor,
      constraints<std::enable_if_t<std::is_convertible<OtherNestedAccessor, NestedAccessor>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Conditionally explicit when the nested accessor is
  constexpr scaled_accessor(scaled_accessor<ScalingFactor, OtherNestedAccessor> other) noexcept(
      std::is_nothrow_constructible<NestedAccessor, OtherNestedAccessor const&>::value &&
      std::is_nothrow_copy_constructible<ScalingFactor>::value
  )
      : NestedAccessor(other.nested_accessor()),
        scaling_factor_{other.scaling_factor()} {}

  /// @brief Converting constructor; explicit SFINAE overload for when the nested accessors are explicitly constructible
  /// @tparam OtherNestedAccessor Accessor type of the other @c scaled_accessor
  /// @param other A @c scaled_accessor with the same @c ScalingFactor type but possibly a different @c NestedAccessor
  template <
      class OtherNestedAccessor,
      constraints<
          std::enable_if_t<std::is_constructible<NestedAccessor, OtherNestedAccessor>::value>,
          std::enable_if_t<!std::is_convertible<OtherNestedAccessor, NestedAccessor>::value>> = nullptr>
  explicit constexpr scaled_accessor(scaled_accessor<ScalingFactor, OtherNestedAccessor> other) noexcept(
      std::is_nothrow_constructible<NestedAccessor, OtherNestedAccessor const&>::value &&
      std::is_nothrow_copy_constructible<ScalingFactor>::value
  )
      : NestedAccessor(other.nested_accessor()),
        scaling_factor_{other.scaling_factor()} {}

  /// @brief Construct a @c scaled_accessor from a scaling factor and an instance of the nested accessor
  /// @param scale Specific scaling factor to use for this accessor
  /// @param inner_accessor Instance of the nested accessor to use
  constexpr scaled_accessor(ScalingFactor const& scale, NestedAccessor const& inner_accessor) noexcept(
      std::is_nothrow_copy_constructible<NestedAccessor>::value &&
      std::is_nothrow_copy_constructible<ScalingFactor>::value
  )
      : NestedAccessor(inner_accessor),
        scaling_factor_{scale} {}

  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG(
      "-Wimplicit-int-float-conversion",
      "The noexcept specification is unevaluated so it can't lose information"
  );
  ARENE_IGNORE_CLANG("-Wsign-conversion", "The noexcept specification is unevaluated so it can't lose information");
  /// @brief Obtain the element at the specified index, multiplied by the scaling factor
  /// @param handle Handle to the underlying data
  /// @param index Offset from the first element
  /// @return The <c>index</c>th element of the given data buffer, multiplied by @c scaling_factor()
  //
  /// @note Despite the return type name, the data returned by this function is *not* a reference, it's a value.
  //
  /// @warning This performs an unchecked multiplication of @c scaling_factor() and the accessed element. Depending on
  /// their values, this may lead to infinities, NaNs, or undefined behaviour (e.g. from signed integer overflow).
  constexpr auto access(data_handle_type handle, std::size_t index) const noexcept(noexcept(
      std::declval<ScalingFactor const&>() *
      typename NestedAccessor::element_type(
          std::declval<decltype(std::declval<NestedAccessor>().access(handle, index))>()
      )
  )) -> reference {
    ARENE_IGNORE_END();
    return scaling_factor() * typename NestedAccessor::element_type(NestedAccessor::access(handle, index));
  }

  /// @brief apply an offset to a pointer
  /// @param handle Handle to the underlying data
  /// @param index offset to apply
  /// @return data handle after applying the offset
  constexpr auto offset(data_handle_type handle, std::size_t index) const
      noexcept(noexcept(std::declval<NestedAccessor>().offset(handle, index))) ->
      typename offset_policy::data_handle_type {
    return NestedAccessor::offset(handle, index);
  }

  /// @brief Get the specific scaling factor used by this accessor
  /// @return A reference to the scaling factor applied to all accessed data
  constexpr auto scaling_factor() const noexcept -> ScalingFactor const& { return scaling_factor_; }

  /// @brief Get the specific nested accessor used by this accessor
  /// @return A reference to the underlying accessor used to get data references
  constexpr auto nested_accessor() const noexcept -> NestedAccessor const& { return *this; }
};

namespace scaled_detail {

/// @brief A version of the given @c mdspan type with its accessor replaced by one which scales by @c ScalingFactor
/// @tparam Mdspan Type of the original @c mdspan type of which this typedef is a scaled version
/// @tparam ScalingFactor Type of the scaling factor to apply to elements of an @c Mdspan instance
template <class Mdspan, class ScalingFactor = typename Mdspan::value_type>
using scaled_mdspan = arene::base::mdspan<
    typename scaled_accessor<ScalingFactor, typename Mdspan::accessor_type>::element_type,
    typename Mdspan::extents_type,
    typename Mdspan::layout_type,
    scaled_accessor<ScalingFactor, typename Mdspan::accessor_type>>;

}  // namespace scaled_detail

/// @brief Return a version of the given mdspan instance that has an accessor which scales by @c scaling_factor
/// @tparam ScalingFactor Type of the scaling factor to be used; normally deduced from @c scaling_factor
/// @tparam ElementType Type of the elements of the original <c>mdspan</c>; normally deduced from @c original
/// @tparam Extents Type of the extents of the original <c>mdspan</c>; normally deduced from @c original
/// @tparam Layout Type of the layout of the original <c>mdspan</c>; normally deduced from @c original
/// @tparam Accessor Type of the accessor of the original <c>mdspan</c>; normally deduced from @c original
/// @param scaling_factor Specific amount by which to scale the elements of @c original
/// @param original The original @c mdspan instance into whose elements this function will create a scaled view
/// @return An @c mdspan instance with the same extents and layout as @c original but whose elements appear scaled
template <class ScalingFactor, class ElementType, class Extents, class Layout, class Accessor>
constexpr auto scaled(ScalingFactor scaling_factor, mdspan<ElementType, Extents, Layout, Accessor> original) noexcept(
    noexcept(scaled_detail::scaled_mdspan<mdspan<ElementType, Extents, Layout, Accessor>, ScalingFactor>{
        original.data_handle(),
        original.mapping(),
        {scaling_factor, original.accessor()}
    })
) -> scaled_detail::scaled_mdspan<mdspan<ElementType, Extents, Layout, Accessor>, ScalingFactor> {
  return {original.data_handle(), original.mapping(), {scaling_factor, original.accessor()}};
}

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SCALED_ACCESSOR_HPP_
