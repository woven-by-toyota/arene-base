#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_BASE_KIND_SET_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_BASE_KIND_SET_HPP_

#include "arene/base/algorithm/transform.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/index_of.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/units/is_quantity_kind.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A3_1_5-a "False positive: all functions are constexpr so are intended to be inlined"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace units_detail {

/// @brief specifies the exponents of a quantity kind in the base kind set
class base_kind_exponents {
  /// @brief positive exponent value
  exponent_t positive_;
  /// @brief negative exponent value
  exponent_t negative_;

 public:
  /// @brief constuct with the provided exponents
  /// @param pos positive exponent value
  /// @param neg negative exponent value
  /// @pre @c pos is non-negative
  /// @pre @c neg is non-negative
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr base_kind_exponents(exponent_t pos, exponent_t neg) noexcept
      : positive_{pos},
        negative_{neg} {
    ARENE_PRECONDITION(pos >= 0);
    ARENE_PRECONDITION(neg >= 0);
  }

  /// @brief default constuct with zero for both positive and negative exponents
  constexpr base_kind_exponents() noexcept
      : base_kind_exponents{0, 0} {}

  /// @brief obtain the positive exponent value
  /// @return positive exponent value
  constexpr auto positive() const noexcept -> exponent_t { return positive_; }

  /// @brief obtain the negative exponent value
  /// @return negative exponent value
  constexpr auto negative() const noexcept -> exponent_t { return negative_; }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  /// @brief scale exponents by a scaling factor
  /// @param scaling factor to scale exponents by
  /// @param self positive and negative exponents
  /// @return exponents after applying @c scaling
  /// @note This function does not guard against overflow/underflow as it is
  ///   should only be invoked in a constant expression.
  friend constexpr auto operator*(exponent_t const scaling, base_kind_exponents const self) noexcept
      -> base_kind_exponents {
    if (scaling < 0) {
      return {-scaling * self.negative(), -scaling * self.positive()};
    }

    return {scaling * self.positive(), scaling * self.negative()};
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  /// @brief combine exponents
  /// @param other exponents to add to @c *this
  /// @return @c *this
  /// @note This function does not guard against overflow/underflow as it is
  ///   should only be invoked in a constant expression.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  constexpr auto operator+=(base_kind_exponents const other) noexcept -> base_kind_exponents& {
    positive_ += other.positive();
    negative_ += other.negative();
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a
};

/// @brief specifies a set of base quantity kinds with associated exponents
/// @tparam Kinds base quantity kind types
template <typename... Kinds>
class base_kind_set {
 public:
  static_assert(all_of_v<is_quantity_kind_v<Kinds>...>, "every type in 'Kinds' must be a quantity kind");

