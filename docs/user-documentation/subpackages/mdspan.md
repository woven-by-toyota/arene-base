<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page mdspan mdspan: Facilities For Multidimensional Arrays

<!-- markdownlint-enable MD041 -->

The `mdspan` sub-package provides a set of classes and functions for
manipulating multidimensional arrays.

The public header is

```{.cpp}
#include "arene/base/mdspan.hpp"
```

The Bazel target is

```text
//:mdspan
```

# Introduction {#subpackages-mdspan-introduction}

This package provides the following classes and functions for describing and
viewing multidimensional array data, as backports of the corresponding
facilities from C++23.

- `arene::base::extents`
- `arene::base::dextents`
- `arene::base::layout_stride::mapping`
- `arene::base::layout_left::mapping`
- `arene::base::layout_right::mapping`
- `arene::base::default_accessor`
- `arene::base::mdspan`

## Extensions {#subpackages-mdspan-extensions}

This package provides the following type traits as extensions on the C++23
facilities.

- `arene::base::is_accessor_policy_v`
- `arene::base::is_layout_mapping_v`
- `arene::base::is_layout_mapping_policy_v`

### Axis views {#subpackages-mdspan-axis-views}

`arene::base::axis_view<Axis>` takes an `mdspan` and returns a range over its
slices along axis `Axis`. The `i`-th element of that range is the slice obtained
by fixing axis `Axis` at `i` while keeping every other axis in full, so each
slice has a rank one less than the source.

\snippet docs/examples/axis_view_examples.cpp axis_view_usage_example

The slices are views of the source rather than copies, so no elements are
copied. For the rank-2 case, `arene::base::linalg::row_view` and
`arene::base::linalg::column_view` name the same operation in matrix terms.
