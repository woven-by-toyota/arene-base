// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/transform_reduce_fwd.hpp"
#include "arene/base/algorithm/transform_reduce.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/stdlib_choice/negate.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace transform_reduce_test {

/// @brief adapts a binary function object to a unary function object
/// @tparam Bop binary function object type
///
/// Function object adaptor that calls the underlying binary function object with duplicated arguments
template <class Bop>
struct duplicate_args {
  Bop bop;

  // this overload is non-const so that it works with the counting wrapper
  template <class Arg>
  constexpr auto operator()(Arg& arg) noexcept(  //
      noexcept(std::declval<Bop&>()(std::declval<Arg&>(), std::declval<Arg&>()))
  ) -> decltype(std::declval<Bop&>()(std::declval<Arg&>(), std::declval<Arg&>())) {
    return bop(arg, arg);
  }

  // this overload is non-const so that it works with the counting wrapper
  template <class Arg>
  constexpr auto operator()(Arg&& arg) noexcept(  //
      noexcept(std::declval<Bop&>()(std::declval<Arg&&>(), std::declval<Arg&&>()))
  ) -> decltype(std::declval<Bop&>()(std::declval<Arg&&>(), std::declval<Arg&&>())) {
    auto arg2 = arg;
    return bop(std::forward<Arg>(arg), std::forward<Arg>(arg2));
  }
};

constexpr auto square = duplicate_args<std::multiplies<>>{{}};

/// @brief adapts the single range overload of @c transform_reduce to work with
/// the provided test harness
///
/// This function object provides overloads that operate on two ranges by simply
/// ignoring the second range and only applies an unary transform operator to
/// the first range.
///
/// @note These overloads simply ignore the second range. It could be possible
/// to synthesize a single range as the zip of the first and second ranges and
/// adapt the binary transform to a unary transform, but that would require a
/// zip_iterator, which is not currently implemented.
struct transform_reduce_1range_overload_wrapper {
  /// @brief calls a single-range overload of transform_reduce with first range
  /// @tparam InputIt1 iterator type of the first range
  /// @tparam InputIt2 iterator type of the second range
  /// @tparam Value initial value type
  /// @tparam Reduce reduce function object
  /// @tparam Transform transform function object
  /// @param first iterator to the beginning of the range
  /// @param last iterator to the end of the range
  /// @param value initial value
  /// @param reduce binary reduce operation
  /// @param transform binary transform operation (adapted by this overload)
  ///
  /// Accepts the iterator signature of the two-range overload, but ignores the
  /// second range and forwards to the single-range overload.
  ///
  /// The binary transform `f(x, y)` is adapted to a unary transform `g(x)`:
  /// @code {cpp}
  /// auto g(x) { return f(x, x); }
  /// @endcode
  // NOLINTBEGIN(hicpp-move-const-arg) it is semantically correct to move a
  // value that is used once
  template <class InputIt1, class InputIt2, class Value, class Reduce, class Transform>
  constexpr auto operator()(  //
      InputIt1 first,
      InputIt1 last,
      InputIt2,
      Value value,
      Reduce reduce,
      Transform transform
  ) const noexcept(  //
      noexcept(::arene::base::transform_reduce(
          std::move(first),
          std::move(last),
          std::move(value),
          std::move(reduce),
          duplicate_args<Transform>{std::move(transform)}
      ))
  )
      -> decltype(::arene::base::transform_reduce(
          std::move(first),
          std::move(last),
          std::move(value),
          std::move(reduce),
          duplicate_args<Transform>{std::move(transform)}
      )) {
    return ::arene::base::transform_reduce(  //
        std::move(first),
        std::move(last),
        std::move(value),
        std::move(reduce),
        duplicate_args<Transform>{std::move(transform)}
    );
    // NOLINTEND(hicpp-move-const-arg)
  }

  /// @brief calls a single-range overload of transform_reduce with first range
  /// @tparam Range1 type of the first range
  /// @tparam InputIt2 iterator type of the second range
  /// @tparam Value initial value type
  /// @tparam Reduce reduce function object
  /// @tparam Transform transform function object
  /// @param range first range
  /// @param value initial value
  /// @param reduce binary reduce operation
  /// @param transform binary transform operation (adapted by this overload)
  ///
  /// Accepts the range signature of the two-range overload, but ignores the
  /// second range and forwards to the single-range overload.
  ///
  /// The binary transform `f(x, y)` is adapted to a unary transform `g(x)`:
  /// @code {cpp}
  /// auto g(x) { return f(x, x); }
  /// @endcode
  // NOLINTBEGIN(hicpp-move-const-arg) it is semantically correct to move a
  // value that is used once
  template <class Range1, class InputIt2, class Value, class Reduce, class Transform>
  constexpr auto operator()(  //
      Range1&& range,
      InputIt2,
      Value value,
      Reduce reduce,
      Transform transform
  ) const noexcept(  //
      noexcept(::arene::base::transform_reduce(
          std::forward<Range1>(range),
          std::move(value),
          std::move(reduce),
          duplicate_args<Transform>{std::move(transform)}
      ))
  )
      -> decltype(::arene::base::transform_reduce(
          std::forward<Range1>(range),
          std::move(value),
          std::move(reduce),
          duplicate_args<Transform>{std::move(transform)}
      )) {
    return ::arene::base::transform_reduce(  //
        std::forward<Range1>(range),
        std::move(value),
        std::move(reduce),
        duplicate_args<Transform>{std::move(transform)}
    );
  }
  // NOLINTEND(hicpp-move-const-arg)

