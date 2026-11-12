// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TEST_ARRAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TEST_ARRAY_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/complex_decomp.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_arithmetic.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/testing/array_distribution.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/default_distribution.hpp"  // IWYU pragma: keep
#include "arene/base/testing/nan_rejecting_distribution.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

namespace arene {
namespace base {
namespace linalg_testing {

/// @brief Default size to use for ranks with dynamic extent
constexpr std::size_t default_dynamic_extent{10U};

/// @brief Generator type specializations for different element types
/// @{
template <class T, class = void>
struct generator_type;

template <class T>
struct generator_type<T, std::enable_if_t<std::is_integral<T>::value>> {
  using type = testing::bitwise_uniform_distribution<T>;
};

template <class T>
struct generator_type<T, std::enable_if_t<std::is_floating_point<T>::value>> {
  using type = testing::nan_rejecting_distribution<testing::bitwise_uniform_distribution<T>>;
};
/// @}

/// @brief Randomly generate a single entry for an array using the provided PRNG
/// @tparam T entry type to generate
template <typename T>
class generate_entry {
 public:
  /// @brief value type produced by this distribution
  using result_type = T;

  /// @brief generate a single entry of type @c T
  /// @tparam Prng A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return A random value of type @c T each of whose components has a magnitude of at most about 100
  /// @{
  template <typename Prng, typename U = T, constraints<std::enable_if_t<std::is_arithmetic<U>::value>> = nullptr>
  constexpr auto operator()(Prng& prng) const noexcept -> result_type {
    // Approximate scaling target - the integer division to calculate divisor will truncate.
    constexpr U target_magnitude{100};
    constexpr auto divisor = std::numeric_limits<U>::max() / target_magnitude;
    static_assert(
        divisor >= target_magnitude,
        "generate_entry cannot bound the magnitude of 8-bit element types; use an in_range_distribution instead"
    );

    typename generator_type<U>::type dist{};

    // As of the time of writing this code, the range-limited distribution adapter is not yet available.
    // We need to limit the range to prevent overflow when doing math.
    return dist(prng) / divisor;
  }

  template <typename Prng, typename U = T, constraints<std::enable_if_t<math::is_complex_v<U>>> = nullptr>
  constexpr auto operator()(Prng& prng) const noexcept -> result_type {
    using real_type = arene::base::remove_cvref_t<decltype(arene::base::math::real(std::declval<U const&>()))>;
    using imag_type = arene::base::remove_cvref_t<decltype(arene::base::math::imag(std::declval<U const&>()))>;

    return result_type{generate_entry<real_type>{}(prng), generate_entry<imag_type>{}(prng)};
  }

  template <typename Prng, typename U = T, constraints<std::enable_if_t<::testing::is_counting_wrapper_v<U>>> = nullptr>
  constexpr auto operator()(Prng& prng) const noexcept -> result_type {
    return generate_entry<decltype(U::value)>{}(prng);
  }
  /// @}
};

/// @brief A simplified version of a linalg object that contains its own data and provides an @c mdspan to access it
/// @tparam Mdspan nominal mdspan type returned by this class
/// @tparam InterpretStaticExtentsAsDynamic adapts the returned mdspan type to
///   always have all dynamic extents, where the extents values are specifed by
///   Mdspan::extents_type::static_extent(i)
template <typename Mdspan, bool InterpretStaticExtentsAsDynamic = false>
class test_array {
 public:
  using mdspan_type = std::conditional_t<
      InterpretStaticExtentsAsDynamic,
      arene::base::mdspan<
          typename Mdspan::element_type,
          arene::base::dextents<typename Mdspan::index_type, Mdspan::rank()>,
          typename Mdspan::layout_type,
          typename Mdspan::accessor_type>,
      Mdspan>;

  using const_mdspan_type = arene::base::testing::mdspan_transform_element_type_t<mdspan_type, std::add_const_t>;

 private:
  using extents_type = typename mdspan_type::extents_type;
  using mapping_type = typename mdspan_type::mapping_type;

  /// @brief Create @c extents where each @c dynamic_extent is set to @c default_dynamic_extent
  static constexpr auto init_extents(std::false_type /* interpret_static_as_dynamic */) noexcept -> extents_type {
    arene::base::array<typename Mdspan::index_type, Mdspan::rank_dynamic()> dynamic_indices{};
    dynamic_indices.fill(default_dynamic_extent);
    return dynamic_indices;
  }

  /// @brief Create an @c extents of all @c dynamic_extent values, where each value is
  /// @c Mdspan::extents_type::static_extent(i)
  static constexpr auto init_extents(std::true_type /* interpret_static_as_dynamic */) noexcept -> extents_type {
    return arene::base::mdspan_detail::to_array(typename Mdspan::extents_type{});
  }

  /// @brief Build a @c layout_stride mapping from equivalent row-major strides
  ///
  /// A @c layout_stride mapping cannot be constructed from extents alone, so derive its strides from a @c layout_right
  /// mapping (which @c layout_stride accepts through its strided-mapping converting constructor).
  template <
      typename M = mapping_type,
      std::enable_if_t<std::is_same<typename M::layout_type, arene::base::layout_stride>::value>* = nullptr>
  static constexpr auto init_mapping() noexcept -> mapping_type {
    extents_type const extents = init_extents(std::integral_constant<bool, InterpretStaticExtentsAsDynamic>{});
    return mapping_type{typename arene::base::layout_right::template mapping<extents_type>{extents}};
  }

