// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_COMBINE_KINDS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_COMBINE_KINDS_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_info/type_name_string.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/sort.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/units/base_kind_set.hpp"
#include "arene/base/units/detail/is_bipartite_match.hpp"
#include "arene/base/units/detail/missing.hpp"
#include "arene/base/units/detail/sum.hpp"
#include "arene/base/units/explicit_base_set.hpp"
#include "arene/base/units/is_base_quantity_kind.hpp"
#include "arene/base/units/is_kind_with_exponent.hpp"
#include "arene/base/units/is_quantity_kind.hpp"
#include "arene/base/units/is_unit.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"
#include "arene/base/units/quantity_kind.hpp"
#include "arene/base/units/quantity_unit.hpp"
#include "arene/base/units/scaled_unit.hpp"
#include "arene/base/units/unit_kind.hpp"

namespace arene {
namespace base {

namespace combine_kinds_detail {

/// @brief A helper template to convert a quantity kind raised to an exponent of 1 to just that quantity kind
/// @tparam Kind the quantity kind to unpack
template <typename Kind>
class unpack_unit_exponent_helper {
 public:
  /// @brief The unpacked quantity kind
  using type = Kind;
};

/// @brief A helper template to convert a quantity kind raised to an exponent of 1 to just that quantity kind
/// @tparam Kind the quantity kind to unpack
template <typename Kind>
class unpack_unit_exponent_helper<units_detail::quantity_kind_with_exponent<Kind, 1, 0>> {
 public:
  /// @brief The unpacked quantity kind
  using type = Kind;
};

/// @brief A helper template to convert a quantity kind raised to an exponent of 1 to just that quantity kind
/// @tparam Kind the quantity kind to unpack
template <typename Kind>
using unpack_unit_exponent_t = typename unpack_unit_exponent_helper<Kind>::type;

/// @brief Helper template to select the base class of a combined quantity kind
/// @tparam CombinedKind the derived combined quantity kind
template <typename CombinedKind, typename = constraints<>>
class combine_kind_base_helper {
 public:
  /// @brief The base class
  using type = quantity_kind<CombinedKind>;
};

/// @brief A template that represents a quantity kind that is a combination of base quantity kinds
/// @tparam BaseKinds The base quantity kinds to combine
template <typename... BaseKinds>
class combined_quantity_kind : public combine_kind_base_helper<combined_quantity_kind<BaseKinds...>>::type {
  static_assert(all_of_v<is_quantity_kind_v<BaseKinds>...>, "All combined elements must be valid quantity kinds");

