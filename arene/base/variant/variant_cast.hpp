// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// parasoft-suppress ALL "False Positive: there is no executable code on line 1"
// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: this header intentionally defines both an implementation
// class and variable template API."
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VARIANT_CAST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VARIANT_CAST_HPP_
// IWYU pragma: private, include "arene/base/variant.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/underlying_type.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/in_place.hpp"
#include "arene/base/variant/variant.hpp"
#include "arene/base/variant/visit.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace variant_cast_detail {

/// @brief Forward declaration of the type compatibility enum class. See @c type_compatibility for details.
enum class type_compatibility : std::uint8_t;

/// @brief Bit position of the explicit-conversion flag within @c type_compatibility.
constexpr std::underlying_type_t<type_compatibility> explicit_conversion_bit{0U};
/// @brief Bit position of the implicit-conversion flag within @c type_compatibility.
constexpr std::underlying_type_t<type_compatibility> implicit_conversion_bit{1U};
/// @brief Bit position of the exact-match flag within @c type_compatibility.
constexpr std::underlying_type_t<type_compatibility> exact_match_bit{2U};

//// @brief The type compatibility between two types for the purposes of determining if two type spaces are compatible.
/// @note This is encoded as a bit map to avoid branches in the @c type_compatibility_v() function which cannot be
/// covered since they're type-dependent not value dependent.
enum class type_compatibility : std::uint8_t {

  /// @brief The two types are incompatible
  incompatible = 0b0000'0000U,
  // parasoft-begin-suppress AUTOSAR-A5_1_1-a "False positive: literals are used to define the named constants"
  // parasoft-begin-suppress AUTOSAR-M5_0_12-a "False positive: shift of uint8_t enumerator is intentional"
  /// @brief The two types satisfy @c std::is_constructible<To,From>
  explicit_conversion = 0b0000'0001U << explicit_conversion_bit,
  /// @brief The two types satisfy @c is_implicitly_constructible<To,From>
  implicit_conversion = 0b0000'0001U << implicit_conversion_bit,
  /// @brief The two types satisfy @c std::is_same<From,To>
  exact_match = 0b0000'0001U << exact_match_bit,
  // parasoft-end-suppress AUTOSAR-M5_0_12-a
  // parasoft-end-suppress AUTOSAR-A5_1_1-a
};

/// @brief The underlying type of @c type_compatibility, used for bitwise operations.
using type_compatibility_bits = std::underlying_type_t<type_compatibility>;

/// @brief Encodes a boolean value as a bit mask to be used in conjunction with @c type_compatibility_bits.
/// @param compatibility The type compatibility to encode.
/// @param has_compatibility A boolean value indicating whether the compatibility is present.
/// @return The bit mask representation of the compatibility, or zero if not present.
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
constexpr auto encode_type_compatibility(type_compatibility const compatibility, bool const has_compatibility) noexcept
    -> type_compatibility_bits {
  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
  return has_compatibility ? static_cast<type_compatibility_bits>(compatibility) : 0U;
}

/// @brief Determine the type compatibility between two types.
/// @tparam From The type to convert from. Should retain the same cvref qualification as the source variant.
/// @tparam To The type to convert to. Should not be cref qualified.
/// @return type_compatibility The type compatibility between @c From and @c To. The order of matching preference is
///         exact match, implicit conversion, explicit conversion, incompatible.
template <typename From, typename To>
constexpr auto type_compatibility_v() noexcept -> type_compatibility_bits {
  return encode_type_compatibility(type_compatibility::explicit_conversion, std::is_constructible<To, From>::value) |
         encode_type_compatibility(type_compatibility::implicit_conversion, is_implicitly_constructible_v<To, From>) |
         encode_type_compatibility(type_compatibility::exact_match, std::is_same<remove_cvref_t<From>, To>::value);
}

/// @brief Determine if a compatibility list contains a compatible type.
/// @param compatibilities A list of type compatibilities between a source type and a list of target types.
/// @param compatibility The type compatibility to check for.
/// @return bool True if the compatibility list contains a compatible type, false otherwise.
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
constexpr auto has_type_compatibility_v(
    type_compatibility_bits const compatibilities,
    type_compatibility const compatibility
) noexcept -> bool {
  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
  return ((compatibilities & static_cast<type_compatibility_bits>(compatibility)) != 0U) ||
         ((compatibility == type_compatibility::incompatible) && (compatibilities == 0U));
}

