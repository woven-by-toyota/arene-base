<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page inline_container inline_container: Non-Allocating, Static Capacity Containers

<!-- markdownlint-enable MD041 -->

Arene Base provides a number of "inline" containers: containers where the
storage for the elements is part of the container object itself rather than
allocated separately. The capacity of these containers is specified at compile
time, and cannot be extended at runtime. This allows them to be used in
scenarios where dynamic memory allocation is not desired, but a container API
for storing a variable number of objects is required.

The Bazel target is

```text
//:inline_container
```

## Inline containers vs dynamic memory containers {#inline-containers-vs-dynamic-memory-containers}

The inline containers are provided as a direct alternative to equivalent
containers that allocate dynamic memory allocation. It is important to know the
trade-offs when choosing one over the other.

### Upsides of in-object storage {#upsides-of-in-object-storage}

The foremost upside is the lack of dynamic memory allocation. This enables the
inline containers to be used where dynamically-allocating containers would not
be permitted due to safety-related coding standards.

Dynamic memory allocation is also often expensive, so the performance of adding
elements to the container can be better than that for the equivalent
dynamically-sized container.

Finally, because the data is directly part of the object, rather than stored on
the heap, access to the stored data can be faster due to better cache locality,
and the lack of indirection.

For small capacities, inline containers can have better performance all round
than the equivalent containers that use dynamic memory allocation, but it is
important to be aware of the potential downsides. **If you are concerned about
performance, profile your code**.

### Downsides of in-object storage {#downsides-of-in-object-storage}

The downside is that the maximum capacity of the containers is fixed at compile
time, and the objects themselves can become quite large. If used as local
variables, function parameters or function return values without thought, they
can consequently lead to large stack consumption. Also, since the values are
contained within the object, if the object is copied or moved, then all the
elements must also be copied or moved. This is in direct contrast to the
equivalent dynamically-allocating containers, which are usually cheap to move.

Instances of inline containers with large capacities should therefore usually be
allocated in static storage, or in storage that is allocated at startup. Where
the object cannot be constructed at startup, but static storage is desired,
`arene::base::optional` can be used to set aside space for the object for later
construction using `emplace`.

### Containers with external fixed-capacity storage {#containers-with-external-fixed-capacity-storage}

For some use cases, containers that use user-provided external storage can
provide a useful alternative to both containers with in-object storage and
containers with dynamically-allocated storage.

Rather than the storage being managed directly by the container, the storage is
provided by the user as a constructor parameter. This allows the user to be in
full control of how and when the storage is allocated, but also requires the
user to be responsible for ensuring that the lifetime of the storage exceeds
that of the container.

The use of external storage also allows the user to specify the capacity at
runtime, since the capacity is determined from the size of the provided storage.

### Exceeding capacity {#exceeding-capacity}

Any operation that would insert elements beyond the specified capacity of the
containers will terminate the program using `ARENE_PRECONDITION`. User code is
responsible for checking the capacity before attempting to insert elements.

Note that this is different from the behaviour of the containers' Standard
Library counterparts, which would attempt to expand the capacity using dynamic
memory allocation, and throw `std::length_error` or `std::bad_alloc` if that
failed.

## Provided containers {#provided-containers}

Arene base provides the following _inline_ containers:

- `arene::base::inline_vector` --- a sequence container equivalent to
  `std::vector`
- `arene::base::inline_deque` --- a sequence container with constant time
  insertion and removal at opposite ends
- `arene::base::inline_map` --- an ordered associative container equivalent to
  `std::map`
- `arene::base::inline_set` --- an ordered associative container equivalent to
  `std::set`

The following type also provides "inline" storage:

- `arene::base::inline_function` --- a type for storing a function or callable
  object, equivalent to `std::function`

Code which uses these containers will likely also be interested in
[`inline_string`](\ref inline_string), an inline equivalent to `std::string`
which works similarly to `arene::base::inline_vector` but is provided from its
own subpackage.

Arene base also provides the following container with _external storage_:

- `arene::base::external_vector` --- a sequence container equivalent to
  `std::vector`. Aside from the constructors (which require the storage in
  addition to any other constructor parameters), the interface is the same as
  that of `arene::base::inline_vector`.

Arene base also provides "reference" types which can be used as function
parameters to "reference" these containers without the capacity being encoded in
the function signature:

- `arene::base::vector_reference` and `arene::base::const_vector_reference` can
  be used to refer to any instantiation of `arene::base::inline_vector` or
  `arene::base::external_vector` with the same element type, independent of
  capacity.
