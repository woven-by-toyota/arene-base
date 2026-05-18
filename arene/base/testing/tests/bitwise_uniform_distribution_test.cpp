// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/bitwise_uniform_distribution.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/array/array.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/prng_xoshiro.hpp"
#include "arene/base/type_list/cartesian_product.hpp"

namespace {

using arene::base::testing::bitwise_uniform_distribution;
using arene::base::testing::prng_xoshiro;

template <typename T>
class BitwiseUniformDistributionTest : public ::testing::Test {};

template <typename ValueType, typename Prng>
struct test_case {
  using value_type = ValueType;
  using prng = Prng;
};

using integer_types = ::testing::Types<
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t,
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t>;

// A testing generator with a small result type
class prng_little : private prng_xoshiro {
 public:
  /// @brief seed argument type
  using seed_type = std::uint64_t;

  /// @brief prng result type
  using result_type = std::uint8_t;

  /// @brief construct the generator with a seed of zero
  constexpr prng_little() noexcept
      : prng_xoshiro{} {}

  /// @brief generate the next pseudorandom value and advance the internal state
  constexpr auto operator()() -> result_type { return static_cast<result_type>(prng_xoshiro::operator()()); }

  /// @brief smallest value that @c operator() can return
  /// @return zero
  static constexpr auto min() noexcept -> result_type { return {}; }

  /// @brief largest value that @c operator() can return
  /// @return ~min()
  static constexpr auto max() noexcept -> result_type { return static_cast<result_type>(~min()); }
};

using bit_gen_types = ::testing::Types<prng_xoshiro, prng_little>;

using test_case_types = arene::base::type_lists::cartesian_product_as_t<test_case, integer_types, bit_gen_types>;

TYPED_TEST_SUITE(BitwiseUniformDistributionTest, test_case_types, );

/// @test When given uniformly-distributed bits, the distribution of typed values covers all possible bits
CONSTEXPR_TYPED_TEST(BitwiseUniformDistributionTest, WholeRangeCovered) {
  using value_type = typename TypeParam::value_type;
  using unsigned_value_type = std::make_unsigned_t<value_type>;
  using prng_type = typename TypeParam::prng;

  auto prng = prng_type{};
  auto const dist = bitwise_uniform_distribution<value_type>{};

  unsigned_value_type covered_bits{};

  // The probability of a given bit to be covered after N iterations is 1 - (1/2)^N
  // The probability P of *all* B bits to be covered after N iterations is (1 - (1/2)^N) ^ B
  // (1/2)^N = 1 - P^(1/B) so 2^N = 1 / (1 - P^(1/B)) or N = -log2(1 - P^(1/B))
  // Even for 64 bits, P > 0.99 gives N below 10. N=20 gives P of over 0.9999 for B=64.
  constexpr std::size_t number_of_iterations{20};
  for (std::size_t i{}; i < number_of_iterations; ++i) {
    covered_bits = static_cast<unsigned_value_type>(covered_bits | static_cast<unsigned_value_type>(dist(prng)));
  }

  CONSTEXPR_ASSERT_EQ(covered_bits, static_cast<unsigned_value_type>(~unsigned_value_type{}));
}

/// @test The noexcept-specification of the @c generator class's call operator matches that of the underlying PRNG's
TYPED_TEST(BitwiseUniformDistributionTest, NoexceptSpecMatchesBitGenerator) {
  using value_type = typename TypeParam::value_type;
  using prng_type = typename TypeParam::prng;

  STATIC_ASSERT_TRUE(noexcept(bitwise_uniform_distribution<value_type>{}));
  STATIC_ASSERT_EQ(
      noexcept(std::declval<bitwise_uniform_distribution<value_type> const&>()(std::declval<prng_type&>())),
      noexcept(std::declval<prng_type&>()())
  );
}

// A testing generator that emits a pre-primed sequence of values, cycling back to the start once exhausted.
template <typename T>
class sequenced_mock_prng {
  arene::base::inline_vector<T, 256> values_{};
  std::size_t index_{};