/// @brief A sentinel index of an incompatible type in a type space.
/// @return The maximum value of @c std::size_t , used as a sentinel.
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
constexpr auto incompatible_type_index() noexcept -> std::size_t { return std::numeric_limits<std::size_t>::max(); }
// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

/// @brief Determine the index of the compatible type in a list of types given a list of type compatibilities.
/// @param compatibilities A list of type compatibilities between a source type and a list of target types.
/// @return std::size_t The index of the compatible type in the list of target types. If there is no compatible type, or
///         if there is more than one compatible type and none are an exact match, then returns
///         @c incompatible_type_index().
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
constexpr auto index_of_compatible_type_v(std::initializer_list<type_compatibility_bits> const compatibilities) noexcept
    -> std::size_t {
  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
  std::size_t current_index{0U};
  std::size_t implicit_conversion_count{0U};
  std::size_t explicit_conversion_count{0U};
  std::size_t non_exact_match_index{incompatible_type_index()};
  // parasoft-begin-suppress CERT_C-INT36-b "False positive: 'const type_compatibility*' not converted to integer"
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: 'compatibility' has type deduced from range element"
  for (auto const compatibility : compatibilities) {
    // parasoft-end-suppress AUTOSAR-A7_1_5-a
    // parasoft-end-suppress CERT_C-INT36-b
    if (has_type_compatibility_v(compatibility, type_compatibility::exact_match)) {
      return current_index;
    }
    if (has_type_compatibility_v(compatibility, type_compatibility::implicit_conversion)) {
      ++implicit_conversion_count;
      non_exact_match_index = current_index;
    } else if (has_type_compatibility_v(compatibility, type_compatibility::explicit_conversion)) {
      if (implicit_conversion_count == 0U) {
        // if there was no implicit conversion, then this is the best match so far
        non_exact_match_index = current_index;
      }
      ++explicit_conversion_count;
    } else {
      // empty clause required by M6-4-2, this means it's incompatible and nothing to mark
    }
    ++current_index;
  }
  // At this point either we returned already with an exact match, or non_exact_match has the index of the last implicit
  // or explicit conversion match, or incompatible_type_index() if there were no matches.
  // Therefore, if there is more than one implicit conversion match, or if there is no implicit conversion match and
  // more than one explicit conversion match, then we have an ambiguous match and return incompatible_type_index().
  if (implicit_conversion_count > 1U) {
    return incompatible_type_index();
  }
  if ((implicit_conversion_count == 0U) && (explicit_conversion_count > 1U)) {
    return incompatible_type_index();
  }
  return non_exact_match_index;
}

/// @brief Determine the index of the type in @c Tos that is compatible with @c From.
/// @tparam From The type to convert from. Should retain the same cvref qualification as the source variant.
/// @tparam Tos The types to convert to. Should not be cref qualified.
/// @return std::size_t The index of the type in @c Tos that is compatible with @c From. If there is no compatible type,
/// or if there is more than one compatible type and none are an exact match, then returns @c incompatible_type_index().
template <typename From, typename... Tos>
constexpr auto index_of_compatible_type_v() -> std::size_t {
  return index_of_compatible_type_v(std::initializer_list<type_compatibility_bits>{type_compatibility_v<From, Tos>()...}
  );
}

/// @brief Trait helper for SFINAE'ing on index of a compatible type. The empty base template for when it's not found.
/// @tparam Idx The index of the compatible type in @c Tos. Should be the result of <c>index_of_compatible_type_v<From,
/// Tos...>()</c>.
// parasoft-begin-suppress AUTOSAR-A11_0_2-a "False positive: inheritance from integral_constant is permitted"
template <std::size_t Idx, bool = (Idx < incompatible_type_index())>
struct compatible_type_index_helper;