- `arene::base::inline_map_reference` and
  `arene::base::const_inline_map_reference` can be used to refer to any
  instantiation of `arene::base::inline_map` with the same key type, mapped
  type, and comparator, independent of capacity.
- `arene::base::inline_string_reference` and
  `arene::base::const_inline_string_reference` can be used to refer to any
  instantiation of `arene::base::inline_string`, independent of capacity. See
  the documentation from \ref inline_string for details.

These should be used when you need the full container functionality, but want to
be independent of the capacity. If you just need a view over the elements,
`arene::base::span` or `arene::base::string_view` might be more appropriate.

### Inline vector {#inline-vector}

The API for `arene::base::inline_vector` is almost identical to that of
`std::vector`. The key difference is that whereas `std::vector` takes an
optional allocator as the second template argument, `arene::base::inline_vector`
requires the capacity to be specified instead. Thus
`arene::base::inline_vector<int, 5>` is a vector holding at most 5 `int` values.
This means that vectors with different capacities have different types.

The fixed capacity means that if elements are only ever inserted or erased at
the end, then none of the existing elements are moved. Inserting or erasing in
the middle of the vector has the same behaviour as `std::vector`. Also, since
the capacity is fixed, there is no `reserve` member function.

#### Constant expressions {#constant-expressions}

All member functions are declared `constexpr`, so for element types that are
usable in a constant expression, `arene::base::inline_vector` can also be used
in a constant expression.

#### Example usage {#example-usage}

\snippet docs/examples/inline_container_examples.cpp vector_usage_example

### External vector {#external-vector}

Whereas `arene::base:::inline_vector` holds the storage _inside_ the object,
`arene::base::external_vector` holds the storage _outside_ the object. The user
is required to supply a storage buffer to the constructor, which is then used as
backing storage for the objects stored in the vector.

\note The user is responsible for the lifetime of the storage, and must ensure
that it outlives the vector that uses it.

The API of `arene::base::external_vector` is almost identical to that of
`arene::base::inline_vector`, apart from the additional `storage` parameter to
the constructors.

#### Capacity fixed at construction time {#capacity-fixed-at-construction-time}

Whereas `arene::base::inline_vector` has a fixed capacity, specified at compile
time, `arene::base::external_vector` has a capacity determined at runtime from
the supplied storage: if the storage has enough space for 10 elements, then the
capacity is 10; if the storage has enough space for 300000 elements, then the
capacity is 300000.

\note Though the size of the storage can vary, it must be correctly aligned for
vector element type, and must have a size that is an exact multiple of the size
of the element. This is expected to be done using the `alignas` keyword and
`sizeof` operator, as in the example below.

#### Example usage {#example-usage}

<!-- prettier-ignore -->
\snippet docs/examples/inline_container_examples.cpp external_vector_usage_example

### Inline deque {#inline-deque}

The API for `arene::base::inline_deque` consists of inserting elements at one of
its ends with `push_front` or `push_back` and accessing as well as removing
elements at its other end with `back`, `front`, `pop_front`, and `pop_back`.

As with `arene::base::inline_vector`, in addition to the template parameter for
the element type, `arene::base::inline_deque` requires a second parameter for
its capacity. The inline storage of fixed capacity is used efficiently, so that
when slots that were occupied by elements inserted via `push_front` become free
via `pop_back`, upcoming elements may occupy those slots again (it is a circular
or ring buffer).

It is also possible to insert new elements at places other than the back using
the `insert` function, although elements can only be _removed_ using
`pop_front()` or by adding new elements when the queue is already full (which
effectively uses `pop_front()` to make room by removing the frontmost elements).
However, this can quickly become inefficient: pushing an element at the front or
back is an `O(1)` operation, while inserting `x` places from the end requires
`O(x)` moves.

Even though `arene::base::inline_deque`'s API is covered by combining
`push_back` and `erase` from the beginning (or `insert` at the beginning and
`pop_back`) in `arene::base::inline_vector`, such usage involves shifting the
existing elements in the storage, which scales linearly with the number of
elements stored. On the other hand, insertion at the beginning or end of
`arene::base::inline_deque` is constant time. An `arene::base::inline_vector`
can be used as a constant time LIFO data structure (a stack) and
`arene::base::inline_deque` is a constant time FIFO data structure (a queue).

#### Constant expressions {#constant-expressions}

Since all member functions are `constexpr`, `arene::base::inline_deque` can be
used in constant expressions, provided that the element type can also be used in
a constant expression.

#### Example usage {#example-usage}

<!-- prettier-ignore -->
\snippet docs/examples/inline_container_examples.cpp deque_usage_example

### Circular buffer {#circular-buffer}

