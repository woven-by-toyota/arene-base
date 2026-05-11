// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::construct_at"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MEMORY_CONSTRUCT_AT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MEMORY_CONSTRUCT_AT_HPP_

// IWYU pragma: private, include "arene/base/memory.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/type_manipulation/give_qualifiers_to.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "This header is required on CR builds"
#include "arene/base/stdlib_choice/new.hpp"  // IWYU pragma: keep
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

namespace construct_at_detail {
/// @brief Trait which deduces the type that would be returned by a placement new call
template <typename T, typename... Args>
using try_construct = decltype(::new (std::declval<void*>()) T(std::declval<Args>()...));

/// @brief Trait which determins if a given type can be placement-new instantiated with the given arguments.
template <typename T, typename... Args>
constexpr bool is_constructible = substitution_succeeds<try_construct, T, Args...>;

/// @brief Implementation helper for @c arene::base::construct_at
class do_construct_at {
 public:
  // parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR50-g-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR51-b-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_5_3-g-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_3_2-a-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-M15_3_4-b-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_2_1-b-2 "False positive: Not invoked during initialization"
  /// @brief Do the construction
  /// @tparam T The type to construct
  /// @tparam Args The types of the arguments
  /// @param ptr The pointer to the storage
  /// @param args The arguments to pass
  /// @return A pointer to the newly constructed object
  /// @see arene::base::construct_at
  /// @throws Any exception thrown by the constructor of @c T
  template <class T, class... Args, constraints<std::enable_if_t<is_constructible<T, Args...>>> = nullptr>
  auto operator()(T* ptr, Args&&... args) const noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T* {
    // parasoft-begin-suppress AUTOSAR-A5_2_3-a-2 "False positive: This is creating a new object, it only removes
    // qualifiers from the storage, which are used to determine the qualifiers on the new object"
    // parasoft-begin-suppress CERT_CPP-EXP55-a-2 "False positive: This is creating a new object, it only removes
    // qualifiers from the storage, which are used to determine the qualifiers on the new object"
    // Justification: False positive about newly created resource
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-type-const-cast)
    return ::new (const_cast<void*>(static_cast<give_qualifiers_to<T, void>*>(ptr))) T(std::forward<Args>(args)...);
    // parasoft-end-suppress AUTOSAR-A5_2_3-a-2
    // parasoft-end-suppress CERT_CPP-EXP55-a-2
  }
  // parasoft-end-suppress CERT_CPP-ERR55-a-2
  // parasoft-end-suppress CERT_CPP-ERR50-g-3
  // parasoft-end-suppress CERT_CPP-ERR50-h-3
  // parasoft-end-suppress CERT_CPP-ERR51-b-3
  // parasoft-end-suppress AUTOSAR-A15_5_3-h-2
  // parasoft-end-suppress AUTOSAR-A15_5_3-g-2
  // parasoft-end-suppress AUTOSAR-A15_3_2-a-2
  // parasoft-end-suppress AUTOSAR-M15_3_4-b-2
  // parasoft-end-suppress AUTOSAR-A15_2_1-b-2
};

}  // namespace construct_at_detail

/// @def arene::base::construct_at
/// @brief @c std::construct_at back ported from C++20
/// @tparam T the type to construct
/// @tparam Args type of arguments used for initialization
/// @param ptr pointer to the uninitialized storage on which a T object will be constructed
/// @param args arguments used for initialization
/// @return a pointer to the initialized object
/// @pre Only participates in overload resolution if <c>new (std::declval<void*>()) T(std::declval<Args>()...)</c>
///      is well formed in an unevaluated context.
/// @post The object located at the returned pointer will have been initalized as if constructed via
///       @c T(std::forward<Args>(args)...)
/// @note Unlike @c std::construct_at, constexpr evaluation is not supported as dynamic memory allocation (even using
///       placement new) is not supported until C++20.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(construct_at_detail::do_construct_at, construct_at);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MEMORY_CONSTRUCT_AT_HPP_