 public:
  using seed_type = T;
  using result_type = T;

  /// @brief Construct from a list of values that will be returned in order
  constexpr sequenced_mock_prng(std::initializer_list<T> values) noexcept
      : values_{values} {}

  /// @brief Return the next value in the buffer and advance the internal index, wrapping at the buffer boundary
  constexpr auto operator()() noexcept -> result_type {
    auto const value = values_[index_ % values_.size()];
    ++index_;
    return value;
  }

  /// @brief Return the next value in the buffer without advancing the state.
  constexpr auto peek() noexcept -> result_type {
    auto const value = values_[index_ % values_.size()];
    return value;
  }

  static constexpr auto min() noexcept -> result_type { return {}; }
  static constexpr auto max() noexcept -> result_type { return static_cast<result_type>(~min()); }
  constexpr auto size() noexcept -> result_type { return values_.size(); }
};

template <typename T>
class BitwiseUniformDistributionOpenBoxTest : public ::testing::Test {};

TYPED_TEST_SUITE(BitwiseUniformDistributionOpenBoxTest, integer_types, );

/// @test @c bitwise_uniform_distribution with a PRNG that generates large types just passes bits through
TYPED_TEST(BitwiseUniformDistributionOpenBoxTest, BigGenerationPassesBitsThrough) {
  bitwise_uniform_distribution<TypeParam> const dist{};

  // Result is all 0s
  sequenced_mock_prng<std::uint64_t> zero_prng{0ULL};
  EXPECT_EQ(dist(zero_prng), TypeParam{});

  // Result is all 1s
  sequenced_mock_prng<std::uint64_t> ones_prng{~0ULL};
  EXPECT_EQ(
      dist(ones_prng),                             //
      std::is_signed<TypeParam>::value             //
          ? static_cast<TypeParam>(-1)             //
          : std::numeric_limits<TypeParam>::max()  //
  );

  // Putting a value on the input side results in the output having the same value (not repeated)
  sequenced_mock_prng<std::uint64_t> prng{0x01ULL, 0x3FULL, 0x74ULL, 0xABULL, 0xF0ULL};
  for (std::size_t i{0}; i < prng.size(); ++i) {
    auto const expected = static_cast<TypeParam>(prng.peek());
    EXPECT_EQ(dist(prng), expected);
  }
}

/// @test @c bitwise_uniform_distribution with a PRNG that generates small types builds the result from chunks
TYPED_TEST(BitwiseUniformDistributionOpenBoxTest, SmallGenerationRepeatsChunks) {
  bitwise_uniform_distribution<TypeParam> const dist{};

  // A size-1 PRNG emits the same byte on every call, which is how the small-generation path builds a whole output
  // value out of copies of that byte.

  // Result is all 0s
  sequenced_mock_prng<std::uint8_t> zero_prng{std::uint8_t{0U}};
  EXPECT_EQ(dist(zero_prng), TypeParam{});

  // Result is all 1s
  sequenced_mock_prng<std::uint8_t> ones_prng{static_cast<std::uint8_t>(~0U)};
  EXPECT_EQ(
      dist(ones_prng),                             //
      std::is_signed<TypeParam>::value             //
          ? static_cast<TypeParam>(-1)             //
          : std::numeric_limits<TypeParam>::max()  //
  );

  // Putting a byte on the input side (which the size-1 PRNG repeats) fills every byte of the output with it
  for (unsigned int const input_val : {0x01U, 0x3FU, 0x74U, 0xABU, 0xF0U}) {
    auto const input_byte = static_cast<std::uint8_t>(input_val);
    sequenced_mock_prng<std::uint8_t> prng{input_byte};
    TypeParam const generated_val{dist(prng)};
    for (std::size_t byte_idx{}; byte_idx < sizeof(TypeParam); ++byte_idx) {
      // NOLINTNEXTLINE(hicpp-signed-bitwise) We're specifically testing the bitwise behaviour of this value
      auto const output_byte = static_cast<std::uint8_t>(generated_val >> (byte_idx * CHAR_BIT));
      EXPECT_EQ(input_byte, output_byte);
    }
  }
}

/// @test With a small-type PRNG, successive @c operator() calls are concatenated with chunk 0 in the low-order bits
TYPED_TEST(BitwiseUniformDistributionOpenBoxTest, ChunkConcatHasExpectedLayout) {
  using unsigned_value_type = std::make_unsigned_t<TypeParam>;
  bitwise_uniform_distribution<TypeParam> const dist{};

  // Prime the PRNG so successive calls return 0x01, 0x02, 0x03, ..., 0x08. The distribution should pull one byte per
  // chunk starting with chunk 0 in the low-order bits, producing the value 0x...040302 01 for TypeParam of the given
  // width.
  sequenced_mock_prng<std::uint8_t> prng{
      std::uint8_t{0x01U},
      std::uint8_t{0x02U},
      std::uint8_t{0x03U},
      std::uint8_t{0x04U},
      std::uint8_t{0x05U},
      std::uint8_t{0x06U},
      std::uint8_t{0x07U},
      std::uint8_t{0x08U},
  };
  TypeParam const generated_val{dist(prng)};

  for (std::size_t byte_idx{}; byte_idx < sizeof(TypeParam); ++byte_idx) {
    auto const expected_byte = static_cast<std::uint8_t>(byte_idx + 1U);
    auto const output_byte =
        static_cast<std::uint8_t>(static_cast<unsigned_value_type>(generated_val) >> (byte_idx * CHAR_BIT));
    EXPECT_EQ(expected_byte, output_byte);
  }
}

/// @test With a small-type PRNG, sweeping every byte value yields a distinct output for each input
TYPED_TEST(BitwiseUniformDistributionOpenBoxTest, SmallGenerationIsInjectiveOverByteSweep) {
  using unsigned_value_type = std::make_unsigned_t<TypeParam>;
  bitwise_uniform_distribution<TypeParam> const dist{};

  // For every possible byte value, check that the resulting output (which fills every byte of the result with that
  // byte) has a bit pattern distinct from every other byte value's output.
  constexpr std::size_t byte_value_count{256U};
  arene::base::array<bool, byte_value_count> seen_outputs{};

  for (std::size_t input_val{}; input_val < byte_value_count; ++input_val) {
    auto const input_byte = static_cast<std::uint8_t>(input_val);
    sequenced_mock_prng<std::uint8_t> prng{input_byte};
    auto const output_bits = static_cast<unsigned_value_type>(dist(prng));

    // The small-generation path replicates the input byte into every byte of the output, so recover it from the low
    // byte and use that as the bucket index.
    auto const output_low_byte = static_cast<std::uint8_t>(output_bits);
    EXPECT_FALSE(seen_outputs[output_low_byte]) << "Duplicate output for input byte " << input_val;
    seen_outputs[output_low_byte] = true;
  }
}

/// @test Feeding a one-hot bit into a big-type PRNG places that bit at the corresponding position of the output
TYPED_TEST(BitwiseUniformDistributionOpenBoxTest, BigGenerationOneHotBitReachesOutputBit) {
  using unsigned_value_type = std::make_unsigned_t<TypeParam>;
  bitwise_uniform_distribution<TypeParam> const dist{};

  // Only bits within TypeParam's width should survive the narrowing cast; feeding a one-hot beyond that width would
  // produce zero, which would be indistinguishable from an unset bit. Test exactly the bits the output can hold.
  constexpr std::size_t output_bit_count{sizeof(TypeParam) * CHAR_BIT};
  for (std::size_t bit_idx{}; bit_idx < output_bit_count; ++bit_idx) {
    sequenced_mock_prng<std::uint64_t> prng{std::uint64_t{1U} << bit_idx};
    auto const output_bits = static_cast<unsigned_value_type>(dist(prng));
    auto const expected_bits = static_cast<unsigned_value_type>(unsigned_value_type{1U} << bit_idx);
    EXPECT_EQ(expected_bits, output_bits) << "Wrong output for one-hot input at bit " << bit_idx;
  }
}

}  // namespace
