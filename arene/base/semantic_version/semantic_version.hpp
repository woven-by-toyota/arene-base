// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SEMANTIC_VERSION_SEMANTIC_VERSION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SEMANTIC_VERSION_SEMANTIC_VERSION_HPP_

// IWYU pragma: private, include "arene/base/semantic_version.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/utility/to_underlying.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_0_1-e "Undefines macros for configuration"
#undef major
#undef minor
// parasoft-end-suppress AUTOSAR-A16_0_1-e

namespace arene {
namespace base {

/// @brief A class representing semantic versions (https://semver.org/). Does not include build metadata or prerelease
/// comparisons.
///
/// @remarks Pre-releases and Build metadata are effectively (ASCII) strings, and therefore do not fit into compile-time
/// computation. Should there be significant enough demand, an @c extended_semantic_version with prerelease and build
/// metadata can be considered, but this class does not retain or track such information.
class semantic_version : generic_ordering_from_three_way_compare<semantic_version> {
 public:
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Default ctor, creates a semver of all 0's.
  /// @post <c> major() == 0 </c>
  /// @post <c> minor() == 0 </c>
  /// @post <c> patch() == 0 </c>
  constexpr semantic_version() noexcept
      : semantic_version(0U, 0U, 0U) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Constructs a semantic_version with the given major, minor, and patch version.
  ///
  /// @param major_version The major version number.
  /// @param minor_version The minor version number.
  /// @param patch_version The patch version number.
  /// @post <c> major() == major_version </c>
  /// @post <c> minor() == minor_version </c>
  /// @post <c> patch() == patch_version </c>
  // NOLINTBEGIN(bugprone-easily-swappable-parameters)
  constexpr explicit semantic_version(
      std::uint16_t const major_version,
      std::uint16_t const minor_version = 0U,
      std::uint32_t const patch_version = 0U
  ) noexcept
      : generic_ordering_from_three_way_compare<semantic_version>{},
        major_(major_version),
        minor_(minor_version),
        patch_(patch_version) {}
  // NOLINTEND(bugprone-easily-swappable-parameters)

  /// @brief Tests whether this semantic version represents an "unstable" entity, which is one with a major of 0.
  /// @return true iff <c> major() == 0 </c>
  /// @return false otherwise
  ARENE_NODISCARD constexpr auto is_unstable() const noexcept -> bool { return this->major_ < 1U; }

  /// @brief Get the major version component.
  /// @return major version number
  ///
  ARENE_NODISCARD constexpr auto major() const noexcept -> std::uint16_t { return this->major_; }

  /// @brief Get the minor version component.
  /// @return minor version number
  ///
  ARENE_NODISCARD constexpr auto minor() const noexcept -> std::uint16_t { return this->minor_; }

  /// @brief Get the patch version component.
  /// @return patch version number
  ///
  ARENE_NODISCARD constexpr auto patch() const noexcept -> std::uint32_t { return this->patch_; }

  /// @brief Implements a strong lexicographic sort order based on <c>[major, minor, path]</c>, in that order.
  ///
  /// @param left The left hand side of the comparison.
  /// @param right The right hand side of the comparison.
  /// @return strong_ordering::less if lhs lexicographically precedes rhs.
  /// @return strong_ordering::equal if lhs and rhs are equal across major, minor, and patch.
  /// @return strong_ordering::less if lhs lexicographically follows rhs.
  // NOLINTBEGIN(readability-avoid-const-params-in-decls) : AUTOSAR M3-9-1 requires this to match
  ARENE_NODISCARD
  static constexpr auto three_way_compare(semantic_version const left, semantic_version const right) noexcept
      -> arene::base::strong_ordering;
  // NOLINTEND(readability-avoid-const-params-in-decls)

