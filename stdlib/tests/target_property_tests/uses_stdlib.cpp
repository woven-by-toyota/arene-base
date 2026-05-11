// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cfloat>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <utility>

static constexpr bool arene_base_stdlib_header_guard_is_defined =
#if defined(INCLUDE_GUARD_STDLIB_TYPE_TRAITS_)
    true
#else
    false
#endif
    ;

auto main() -> int { static_assert(arene_base_stdlib_header_guard_is_defined, ""); }
