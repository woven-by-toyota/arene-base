<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page units units: Facilities For Representing Values With Units

<!-- markdownlint-enable MD041 -->

Arene base provides facilities for working with values that represent quantities
with units.

The public header is:

```{.cpp}
#include "arene/base/units.hpp"
```

The Bazel target is

```text
//:units
```

# Introduction {#introduction}

The `units` sub-package provides facilities for working with values that
represent quantities with units.

## Quantities {#quantities}

At the basic level, the `arene::base::quantity` class template is intended to
represent an amount of a physical quantity, such as the distance in millimetres
to a nearby object, the road speed of a car in miles per hour, or the rotational
speed of a car engine in RPM. Quantities measuring different things cannot be
assigned to each other, and quantities representing the same thing but with
different units are automatically scaled during assignment, thus preventing a
category of bugs where incorrect calculation results are assigned to a variable,
or there is a mismatch in units between one module and another.

`arene::base::quantity` takes two template parameters.

The first template parameter is the **physical quantity type** of the value.
This specifies _what_ the value represents, and is a tag type such as
`wheel_rotation_speed_in_rpm`, or `vehicle_speed_in_mph`. This is used to ensure
that measurements of different things cannot accidentally be assigned to each
other, and measurements of the same thing in different units are appropriately
scaled when assigned. No instances of this tag type are ever instantiated.

The second template parameter is the **representation type**: the actual type
used to store the value. It defaults to `double`, but can be any arithmetic
type, including class types with arithmetic operations. This allows developers
to use `arene::base::quantity` as a direct replacement for any variable that
would otherwise have stored a physical quantity value, maintaining the same
storage type.

# Avoiding errors {#avoiding-errors}

The primary purpose of the `units` sub-package is to avoid errors when writing
code. The use of distinct tag types for distinct measurements, or for your
measurements in different units, avoids accidental errors. If _latitude_ and
_longitude_ are different types, then it is impossible to get them confused,
whereas if they are merely `double` values, then it is easy to pass them in the
wrong order to a function.

## Type aliases are error-prone {#type-aliases-are-error-prone}

Suppose we have a `geographic_position` which is made up of a _latitude_ and a
_longitude_, and a `make_position` function to construct one. Since the fields
have the same type, it is easy to get them in the wrong order: the compiler
won't complain if we construct the `geographic_position` as
`{longitude, latitude}` even though the fields are in the opposite order.

\snippet docs/examples/subpackages/units_examples.cpp latlong_alias

The same applies to function parameters: the compiler will merrily accept the
latitude and longitude values in the wrong order, even though we have carefully
checked the values, and used the correct type aliases.

\snippet docs/examples/subpackages/units_examples.cpp eiffel_tower_alias

## Quantity types prevent errors {#quantity-types-prevent-errors}

Using `arene::base::quantity` prevents these errors:

\snippet docs/examples/subpackages/units_examples.cpp latlong

By using `arene::base::quantity` with distinct _physical quantity types_ for the
fields, it is very hard to construct an instance with the wrong fields, as the
compiler will issue a compilation error.

Similarly, if we were to accidentally reverse the arguments to `make_position`
when constructing the location of the Eiffel Tower as below, then the
compilation would fail.

\snippet docs/examples/subpackages/units_examples.cpp eiffel_tower

# Conversions {#conversions}

By default, `arene::base::quantity` types with different _physical quantity
types_ are treated as entirely independent things. This is important: we don't
want `quantity<distance>` to be convertible to `quantity<time>`.

However, sometimes we _do_ want conversions: `quantity<time_in_hours>` should be
convertible to `quantity<time_in_seconds>`, with appropriate scaling of the
stored value (multiply by 3600).

This is achieved by specializing `arene::base::units_conversion_traits` for the
two types. Setting the `compatible` member to `true` indicates that the
quantities can be implicitly converted from the first _physical quantity type_
to the second. The `scale_factor` member then specifies the scaling to apply.

\snippet docs/examples/subpackages/units_examples.cpp time_conversions_tags
\snippet docs/examples/subpackages/units_examples.cpp time_conversions

Here, we specialize both directions because we want conversion to be permitted
in both directions. One hour is 3600 seconds, and 1 second is 1/3600 of an hour,
so the scale factors are set appropriately.

Implicit conversions then work as expected:

\snippet docs/examples/subpackages/units_examples.cpp time_conversions_code

The upcoming [Units Framework](#units-framework) will simplify the specification
of relationships between _physical quantity types_ so manual specialization of
`arene::base::units_conversion_traits` is not required.

# Comparisons {#comparisons}

Any two `arene::base::quantity` types can be compared for equality, ordering,
and three-way-comparison if they have the _same_ _physical quantity types_, or
one can be converted to the other.

\snippet docs/examples/subpackages/units_examples.cpp conversion_comparison

You cannot compare quantities with different _physical quantity types_ if they
are not interconvertible:

<!-- prettier-ignore -->
\snippet docs/examples/subpackages/units_examples.cpp conversion_comparison_error

# Arithmetic {#arithmetic}

By default, you can add and subtract two `arene::base::quantity` instances with
the same _physical quantity type_, yielding another quantity of the same type,
with the sum or difference of the values. However, you cannot add or subtract
quantities with different _physical quantity types_, since these are assumed to
be unrelated.

<!-- prettier-ignore -->
\snippet docs/examples/subpackages/units_examples.cpp time_addition_and_subtraction

By default, instances of `arene::base::quantity` can be multiplied or divided by
a scalar value, yielding a new `arene::base::quantity` holding the resulting
value, but `arene::base::quantity` values cannot be multiplied by other
`arene::base::quantity` values, even if they have the same _physical quantity
type_. This is because multiplying two **lengths** should yield an **area**, but
until the [Units Framework](#units-framework) is ready, the library cannot
determine the _physical quantity type_ of the result.

You can manually control the result of adding, subtracting, multiplying and
dividing quantities by specializing `arene::base::units_combination_traits`.

<!-- prettier-ignore -->
\snippet docs/examples/subpackages/units_examples.cpp multiplication_and_division_tags
\snippet docs/examples/subpackages/units_examples.cpp multiplication_and_division

The upcoming [Units Framework](#units-framework) will simplify the specification
of relationships between _physical quantity types_ so manual specialization of
`arene::base::units_combination_traits` is not required.

# Units Framework {#units-framework}

A **Units Framework** is under development, which will provide better ways of
declaring the relationship between _physical quantity types_, so that manual
specialization of `arene::base::units_conversion_traits` and
`arene::base::units_combination_traits` is unnecessary.