`arene::base::circular_buffer` is a variation on `arene::base::inline_deque`,
offering the same constant-time insertion and removal at the ends. However,
whereas adding an element to a full `arene::base::inline_deque` is a
precondition violation, adding an element to a full
`arene::base::circular_buffer` causes the element at the front to be removed to
make room for the new element.

This is a convenient interface for ring buffers that want to keep track of the
most recent `N` objects of some kind, such as messages or frames of animation.
To support this use case, `arene::base::circular_buffer` is one-sided in that it
removes the `push_front`, `emplace_front`, and `pop_back` member functions.
Elements in the middle can still be inspected or even replaced using
`operator[]` or the `begin()` and `end()` iterators if necessary, although users
should take care to leave the contents of the buffer in a sensible state when
doing this.

### Inline map {#inline-map}

The API for `arene::base::inline_map` is almost identical to that of `std::map`.
The key difference is that whereas `std::map` takes an optional allocator as the
final template argument, `arene::base::inline_map` requires the capacity to be
specified as the third template argument instead. Thus
`arene::base::inline_map<std::uint32_t, my_struct, 42>` is a map with a key type
of `std::uint32_t`, a mapped type of `my_struct`, and at most 42 elements.

#### Comparisons {#comparisons}

The comparator type, which is now the fourth rather than third template
parameter, is also different to that of `std::map`, in that it is intended to
support \ref three-way-compare "three-way comparisons". If the result of
`compare(first_key, second_key)` is an instance of
`arene::base::strong_ordering` rather than `bool`, then the comparator is
assumed to be a three-way compare function rather than an direct ordering
relation such as `std::less<>`. The default comparison is
`arene::base::compare_three_way`, which uses the provided three-way comparison
function for the specified key type, and defaults to `operator<` otherwise.

#### Operation complexity {#operation-complexity}

Each individual entry in the map is stored in a fixed position, and never moves
from when it is inserted until the map is destroyed or the element is removed.
In order to maintain the sorted order, the map also stores a separate array of
indices into the element storage. Inserting and removing elements thus performs
_O(n)_ integer moves to update the index, in addition to the _O(log n)_
comparisons to find the appropriate entry.

#### Iterator validity {#iterator-validity}

Unlike `std::map`, iterators do not remain valid across inserts and removals.

#### Example usage {#example-usage}

\snippet docs/examples/inline_container_examples.cpp map_usage_example

### Inline set {#inline-set}

The API for `arene::base::inline_set` is almost identical to that of `std::set`.
The key difference is that whereas `std::set` takes an optional allocator as the
final template argument, `arene::base::inline_set` requires the capacity to be
specified as the second template argument instead. Thus
`arene::base::inline_set<my_struct, 42>` is a set of `my_struct` elements, with
at most 42 entries.

#### Comparisons {#comparisons}

The comparator type, which is now the third rather than second template
parameter, is also different to that of `std::set`, in that it is intended to
support \ref three-way-compare "three-way comparisons". If the result of
`compare(first_key, second_key)` is an instance of
`arene::base::strong_ordering` rather than `bool`, then the comparator is
assumed to be a three-way compare function rather than an direct ordering
relation such as `std::less<>`. The default comparison is
`arene::base::compare_three_way`, which uses the provided three-way comparison
function for the specified element type, and defaults to `operator<` otherwise.

#### Operation complexity {#operation-complexity}

Each individual entry in the set is stored in a fixed position, and never moves
from when it is inserted until the set is destroyed or the element is removed.
In order to maintain the sorted order, the set also stores a separate array of
indices into the element storage. Inserting and removing elements thus performs
_O(n)_ integer moves to update the index, in addition to the _O(log n)_
comparisons to find the appropriate entry.

#### Iterator validity {#iterator-validity}

Unlike `std::set`, iterators do not remain valid across inserts and removals.

#### Example usage {#example-usage}

\snippet docs/examples/inline_container_examples.cpp set_usage_example

### Inline function {#inline-function}

`arene::base::inline_function` has a similar API to `std::function`.

The `const` qualification of the function call operator matches the supplied
function signature. Thus `arene::base::inline_function<void(int)>` has a
non-`const` function call operator, whereas
`arene::base::inline_function<void(int) const>` has a `const` function call
operator. This allows invoking function objects that themselves only provide a
`const` or non-`const` function call operator as applicable.

