#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_BITWISE_UNIFORM_DISTRIBUTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_BITWISE_UNIFORM_DISTRIBUTION_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief A uniform distribution to apply on top of a bit generator. Primary template (deliberately undefined).
/// @tparam T The value type to generate
template <typename T, typename = arene::base::constraints<>>
class bitwise_uniform_distribution;

/// @brief A uniform distribution to apply on top of a bit generator. SFINAE overload for integral types.
/// @tparam T The value type to generate
template <typename T>
class bitwise_uniform_distribution<T, arene::base::constraints<std::enable_if_t<std::is_integral<T>::value>>> {
  /// @brief Assert that the PRNG being used covers its type's whole range, since that's all our code currently handles
  /// @tparam BitGenerator A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  template <typename BitGenerator>
  static constexpr auto assert_generator_is_ok() noexcept -> void {
    using result_type = typename BitGenerator::result_type;
    static_assert(
        std::is_integral<result_type>::value && std::is_unsigned<result_type>::value,
        "BitGenerator::result_type must be unsigned integral for BitGenerator to satisfy UniformRandomBitGenerator"
    );
    static_assert(
        BitGenerator::min() == result_type{},
        "More sophisticated generation code is needed to handle cases where the real range is smaller than the type"
    );
    static_assert(
        BitGenerator::max() == static_cast<result_type>(~result_type{}),
        "More sophisticated generation code is needed to handle cases where the real range is smaller than the type"
    );
  }

  /// @brief Use a PRNG to generate a requested number of bits; overload for when the PRNG @c result_type is big enough
  /// @tparam ExpectedBitsType An unsigned integer type that's bit enough to be used for the subsequent type conversion
  /// @tparam BitGenerator A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return A uniformly-distributed random sequence of bits which can be converted into @c T
  template <
      typename ExpectedBitsType,
      typename BitGenerator,
      arene::base::constraints<
          std::enable_if_t<sizeof(typename BitGenerator::result_type) >= sizeof(ExpectedBitsType)>> = nullptr>
  static constexpr auto generate_bits(BitGenerator& prng) noexcept(noexcept(prng())) -> ExpectedBitsType {
    assert_generator_is_ok<BitGenerator>();

    return static_cast<ExpectedBitsType>(prng());
  }

  /// @brief Use a PRNG to generate a requested number of bits; overload for when the PRNG @c result_type is too small
  /// @tparam ExpectedBitsType An unsigned integer type that's bit enough to be used for the subsequent type conversion
  /// @tparam BitGenerator A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return A uniformly-distributed random sequence of bits which can be converted into @c T
  template <
      typename ExpectedBitsType,
      typename BitGenerator,
      arene::base::constraints<
          std::enable_if_t<sizeof(typename BitGenerator::result_type) < sizeof(ExpectedBitsType)>> = nullptr>
  static constexpr auto generate_bits(BitGenerator& prng) noexcept(noexcept(prng())) -> ExpectedBitsType {
    assert_generator_is_ok<BitGenerator>();

    using chunk_type = typename BitGenerator::result_type;
    static_assert(
        sizeof(ExpectedBitsType) % sizeof(chunk_type) == 0U,
        "Implementation assumes that an integer number of chunks fits in ExpectedBitsType"
    );

    ExpectedBitsType result_bits{};
    for (std::size_t chunk_idx{}; chunk_idx < (sizeof(ExpectedBitsType) / sizeof(chunk_type)); ++chunk_idx) {
      auto new_bits = static_cast<ExpectedBitsType>(prng());
      result_bits = static_cast<ExpectedBitsType>(
          result_bits | static_cast<ExpectedBitsType>(new_bits << (chunk_idx * sizeof(chunk_type) * CHAR_BIT))
      );
    }

    return result_bits;
  }

 public:
  /// @brief Generate a value of type <c>T</c>; overload for when T is integral
  /// @tparam BitGenerator A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return A random, uniformly-distributed @c T value
  template <
      typename BitGenerator,
      arene::base::constraints<std::enable_if_t<std::is_integral<typename BitGenerator::result_type>::value>> = nullptr>
  constexpr auto operator()(BitGenerator& prng) const noexcept(noexcept(prng())) -> T {
    return static_cast<T>(generate_bits<std::make_unsigned_t<T>>(prng));
  }
};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_BITWISE_UNIFORM_DISTRIBUTION_HPP_
