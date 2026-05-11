// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_ALIGNMENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_ALIGNMENT_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// AUTOSAR exceptions:
// A5-2-4 "reinterpret_cast shall not be used"
// The rationale for this rule indicates that use of reinterpret_cast may
// violate type-safety and cause the program to access a variable as if it
// were of another, unrelated type.
//
// However, the usage of reinterpret_cast in this file is only used to inspect
// the bit-representation of a pointer as an integer for the purposes of
// determining whether or not a pointer is aligned and if not, what offset
// must be applied to that pointer to obtain an aligned pointer.
// This restricted usage is thus not able to violate type-safety by
// interpreting an object of one type as if it were another type as
// the result of the reinterpret_cast is not used to access another object.

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/math/power_of_2.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-PRE31-c-3 "False Positive: static_assert is not a macro"

namespace arene {
namespace base {

namespace alignment_detail {
// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "volatile used to allow pointing to volatile objects"
/// @brief A pointer to anything at all
using pointer_to_anything = void const volatile*;
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

}  // namespace alignment_detail

/// @brief Check if an integer is a multiple of the specified power-of-two alignment.
///
/// @tparam Alignment The alignment to check against. Must be a positive power-of-two.
/// @tparam Integer An integral type.
/// @param value The value to check.
/// @return @c true if @c value is a multiple of @c alignment.
template <
    std::size_t Alignment,
    typename Integer,
    constraints<std::enable_if_t<std::is_integral<Integer>::value>> = nullptr>
constexpr auto is_aligned(Integer value) -> bool {
  static_assert(::arene::base::is_power_of_2(Alignment), "Alignment must be a positive power-of-two");
  static_assert(
      Alignment <= std::numeric_limits<Integer>::max(),
      "Attempting to align to a value that is larger than the "
      "maximum representable value."
  );
  using unsigned_integer = std::make_unsigned_t<Integer>;
  constexpr std::size_t mask{Alignment - 1U};
  return (static_cast<unsigned_integer>(value) & mask) == 0U;
}

/// @brief Check if an address is a multiple of the specified alignment.
///
/// @tparam Alignment The power-of-2 alignment to check for.
/// @param pointer The pointer to check the address of.
/// @return @c true if the pointer is aligned to a multiple of @c Alignment , otherwise @c false.
template <std::size_t Alignment>
// parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: 'pointer' does not hide anything"
// parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
constexpr auto is_aligned(alignment_detail::pointer_to_anything const pointer) -> bool {
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return is_aligned<Alignment>(reinterpret_cast<std::uintptr_t>(pointer));
}
// parasoft-end-suppress AUTOSAR-A2_10_1-a

/// @brief Align a value to the nearest multiple of alignment, rounding down.
///
/// @tparam Alignment The alignment to align to. Must be a positive power-of-two.
/// @tparam Integer The type of the integer to align.
/// @param value The value to align.
/// @return Either @c value if it was already a multiple of @c alignment , otherwise the closest multiple of @c
///         alignment less than @c value.
template <
    std::size_t Alignment,
    typename Integer,
    constraints<std::enable_if_t<std::is_integral<Integer>::value>> = nullptr>
constexpr auto align_floor(Integer value) -> Integer {
  static_assert(::arene::base::is_power_of_2(Alignment), "Alignment must be a positive power-of-two");
  static_assert(
      Alignment <= std::numeric_limits<Integer>::max(),
      "Attempting to align to a value that is larger than the "
      "maximum representable value."
  );
  using unsigned_integer = std::make_unsigned_t<Integer>;
  constexpr std::size_t mask{Alignment - 1U};
  return static_cast<Integer>(static_cast<unsigned_integer>(value) & ~mask);
}

/// @brief Align a value to the nearest multiple of alignment, rounding up.
///
/// @tparam Alignment The alignment to align to. Must be a positive power-of-two.
/// @tparam Integer The type of integer to align.
/// @param value The value to align.
/// @return Either @c value if it was already a multiple of @c alignment , otherwise the closest multiple of @c
///         alignment greather than @c value .
template <
    std::size_t Alignment,
    typename Integer,
    constraints<std::enable_if_t<std::is_integral<Integer>::value>> = nullptr>
constexpr auto align_ceil(Integer value) -> Integer {
  static_assert(::arene::base::is_power_of_2(Alignment), "Alignment must be a positive power-of-two");
  static_assert(
      Alignment <= std::numeric_limits<Integer>::max(),
      "Attempting to align to a value that is larger than the "
      "maximum representable value."
  );
  constexpr std::size_t mask{Alignment - 1U};
  using unsigned_integer = std::make_unsigned_t<Integer>;
  return static_cast<Integer>((static_cast<unsigned_integer>(value) + mask) & ~mask);
}

/// @brief Calculate the amount by which ptr needs to be adjusted to have the specified Alignment.
///
/// @tparam Alignment The alignment to calculate the offset to.
/// @param ptr The address to calculate the offset for.
/// @return The number of bytes that must be added to @c ptr address to obtain an address that is a multiple of @c
///         Alignment.
template <std::size_t Alignment>
// parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
auto alignment_offset(alignment_detail::pointer_to_anything const ptr) noexcept -> std::size_t {
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  std::uintptr_t const base{reinterpret_cast<std::uintptr_t>(ptr)};
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2
  std::size_t const offset{::arene::base::align_ceil<Alignment>(base) - base};
  return offset;
}

/// @brief Construct a bit-mask with the lowest @c one_bits bits set to 1 and the rest of the bits set to 0.
///
/// For example:
/// @snippet docs/examples/utility_examples.cpp bit_mask_usage_example
///
/// @tparam Integer The mask type to return.
/// @param one_bits The number of bits to set to 1 in the mask. Must be in range [0, Integer-bit-width].
/// @return A value of type @c Integer with the lowest @c one_bits bits set to 1 and the rest of the bits set to 0.
template <typename Integer, constraints<std::enable_if_t<std::is_unsigned<Integer>::value>> = nullptr>
constexpr auto bit_mask(std::uint32_t const one_bits) -> Integer {
  constexpr std::uint32_t max_bits{std::uint32_t{sizeof(Integer)} * std::uint32_t{CHAR_BIT}};
  ARENE_PRECONDITION(one_bits <= max_bits);

  // Note it is UB to shift by a number of bits greater or equal to the number of bits in the integer representation so
  // need to handle this case separately.
  if (one_bits != max_bits) {
    constexpr Integer one{1U};
    // parasoft-begin-suppress AUTOSAR-M5_8_1 "False Positive: this shifting operation is enclosed with if checking that
    // RHS does not exceed the upper limit"
    // parasoft-begin-suppress AUTOSAR-M4_5_1 "This function handles generic integer types, including bool"
    // parasoft-begin-suppress CERT_C-EXP46-b "This function handles generic integer types, including bool"
    return static_cast<Integer>((one << one_bits) - one);
    // parasoft-end-suppress CERT_C-EXP46-b
    // parasoft-end-suppress AUTOSAR-M4_5_1
    // parasoft-end-suppress AUTOSAR-M5_8_1
  }

  return std::numeric_limits<Integer>::max();
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_ALIGNMENT_HPP_
