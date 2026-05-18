<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page memory memory: Facilities For Memory Region Manipulation

<!-- markdownlint-enable MD041 -->

Arene Base provides facilities for managing the lifetime of objects within a
region of memory.

The public header is

```{.cpp}
#include "arene/base/memory.hpp""
```

The Bazel target is

```text
//:memory
```

## Introduction {#introduction}

The `memory` subpackage provides backports of
[`std::construct_at`](https://en.cppreference.com/w/cpp/memory/construct_at) and
[`std::destroy_at`](https://en.cppreference.com/w/cpp/memory/destroy_at) in the
form of `arene::base::construct_at` and `arene::base::destroy_at` respectively.

`arene::base::construct_at` is a wrapper for placement-`new`, to construct an
object at a specific address. It avoids the need to cast the supplied address to
`void*` to avoid class-specific `operator new` overloads.

`arene::base::destroy_at` is a wrapper for a destructor call on the supplied
object. After the call, the object has been destroyed, and pointers to it are
invalid.