/// @brief Specialization of @c compatible_type_index_helper for when a compatible type is found in @c Tos.
/// @tparam Idx The index of the compatible type in @c Tos. Should be the result of <c>index_of_compatible_type_v<From,
/// Tos...>()</c>.
template <std::size_t Idx>
struct compatible_type_index_helper<Idx, true> : std::integral_constant<std::size_t, Idx> {};

/// @brief Trait for determining the index of the type in @c Tos that is compatible with @c From.
/// @tparam From The type to convert from. Should retain the same cvref qualification as the source variant.
/// @tparam Tos The types to convert to. Should not be cref qualified.
template <typename From, typename... Tos>
struct compatible_type_index : compatible_type_index_helper<index_of_compatible_type_v<From, Tos...>()> {};
// parasoft-end-suppress AUTOSAR-A11_0_2-a

/// @brief Empty base template of cast operation to hoist Tos... out of ToVariant
/// @tparam ToVariant The variant type to cast to.
template <typename ToVariant>
class cast_impl;

/// @brief Specialization of cast_impl for casting to a variant type.
/// @tparam Tos The types that the variant can hold.
template <typename... Tos>
class cast_impl<variant<Tos...>> {
 public:
  /// @brief Cast a value of type @c From to a variant of type @c variant<Tos...> by constructing the value in place.
  /// @tparam From The type to convert from.
  /// @param from The value to convert from.
  /// @return variant<Tos...> A variant of type @c variant<Tos...> containing the value from @c from .
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
  template <typename From, typename CompatibleTypeIndex = compatible_type_index<From&&, Tos...>>
  constexpr auto operator()(From&& from) const
      noexcept(noexcept(variant<Tos...>{in_place_index<CompatibleTypeIndex::value>, std::forward<From>(from)}))
          -> variant<Tos...> {
    return variant<Tos...>{in_place_index<CompatibleTypeIndex::value>, std::forward<From>(from)};
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a-2
};

/// @brief Implementation of variant_cast function object.
/// @tparam ToVariant The variant type to cast to.
template <typename ToVariant>
class variant_cast_fn {
  static_assert(is_instantiation_of_v<ToVariant, variant>, "'ToVariant' must be a variant type");

 public:
  /// @brief Cast a variant to another variant type.
  /// @tparam FromVariant The type of the variant to cast from.
  /// @param from The variant to cast from.
  /// @return ToVariant A variant of type @c ToVariant containing the value from @c from .
  template <typename FromVariant>
  auto operator()(FromVariant&& from) const
      noexcept(noexcept(::arene::base::visit<ToVariant>(cast_impl<ToVariant>{}, std::forward<FromVariant>(from))))
          -> ToVariant {
    static_assert(is_instantiation_of_v<remove_cvref_t<FromVariant>, variant>, "'FromVariant' must be a variant type");
    return ::arene::base::visit<ToVariant>(cast_impl<ToVariant>{}, std::forward<FromVariant>(from));
  }
};
}  // namespace variant_cast_detail

// NOLINTNEXTLINE(google-build-namespaces) explicitly want per-TU instantiations
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
namespace {
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) FP, there is no non-const global variable here.
/// @brief Cast a variant to another variant type of a compatible type space.
/// @tparam ToVariant The variant type to cast to. The type space of @c ToVariant must be compatible with the type space
///         of the variant being cast from.
/// @copydoc variant_cast_detail::variant_cast_fn<ToVariant>::operator()
/// @note Casting to a variant with duplicate types is not supported until
///       <c>variant(std::in_place_index_t<I>, Args&&...)</c> constructor is implemented.
/// @details A compatible _type space_ is defined as follows: For every type <c>From...</c> in the source variant,
///          there must be a type in <c>To...</c> which satisfies <c>std::is_constructible<To, From></c> where
///          <c>From</c>'s cref qualification matches the source variant's. There must be a single unambiguously
///          matched type in <c>To...</c> for every type in <c>From...</c>, where _unambiguously_ follows normal C++
///          overload resolution rules.
template <typename ToVariant>
ARENE_MAYBE_UNUSED constexpr auto const& variant_cast = variant_cast_detail::variant_cast_fn<ToVariant>{};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A2_8_1-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VARIANT_CAST_HPP_
