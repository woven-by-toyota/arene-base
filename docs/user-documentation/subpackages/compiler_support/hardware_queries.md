<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compiler_support_hardware_queries Facilities For Querying Hardware Properties

<!-- markdownlint-enable MD041 -->

When writing certain types of low level functionality, information about
hardware specific properties such as the size of cache lines is needed in order
to produce optimal code. Support for these queries is provided via the following
header:

```cpp
#include "arene/base/compiler_support/hardware_queries.hpp"

```

## Provided Queries For Hardware Properties {#provided-queries-for-hardware-properties}

|                         Query                         |                                                                                                                                               Description                                                                                                                                                |
| :---------------------------------------------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| `arene::base::hardware_destructive_interference_size` | An approximate backport of [`std::hardware_destructive_interference_size`](https://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size). This defines the minimum memory spacing required between two items to prevent [false sharing](https://en.wikipedia.org/wiki/False_sharing). |