  /// @brief Build a mapping for any layout constructible directly from its extents
  template <
      typename M = mapping_type,
      std::enable_if_t<!std::is_same<typename M::layout_type, arene::base::layout_stride>::value>* = nullptr>
  static constexpr auto init_mapping() noexcept -> mapping_type {
    extents_type const extents = init_extents(std::integral_constant<bool, InterpretStaticExtentsAsDynamic>{});
    return mapping_type{extents};
  }

 public:
  /// @brief Number of elements needed to back an instance of @c Mdspan
  static constexpr std::size_t buffer_size{static_cast<std::size_t>(init_mapping().required_span_size())};

  /// @brief Type of the contents of this test vector
  using buffer_type = arene::base::array<typename mdspan_type::value_type, buffer_size>;

 private:
  /// @brief Data member containing the mapping used to create the buffer
  mapping_type mapping_{init_mapping()};

  /// @brief Data member providing enough buffer space to back an instance of @c Mdspan
  buffer_type buffer_{};

 public:
  /// @brief Default-construct a test vector with an array full of zeros for its contents
  constexpr test_array() noexcept = default;

  /// @brief Construct a test vector holding the given contents
  /// @param buffer contents of the test vector
  explicit constexpr test_array(buffer_type buffer) noexcept
      : buffer_{std::move(buffer)} {}

  /// @brief Reference to the underlying buffer
  constexpr auto buffer() noexcept -> auto& { return buffer_; }

  /// @brief Const reference to the underlying buffer
  constexpr auto buffer() const noexcept -> auto const& { return buffer_; }

  /// @brief Get an instance of the @c mdspan type referring to the contents of this test vector
  constexpr auto mdspan() noexcept -> mdspan_type { return mdspan_type{buffer_.data(), mapping_}; }
  /// @brief Get an instance of the @c const_mdspan type referring to the contents of this const test vector
  constexpr auto mdspan() const noexcept -> const_mdspan_type { return const_mdspan_type{buffer_.data(), mapping_}; }
  // NOLINTNEXTLINE(hicpp-explicit-conversions) The algorithms that this class is testing operate on mdspans
  constexpr operator mdspan_type() noexcept { return mdspan(); }

  /// @brief Negate all of the entries in this test vector
  constexpr auto negate() noexcept -> void {
    for (auto& entry : buffer_) {
      entry = -entry;
    }
  }

  /// @brief Multiply all of the entries in this test vector by a scalar
  constexpr auto scalar_mul(typename Mdspan::value_type value) noexcept -> void {
    for (auto& entry : buffer_) {
      entry *= value;
    }
  }

  /// @brief Obtain a pointer to the underlying data
  constexpr auto data() noexcept -> typename Mdspan::value_type* { return buffer_.data(); }

  /// @brief Obtain number of data elements
  constexpr auto size() const noexcept -> std::size_t { return buffer_.size(); }
};

template <typename Mdspan, bool InterpretStaticExtentsAsDynamic>
constexpr std::size_t test_array<Mdspan, InterpretStaticExtentsAsDynamic>::buffer_size;

/// @brief Distribution producing a @c test_array whose entries are bounded random values
/// @tparam Mdspan nominal mdspan type of the produced @c test_array
/// @tparam InterpretStaticExtentsAsDynamic forwarded to the produced @c test_array
///
/// Adapts an @c array_distribution over @c generate_entry so that the produced value is the @c test_array itself
/// rather than its buffer, as @c default_distribution requires of the distributions registered with it.
template <typename Mdspan, bool InterpretStaticExtentsAsDynamic = false>
class test_array_distribution {
 public:
  /// @brief value type produced by this distribution
  using result_type = test_array<Mdspan, InterpretStaticExtentsAsDynamic>;

 private:
  /// @brief distribution producing the buffer backing the @c test_array
  using buffer_distribution_type = testing::
      array_distribution<generate_entry<typename result_type::buffer_type::value_type>, result_type::buffer_size>;

 public:
  /// @brief generate a @c test_array with every entry drawn from @c generate_entry
  /// @tparam Prng A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return A @c test_array whose entries are independent draws, in buffer order
  template <typename Prng>
  constexpr auto operator()(Prng& prng) const noexcept(noexcept(std::declval<buffer_distribution_type const&>()(prng)))
      -> result_type {
    return result_type{buffer_dist_(prng)};
  }

 private:
  /// @brief distribution each buffer is drawn from
  buffer_distribution_type buffer_dist_{};
};

/// @brief A simplified version of a linalg object that contains its own data and provides an @c mdspan to access it
/// @note The provided @c mdspan always has all dynamic extents with the extents values specified as @c Extents...
template <typename T, typename IndexType, std::size_t... Extents>
using dynamic_test_array = test_array<arene::base::mdspan<T, arene::base::extents<IndexType, Extents...>>, true>;

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene

namespace arene {
namespace base {
namespace testing {

/// @brief Generate @c test_array parameters of property tests with bounded random entries
///
/// Entries come from @c generate_entry, which should become an @c in_range_distribution once that adaptor is
/// available.
template <typename Mdspan, bool InterpretStaticExtentsAsDynamic>
constexpr auto default_distribution<linalg_testing::test_array<Mdspan, InterpretStaticExtentsAsDynamic>> =
    linalg_testing::test_array_distribution<Mdspan, InterpretStaticExtentsAsDynamic>{};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TEST_ARRAY_HPP_