This is different to `std::function`, where `std::function<void(int)>` invokes
the wrapped callable using the non-`const` function call operator if available,
even for `const` instances of `std::function`, and
`std::function<void(int) const>` does not compile. The behaviour of
`inline_function` matches the behaviour of
[`std::move_only_function`](https://en.cppreference.com/w/cpp/utility/functional/move_only_function/move_only_function)
and the proposed [`std::copyable_function`](https://wg21.link/p2548).

#### Storage capacity {#storage-capacity}

The second template parameter specifies the maximum allowed size of the stored
callable. All content related to the callable, including any captured state in a
lambda or members in a user defined callable object, must fit into this storage
capacity; **attempting to store a callable that is too large results in a
compile time error**. This is a significant restriction on API boundaries
relative to `std::function`, which can hold arbitrary sized callable objects
using dynamic memory allocation. If ownership over the lifetime of the provided
callable is not required, `arene::base::function_ref` is a better choice as it
has the same zero-allocation properties without restricting the size of the
input callable.

The default capacity is the size of 4 pointers. This is enough to store any
stateless function objects, a function pointer, a member function pointer, or a
function object with a small amount of internal state such as a lambda with only
a small number of captures.

#### Example usage {#example-usage}

\snippet docs/examples/inline_container_examples.cpp function_usage_example

### Storage-independent reference wrappers {#storage-independent-reference-wrappers}

#### Storage-independent vector references {#storage-independent-vector-references}

`arene::base::vector_reference` and `arene::base::const_vector_reference`
wrapper classes are provided to pass the inline vector and external vector
objects to functions not templated on the storage location or capacity of the
vector.

These size-erased reference wrappers can be created either by explicit
construction or by use of helpers `arene::base::make_vector_reference` and
`arene::base::make_const_vector_reference`.

\snippet docs/examples/inline_container_examples.cpp vector_ref_usage_example

#### Storage-independent map references {#storage-independent-map-references}

`arene::base::inline_map_reference` and
`arene::base::const_inline_map_reference` wrapper classes are provided to pass
inline map objects to functions not templated on the storage capacity of the
map.

These size-erased reference wrappers can be created by explicitly specifying the
reference type.

\snippet docs/examples/inline_container_examples.cpp map_ref_usage_example

## Testing with custom types {#testing-with-custom-types}

The unit test suites of `inline_vector` and `external_vector` are type
parameterized so that user applications can test the classes with their own
types and verify that the behaviour is correct and the class has full test
coverage for that type in particular. **This is currently only supported for
types with move construction, move assignment, and an equality operator
defined.** On GCC8 (and therefore QCC), the type may also need an input stream
`operator>>` but this does not seem to be the case on other compilers or newer
versions of GCC. For types that are default constructible, two default
constructed instances must compare equal.

### Testing inline vector {#testing-inline-vector}

In a test file in your project, set `//:inline_container_testing` as a Bazel
dependency and then include `arene/base/inline_container/testing/vector.hpp`.
Then, instantiate the tests using the `ARENE_INSTANTIATE_TESTS` macro with the
`InlineVector` suite name; this has the same syntax as the Google Test macro
`INSTANTIATE_TYPED_TEST_SUITE_P` but without the undocumented fourth parameter:

\snippet docs/examples/inline_vector_test_examples.cpp vector_testing_example

In the example above, the tests are instantiated for `inline_vector`s with
values of the builtin types `std::int32_t`, `double`, and `std::string` as well
as the custom type `my_class`. This will not quite compile yet, however, because
Arene Base does not yet know which values of these classes to use for the tests
(or in the case of `my_class`, even how to instantiate it at all). For built-in
arithmetic types, the parameterized tests work out of the box, but for anything
else, test values must be provided via the following method:

<!-- prettier-ignore -->
\snippet docs/examples/inline_vector_test_examples.cpp vector_test_values_example

The parameterized tests rely on the `test_value` function template to provide a
test value to individual test cases. The default implementation of `test_value`
relies on copy constructing a new value from the `test_value_array`, which will
not work if the type does not support copy construction. In this case, you can
tell Arene Base an alternative method of constructing your type:

<!-- prettier-ignore -->
\snippet docs/examples/inline_vector_test_examples.cpp non_copy_constructible_test_example

Compiling and running the resulting executable will produce a GoogleTest test
report describing the results of the `inline_vector` unit tests with your types.

### Testing external vector {#testing-external-vector}

Testing `external_vector` with your chosen types is similar to testing
`inline_vector`, but the test suite is different.

In a test file in your project, set `//:inline_container_testing` as a Bazel
dependency and then include
`arene/base/inline_container/testing/external_vector.hpp`. Then, instantiate the
tests using the `ARENE_INSTANTIATE_TESTS` macro with the `ExternalVector` suite
name:

<!-- prettier-ignore -->
\snippet docs/examples/external_vector_test_examples.cpp external_vector_testing_example
