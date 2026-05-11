#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_RUN_PROPERTY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_RUN_PROPERTY_HPP_

#include "arene/base/functional/function_traits.hpp"
// IWYU pragma: no_include "arene/base/optional/optional.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/detail/property_context.hpp"
#include "arene/base/testing/prng_xoshiro.hpp"
#include "arene/base/testing/property_config.hpp"
#include "arene/base/testing/property_error.hpp"
#include "arene/base/testing/property_summary.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/detail/tuple_transform.hpp"
#include "arene/base/tuple/tuple_cat.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/drop.hpp"
#include "arene/base/type_traits/type_identity.hpp"

namespace arene {
namespace base {
namespace testing {
namespace run_property_detail {

/// @brief function object that generates a value of a requested type
/// @tparam Prng PRNG type used to generate a value
template <class Prng>
struct generate_argument {
  /// @brief reference to a PRNG
  Prng& prng;

  /// @brief generate a value of type @c T
  /// @tparam T type to generate
  /// @return generated value of type @c T
  template <class T>
  constexpr auto operator()(type_identity<T>) const -> T {
    // TODO: use prng to generate the argument
    return {};
  }
};

}  // namespace run_property_detail

/// @brief run a property callable against a battery of trials with generated arguments
/// @tparam Callable function object with a call operator where the first
///   parameter is a reference to the PRNG and whose remaining parameters are
///   generated with @c generator<T>
/// @param callable property to exercise
/// @param config seed and stopping criteria for a property run
///
/// Repeatedly invokes @p callable with generated arguments until one of the
/// following terminal conditions is reached:
/// * a trial reports @c property_status::failure -- the run stops
///   immediately and the failing @c property_error is captured in @c
///   property_summary::first_failure;
/// * the number of discarded trials exceeds @c property_config::max_discards --
///   the run is reported as failed;
/// * @c property_config::required_successes successful trials have
///   been observed -- the run is reported as passing.
///
/// @return A @c property_summary describing the outcome: number of successes
/// and discards, whether the property failed, the seed actually used, and (on
/// failure) the first failed property assertion.
///
/// @note @c Callable is synthesized by the @c ARENE_PROPERTY or
/// @c ARENE_TYPED_PROPERTY macro. It is not typically user provided.
template <class Callable>
auto run_property(Callable callable, property_config const config) -> property_summary {
  auto prng = prng_xoshiro{config.seed};
  auto summary = property_summary{0U, 0U, false, config.seed, {}};

  using argument_types = type_lists::apply_all_t<                                               //
      type_lists::apply_each_t<                                                                 //
          type_lists::drop_t<member_function_arguments_t<decltype(&Callable::operator())>, 1>,  //
          type_identity>,                                                                       //
      std::tuple>;

  while ((summary.successes < config.required_successes) && (summary.discards <= config.max_discards)) {
    auto& outcome = detail::property_context();
    outcome.reset();

    apply(
        callable,
        tuple_cat(  //
            std::tuple<decltype(prng)&>{prng},
            tuple_detail::tuple_transform(
                argument_types{},
                run_property_detail::generate_argument<decltype(prng)>{prng}
            )
        )
    );

    if (outcome.has_value()) {
      using error_code = property_error::error_code;
      switch (outcome->error) {
        case error_code::failure: {
          summary.failed = true;
          summary.first_failure = outcome;
          return summary;
        }
        case error_code::discard: {
          ++summary.discards;
          break;
        }
      }
    } else {
      ++summary.successes;
    }
  }

  summary.failed = summary.successes != config.required_successes;
  return summary;
}

}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_RUN_PROPERTY_HPP_
