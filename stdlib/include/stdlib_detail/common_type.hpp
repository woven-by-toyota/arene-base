// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COMMON_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COMMON_TYPE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/decay.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"

namespace std {

namespace common_type_detail {

// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: Everything in this file has a comment that contains @brief"

/// @brief Helper type that determines the type of a conditional expression between @c T1 and @c T2
/// @tparam T1 First type
/// @tparam T2 Second type
template <class T1, class T2>
using conditional_result_t = decltype(false ? declval<T1>() : declval<T2>());

/// @brief Primary template for decaying conditional result types
/// @tparam T1 First type
/// @tparam T2 Second type
/// @tparam Empty SFINAE constraints parameter
template <class T1, class T2, class = arene::base::constraints<>>
class decayed_conditional_result {};

/// @brief Specialization that provides the decayed type of a conditional expression
/// @tparam T1 First type
/// @tparam T2 Second type
template <class T1, class T2>
class decayed_conditional_result<T1, T2, arene::base::constraints<conditional_result_t<T1, T2>>> {
 public:
  using type = ::std::decay_t<conditional_result_t<T1, T2>>;
};

/// @brief Helper alias template for decayed_conditional_result
/// @tparam T1 First type
/// @tparam T2 Second type
template <class T1, class T2>
using decayed_conditional_result_t = typename decayed_conditional_result<T1, T2>::type;

/// @brief Helper variable template that checks if both types are already decayed
/// @tparam T1 First type
/// @tparam T2 Second type
template <class T1, class T2>
extern constexpr bool are_already_decayed_v = is_same_v<T1, decay_t<T1>> && is_same_v<T2, decay_t<T2>>;

}  // namespace common_type_detail

/// @brief Primary template for determining the common type of a set of types
///
/// The primary template is the base case for the empty parameter pack, which defines no member alias named @c type.
/// Note: This differs from the original published C++14 specification, which was not SFINAE friendly, to be compliant
/// with LWG issues 2408 and 2465.
///
/// @tparam Empty parameter pack
template <class...>
class common_type {};

/// @brief Specialization for a single type
///
/// For a single type T, the common type is defined as the common type of T with itself.
///
/// @tparam T The single type
template <class T>
class common_type<T> : public common_type<T, T> {};

/// @brief Specialization for two types
///
/// Determines the common type of two types @c T1 and @c T2. If both types are already
/// decayed, computes the decayed type of the conditional expression <c> false ? T1 : T2 </c>
/// Otherwise, determines the common type of the decayed forms of @c T1 and @c T2.
///
/// @tparam T1 First type
/// @tparam T2 Second type
template <typename T1, typename T2>
class common_type<T1, T2>
    : public conditional_t<
          common_type_detail::are_already_decayed_v<T1, T2>,
          common_type_detail::decayed_conditional_result<T1, T2>,
          common_type<decay_t<T1>, decay_t<T2>>> {};

/// @brief Helper alias template for common_type
/// @tparam Ts Types to find common type for
template <class... Ts>
using common_type_t = typename common_type<Ts...>::type;

namespace common_type_detail {

/// @brief Primary template for computing common type of 3 or more types
/// @tparam Constraints SFINAE constraints parameter
/// @tparam T1 First type
/// @tparam T2 Second type
/// @tparam Ts Remaining types
template <class Constraints, typename T1, typename T2, typename... Ts>
class common_type_3_or_more {};

/// @brief Specialization for computing common type of 3 or more types
/// @tparam T1 First type
/// @tparam T2 Second type
/// @tparam Ts Remaining types
template <typename T1, typename T2, typename... Ts>
class common_type_3_or_more<arene::base::constraints<common_type_t<T1, T2>>, T1, T2, Ts...>
    : public common_type<common_type_t<T1, T2>, Ts...> {};

}  // namespace common_type_detail

/// @brief Specialization for three or more types
///
/// This specialization handles the case of three or more types by recursively
/// finding the common type. It first finds the common type of the first two types,
/// then finds the common type of that result with the remaining types.
///
/// @tparam T1 First type
/// @tparam T2 Second type
/// @tparam Ts Remaining types
template <typename T1, typename T2, typename... Ts>
class common_type<T1, T2, Ts...>
    : public common_type_detail::common_type_3_or_more<arene::base::constraints<>, T1, T2, Ts...> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COMMON_TYPE_HPP_