 public:
  /// @brief Denotes that quantity kind is implicit and does not mark a
  /// boundary when aggregating and simplifying quantity kind types.
  using is_implicit = void;
};

/// @brief A helper template to normalize a quantity kind ready for combining
/// @tparam Kind the quantity kind to normalize
template <typename Kind>
class normalize_for_combining_helper {
 public:
  /// @brief The normalized quantity kind
  using type = units_detail::quantity_kind_with_exponent<Kind, 1, 0>;
};

/// @brief A type alias for a normalized quantity kind ready for combining
/// @tparam Kind the quantity kind to normalize
///
/// This converts all plain quantity kinds into a @c quantity_kind_with_exponent with an exponent of 1, so that the
/// logic for combining kinds only needs to deal with comparing and merging exponents. After the types have been
/// combined, @c unpack_unit_exponent_t is then used to convert any remaining instances of @c
/// quantity_kind_with_exponent with an exponent of 1 back into their base kind.
template <typename Kind>
using normalize_for_combining_t = typename normalize_for_combining_helper<Kind>::type;

/// @brief A helper template to normalize a quantity kind ready for combining
/// @tparam Kind the base quantity kind of a kind raised to an exponent being normalized
/// @tparam PositiveExponent the positive exponent the kind is raised to
/// @tparam NegativeExponent the negative exponent the kind is raised to
template <typename Kind, units_detail::exponent_t PositiveExponent, units_detail::exponent_t NegativeExponent>
class normalize_for_combining_helper<
    units_detail::quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>> {
 public:
  /// @brief The normalized quantity kind
  using type = units_detail::quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>;
};

/// @brief A helper template to normalize a quantity kind ready for combining
/// @tparam Kind the base quantity kind of a kind raised to an exponent being normalized
/// @tparam PositiveExponent the positive exponent the kind is raised to
/// @tparam NegativeExponent the negative exponent the kind is raised to
///
/// Combination logic uses the explicit base set to combine quantity kinds
/// across multiple types. However, we need to temporarily wrap scaled units to
/// preserve them.
template <typename BaseUnit, typename Scale>
class normalize_for_combining_helper<scaled_unit_detail::scaled_unit_impl<BaseUnit, Scale>> {
 public:
  /// @brief The normalized quantity kind
  using type = units_detail::quantity_kind_with_exponent<
      units_detail::explicit_base_barrier<scaled_unit_detail::scaled_unit_impl<BaseUnit, Scale>>,
      1,
      0>;
};

/// @brief A helper template to normalize a quantity kind ready for combining
/// @tparam Kinds the quantity kinds that are already combined
template <typename... Kinds>
class normalize_for_combining_helper<combined_quantity_kind<Kinds...>> {
 public:
  /// @brief The normalized quantity kind
  using type = combined_quantity_kind<normalize_for_combining_t<Kinds>...>;
};

/// @brief Helper template that compares two quantity kinds to determine which should be ordered before the other when
/// combining
/// @tparam LhsKind The left-hand kind being compared
/// @tparam RhsKind The right-hand kind being compared
template <typename LhsKind, typename RhsKind>
class kind_sorts_before {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This member is the public API"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: type_name_v is initialized"
  /// @brief The result of the comparison
  static constexpr bool value{type_name_v<LhsKind> < type_name_v<RhsKind>};
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Helper template that compares two quantity kinds to determine which should be ordered before the other when
/// combining. The important part is that this groups all instances with the same base quantity kind together, to make
/// it easier to combine them; the ordering on the exponents is to make the order stable.
/// @tparam LhsKind The base kind of the left-hand kind being compared
/// @tparam LhsPositiveExponent The positive exponent of the left-hand kind being compared
/// @tparam LhsNegativeExponent The negative exponent of the left-hand kind being compared
/// @tparam RhsKind The base kind of the right-hand kind being compared
/// @tparam RhsPositiveExponent The positive exponent of the right-hand kind being compared
/// @tparam RhsNegativeExponent The negative exponent of the right-hand kind being compared
template <
    typename LhsKind,
    units_detail::exponent_t LhsPositiveExponent,
    units_detail::exponent_t LhsNegativeExponent,
    typename RhsKind,
    units_detail::exponent_t RhsPositiveExponent,
    units_detail::exponent_t RhsNegativeExponent>
class kind_sorts_before<
    units_detail::quantity_kind_with_exponent<LhsKind, LhsPositiveExponent, LhsNegativeExponent>,
    units_detail::quantity_kind_with_exponent<RhsKind, RhsPositiveExponent, RhsNegativeExponent>> {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This member is the public API"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: type_name_v is initialized"
  /// @brief The result of the comparison
  static constexpr bool value{
      (type_name_v<LhsKind> < type_name_v<RhsKind>) ||
      ((type_name_v<LhsKind> == type_name_v<RhsKind>)&&(
          (LhsPositiveExponent < RhsPositiveExponent) ||
          ((LhsPositiveExponent == RhsPositiveExponent) && (LhsNegativeExponent < RhsNegativeExponent))
      ))
  };
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Helper template to strip any unnecessary @c combined_quantity_kind or @c quantity_kind_with_exponent wrappers
/// @tparam Kinds the @c combined_quantity_kind wrapper being stripped
template <typename Kind>
class canonicalize_post_combining {
 public:
  /// @brief quantity kind type
  using type = Kind;
};

/// @brief Helper template to strip any unnecessary @c combined_quantity_kind or @c quantity_kind_with_exponent wrappers
/// @tparam Kinds the quantity kinds in the list
template <typename... Kinds>
class canonicalize_post_combining<combined_quantity_kind<Kinds...>> {
 public:
  /// @brief The stripped kind
  using type = combined_quantity_kind<typename canonicalize_post_combining<Kinds>::type...>;
};

/// @brief Helper template to strip any unnecessary @c combined_quantity_kind or @c quantity_kind_with_exponent wrappers
/// @tparam Kind the single quantity kind in the list
template <typename Kind>
class canonicalize_post_combining<combined_quantity_kind<Kind>> : public canonicalize_post_combining<Kind> {};

/// @brief Helper template to strip any unnecessary @c combined_quantity_kind or @c quantity_kind_with_exponent wrappers
template <typename Kind, units_detail::exponent_t PositiveExponent, units_detail::exponent_t NegativeExponent>
class canonicalize_post_combining<units_detail::quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>> {
 public:
  /// @brief simplified quantity kind type
  using type = conditional_t<
      (PositiveExponent == 1 && NegativeExponent == 0),
      typename canonicalize_post_combining<Kind>::type,
      units_detail::quantity_kind_with_exponent<
          typename canonicalize_post_combining<Kind>::type,
          PositiveExponent,
          NegativeExponent>>;
};

/// @brief Helper template to strip any unnecessary @c combined_quantity_kind or @c quantity_kind_with_exponent wrappers
/// @tparam Kind quantity kind
///
/// Unwrap any barriers introduced when determining the explicit base set.
template <typename Kind>
class canonicalize_post_combining<units_detail::explicit_base_barrier<Kind>> {
 public:
  /// @brief quantity kind type
  using type = Kind;
};

/// @brief Helper type sort a list of quantity kinds and combine them
/// @tparam Kinds the quantity kinds to combine
/// Provides the combined quantity kind types in combined normal form.
template <typename... Kinds>
class internal_combine_kinds {
  /// @brief defines the combined explicit base sets of @c Kinds
  struct explicit_base_set_union {
    // parasoft-begin-suppress AUTOSAR-A3_3_2-a "False positive: variable 'value' is initialized"
    // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable 'value' is initialized"
    /// @brief explicit bases set for @c Kinds...
    static constexpr auto value = units_detail::set_union(units_detail::explicit_base_set<Kinds>...);
    // parasoft-end-suppress AUTOSAR-A3_3_2-a
    // parasoft-end-suppress CERT_CPP-DCL56-a
  };

 public:
  /// @brief the combined quantity kind in combined normal form
  using type = type_lists::apply_all_t<
      type_lists::sort_t<
          units_detail::as_list_of_kinds_with_exponents_t<explicit_base_set_union>,
          combine_kinds_detail::kind_sorts_before>,
      combined_quantity_kind>;
};

/// @brief Helper type to sort a list of quantity kinds and combine them
/// @tparam Kinds the quantity kinds to combine
/// Provides the combined quantity kind types in combined normal form.
template <typename... Kinds>
using internal_combine_kinds_t = typename internal_combine_kinds<Kinds...>::type;

}  // namespace combine_kinds_detail

/// @brief Template alias to combine a list of quantity kinds
/// @tparam Kinds the quantity kinds to combine
template <typename... Kinds>
using combine_kinds_t =                                                //
    typename combine_kinds_detail::canonicalize_post_combining<        //
        typename combine_kinds_detail::internal_combine_kinds<         //
            combine_kinds_detail::normalize_for_combining_t<Kinds>...  //
            >::type                                                    //
        >::type;

namespace units_detail {

/// @brief Helper class to determine the result of raising a combined quantity kind to a positive and negative exponent
/// @tparam BaseKinds The quantity kinds to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename... BaseKinds, exponent_t PositiveExponent, exponent_t NegativeExponent>
class kind_with_exponent_helper<
    combine_kinds_detail::combined_quantity_kind<BaseKinds...>,
    PositiveExponent,
    NegativeExponent,
    // negated expr matches constraint for other specialization
    // NOLINTNEXTLINE(readability-simplify-boolean-expr)
    constraints<std::enable_if_t<!(PositiveExponent == 1 && NegativeExponent == 0)>>> {
 public:
  /// @brief An alias for a quantity kind representing the combined quantity kind raised to the specified exponents
  using type =
      combine_kinds_t<typename kind_with_exponent_helper<BaseKinds, PositiveExponent, NegativeExponent>::type...>;
};

}  // namespace units_detail

namespace combine_kinds_detail {

/// @brief Helper template to select the base class of a combined quantity kind
/// @tparam BaseKinds the individual quantity kinds that are combined in the derived quantity kind
///
/// This specialization handles the case that the kinds being combined are all units, so the result is a unit
template <typename... BaseKinds>
class combine_kind_base_helper<
    combined_quantity_kind<BaseKinds...>,
    constraints<std::enable_if_t<all_of_v<is_unit_v<BaseKinds>...>>>> {
 public:
  /// @brief The base class
  using type = quantity_unit<  //
      combined_quantity_kind<BaseKinds...>,
      combine_kinds_t<unit_kind_t<BaseKinds>...>>;
};

/// @brief obtain a quantity kind with exponent values
/// @tparam Kind quantity kind type
///
/// Primary template specifying a quantity kind with positive exponent 1.
template <class Kind>
class with_exponent {
 public:
  /// @brief quantity kind with exponent
  using type = units_detail::quantity_kind_with_exponent<Kind, 1, 0>;
};

/// @brief obtain a quantity kind with exponent values
/// @tparam Kind quantity kind type
/// @tparam Positive positive expoenent
/// @tparam Negative negative expoenent
///
/// Template specialization for a quantity kind that is already a specialization of @c quantity_kind_with_exponent
template <class Kind, units_detail::exponent_t Positive, units_detail::exponent_t Negative>
class with_exponent<units_detail::quantity_kind_with_exponent<Kind, Positive, Negative>> {
 public:
  /// @brief quantity kind with exponent
  using type = units_detail::quantity_kind_with_exponent<Kind, Positive, Negative>;
};

/// @brief obtain a quantity kind with exponent values
/// @tparam Kind quantity kind type
template <typename Kind>
using with_exponent_t = typename with_exponent<Kind>::type;

/// @brief polarity states allowing independent analysis of
/// positive/negative/zero quantity kind exponents
enum class polarity : std::uint8_t {
  /// @brief corresponds to quantity kinds with negative exponent values
  negative,
  /// @brief corresponds to special case of quantity kinds with 0 positive and 0 negative exponent values
  zero,
  /// @brief corresponds to quantity kinds with positive exponent values
  positive
};

/// @brief extracts a single exponent value of a quantity kind
/// @tparam Sign polarity sign
/// @tparam KindWithExponent quantity kind with exponents
template <polarity Sign, class KindWithExponent>
struct polarity_wrapper;

/// @brief extracts a single exponent value of a quantity kind
/// @tparam Kind quantity kind
/// @tparam Positive positive exponent value
/// @tparam Negative negative exponent value
///
/// Specialization for negative exponents.
template <typename Kind, units_detail::exponent_t Positive, units_detail::exponent_t Negative>
struct polarity_wrapper<polarity::negative, units_detail::quantity_kind_with_exponent<Kind, Positive, Negative>> {
  /// @brief polarity sign
  static constexpr auto sign = polarity::negative;
  // parasoft-begin-suppress AUTOSAR-M5_0_4-a "No implementation-defined behavior in this constant expression"
  /// @brief number of powers with the same sign
  static constexpr std::size_t count{Negative};
  // parasoft-end-suppress AUTOSAR-M5_0_4-a
  /// @brief quantity kind
  using quantity_kind_type = Kind;
};

/// @brief extracts a single exponent value of a quantity kind
/// @tparam Kind quantity kind
/// @tparam Positive positive exponent value
/// @tparam Negative negative exponent value
///
/// Specialization for zero exponents.
template <typename Kind, units_detail::exponent_t Positive, units_detail::exponent_t Negative>
struct polarity_wrapper<polarity::zero, units_detail::quantity_kind_with_exponent<Kind, Positive, Negative>> {
  /// @brief polarity sign
  static constexpr auto sign = polarity::zero;
  // parasoft-begin-suppress AUTOSAR-A5_16_1-a "False positive: Conditional operator not used as subexpression"
  /// @brief number of powers with the same sign
  /// This handles the special case where a quantity kind explicitly has a 0
  /// positive exponent and a 0 negative exponent.
  static constexpr std::size_t count{((Positive == 0) && (Negative == 0)) ? 1U : 0U};
  // parasoft-end-suppress AUTOSAR-A5_16_1-a
  /// @brief quantity kind
  using quantity_kind_type = Kind;
};

/// @brief extracts a single exponent value of a quantity kind
/// @tparam Kind quantity kind
/// @tparam Positive positive exponent value
/// @tparam Negative negative exponent value
///
/// Specialization for positive exponents.
template <typename Kind, units_detail::exponent_t Positive, units_detail::exponent_t Negative>
struct polarity_wrapper<polarity::positive, units_detail::quantity_kind_with_exponent<Kind, Positive, Negative>> {
  /// @brief polarity sign
  static constexpr auto sign = polarity::positive;
  // parasoft-begin-suppress AUTOSAR-M5_0_4-a "No implementation-defined behavior in this constant expression"
  /// @brief number of powers with the same sign
  static constexpr std::size_t count{Positive};
  // parasoft-end-suppress AUTOSAR-M5_0_4-a
  /// @brief quantity kind
  using quantity_kind_type = Kind;
};

/// @brief determine if a kind is a sink for another
/// @tparam Source source polarity wrapped quantity kind
/// @tparam Sink sink polarity wrapped quantity kind
///
/// @c Sink is a sink for @c Source if they have the same polarity and the @c
/// Sink::quantity_kind_type is the same or a base quantity kind of @c
/// Source::quantity_kind_type.
template <typename Source, typename Sink>
using is_kind_sink = std::integral_constant<
    bool,
    ((Source::sign == Sink::sign) &&
     (std::is_same<typename Source::quantity_kind_type, typename Sink::quantity_kind_type>::value ||
      is_base_quantity_kind_of_v<typename Sink::quantity_kind_type, typename Source::quantity_kind_type>))>;

/// @brief determine is there is a bijective bipartite match for two sets of types
/// @tparam Sign polarity sign
/// @tparam Sources source quantity kind types
/// @tparam Sink sink quantity kind types
///
/// Dummy primary template
template <polarity Sign, typename Sources, typename Sinks>
extern constexpr auto has_pairwise_match_v = units_detail::missing{};

/// @brief helper to ensure that types are unique
/// @tparam Ts types
///
/// Helper used to ensure that quantity kind types are correctly grouped. Causes
/// a hard error if a type is duplicated.
template <typename... Ts>
class error_if_duplicate_types : Ts... {};

/// @brief determine is there is a bijective bipartite match for two sets of types
/// @tparam Sign polarity sign
/// @tparam Sources source quantity kind types
/// @tparam Sink sink quantity kind types
template <polarity Sign, typename... Sources, typename... Sinks>
extern constexpr bool has_pairwise_match_v<
    Sign,
    type_list<Sources...>,
    type_list<Sinks...>>  //
    {
        (error_if_duplicate_types<typename Sources::quantity_kind_type...>{}, true) &&
        (error_if_duplicate_types<typename Sinks::quantity_kind_type...>{}, true) &&
        (units_detail::sum({polarity_wrapper<Sign, with_exponent_t<Sources>>::count...}) ==
         units_detail::sum({polarity_wrapper<Sign, with_exponent_t<Sinks>>::count...})) &&
        units_detail::is_bipartite_match_v<                                  //
            type_list<polarity_wrapper<Sign, with_exponent_t<Sources>>...>,  //
            type_list<polarity_wrapper<Sign, with_exponent_t<Sinks>>...>,    //
            is_kind_sink>                                                    //
    };

}  // namespace combine_kinds_detail

namespace is_base_quantity_kind_detail {

/// @brief Type trait for checking if one type is a base quantity kind for another. Evaluates to @c true if the first
/// type is a base quantity kind of the second (the second is a derived quantity kind of the first), @c false  otherwise
/// @tparam BaseKinds the quantity kinds combined for the base quantity kind
/// @tparam CheckKinds the quantity kinds combined for the quantity kind to be checked
///
/// For combined quantity kinds, one kind is a base kind of the other if they are not the same, they have the same
/// number of elements, and for the elements can be paired off such that there is a corresponding element from the
/// former that is the same as or a base of each element of the latter.
template <typename... BaseKinds, typename... CheckKinds>
extern constexpr bool is_base_quantity_kind_of_v<
    combine_kinds_detail::combined_quantity_kind<BaseKinds...>,
    combine_kinds_detail::combined_quantity_kind<CheckKinds...>,
    constraints<           //
        std::enable_if_t<  //
            !std::is_same<
                combine_kinds_detail::combined_quantity_kind<BaseKinds...>,
                combine_kinds_detail::combined_quantity_kind<CheckKinds...>>::value>>>  //
    {
        combine_kinds_detail::has_pairwise_match_v<       //
            combine_kinds_detail::polarity::negative,     //
            type_list<CheckKinds...>,                     //
            type_list<BaseKinds...>>                      //
        && combine_kinds_detail::has_pairwise_match_v<    //
               combine_kinds_detail::polarity::zero,      //
               type_list<CheckKinds...>,                  //
               type_list<BaseKinds...>>                   //
        && combine_kinds_detail::has_pairwise_match_v<    //
               combine_kinds_detail::polarity::positive,  //
               type_list<CheckKinds...>,                  //
               type_list<BaseKinds...>>                   //
    };

/// @brief Type trait for checking if one type is a base quantity kind for another. Evaluates to @c true if the first
/// type is a base quantity kind of the second (the second is a derived quantity kind of the first), @c false  otherwise
/// @tparam BaseKind the quantity kind raised to the exponent for the base quantity kind
/// @tparam PositiveExponent the positive exponent for the base quantity kind
/// @tparam NegativeExponent the negative exponent for the base quantity kind
/// @tparam CheckKinds the quantity kinds combined for the quantity kind to be checked
///
/// A kind with exponent is the base of a combined kind if each of the combined elements are derived from that base with
/// the right total exponents
template <
    typename BaseKind,
    typename... CheckKinds>
extern constexpr bool is_base_quantity_kind_of_v<  //
    BaseKind,
    combine_kinds_detail::combined_quantity_kind<CheckKinds...>,
    constraints<           //
        std::enable_if_t<  //
            !is_instantiation_of_v<
                BaseKind,
                combine_kinds_detail::combined_quantity_kind>>>>  //
    {
        is_base_quantity_kind_of_v<                                       //
            combine_kinds_detail::combined_quantity_kind<BaseKind>,       //
            combine_kinds_detail::combined_quantity_kind<CheckKinds...>>  //
    };

/// @brief Type trait for checking if one type is a base quantity kind for another. Evaluates to @c true if the first
/// type is a base quantity kind of the second (the second is a derived quantity kind of the first), @c false  otherwise
/// @tparam BaseKinds the quantity kinds combined for the base quantity kind
/// @tparam CheckKind the quantity kind raised to the exponent for the quantity kind to check
/// @tparam PositiveExponent the positive exponent for the quantity kind to check
/// @tparam NegativeExponent the negative exponent for the quantity kind to check
///
/// A combined kind is the base for a kind with exponent if each of the combined kinds is a base for the kind with
/// exponent, with the appropriate total exponents
template <typename... BaseKinds, typename CheckKind>
extern constexpr bool is_base_quantity_kind_of_v<
    combine_kinds_detail::combined_quantity_kind<BaseKinds...>,
    CheckKind,
    constraints<                                                             //
        std::enable_if_t<units_detail::is_kind_with_exponent_v<CheckKind>>,  //
        std::enable_if_t<                                                    //
            !is_base_quantity_kind_of_v<                                     //
                combine_kinds_detail::combined_quantity_kind<BaseKinds...>,  //
                typename CheckKind::parent_quantity_kind_type>>              //
        >>                                                                   //
    {
        is_base_quantity_kind_of_v<                                      //
            combine_kinds_detail::combined_quantity_kind<BaseKinds...>,  //
            combine_kinds_detail::combined_quantity_kind<CheckKind>>     //
    };

}  // namespace is_base_quantity_kind_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_COMBINE_KINDS_HPP_
