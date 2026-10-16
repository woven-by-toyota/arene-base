<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page ranges ranges: Range Utilities

<!-- markdownlint-enable MD041 -->

The `ranges` subpackage provides backports of functionality from the C++
Standard Library's `std::ranges` namespace, as well as extensions specific to
Arene Base.

The public export header is:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/type_traits_examples.cpp include_header
<!-- prettier-ignore-end -->

The Bazel target is `//:ranges`.

The following category of facilities is provided:

- \subpage ranges_range_access
- \subpage ranges_type_traits
