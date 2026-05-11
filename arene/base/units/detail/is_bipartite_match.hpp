// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_DETAIL_IS_BIPARTITE_MATCH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_DETAIL_IS_BIPARTITE_MATCH_HPP_

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/units/detail/sum.hpp"

namespace arene {
namespace base {
namespace units_detail {
namespace bipartite_match_detail {

/// @brief helper to determine the number of matches for a type with a count
/// @tparam Ts types
template <class... Ts>
class matches;

/// @brief helper to determine the number of matches for a type with a count
/// @tparam List type list
/// @tparam Ts types
template <template <class...> class List, class... Ts>
class matches<List<Ts...>> {
 public:
  /// @brief provides the total number of matches for @c Type in @c Ts
  /// @tparam Type input type
  /// @tparam Pred binary predicate metafunction
  template <class Type, template <class, class> class Pred>
  static constexpr std::size_t for_v{sum(  //
      {(Pred<Type, Ts>::value              //
            ? Ts::count
            : std::size_t{})...}
  )};
};

}  // namespace bipartite_match_detail

/// @brief determine if there is a bipartite match for types in two lists
/// @tparam Sources source type list
/// @tparam Sinks sink type list
/// @tparam Pred binary predicate metafunction
///
template <typename Sources, typename Sinks, template <class, class> class Pred>
class is_bipartite_match;

/// @brief determine if there is a bipartite match for types in two lists
/// @tparam Sources source type list
/// @tparam Sinks sink type list
/// @tparam Pred binary predicate metafunction
///
/// For each type in @c Sources, determine if the capacity in @c Sinks is
/// greater than the demand in @c Sources. When calculating total demand of
/// @c Sj in @c Sources, we must account for other types @c Sources[i] that
/// contribute to the demand of @c Sj.
///
template <template <class...> class List, class... Source, class Sinks, template <class, class> class Pred>
class is_bipartite_match<List<Source...>, Sinks, Pred>
    : public std::integral_constant<
          bool,    //
          all_of(  //
              {(bipartite_match_detail::matches<Sinks>::template for_v<Source, Pred> >=
                bipartite_match_detail::matches<List<Source...>>::template for_v<Source, Pred>)...}
          )> {};

/// @brief determine if there is a bipartite match for types in two lists
/// @tparam Sources source type list
/// @tparam Sinks sink type list
/// @tparam Pred binary predicate metafunction
///
/// Determine if for each element of @c Sources, there is a corresponding
/// matching element in @c Sinks.
///
/// @c Pred is a binary type trait with a boolean convertible
/// @c std::integral_constant base characteristic which is @c true if
/// @c Pred<Sources[i], Sinks[j]> is @c true.
///
/// @note The number of types in @c Sources needn't equal the number of types
///   in @c Sinks. Types in both @c Sources and @c Sinks type lists may contain repetitions
///   and each type must define a @c constexpr @c static @c count member.
///
/// @note This uses an application of Hall's theorem.
///
template <class Sources, class Sinks, template <class, class> class Pred>
extern constexpr bool is_bipartite_match_v = is_bipartite_match<Sources, Sinks, Pred>::value;

}  // namespace units_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_DETAIL_IS_BIPARTITE_MATCH_HPP_
