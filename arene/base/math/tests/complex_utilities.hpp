#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_TESTS_COMPLEX_UTILITIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_TESTS_COMPLEX_UTILITIES_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/move.hpp"

namespace complex_testing {

enum class real_imag_tag : std::uint8_t {
  none = 0b0U,
  real = 0b1U,
  imag = 0b10U,
  both = 0b11U,
};

constexpr auto has_real(real_imag_tag tag) noexcept -> bool {
  return (static_cast<std::uint8_t>(tag) & static_cast<std::uint8_t>(real_imag_tag::real)) != 0;
}

constexpr auto has_imag(real_imag_tag tag) noexcept -> bool {
  return (static_cast<std::uint8_t>(tag) & static_cast<std::uint8_t>(real_imag_tag::imag)) != 0;
}

template <class T, real_imag_tag MemberTag, real_imag_tag FriendTag, bool Noexcept = true>
class configurable_complex {
  T member_real_;
  T member_imag_;
  T adl_real_;
  T adl_imag_;

 public:
  // Construct an object where both member and ADL real/imag return the same value
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters) It's conventional that real is first and complex is second
  constexpr configurable_complex(T real, T imag)
      : member_real_(std::move(real)),
        member_imag_(std::move(imag)),
        adl_real_(std::move(member_real_)),
        adl_imag_(std::move(member_imag_)) {}

  // Construct an object where member and ADL real/imag return different values so they can be distinguished
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters) This is just test code, all uses are in this file
  constexpr configurable_complex(T member_real, T member_imag, T adl_real, T adl_imag)
      : member_real_(std::move(member_real)),
        member_imag_(std::move(member_imag)),
        adl_real_(std::move(adl_real)),
        adl_imag_(std::move(adl_imag)) {}

  // Member accessors are enabled if MemberTag has the corresponding bit
  template <bool Enable = has_real(MemberTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  constexpr auto real() const noexcept(Noexcept) -> T const& {
    return member_real_;
  }

  template <bool Enable = has_real(MemberTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  constexpr auto real() noexcept(Noexcept) -> T& {
    return member_real_;
  }

  template <bool Enable = has_imag(MemberTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  constexpr auto imag() const noexcept(Noexcept) -> T const& {
    return member_imag_;
  }

  template <bool Enable = has_imag(MemberTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  constexpr auto imag() noexcept(Noexcept) -> T& {
    return member_imag_;
  }

  // Friend (ADL) accessors are enabled if FriendTag has the corresponding bit
  template <bool Enable = has_real(FriendTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  friend constexpr auto real(configurable_complex const& conf) noexcept(Noexcept) -> T const& {
    return conf.adl_real_;
  }

  template <bool Enable = has_real(FriendTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  friend constexpr auto real(configurable_complex& conf) noexcept(Noexcept) -> T& {
    return conf.adl_real_;
  }

  template <bool Enable = has_imag(FriendTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  friend constexpr auto imag(configurable_complex const& conf) noexcept(Noexcept) -> T const& {
    return conf.adl_imag_;
  }

  template <bool Enable = has_imag(FriendTag), arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  friend constexpr auto imag(configurable_complex& conf) noexcept(Noexcept) -> T& {
    return conf.adl_imag_;
  }
};

}  // namespace complex_testing

// zero_v of a configurable_complex has zero_v for all of the components
/// @cond
template <class T, complex_testing::real_imag_tag MemberTag, complex_testing::real_imag_tag FriendTag, bool Noexcept>
constexpr complex_testing::configurable_complex<T, MemberTag, FriendTag, Noexcept>
    arene::base::numbers::zero_v<complex_testing::configurable_complex<
        T,
        MemberTag,
        FriendTag,
        Noexcept>>{arene::base::numbers::zero_v<T>, arene::base::numbers::zero_v<T>};
/// @endcond

namespace complex_testing {

template <class T, bool Noexcept = true>
using complex_with_members = configurable_complex<T, real_imag_tag::both, real_imag_tag::none, Noexcept>;

template <class T, bool Noexcept = true>
using complex_with_adl = configurable_complex<T, real_imag_tag::none, real_imag_tag::both, Noexcept>;

}  // namespace complex_testing

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_TESTS_COMPLEX_UTILITIES_HPP_