 private:
  /// @brief The major change version
  std::uint16_t major_;
  /// @brief The minor change version
  std::uint16_t minor_;
  /// @brief The patch change version
  std::uint32_t patch_;
};

/// @brief Implements a strong lexicographic sort order based on <c>[major, minor, path]</c>, in that order.
///
/// @param left The left hand side of the comparison.
/// @param right The right hand side of the comparison.
/// @return strong_ordering::less if lhs lexicographically precedes rhs.
/// @return strong_ordering::equal if lhs and rhs are equal across major, minor, and patch.
/// @return strong_ordering::less if lhs lexicographically follows rhs.
ARENE_NODISCARD
constexpr auto semantic_version::three_way_compare(semantic_version const left, semantic_version const right) noexcept
    -> arene::base::strong_ordering {
  constexpr auto cmp = compare_three_way{};
  auto order = cmp(left.major_, right.major_);
  if (order == arene::base::strong_ordering::equal) {
    order = cmp(left.minor_, right.minor_);
  }
  if (order == arene::base::strong_ordering::equal) {
    order = cmp(left.patch_, right.patch_);
  }
  return order;
}

/// @brief Compares ONLY the major part of two semantic versions, to see if
/// there's a major change between the two.
///
/// @param left The first version value.
/// @param right The second version value.
/// @return @c true if there is a major change, otherwise @c false
///
ARENE_NODISCARD
inline constexpr auto contains_major_change(semantic_version const left, semantic_version const right) noexcept
    -> bool {
  return left.major() != right.major();
}

/// @brief Compares ONLY the minor part of two semantic versions, to see if
/// there's a minor change between the two.
///
/// @param left The first version value.
/// @param right The second version value.
/// @return @c true if there is a minor change, otherwise @c false
///
ARENE_NODISCARD
inline constexpr auto contains_minor_change(semantic_version const left, semantic_version const right) noexcept
    -> bool {
  return left.minor() != right.minor();
}

/// @brief Compares ONLY the patch part of two semantic versions, to see if
/// there's a patch change between the two.
///
/// @param left The first version value.
/// @param right The second version value.
/// @return @c true if there is a patch change, otherwise @c false
///
ARENE_NODISCARD
inline constexpr auto contains_patch_change(semantic_version const left, semantic_version const right) noexcept
    -> bool {
  return left.patch() != right.patch();
}

/// @brief The set of differences between two @c semantic_version s, encoded as a bitfield of @c [patch.minor.major].
/// @see compute_difference to create a @c sem_ver_diff from two @c semantic_version instances.
enum class sem_ver_diff : std::uint8_t {
  /// @brief No changes. 0b000
  none = 0b000,
  /// @brief A major change. 0b001
  major = 0b001,
  /// @brief A minor change. 0b010
  minor = 0b010,
  /// @brief A patch change. 0b100
  patch = 0b100,

