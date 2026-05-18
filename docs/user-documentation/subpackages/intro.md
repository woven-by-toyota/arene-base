<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page subpackages Subpackages

<!-- markdownlint-enable MD041 -->

Arene Base is divided into a set of sub-packages, so that applications using
Arene Base only need to depend on the specific subset that they require. Each
sub-package has a corresponding top-level Bazel target in the Arene Base
repository, which are enumerated below.

In addition to the individual targets, there is a "grab bag" target,
`//:everything`, which contains all of the subpackages in `arene_base`. This
target is convenient for prototyping when evaluating `arene_base`, but should
generally not be depended on directly in production as it may include content
not suitable for a given domain, and may also needlessly slow compile times.

## Available Subpackages {#available-subpackages}

- \subpage algorithm
- \subpage array
- \subpage atomic
- \subpage bit
- \subpage byte
- \subpage compare
- \subpage compiler_support
- \subpage constraints
- \subpage contracts
- \subpage endian
- \subpage filesystem
- \subpage functional
- \subpage inline_container
- \subpage inline_string
- \subpage integer_sequences
- \subpage intrusive
- \subpage iterator
- \subpage math
- \subpage mdspan
- \subpage memory
- \subpage monostate
- \subpage mutex
- \subpage optional
- \subpage pointer
- \subpage result
- \subpage scope_guard
- \subpage semantic_version
- \subpage source_location
- \subpage span
- \subpage string_algorithms
- \subpage string_view
- \subpage synchronization
- \subpage testing
- \subpage tuple
- \subpage typeinfo
- \subpage type_list
- \subpage type_manipulation
- \subpage type_traits
- \subpage unicode
- \subpage units
- \subpage utility
- \subpage variant
