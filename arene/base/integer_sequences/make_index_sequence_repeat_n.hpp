// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_MAKE_INDEX_SEQUENCE_REPEAT_N_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_MAKE_INDEX_SEQUENCE_REPEAT_N_HPP_

// IWYU pragma: private, include "arene/base/integer_sequences.hpp"

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_manipulation/repeat_type.hpp"

namespace arene {
namespace base {

namespace make_index_sequence_repeat_n_detail {

/// @brief adapt a pack of @c std::integral_constant types to @c std::index_sequence
/// @tparam ICs pack of @c std::integral_constant specializations whose values seed the sequence
template <class... ICs>
using to_index_sequence = std::index_sequence<ICs::value...>;

}  // namespace make_index_sequence_repeat_n_detail

/// @brief An @c std::index_sequence whose elements are @p Count copies of @p Value.
/// @tparam Value the value to repeat
/// @tparam Count the number of copies to emit
template <std::size_t Value, std::size_t Count>
using make_index_sequence_repeat_n = repeat_type_t<
    Count,
    make_index_sequence_repeat_n_detail::to_index_sequence,
    std::integral_constant<std::size_t, Value>>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_MAKE_INDEX_SEQUENCE_REPEAT_N_HPP_
