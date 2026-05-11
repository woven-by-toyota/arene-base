<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page result result: A Return Value That Holds Either a Value, or an Error

<!-- markdownlint-enable MD041 -->

Facilities for handling a `result` type similar to `std::expected` from C++23,
or `Result` from Rust that encapsulates either a return value or an error code.

The public header is

```{.cpp}
#include "arene/base/result.hpp"
```

The Bazel target is

```text
//:result
```

## Introduction {#introduction}

A C++ function can only return a single return value. When designing APIs which
might fail to perform the desired task, this leads developers to generally use
one of a few workarounds:

- The function can throw an exception to signal failure.
- The function can produce a _status code_ for the caller to indicate if the
  operation was successful. This can be returned, or provided as an outparam.
- The function can reserve a _sentinel_ value in the domain of the return value,
  such as an iterator pointing to the end of a sequence or a null pointer.

All of these approaches have significant drawbacks:

- Exceptions:
  - are not part of the signature of the function, so there is no static
    analysis to validate an exception is handled. This often leads to exceptions
    being banned as _hidden control flow_ by many coding standards for safety
    critical systems.
  - are expensive: dynamic allocation and stack unwinding occurs, which can have
    significant performance implications when in the _hot path_ of execution.
  - It is unclear when it is correct to use exceptions when they are allowed.
    The core guidelines simply specify
    ["for exceptional situations"](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Re-errors),
    and leave it up to the reader to decide what that means. This makes it
    difficult to apply them consistently in an error model.
- Status Codes:
  - are easy to ignore, and frequently are.
  - _Success_ has to be part of the value space of the status code, which can
    lead to confusion.
  - No mater if the status code is the return or an outparam, composing APIs
    becomes awkward while handling errors.
  - When the status code is the return value, this actual value must be returned
    as an out parameter. This is unfortunately the most common pattern, but has
    several drawbacks:
    - It makes it difficult to write `const`-correct code, because out
      parameters str inherently non-`const`. This also means it cannot be used
      with a type which should otherwise be immutable once created.
    - If the type is non-regular or there is no reasonable default value, the
      outparam must be a pointer, and this risks dynamic allocation as well as
      lifetime issues.
  - When the status code is an out parameter, a major drawback is that the
    return value must still be provided. This is error prone at best, and
    impossible at worse if the return type is non-regular, has no reasonable
    default, or would risk undefined behaviors.
- Sentinel values:
  - are similar to status codes in that it is easy to forget to check against
    the sentinel value, and developers often do.
  - The value space of the return type must be able to accommodate the sentinel
    unambiguously. This is not always possible.