  /// @brief A change to both the major and minor versions.
  major_minor = 0b011,
  /// @brief A change to both the major and patch versions.
  major_patch = 0b101,
  /// @brief A change to both the minor and patch versions.
  minor_patch = 0b110,
  /// @brief A change to major, minor, and patch versions.
  major_minor_patch = 0b111
};

/// @brief Computes the set of the potential changes between two different semantic versions.
///
/// @param left The first version value.
/// @param right The second version value.
/// @return A sem_ver_diff with the following properties:
///          * The major bit will be set if <c>contains_major_change(left, right) == true</c>
///          * The minor bit will be set if <c>contains_minor_change(left, right) == true</c>
///          * The patch bit will be set if <c>contains_patch_change(left, right) == true</c>
ARENE_NODISCARD inline constexpr auto
compute_difference(semantic_version const left, semantic_version const right) noexcept -> sem_ver_diff {
  std::uint8_t diff{to_underlying(sem_ver_diff::none)};
  if (contains_major_change(left, right)) {
    diff = static_cast<std::uint8_t>(diff | to_underlying(sem_ver_diff::major));
  }
  if (contains_minor_change(left, right)) {
    diff = static_cast<std::uint8_t>(diff | to_underlying(sem_ver_diff::minor));
  }
  if (contains_patch_change(left, right)) {
    diff = static_cast<std::uint8_t>(diff | to_underlying(sem_ver_diff::patch));
  }
  // parasoft-begin-suppress AUTOSAR-A7_2_1-a "False positive: values correspond to enumerators in the enum"
  // parasoft-begin-suppress CERT_CPP-INT50-a "False positive: values correspond to enumerators in the enum"
  return static_cast<sem_ver_diff>(diff);
  // parasoft-end-suppress CERT_CPP-INT50-a
  // parasoft-end-suppress AUTOSAR-A7_2_1-a
}

///
/// @brief Predicate to test if a sem_ver_diff contains a major change
///
/// @param diff The computed difference between two versions
/// @return true if the major bit is set.
/// @return false otherwise.
///
ARENE_NODISCARD inline constexpr auto contains_major_change(sem_ver_diff const diff) noexcept -> bool {
  return (to_underlying(diff) & to_underlying(sem_ver_diff::major)) == to_underlying(sem_ver_diff::major);
}

///
/// @brief Predicate to test if a sem_ver_diff contains a minor change
///
/// @param diff The computed difference between two versions
/// @return true if the minor bit is set.
/// @return false otherwise.
///
ARENE_NODISCARD inline constexpr auto contains_minor_change(sem_ver_diff const diff) noexcept -> bool {
  return (to_underlying(diff) & to_underlying(sem_ver_diff::minor)) == to_underlying(sem_ver_diff::minor);
}

///
/// @brief Predicate to test if a sem_ver_diff contains a patch change
///
/// @param diff The computed difference between two versions
/// @return true if the patch bit is set.
/// @return false otherwise.
///
ARENE_NODISCARD inline constexpr auto contains_patch_change(sem_ver_diff const diff) noexcept -> bool {
  return (to_underlying(diff) & to_underlying(sem_ver_diff::patch)) == to_underlying(sem_ver_diff::patch);
}

///
/// @brief Predicate to test if a sem_ver_diff represents a change of at least one major version.
///
/// @param diff The computed difference between two versions.
/// @return true if the major bit is set.
/// @return false otherwise.
///
ARENE_NODISCARD inline constexpr auto is_major_change(sem_ver_diff const diff) noexcept -> bool {
  return contains_major_change(diff);
}

///
/// @brief Binary predicate to test if two semantic_versions differ by major versions.
///
/// @param left The left hand operand.
/// @param right The right hand operand.
/// @return true if @c major() is different between @c left and @c right .
/// @return false otherwise.
///
ARENE_NODISCARD
inline constexpr auto is_major_change(semantic_version const left, semantic_version const right) noexcept -> bool {
  return is_major_change(compute_difference(left, right));
}

///
/// @brief Predicate to test if a sem_ver_diff represents a change of minor versions, ignoring patch versions.
///
/// @param diff The computed difference between two versions.
/// @return true if the minor bit is set, and the major bit is not.
/// @return false if the major bit is set, or the minor bit is not.
///
ARENE_NODISCARD inline constexpr auto is_minor_change(sem_ver_diff const diff) noexcept -> bool {
  return (!is_major_change(diff)) && contains_minor_change(diff);
}

///
/// @brief Binary predicate to test if two semantic_versions differ by minor versions, ignoring patch versions.
///
/// @param left The left hand operand.
/// @param right The right hand operand.
/// @return true if @c minor() is different between @c left and @c right, but @c major() is equivalent.
/// @return false if either @c major() varies between the operands, or @c minor() is equal.
///
ARENE_NODISCARD
inline constexpr auto is_minor_change(semantic_version const left, semantic_version const right) noexcept -> bool {
  return is_minor_change(compute_difference(left, right));
}

///
/// @brief Predicate to test if a sem_ver_diff represents a change of exclusively patch versions.
///
/// @param diff The computed difference between two versions.
/// @return true if exclusively the patch bit is set.
/// @return false if either the major or minor bits are set.
///
ARENE_NODISCARD inline constexpr auto is_patch_change(sem_ver_diff const diff) noexcept -> bool {
  return diff == sem_ver_diff::patch;
}

///
/// @brief Binary predicate to test if two semantic_versions differ by exclusively patch versions.
///
/// @param left The left hand operand.
/// @param right The right hand operand.
/// @return true if the difference between @c left and @c right is exclusively in @c patch() .
/// @return false if either @c major() or @c minor() vary between the operands, or @c patch() is equal.
///
ARENE_NODISCARD
inline constexpr auto is_patch_change(semantic_version const left, semantic_version const right) noexcept -> bool {
  return is_patch_change(compute_difference(left, right));
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SEMANTIC_VERSION_SEMANTIC_VERSION_HPP_
