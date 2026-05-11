// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_MUTATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_MUTATE_HPP_

#include "arene/base/type_list.hpp"

namespace testing {

namespace mutate_detail {

/// @brief Mutate the types in @c TypeList by all of @c Mutators
/// @tparam TypeList Storage for the types to mutate
/// @tparam Mutators To apply to type
template <class TypeList, template <class> class... Mutators>
struct mutate {
  using type = arene::base::type_lists::concat_t<typename mutate<TypeList, Mutators>::type...>;
};

/// @brief Take all of @c Types in list @c TypeList, mutate each by @c Mutator
/// Base case for expansion of @c Mutators
/// @tparam TypeList Storage for the types to mutate
/// @tparam Types The types to mutate
/// @tparam Mutator To apply to types
template <template <class...> class TypeList, class... Types, template <class> class Mutator>
struct mutate<TypeList<Types...>, Mutator> {
  using type = TypeList<typename Mutator<Types>::type...>;
};

}  // namespace mutate_detail

/// @brief Does not modify input type
/// @tparam Type To modify
template <class Type>
struct type_identity {
  using type = Type;
};

/// @brief Add @c const to input type
/// @tparam Type To modify
template <class Type>
struct add_const {
  using type = Type const;
};

/// @brief Add @c volatile to input type
/// @tparam Type To modify
template <class Type>
struct add_volatile {
  using type = Type volatile;
};

/// @brief Add @c const @c volatile to input type
/// @tparam Type To modify
template <class Type>
struct add_const_volatile {
  using type = Type const volatile;
};

/// @brief Add lvalue reference to input type
/// @tparam Type To modify
template <class Type>
struct add_lvalue_reference {
  using type = Type&;
};

/// @brief Add rvalue reference to input type
/// @tparam Type To modify
template <class Type>
struct add_rvalue_reference {
  using type = Type&&;
};

/// @brief Mutate all types within @c Types by each mutator in @c Mutators
/// @tparam Types Contains types to mutate
/// @tparam Mutators Mutates a given type
template <class Types, template <class> class... Mutators>
using mutate_t = typename mutate_detail::mutate<Types, Mutators...>::type;

/// @brief Mutate all types within @c Types to contain the type identity, @c const, @c volatile, and @c const
/// @c volatile
/// @tparam Types Contains types to mutate
template <class Types>
using extend_with_cv_qualifiers_t = mutate_t<Types, type_identity, add_const, add_volatile, add_const_volatile>;

/// @brief Mutate all types within @c Types to contain the type identity, lvalue reference, and rvalue reference
/// @tparam Types Contains types to mutate
template <class Types>
using extend_with_references_t = mutate_t<Types, type_identity, add_lvalue_reference, add_rvalue_reference>;

/// @brief Mutate all types within @c Types to contain the type identity and all possible mutations
/// @tparam Types Contains types to mutate
template <class Types>
using extend_with_all_t = extend_with_references_t<extend_with_cv_qualifiers_t<Types>>;

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_MUTATE_HPP_