  /// @brief calls a single-range overload of transform_reduce with first range
  /// @tparam InputIt1 iterator type of the first range
  /// @tparam InputIt2 iterator type of the second range
  /// @tparam Value initial value type
  /// @param first iterator to the beginning of the range
  /// @param last iterator to the end of the range
  /// @param value initial value
  ///
  /// Accepts the iterator signature of the two-range overload, but ignores the
  /// second range and forwards to the single-range overload. Provides
  /// std::plus<> and square as arguments for the reduce and transform
  /// operations.
  template <class InputIt1, class InputIt2, class Value>
  constexpr auto operator()(  //
      InputIt1 first,
      InputIt1 last,
      InputIt2,
      Value value
  ) const noexcept(                              //
      noexcept(::arene::base::transform_reduce(  //
          std::move(first),
          std::move(last),
          std::move(value),
          std::plus<>{},
          square
      ))
  )
      -> decltype(::arene::base::transform_reduce(
          std::move(first),
          std::move(last),
          std::move(value),
          std::plus<>{},
          square
      )) {
    return ::arene::base::transform_reduce(  //
        std::move(first),
        std::move(last),
        std::move(value),
        std::plus<>{},
        square
    );
  }
};

}  // namespace transform_reduce_test

template <>
constexpr auto transform_reduce_test::config::version<transform_reduce_test::config::override> =
    transform_reduce_test::config::flavor::transform_reduce;
template <>
constexpr auto transform_reduce_test::config::range_arity<transform_reduce_test::config::override> = 1U;
template <>
constexpr auto const& transform_reduce_test::config::function_under_test<transform_reduce_test::config::override> =
    transform_reduce_1range_overload_wrapper{};

#include "arene/base/algorithm/tests/transform_reduce.hpp"  // IWYU pragma: keep

namespace {

/// @test the single range overload of @c transform_reduce requires iterators as
/// arguments
TEST(TransformReduce1Range, RequiresIterators) {
  ASSERT_TRUE((arene::base::is_invocable_v<  //
               decltype(arene::base::transform_reduce),
               int*,
               int*,
               int,
               std::plus<>,
               std::negate<>>));

  ASSERT_FALSE((arene::base::is_invocable_v<  //
                decltype(arene::base::transform_reduce),
                int,
                int*,
                int,
                std::plus<>,
                std::negate<>>));
  ASSERT_FALSE((arene::base::is_invocable_v<  //
                decltype(arene::base::transform_reduce),
                int*,
                int,
                int,
                std::plus<>,
                std::negate<>>));
}

/// @test @c transform_reduce is @c noexcept(false) if constructing the reduce
/// policy value is @c noexcept(false)
TEST(TransformReduce, NoexceptIfReducePolicyValueConstructionIsFalse) {
  using testing::disable;
  using testing::throws_on;

  using move_assignable_acc = testing::configurable_value<int, throws_on::move_construct>;
  using non_move_assignable_acc =
      testing::configurable_value<int, throws_on::move_construct, disable::move_assign | disable::copy_assign>;

  ASSERT_TRUE((arene::base::is_invocable_v<  //
               decltype(arene::base::transform_reduce),
               move_assignable_acc*,
               move_assignable_acc*,
               move_assignable_acc,
               std::plus<>,
               arene::base::identity>));
  ASSERT_TRUE((arene::base::is_invocable_v<  //
               decltype(arene::base::transform_reduce),
               non_move_assignable_acc*,
               non_move_assignable_acc*,
               non_move_assignable_acc,
               std::plus<>,
               arene::base::identity>));

  ASSERT_FALSE((arene::base::is_nothrow_invocable_v<  //
                decltype(arene::base::transform_reduce),
                move_assignable_acc*,
                move_assignable_acc*,
                move_assignable_acc,
                std::plus<>,
                arene::base::identity>));
  ASSERT_FALSE((arene::base::is_nothrow_invocable_v<  //
                decltype(arene::base::transform_reduce),
                non_move_assignable_acc*,
                non_move_assignable_acc*,
                non_move_assignable_acc,
                std::plus<>,
                arene::base::identity>));
}

}  // namespace
