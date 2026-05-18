<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page semantic_version semantic_version: Facilities For Working With Semvers

<!-- markdownlint-enable MD041 -->

The `semantic_version` sub-package provides a facility for working with semantic
version numbers.

The public header is

```{.cpp}
#include "arene/base/semantic_version.hpp"
```

The Bazel target is

```text
//:semantic_version
```

## Semantic Versions {#semantic-versions}

`arene::base::semantic_version` holds a version number with 3 parts: the
**major** version, the **minor** version, and the **patch** version, like
`15.1.7`. The **semantic** part indicates that there is _meaning_ to changes in
the different parts of a version number, usually as described by the
[Semantic Versioning Specification](https://semver.org/).

The ordering comparison operators for `arene::base::semantic_version` handle the
3 parts in order, so `1.2.3` is less than `2.0.1`, but greater than `1.1.9`.

There are also functions to check for specific changes:

- `arene::base::contains_major_change` will return `true` if the major numbers
  of the two versions supplied differ. `arene::base::contains_minor_change` does
  the same for minor versions, and `arene::base::contains_patch_change` for
  patch numbers.
- `arene::base::compute_difference` identifies which parts have changed,
  returning an `arene::base::sem_ver_diff` indicating which fields have changed.
- `arene::base::is_major_change`, `arene::base::is_minor_change` and
  `arene::base::is_patch_change` bring things together:
  - a **major** change has a change in the major version number,
  - a **minor** change means the major version is unchanged, but the minor
    version has changed, and
  - a **patch** change has the same major and minor versions, but a different
    patch number.
