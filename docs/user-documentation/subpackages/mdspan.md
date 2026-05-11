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

## Introduction {#introduction}

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

### Extensions {#extensions}

This package provides the following type traits as extensions on the C++23
facilities.

- `arene::base::is_accessor_policy_v`
- `arene::base::is_layout_mapping_v`
- `arene::base::is_layout_mapping_policy_v`
