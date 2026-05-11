// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_TAGS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_TAGS_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <iterator>

namespace std {

/// @brief An empty tag type corresponding to an input iterator
class input_iterator_tag {};

/// @brief An empty tag type corresponding to an output iterator
class output_iterator_tag {};

/// @brief An empty tag type corresponding to a forward iterator
class forward_iterator_tag : public input_iterator_tag {};

/// @brief An empty tag type corresponding to a bidirectional iterator
class bidirectional_iterator_tag : public forward_iterator_tag {};

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: Declaration *is* preceeded by the @brief tag."
/// @brief An empty tag type corresponding to a random access iterator
class random_access_iterator_tag : public bidirectional_iterator_tag {};
// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_TAGS_HPP_