  /// @brief exponents associated with each quantity kind base
  ///
  /// Exponent values are associated the quantity kind with the same index.
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "'base_kind_set' is an implementation detail with no class invariants"
  // parasoft-begin-suppress AUTOSAR-M8_5_2-a "False positive: correct initialization"
  // parasoft-begin-suppress AUTOSAR-M8_5_2-b "False positive: correct initialization"
  arene::base::array<base_kind_exponents, sizeof...(Kinds)> exponents{};
  // parasoft-end-suppress AUTOSAR-M8_5_2-a
  // parasoft-end-suppress AUTOSAR-M8_5_2-b
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: This is a
  // public function, and doesn't need to be used in the translation unit"
  /// @brief scale exponents by a scaling factor
  /// @param scaling factor to scale exponents by
  /// @param self set of base quantity kinds with associated exponents
  /// @return base quantity kinds after scaling exponents values
  friend constexpr auto operator*(exponent_t const scaling, base_kind_set const& self) noexcept -> base_kind_set {
    auto updated = base_kind_set{};
    arene::base::transform(  //
        self.exponents.begin(),
        self.exponents.end(),
        updated.exponents.begin(),
        bind_front(std::multiplies<>{}, scaling)
    );
    return updated;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  /// @brief obtain the exponent values for the specified quantity kind
  /// @tparam Kind quantity kind of which to obtain the exponent values
  /// @return reference to the exponents
  /// @note error if @c Kind is not in @c Kinds...
  template <typename Kind>
  constexpr auto exponent_for() noexcept -> base_kind_exponents& {
    return get<type_lists::index_of_v<base_kind_set, Kind>>(exponents);
  }
  /// @brief obtain the exponent values for the specified quantity kind
  /// @tparam Kind quantity kind of which to obtain the exponent values
  /// @return reference to the exponents
  /// @note error if @c Kind is not in @c Kinds...
  template <typename Kind>
  constexpr auto exponent_for() const noexcept -> base_kind_exponents const& {
    return get<type_lists::index_of_v<base_kind_set, Kind>>(exponents);
  }

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
  /// @brief update exponents values from another base kind set
  /// @tparam OtherKinds base quantity kind types of the other base kind set
  /// @param other base kind set to combine with @c *this
  /// @note error if any type in @c OtherKinds is not in @c Kinds..., i.e. @c
  ///   other must be a subset of @c *this
  /// @return @c *this
  ///
  /// Updates @c *this with the exponent values from @c other.
  template <typename... OtherKinds>
  constexpr auto add_exponents_from(base_kind_set<OtherKinds...> const& other) noexcept -> base_kind_set& {
    // parasoft-begin-suppress AUTOSAR-M5_18_1-a "Comma operator avoids an expensive recursive iteration"
    consume_values({(exponent_for<OtherKinds>() += other.template exponent_for<OtherKinds>(), true)...});
    // parasoft-end-suppress AUTOSAR-M5_18_1-a
    return *this;
  }
};
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief combine base kind sets
/// @tparam Bases base kind set types
/// @param bases base kind sets to combine
/// @return an base kind set which is the combination of all input sets
template <typename... Bases>
constexpr auto set_union(Bases const&... bases) noexcept -> type_lists::concat_unique_t<Bases...> {
  static_assert(
      all_of_v<is_instantiation_of_v<Bases, base_kind_set>...>,
      "every type in 'Bases' must be a base kind set"
  );

  auto combined = type_lists::concat_unique_t<Bases...>{};

  // parasoft-begin-suppress AUTOSAR-M5_18_1-a "Comma operator avoids an expensive recursive iteration"
  consume_values({(combined.add_exponents_from(bases), true)...});
  // parasoft-end-suppress AUTOSAR-M5_18_1-a

  return combined;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief converts a base kind set constant into a list of quantity kind types with exponents
/// @tparam BaseKindSetConstant type constant that provides a base kind set as a
///   @c static @c constexpr member @c value
template <typename BaseKindSetConstant>
class as_list_of_kinds_with_exponents {
  // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(
      is_instantiation_of_v<std::remove_const_t<decltype(BaseKindSetConstant::value)>, base_kind_set>,
      "'BaseKindSetConstant::value' must be a 'base_kind_set'"
  );
  // parasoft-end-suppress CERT_C-PRE31-c

  /// @brief lifts a base kind set into a type list
  /// @tparam BaseKinds quantity base kind types
  /// @return base kind set lifted into a type list
  template <typename... BaseKinds>
  static auto impl(base_kind_set<BaseKinds...>) -> type_list<quantity_kind_with_exponent<
      BaseKinds,
      BaseKindSetConstant::value.template exponent_for<BaseKinds>().positive(),
      BaseKindSetConstant::value.template exponent_for<BaseKinds>().negative()>...>;

 public:
  /// @brief base kind set type lifted into a type list
  using type = decltype(impl(BaseKindSetConstant::value));
};

/// @brief converts a base kind set constant into a list of quantity kind types with exponents
/// @tparam BaseKindSetConstant type constant that provides a base kind set as a
///   @c static @c constexpr member @c value
///
/// Lifts the base kind set value specified by @c BaseKindSetConstant::value into a type.
template <typename BaseKindSetConstant>
using as_list_of_kinds_with_exponents_t = typename as_list_of_kinds_with_exponents<BaseKindSetConstant>::type;

}  // namespace units_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_BASE_KIND_SET_HPP_
