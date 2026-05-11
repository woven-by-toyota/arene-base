<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page integer_sequences integer_sequences: Facilities For Manipulating std::integer_sequence

<!-- markdownlint-enable MD041 -->

The `integer_sequences` sub-package provides a set of facilities for creating
and manipulating sequences of integers. Mostly, these are represented using
`std::integer_sequence`, for manipulation during compile time.

The public header is

```{.cpp}
#include "arene/base/integer_sequences.hpp"
```

The Bazel target is

```text
//:integer_sequences
```

## Introduction {#introduction}

[`std::integer_sequence`](https://en.cppreference.com/w/cpp/utility/integer_sequence)
is great for holding a list of integers, but it can be difficult to manipulate.
The `integer_sequences` package provides a set of facilities to make such
manipulation easier:

- Concatenating two sequences into one with `arene::base::integer_sequence_cat`
- Getting the n-th element of a sequence with
  `arene::base::integer_sequence_element` and
  `arene::base::integer_sequence_element_v`
- Checking if a sequence contains an element, with
  `arene::base::integer_sequence_contains_v`
- Finding the index of an element in a sequence, with
  `arene::base::integer_sequence_index_of`
  and`arene::base::integer_sequence_index_of_v`
- Counting the number of occurrences of an element in a sequence with
  `arene::base::integer_sequence_count_of` and
  `arene::base::integer_sequence_count_of_v`
- Removing duplicate elements from a sequence, with
  `arene::base::integer_sequence_unique_elements`
- Building an `std::index_sequence` by repeating a single value a given number
  of times, with `arene::base::make_index_sequence_repeat_n`

Arene Base also provides `arene::base::sequential_values`, which is a variable
template for an `arene::base::array` holding the specified number of objects,
starting from 0.

### Concatenating Sequences {#concatenating-sequences}

`arene::base::integer_sequence_cat` is a template alias for a sequence that is
the concatenation of the supplied sequences. All sequences must have the same
element type.

```{.cpp}
using sequence1 = std::integer_sequence<std::int32_t, 10, 20, 30, 40>;
using sequence2 = std::integer_sequence<std::int32_t, 99, -123, -456>;
using sequence3 = std::integer_sequence<std::int32_t, 0, 3, 42>;

using result = arene::base::integer_sequence_cat<sequence1, sequence2, sequence3>;
```

Here, the resulting sequence is
`std::integer_sequence<std::int32_t, 10, 20, 30, 40, 99, -123, -456, 0, 3, 42>`

### Getting A Specific Element {#getting-a-specific-element}

`arene::base::integer_sequence_element<Index, Sequence>` derives from
`std::integral_constant<T, Value>`, where `T` is the element type of the
sequence, and `Value` is the `Index`-th element. Thus the value can be retrieved
using the `value` member, or the function call operator.

On the other hand, `arene::base::integer_sequence_element_v<Index, Sequence>` is
a variable template that holds the value of the `Index`-th element of `Sequence`
directly.

```{.cpp}
using values = std::integer_sequence<std::int32_t, 10, 20, 30, 40, 99, -123, -456, 0, 3, 42>;

static_assert(arene::base::integer_sequence_element<5, values>::value == -123, "got the correct element");
static_assert(arene::base::integer_sequence_element_v<6, values> == -456, "got the correct element");
```

Both these templates are undefined (and will thus fail to compile) if the index
is greater than or equal to the number of elements in the sequence.

### Checking If An Element Is Present In A Sequence {#checking-if-an-element-is-present-in-a-sequence}

This falls to `arene::base::integer_sequence_contains_v`, which is a boolean
variable template that is `true` if the element is in the sequence, and `false`
otherwise.

```{.cpp}
using values = std::integer_sequence<std::int32_t, 10, 20, 30, 40, 99, -123, -456, 0, 3, 42>;

static_assert(arene::base::integer_sequence_contains_v<values, 3>,"Value is present");
static_assert(!arene::base::integer_sequence_contains_v<values, 500>,"Value is not present");
```

### Finding The Index Of An Element In A Sequence {#finding-the-index-of-an-element-in-a-sequence}

Knowing that an element is present in a sequence is not always enough: sometimes
you need to know the exact index where it occurs.
`arene::base::integer_sequence_index_of<Sequence, Value>` is a `struct` that
derives from `std::integral_constant<std::size_t, Index>`, where `Index` is the
index of `Value` in `Sequence`;
`arene::base::integer_sequence_index_of_v<Sequence, Value>` is a variable
template that holds the index value directly.

Both these templates are undefined, and will cause a compilation error, if the
element is not in the sequence.

```{.cpp}
using values = std::integer_sequence<std::int32_t, 10, 20, 30, 40, 99, -123, -456, 0, 3, 42>;

static_assert(arene::base::integer_sequence_index_of<values, 3>::value == 8,"Value is at index 8");
static_assert(arene::base::integer_sequence_index_of_v<values, 40> == 3,"Value is at index 3");

// const auto val = arene::base::integer_sequence_index_of_v<values, 500>; // compilation error
```

### Counting Copies Of An Element In A Sequence {#counting-copies-of-an-element-in-a-sequence}

`arene::base::integer_sequence_count_of<Sequence, Value>` and
`arene::base::integer_sequence_count_of_v<Sequence, Value>` answer the perennial
question _how many times does `Value` appear in `Sequence`?_
`arene::base::integer_sequence_count_of<Sequence, Value>` is a `struct` that
derives from `std::integral_constant<std::size_t, Count>`, where `Count` is the
number of times `Value` appears in `Sequence`;
`arene::base::integer_sequence_count_of_v<Sequence, Value>` is a variable
template that holds the count value directly.

If `Value` does not appear in `Sequence`, then the value is just zero.

```{.cpp}
using values = std::integer_sequence<std::uint16_t, 1, 1, 1, 2, 2, 1, 3, 5, 100, 99, 1>;

static_assert(arene::base::integer_sequence_count_of<values, 1>::value == 5,"1 appears 5 times");
static_assert(arene::base::integer_sequence_count_of_v<values, 40> == 0,"40 appears 0 times");
static_assert(arene::base::integer_sequence_count_of_v<values, 5> == 1,"5 appears once");
```

### Removing Duplicates {#removing-duplicates}

If you want to ensure that
`arene::base::integer_sequence_count_of_v<Sequence, Value>` is either 0 or 1 for
some `Sequence`, whatever `Value` is passed, then
`arene::base::integer_sequence_unique_elements<Sequence>` is what you need. It
is a type alias for a new `std::integer_sequence` that removes any duplicated
elements from the original `Sequence`, so there is one copy of each value. In
each case, it is the _first_ element of each value that is preserved.

```{.cpp}
using values = std::integer_sequence<std::uint16_t, 1, 1, 1, 2, 2, 1, 3, 5, 100, 99, 1>;
using unique_values = arene::base::integer_sequence_unique_values<values>;

static_assert(std::is_same<unique_values, std::integer_sequence<std::uint16_t, 1, 2, 3, 5, 100, 99>>::value, "1 copy of each value");

static_assert(arene::base::integer_sequence_count_of<unique_values, 1>::value == 1,"1 appears once");
static_assert(arene::base::integer_sequence_count_of_v<unique_values, 40> == 0,"40 appears 0 times");
```

### Repeating A Value {#repeating-a-value}

`arene::base::make_index_sequence_repeat_n<Value, Count>` is a template alias
for an `std::index_sequence` whose elements are `Count` copies of `Value`.

```{.cpp}
using repeated = arene::base::make_index_sequence_repeat_n<3, 4>;

static_assert(std::is_same<repeated, std::index_sequence<3, 3, 3, 3>>::value, "4 copies of 3");
```

When `Count` is `0`, the resulting sequence is empty:

```{.cpp}
using empty = arene::base::make_index_sequence_repeat_n<7, 0>;

static_assert(std::is_same<empty, std::index_sequence<>>::value, "no copies yields empty sequence");
```

### Sequential Values {#sequential-values}

`arene::base::sequential_values<Type, Count>` is a variable template, which is
an `arene::base::array<Type, Count>` holding the values 0 to `Count - 1`. Thus
`arene::base::sequential_values<std::int16_t, 10>` is an array of 10
`std::int16_t` objects with the values 0, 1, 2, 3, 4, 5, 6, 7, 8, and 9.

This can be used to initialize other containers:

```{.cpp}
std::vector<std::uint8_t> vec(arene::base::sequential_values<std::uint8_t,25>);
```

This will initialize the vector with the values 0 to 24.
