<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page pointer pointer: Checked Pointer Types

<!-- markdownlint-enable MD041 -->

Provides facilities for reducing defects in code dealing with pointers, both raw
and fancy.

The public header is

```{.cpp}
#include "arene/base/pointer.hpp"
```

The Bazel target is

```text
//:pointer
```

The following facilities are provided:

- \subpage non_owning_ptr
- \subpage non_null_ptr