The `arene::base::result` type attempts to address these drawbacks by providing
a standard vocabulary type to return which unambiguously represents two possible
states: either the function successfully performed its action and returned a
value, or it failed and returned an error. The API design allows writing
natural, declarative code to check and handle errors, while allowing
straightforward composition and `const`-correctness. It is similar in concept to
Rust's [`Result`](https://doc.rust-lang.org/std/result/) or C++23's
[std::expected](https://en.cppreference.com/w/cpp/utility/expected).

## Basic Usage {#basic-usage}

The `arene::base::result<V,E>` type is always in one of two states:

- It contains a _value_ of type `V`.
- It contains an _error_ of type `E`.

The constraints on the types suitable for use as `V` and `E` are minimal: `E`
cannot be `void`, and nether `V` nor `E` can be references, c-v qualified, or
c-style arrays.

For example, the following might represent the result of opening a file

```cpp
enum class open_failure {
    path_does_not_exist,
    invalid_permissions,
    invalid_filetype
};

using open_result = result<non_null_ptr<std::FILE>, open_failure>;
```

And the open function would look like:

```cpp
open_result open(string_view path, string_view mode);
```

When the `open` API performs its task successfully, it populates the _value
channel_ of the result using the `arene::base::value_result` helper:

```cpp
return value_result(file_ptr);
```

When the `open` API is unable to perform its task successfully, it populates the
_error channel_ of the result using the `arene::base::error_result` helper:

```cpp
return error_result(open_failure::path_does_not_exist);
```

The `value_result` and `error_result` helpers are simple reference wrappers that
tell the `result` type itself which channel to populate. They also help make it
declarative to future maintainers of the code what channel was intended to be
populated at a given `return` location. They generally do not incur a
performance penalty, but values should be moved into them when they are
non-trivial.

The caller of `open` then can use the result of `open` as follows:

```cpp
const auto maybe_opened = open(path, options);
if(!maybe_opened.has_value()) { //  check if the error channel is populated.
    switch (maybe_opened.error()) { // and handle the error
        case open_failure::path_does_not_exist:
        ... // and so on
    }
}
const auto file_ptr = *maybe_opened; // the deref operator is shorthand for result::value()
... // and then do something with the file
```

Efficiently replacing the content of a `result` after it has been instantiated
can be accomplished via the `arene::base::result::emplace` API:

```cpp
struct point{
  int x;
  int y;
};

result<point, int> result = value_result(point{10, 11});
result.emplace(in_place_value, 15, -10);
EXPECT_EQ(result.value().y, -15);
result.emplace(in_place_error, 100);
EXPECT_TRUE(result.has_error());
EXPECT_EQ(result.error(), 100);
```

It is also possible to query for equality against the held value/error directly
through `arene::base::result::has_value(const value_type&)` and
`arene::base::result::has_error(const error_type&)`:

```cpp
result<int, int> some_result = value_result(10);
EXPECT_TRUE(some_result.has_value(10));
EXPECT_FALSE(some_result.has_error(10));
```

For full API details, please see the API reference.

## Access Safety {#access-safety}

`arene::base::result` is a _checked type_. This means that `ARENE_PRECONDITION`
is used on any access method which returns a reference, such as `result::value`
or `result::error`, to validate that the channel being accessed is actually
populated.

## Advanced Usage {#advanced-usage}

Beyond simple procedural access of the `error` and `value` channels, there are
advanced techniques which can be used in some cases to further reduce
boilerplate and write more declarative code.

### Monadic/Functional APIs {#monadicfunctional-apis}

#### and_then {#and_then}

`arene::base::result::and_then(F&& value_handler)` is a _monadic_ API which
consumes an _invocable_ with the signature `result<U, error_type>(value_type)`,
where `value_type` is compatible with the `const`/reference qualification of the
result it is invoked on, `error_type` is the error type of the result it is
invoked on and `U` is any valid value type (including `void`). If the `result`'s
value channel is populated, this handler is invoked and its result returned.
Otherwise, a `result<U, error_type>` created from the content of the error
channel is returned.

`and_then` is used to chain together result producing operations which share a
common error type. An example usage opening a file:

```cpp
// directory_handle::open() returns result<directory_handle, error_code>
result<file_handle, error_code> maybe_file = directory_handle::open(dir_path).and_then(
  [&filename](directory_handle dir) -> result<file_handle, error_code> {
    return value_result(dir.open_file(filename));
  }
);
```

After this statement, `maybe_file` will contain either a valid instance of
`arene::base::file_handle` in the value channel, or it will contain an instance
of `arene::base::error_code` representing either the failure to open the file or
the failure to open the directory.

@note Note the explicitly specified return type on the lambda in this example.
This is needed because `value_result` is not a `result`, it is _convertible_ to
a `result`. `and_then` requires the handler to return a `result`.

#### or_else {#or_else}

`arene::base::result::or_else(F&& error_handler)` is a _monadic_ API which
consumes an _invocable_ with the signature `result<value_type, U>(error_type)`,
where `error_type` is compatible with the `const`/reference qualification of the
result it is invoked on, `value_type` is the value type of the result it is
invoked on and `U` is any valid error type. If the `result`'s error channel is
populated, this handler is invoked and its result returned. Otherwise, a
`result<value_type, U>` created from the content of the value channel is
returned.

`or_else` is generally used to implement "fallback" logic for an operation,
where the fallback logic itself may fail or may have variant behavior depending
on how the non-default operation failed. For example, opening a configuration
file and falling back to some default configuration file if the given path
doesn't exist could look like:

```cpp
// directory_handle::open_file() returns result<file_handle, error_code>
auto maybe_configuration = dir.open_file(config_name, open_flags::read_only).or_else(
  [&dir, &default_config_name](error_code err) -> result<file_handle, error_code> {
    // if the error was "no file or directory" try to open the default
    if(err == ENOENT) {
      return dir.open_file(default_config_name, open_flags::read_only);
    }
    // otherwise, pass through the error
    return error_result(err);
  }
);
```

After this statement, `maybe_configuration` is
`result<file_handle, error_code>`, and will contain either an opened file handle
in the value channel produced from ether the requested or default configuration
names, or it will contain an `error_code` that represents the failure to open
either file.

@note Note the explicitly specified return type on the lambda in this example.
This is needed because `error_result` is not a `result`, it is _convertible_ to
a `result`. `or_else` requires the handler to return a `result`.

#### transform {#transform}

`arene::base::result::transform(F&& value_handler)` is a _functional_ API which
consumes an _invocable_ with the signature `U(value_type)`, where `value_type`
is compatible with the `const`/reference qualification of the result it is
invoked on and `U` is any valid value type (including `void`). If the `result`'s
value channel is populated, this handler is invoked and a
`result<U, error_type>` is created from the return, where `error_type` is the
error type of the result it is invoked on. Otherwise, a `result<U, error_type>`
created from the content of the error channel is returned.

`transform` is generally used to dispatch to functions which operate on the
value channel and cannot themselves fail. An example is printing the next entry
in an `arene::base::directory_handle`:

```cpp
// directory_handle::next_entry() returns result<null_terminated_string_view, error_code>
// print_entry has the signature void(null_terminated_string_view), as it cannot fail.
auto maybe_processed_entry = dir_handle.next_entry().transform(print_entry);
```

In this example, `maybe_processed_entry` will be
`result<void, arene::base::error_code>`. If `next_entry()`'s return had its
value channel populated, `print_entry` is called and `maybe_processed_entry`
will have its value channel populated. Otherwise `maybe_processed_entry` will
contain the `error_code` from `next_entry()`.

#### transform_error {#transform_error}

`arene::base::result::transform_error(F&& value_handler)` is a _functional_ API
which consumes a _invocable_ with the signature `U(error_type)`, where
`error_type` is cref compatible with the cref qualification of the result it is
invoked on and `U` is any valid error type. If the `result`'s error channel is
populated, this handler is invoked and a `result<value_type, U>` is created from
the return, where `value_type` is the value type of the result it is invoked on.
Otherwise, a `result<value_type, U>` created from the content of the value
channel is returned.

`transform_error` is generally used for situations where some unconditional
processing of the error type returned by an API is needed, and that processing
cannot fail. For example, interposing logging the error returned by attempting
to open a file might look like:

```cpp
// directory_handle::open_file() returns result<file_handle, error_code>
auto maybe_file = dir_handle.open_file(file_name).transform_error(
  [&logger](error_code err) {
    logger.log_error(err);
  }
  return err; // we pass the error value through so programmatic action can be taken.
);
```

### `expand_result` {#expand_result}

The `arene::base::expand_result` API can be used to access the content of an
`arene::base::result` in a similar manner to accessing a `std::variant` using a
visitor. The example from [basic usage](#basic-usage) rewritten to use
`expand_result` would be implemented as follows:

```cpp
expand_result(open(path, options),
  [](non_null_ptr<std::FILE> file_ptr) {
    // do something with the file
  }
  [](open_failure error) {
    switch (error) {
    case open_failure::path_does_not_exist:
        ... // and so on
    }
  }
)
```
