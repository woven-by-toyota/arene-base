// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTED_ALGORITHM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTED_ALGORITHM_HPP_

#include "stdlib/include/cstddef"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"

namespace testing {

/// @brief function object adaptor that records the number of invocations
/// @tparam F default constructible function object
///
template <class F>
struct counted : F {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members,misc-non-private-member-variables-in-classes)
  std::size_t& count;

  template <class G = F, class = std::enable_if_t<std::is_default_constructible<G>::value>>
  constexpr explicit counted(std::size_t& ref)
      : count{ref} {}

  constexpr counted(F func, std::size_t& ref)
      : F{std::move(func)},
        count{ref} {}

  constexpr auto function() & noexcept -> F& { return static_cast<F&>(*this); }

  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) &  // NOLINT(modernize-use-trailing-return-type)
      noexcept(noexcept(std::declval<F&>()(std::forward<Args>(args)...))) {
    ++count;
    return function()(std::forward<Args>(args)...);
  }
};

/// @brief result type of 'counted_algorithm'
///
/// @{
template <class Result>
struct counted_algorithm_result {
  Result iter{};
  std::size_t applications{};
};

template <>
struct counted_algorithm_result<void> {
  std::size_t applications{};
};
/// @}

/// @brief counting algorithm wrapper
/// @tparam Algo algorithm function type
/// @tparam Range range type
/// @tparam Pred predicate type
/// @param algo algorithm to invoke
/// @param range range with which to invoke the algorithm
/// @param pred algorithm predicate to adapt
///
/// Helper for determining the complexity of certain classes of algorithms.
///
/// Invokes an algorithm that takes a predicate as the *last* argument, adapting
/// the predicate to use `counted`. The return of this function is a type that
/// defines two members:
///
/// ~~~{.cpp}
/// struct unnamed-type {
///   result-type iter{};
///   std::size_t applications{};
/// };
/// ~~~
///
/// where `iter` is the return value of the algorithm (which is not always an
/// iterator) and `applications` the number of invocations of `pred`.
///
/// Examples:
/// ~~~{.cpp}
/// const auto result = testing::counted_algorithm(
///   FUNCTION_LIFT(std::unique),
///   first,
///   last,
///   has_same_sign
/// );
///
/// ASSERT_EQ(result.iter, ...);
/// ASSERT_EQ(result.applications, ...);
/// ~~~
///
/// ~~~{.cpp}
/// const auto result = testing::counted_algorithm(
///   FUNCTION_LIFT(std::unique),
///   range,
///   has_same_sign
/// );
///
/// ASSERT_EQ(result.iter, ...);
/// ASSERT_EQ(result.applications, ...);
/// ~~~
///
/// @note `counted_algorithm` provides an overload that obtains the begin and
///   end iterators of a range.
/// @note `counted_algorithm` requires a range argument to be an lvalue
///   reference to avoid dangling iterators.
/// @note `counted_algorithm` requires a range argument to define `begin` and
///   `end` member functions.
///
static constexpr class {
  template <class Algo, class... Args>
  using algorithm_result_t = decltype(std::declval<Algo const&>()(std::declval<Args&&>()...));

  template <class Algo, class... Args>
  using counted_algorithm_result_t =
      counted_algorithm_result<decltype(std::declval<Algo const&>()(std::declval<Args&&>()...))>;

  // forward the argument if the tag is true_type, otherwise apply the counted
  // function object adaptor if the tag is false_type
  //
  // @{
  template <class T>
  static constexpr auto forward_or_adapt(std::true_type, T&& value, std::size_t&) -> T&& {
    return std::forward<T>(value);
  }
  template <class Pred>
  static constexpr auto forward_or_adapt(std::false_type, Pred pred, std::size_t& counter) {
    return counted<Pred>{std::move(pred), counter};
  }
  // @}

  // implementation function that adapts the last argument
  //
  // @{
  template <std::size_t... Is, class Algo, class... Args, class R = algorithm_result_t<Algo, Args&&...>>
  static constexpr auto invoke_with_counted_last(std::index_sequence<Is...>, Algo algo, Args&&... args)
      -> std::enable_if_t<!std::is_void<R>::value, counted_algorithm_result<R>> {
    auto info = counted_algorithm_result<R>{};

    info.iter = algo(                                                    //
        forward_or_adapt(                                                //
            std::integral_constant<bool, Is != sizeof...(Args) - 1U>{},  //
            std::forward<Args>(args),                                    //
            info.applications                                            //
        )...                                                             //
    );

    return info;
  }

  template <std::size_t... Is, class Algo, class... Args, class R = algorithm_result_t<Algo, Args&&...>>
  static constexpr auto invoke_with_counted_last(std::index_sequence<Is...>, Algo algo, Args&&... args)
      -> std::enable_if_t<std::is_void<R>::value, counted_algorithm_result<R>> {
    auto info = counted_algorithm_result<R>{};

    algo(                                                                //
        forward_or_adapt(                                                //
            std::integral_constant<bool, Is != sizeof...(Args) - 1U>{},  //
            std::forward<Args>(args),                                    //
            info.applications                                            //
        )...                                                             //
    );

    return info;
  }
  // @}

 public:
  // overload that adapts the last argument and forwards the rest
  template <class Algo, class... Args>
  constexpr auto operator()(Algo algo, Args&&... args) const -> counted_algorithm_result_t<Algo, Args&&...>  //
  {
    static_assert(sizeof...(Args) != 0U, "");

    return invoke_with_counted_last(
        std::make_index_sequence<sizeof...(Args)>{},  //
        algo,
        std::forward<Args>(args)...
    );
  }

  // overload that adapts the last argument and treats the first as a range
  template <class Algo, class Range, class Pred>
  constexpr auto operator()(Algo algo, Range& range, Pred pred) const
      -> counted_algorithm_result<decltype(algo(range.begin(), range.end(), pred))>  //
  {
    return (*this)(algo, range.begin(), range.end(), std::move(pred));
  }

} counted_algorithm{};

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTED_ALGORITHM_HPP_
