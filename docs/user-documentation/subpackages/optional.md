<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page optional optional: A Nullable Value Type

<!-- markdownlint-enable MD041 -->

`arene::base::optional` is a backport of
[`std::optional`](https://en.cppreference.com/w/cpp/utility/optional), with
additional checking to prevent undefined behaviour.

The public header is

```cpp
#include "arene/base/optional.hpp"
```

The Bazel target is

```text
//:optional
```

## Introduction {#introduction}

`arene::base::optional` is either _engaged_, with a stored value, or
_disengaged_, without a stored value. A _disengaged_ optional does not contain
any object of the specified type at all, whereas an _engaged_ optional can store
any value of the specified type. This means that code can indicate "there isn't
a value here", without requiring special values like empty strings, null
pointers or zero to represent the "no data" case, thus allowing the full range
of values of the contained type to be stored.

`arene::base::optional` is a backport of
[`std::optional`](https://en.cppreference.com/w/cpp/utility/optional).

### Checked operations {#checked-operations}

All accesses to the stored value of an `arene::base::optional` are checked;
attempting to access the stored value of a _disengaged_ optional is thus a
[precondition violation](\ref contracts).

## Basic Usage {#basic-usage}

### Initialization And Value Access {#initialization-and-value-access}

The following snippet shows off the basic initialization and value access
operations:

\snippet docs/examples/subpackages/optional.cpp init_access

### Disengaging Optionals

The following snippet demonstrates several ways to disengage an optional:

\snippet docs/examples/subpackages/optional.cpp disengage

### Moving Optionals {#moving-optionals}

From the point of view of "move" operations, an _engaged_
`arene::base::optional<T>` is just like a `T`: after moving, the
`arene::base::optional<T>` is still _engaged_, but the stored `T` might have a
"moved-from" state. This means that as with `T`, the only valid thing to do with
a moved-from `arene::base::optional` is to re-assign it or allow it to be
destroyed.

In addition, dereferencing an rvalue optional yields an rvalue reference to the
stored value, so that `*std::move(source)` and `std::move(*source)` are
equivalent.

### Optional As A Return Type {#optional-as-a-return-type}

`arene::base::optional` can be used as the return type of a function, where the
function can return a disengaged optional where there is no value to return:

\snippet docs/examples/subpackages/optional.cpp as_return

### Optional As A Parameter {#optional-as-a-parameter}

`arene::base::optional` can be used as a parameter type to make it clear that a
parameter is not being provided. This avoids the need to define a _sentinel
value_ which represents "not provided," avoiding potential ambiguity. For
example, assume we have a function which wants to execute some functionality,
with an optional timeout value in milliseconds. If `std::chrono::milliseconds`
is used directly, a value of `0` could mean "timeout instantly" or it could mean
"never timeout." Worse, different functions might use different conventions,
increasing the probability that a defect is introduced when the caller
incorrectly uses the `0` value. Using
`arene::base::optional<std::chrono::milliseconds>` avoids this ambiguity, as
demonstrated in the following example:

\snippet docs/examples/subpackages/optional.cpp as_param

A few best practices around consuming optional parameters to note:

- An optional as a parameter should almost always be defaulted to
  `arene::base::nullopt`, as this makes it clear the parameter is truely
  _optional_ and allows the caller to omit it.
- A reference to an optional as a parameter is generally code smell; it requires
  the caller to already have an optional in order to call the function, and
  implies a tight coupling between the caller and callee. If the intent is to
  provide an "optional reference" in order to avoid copying the held value,
  prefer consuming `arene::base::non_owning_ptr<T>` instead, and defaulting it
  to `nullptr`. This represents the same thing semantically, and may be created
  from any instance of `T`.

### Optional As A Data Member {#optional-as-a-data-member}

Another use of `arene::base::optional` is as a data member in configuration
structures; the effect is the same as for a
[parameter](#optional-as-a-parameter), which is to clearly differentiate "the
value was not provided" from "the value is some default value." Continuing the
timeout example:

\snippet docs/examples/subpackages/optional.cpp as_config

A less common usage of data member optionals is to differ initialization of a
value which cannot be provided at class construction time, while avoiding
dynamic allocation:

\snippet docs/examples/subpackages/optional.cpp as_differed

A more advanced example shows leveraging the full lifetime management
capabilities of `arene::base::optional` to avoid waisting resources constructing
the extended state information until it is actually needed, and freeing those
resources when they are no longer needed:

\snippet docs/examples/subpackages/optional.cpp as_lifetime_management

## Advanced Usage {#advanced-usage}

Beyond simple procedural access of the `value` held by the optional, there are
advanced techniques which can be used in some cases to further reduce
boilerplate and write more declarative code.

### Accessing The Value With A Default Fallback {#accessing-the-value-with-a-default-fallback}

There are situations where a default value should be used if an optional is
empty. There are two APIs to help with this situation.

#### value_or {#value_or}

`arene::base::optional<T>::value_or(U&& default_value)` allows accessing the
value with a fallback to a default value if the optional is null. For example:

\snippet docs/examples/subpackages/optional.cpp value_or

#### value_or_else {#value_or_else}

`arene::base::optional<T>::value_or_else(F&& default_generator)` is similar to
`value_or`. However instead of consuming a value, it consumes an _invocable_
which can be invoked with no arguments to generate the default value. If the
optional is not _engaged_, this default generator is called and its result
returned, otherwise a copy/move-constructed instance of the held value is
returned. For example:

\snippet docs/examples/subpackages/optional.cpp value_or_else

### Monadic/Functional APIs {#monadicfunctional-apis}

#### and_then {#and_then}

`arene::base::optional<T>::and_then(F&& value_handler)` is a _monadic_ API which
consumes an _invocable_ that can be invoked with a value of type `T` and returns
an `optional<U>`. The value passed to the handler will have the
`const`/reference qualification of the `optional` that `and_then` is called on.
If the `optional` `and_then` is called on is engaged, the handler is invoked and
its result returned. Otherwise, a null `optional<U>` is returned.

`and_then` is generally used to chain together `optional` producing operations.
For example, a simple number processing pipeline might look like:

\snippet docs/examples/subpackages/optional.cpp and_then

#### or_else {#or_else}

`arene::base::optional<T>::or_else(F&& null_handler)` is a _monadic_ API which
consumes an _invocable_ that can be invoked with no arguments and returns
`optional<T>`. If the `optional` that `or_else` is called on is not engaged, the
handler is invoked and its result returned. Otherwise a copied/moved instance of
the original optional is returned.

`or_else` is generally used to implement "fallback" logic for an operation,
where the fallback logic itself may fail. For example:

\snippet docs/examples/subpackages/optional.cpp or_else

For situations where the fallback logic cannot fail,
[`arene::base::optional<T>::value_or_else()`](#value_or_else) may be a better
choice as it returns a value rather than an optional.

#### transform {#transform}

`arene::base::optional<T>::transform(F&& value_handler)` is a _functional_ API
which consumes an _invocable_ that can be invoked with a value of type `T` and
returns `U`. The value passed to the handler will have the `const`/reference
qualification of the `optional` that `and_then` is called on. If the `optional`
`transform` is called on is engaged, the handler is invoked and an `optional<U>`
is returned which is constructed from its result. Otherwise, a null
`optional<U>` is returned.

`transform` is generally used to dispatch to functions which operate on the
value and cannot themselves fail. For example:

\snippet docs/examples/subpackages/optional.cpp transform
