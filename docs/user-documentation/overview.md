<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->

\mainpage

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

Arene Base is a loosely-connected collection of low-level utility features for
use in C++ programs. These include:

- back-ports of features from more recent C++ standards,
- data structures that do not require dynamic memory allocation
- other basic building block data structures
- low level type traits and functions
- string algorithms
- facilities to simplify safe usage of pointers
- a portable filesystem API
- other supporting library facilities

Arene Base also provides a partial implementation of the <!-- break comment -->
[C++ Standard Library](\ref stdlib) for use on platforms that do not have a
suitable C++ Standard Library provided by the toolchain.

## Documentation Structure {#documentation-structure}

This manual is broken down into the following sections for easier browsing:

- \subpage getting_started
- \subpage subpackages
- \subpage tutorials
- \subpage best_practices
- \subpage errorhandling
- \subpage stdlib
