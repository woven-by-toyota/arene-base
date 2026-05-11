// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTER_MIXIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTER_MIXIN_HPP_

#include "stdlib/include/cstddef"

namespace testing {

/// test class that counts invocations of special member functions
/// @tparam Tag tag type used to create distinct 'counter_mixin' types with
///   distinct static class counters
///
/// Instances of this type count invocations of special member functions with a
/// static class counter.
///
/// When used in tests, distinct 'Tag' types can be used to create distinct
/// counters.
///
template <class Tag>
struct counter_mixin {
  struct count_t {
    std::size_t dtor{};
    std::size_t default_ctor{};
    std::size_t value_ctor{};
    std::size_t copy_ctor{};
    std::size_t move_ctor{};
    std::size_t copy_assign{};
    std::size_t move_assign{};
    std::size_t swap{};

    friend constexpr auto operator==(count_t const& lhs, count_t const& rhs) noexcept -> bool {
      return (lhs.dtor == rhs.dtor) &&                  //
             (lhs.default_ctor == rhs.default_ctor) &&  //
             (lhs.value_ctor == rhs.value_ctor) &&      //
             (lhs.move_ctor == rhs.move_ctor) &&        //
             (lhs.copy_ctor == rhs.copy_ctor) &&        //
             (lhs.move_assign == rhs.move_assign) &&    //
             (lhs.copy_assign == rhs.copy_assign) &&    //
             (lhs.swap == rhs.swap);
    }

    friend constexpr auto operator!=(count_t const& lhs, count_t const& rhs) noexcept -> bool { return !(lhs == rhs); }

    template <class Ostream>
    friend auto operator<<(Ostream& out, count_t const& arg) -> Ostream& {
      out << "dtor: " << arg.dtor                    //
          << ", default_ctor: " << arg.default_ctor  //
          << ", value_ctor: " << arg.value_ctor      //
          << ", move_ctor: " << arg.move_ctor        //
          << ", copy_ctor: " << arg.copy_ctor        //
          << ", move_assign: " << arg.move_assign    //
          << ", copy_assign: " << arg.copy_assign    //
          << ", swap: " << arg.swap;
      return out;
    }
  };

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static count_t count;

  ~counter_mixin() noexcept { ++count.dtor; }
  counter_mixin() noexcept { ++count.default_ctor; }
  counter_mixin(counter_mixin const&) noexcept { ++count.copy_ctor; }
  counter_mixin(counter_mixin&&) noexcept { ++count.move_ctor; }
  auto operator=(counter_mixin const&) noexcept -> counter_mixin& {
    ++count.copy_assign;
    return *this;
  }
  auto operator=(counter_mixin&&) noexcept -> counter_mixin& {
    ++count.move_assign;
    return *this;
  }
  friend void swap(counter_mixin const&, counter_mixin const&) noexcept { ++count.swap; }
};

template <class Tag>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
typename counter_mixin<Tag>::count_t counter_mixin<Tag>::count = {};

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COUNTER_MIXIN_HPP_
