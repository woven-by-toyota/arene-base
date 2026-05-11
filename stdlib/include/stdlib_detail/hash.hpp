// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_HASH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_HASH_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints.hpp"
#include "arene/base/span.hpp"
#include "arene/base/utility.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_enum.hpp"
#include "stdlib/include/stdlib_detail/is_fundamental.hpp"
#include "stdlib/include/stdlib_detail/is_pointer.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// The iwyu-private pragma below will direct users to include '<functional>' for use
// of 'std::hash'
//
// Components that provide a specializtion of 'std::hash', should also provide
// an iwyu-export pragma so that an include of <functional> is not needlessly
// recommended.

// IWYU pragma: private, include <functional>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

namespace std {

namespace hash_detail {

/// @brief The FNV prime to use depending on bit variant.
/// @tparam Scalar the scalar type
/// @tparam Dummy a dummy type to allow partial specialization of variable template
///
/// The value of this constant depends on the bit length of the hash being used, determined by the type of @c Scalar.
/// The value of this constant for common bit-lengths is provided by the paper "The FNV Non-Cryptographic Hash
/// Algorithm".
template <class Scalar, class Dummy = nullptr_t>
extern constexpr Scalar fnv_prime_v{0};

/// @brief The FNV 64-bit prime
/// @tparam Dummy a dummy type to allow partial specialization of variable template
template <class Dummy>
extern constexpr uint64_t fnv_prime_v<uint64_t, Dummy>{1099511628211ULL};

/// @brief The FNV 32-bit prime
/// @tparam Dummy a dummy type to allow partial specialization of variable template
template <class Dummy>
extern constexpr uint32_t fnv_prime_v<uint32_t, Dummy>{16777619UL};

/// @brief The FNV offset basis to use depending on bit variant.
/// @tparam Scalar the scalar type
/// @tparam Dummy a dummy type to allow partial specialization of variable template
///
/// The value of this constant depends on the bit length of the hash being used, determined by the type of @c Scalar.
/// The value of this constant for common bit-lengths is provided by the paper "The FNV Non-Cryptographic Hash
/// Algorithm".
template <class Scalar, class Dummy = nullptr_t>
extern constexpr Scalar fnv_offset_basis_v{0};

/// @brief The FNV 64-bit offset basis
/// @tparam Dummy a dummy type to allow partial specialization of variable template
template <class Dummy>
constexpr uint64_t fnv_offset_basis_v<uint64_t, Dummy>{14695981039346656037ULL};

/// @brief The FNV 32-bit offset basis
/// @tparam Dummy a dummy type to allow partial specialization of variable template
template <class Dummy>
constexpr uint32_t fnv_offset_basis_v<uint32_t, Dummy>{2166136261UL};

/// @brief FNV-1a hash algorithm
/// @tparam T the type to hash
///
/// Implement the Fowler-Noll-Vo hashing algorithm with the 1a variant. This hashing algorithm uses two constants that
/// depend on the bit-length of @c std::size_t for the current platform.
///
/// The algorithm starts with an initial hash value of FNV offset basis. For each byte in the input, XOR the hash with
/// the byte from the input, then multiply it by the FNV prime:
/// @code
///    hash := FNV_offset_basis
///    for each byte_of_data to be hashed do
///        hash := hash XOR byte_of_data
///        hash := hash * FNV_prime
///    return hash
/// @endcode
template <typename T>
class fnv1a_hash {
 public:
  /// @brief The type of the result of the function call operator
  using result_type = size_t;
  /// @brief The type of the argument of the function call operator
  using argument_type = T;

  /// @brief Calculate the hash of a value
  /// @param value The value to hash
  /// @return The hash of the value
  auto operator()(T const& value) const noexcept -> size_t {
    using scalar_type = conditional_t<sizeof(size_t) == sizeof(uint32_t), uint32_t, uint64_t>;

    scalar_type hash{fnv_offset_basis_v<scalar_type>};

    for (auto const elem : arene::base::as_bytes(arene::base::span<T const, 1U>(&value, 1U))) {
      hash ^= static_cast<scalar_type>(arene::base::to_underlying(elem));
      hash *= fnv_prime_v<scalar_type>;
    }
    return hash;
  }
};

/// @brief Default implementation of @c std::hash implemented only for fundamental types, enumerations and pointers
/// @tparam T type to hash
template <class T, typename = arene::base::constraints<>>
class default_hash;

/// @brief Default implementation of @c std::hash
///
/// Specialization for fundamental types, enumerations and pointers
/// @tparam T type to hash
template <class T>
class default_hash<T, arene::base::constraints<enable_if_t<is_fundamental_v<T> || is_enum_v<T> || is_pointer_v<T>>>>
    : public fnv1a_hash<T> {};

}  // namespace hash_detail

// parasoft-begin-suppress AUTOSAR-A11_0_2-a "Specified as struct in C++ Standard"
/// @brief hash function primary template
/// @tparam T type to hash
///
template <class T>
struct hash : hash_detail::default_hash<T> {};
// parasoft-end-suppress AUTOSAR-A11_0_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_HASH_HPP_
