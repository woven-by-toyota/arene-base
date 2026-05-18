<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page gtest testing/gtest: Helpers for Working With GoogleTest/Mock

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

The gtest library provides facilities that make writing tests in gtest simpler
and more robust.

## Public Header {#public-header}

Consumers of `arene_base` get access to the facilities of this library through
the public export location:

```cpp
#include "arene/base/testing/gtest.hpp"
```

The namespace for facilities in this library is:

```cpp
arene::base::testing::gtest
```

## Public Target {#public-target}

The externally exported bazel target is:

```bazel
://testing/gtest
```

## Functionality {#functionality}

The following functionality is provided by this library:

- \subpage conditional_tests
- \subpage static_assertions
- \subpage constexpr_test
