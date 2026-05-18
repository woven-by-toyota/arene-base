<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page contracts contracts: Precondition and Invariant Checking

<!-- markdownlint-enable MD041 -->

Macros for checking preconditions and invariants.

The public header is

```{.cpp}
#include "arene/base/contracts.hpp"
```

The Bazel target is

```text
//:contracts
```

## Introduction {#introduction}

_Preconditions_, _Postconditions_ and _Invariants_ are common terms in software
engineering. They are often associated with the term _Design by Contract_, and
form part of the _Contract_ a function has with its callers: if the
_preconditions_ are true on entry, then the function will either report an
error, or perform the intended function and ensure that the _postconditions_ are
true on return.

- A _precondition_ is something that must be true before a function is called;
  otherwise the function is being called _out of contract_. This may also be
  described as something that the function _expects_ to be true.
- A _postcondition_ is something that the function itself guarantees if the
  _preconditions_ are met. A function that cannot meet its _postconditions_ must
  report an error. It is common to throw an exception in this case, but any
  error reporting mechanism can be used. A _postcondition_ may also be described
  as something that a function _ensures_.
- An _invariant_ is something that must "always" be true. Usually there is a bit
  of leeway on that "always". For example, it might be required to be true when
  a function is called, be temporarily false during the implementation of a
  function, and then restored before the function returns or reports an error.
  Alternatively, it might be that it should "always" be true only on a
  particular line of code.

An important aspect of all of these is that if they are not true, then the
program has a bug. The function has been called _out of contract_, and thus
cannot perform its intended task, and the behaviour of the entire program is now
suspect, as an unintended state has been entered. We therefore want to detect
such problems as simply and quickly as possible.

Sometimes the preconditions, postconditions and invariants of a piece of code
are things that cannot be reasonably checked, such as "this function must not be
called concurrently with a specific other function", or things that are too
expensive to check, such as "this container must be sorted" or "this integer
must be a prime number", but often we could write an `if` statement that
expressed the requirement.

If we can write an `if`, then we can check it programmatically, but then we have
to manually implement the behaviour in the case that the check fails separately
for every check. This is where the Arene contract check macros come in.

## Arene precondition checks {#arene-precondition-checks}

The `ARENE_PRECONDITION()` macro can be used to document that the supplied
condition is a _precondition_ of the function, and verify that it is true. The
usage is simple:

```{.cpp}
ARENE_PRECONDITION( /* boolean condition */);
```

If the boolean condition evaluates to `false`, then the
[violation handler](#violation-handlers-and-contract-violations) is invoked.

Since this is a _precondition_ check, the expectation is that this macro will be
used immediately upon function entry. For example:

```{.cpp}
void foo(int i) {
  ARENE_PRECONDITION((i > -5) && (i < 42));
  ARENE_PRECONDITION(global_data_initialized);
  /* rest of function body */
}
```

These precondition checks are always present, independent of build mode or
definitions of other macros.

## Arene invariant checks {#arene-invariant-checks}

A developer may want to express that a condition _must_ be true, at a place in
the code other than the start of a function. This can be expressed with the
`ARENE_INVARIANT()` macro. Functionally, this is identical to
`ARENE_PRECONDITION()`, except that the error message says "invariant" rather
than "precondition": if the condition is `false`, then the
[violation handler](#violation-handlers-and-contract-violations) is invoked.

For example:

```{.cpp}
std::vector<output_data> foo(input_type input,std::size_t max_size) {
  std::vector<output_data> output;

  for(auto entry: input) {
    ARENE_INVARIANT((output.size() + entry.required_size()) < max_size);
    append_data(output, entry);
  }
  return output;
}
```

In this case, there is a requirement that the output vector is smaller than the
specified `max_size` at all times through the loop, but this is not something
that can be checked without running the loop twice.

As another example:

```{.cpp}
foo_result foo(input_type input) {
  auto initial_result = initial_processing(input);
  ARENE_INVARIANT(is_valid_value_for_foo(initial_result));
  return final_processing(initial_result);
}
```

In this case, the result of the `initial_processing` must be valid for `foo`,
but we have no good way of checking prior to doing this initial processing, so
can't use `ARENE_PRECONDITION()`, and have to instead check mid-function.

### Use of `ARENE_INVARIANT()` for postconditions {#use-of-arene_invariant-for-postconditions}

Postconditions are not usually easy to check. They often depend on the return
value of the function, or rely on RAII cleanup having been completed, or are
otherwise hard or expensive to check. For this reason, there is not a dedicated
Arene postcondition check macro.

However, for those cases where the condition _can_ be easily checked, you can
use `ARENE_INVARIANT()` to perform the check.

## Unreachable code {#unreachable-code}

Sometimes it is necessary to document that a particular line of code should be
unreachable. The `ARENE_INVARIANT_UNREACHABLE()` macro can be used to identify
this scenario; the
[violation handler](#violation-handlers-and-contract-violations) is invoked to
output the specified message and terminate the process.

The macro is marked as "will not return" internally, so that the compiler will
not give a warning about a missing return statement following the
`ARENE_INVARIANT_UNREACHABLE()` invocation.

For example, a `switch` statement where the condition is an enumeration might
have `case` labels for all the enumerators, but it is possible to for the value
to not be any of the specified values due to a bug elsewhere, and so none of the
`case` labels are selected. For the valid values, the function returns with the
appropriate result; for invalid values it skips over the `switch` statement and
executes the `ARENE_INVARIANT_UNREACHABLE()` line to terminate the process.
There is no way to check if a value of an enumerated type is one of the named
enumerators, so this cannot be checked with a precondition.

```{.cpp}
enum colour { red, green, blue };

foo_result foo(colour col) {
  switch(col) {
  case red:
    return make_red_result();
  case green:
    return make_green_result();
  case blue:
    return make_blue_result();
  }
  ARENE_INVARIANT_UNREACHABLE("Invalid colour value");
}
```

The difference between `ARENE_INVARIANT_UNREACHABLE` and an `ARENE_PRECONDITION`
with an always-false predicate can be subtle sometimes. Both will terminate the
program if the statement is ever executed, but the former represents an internal
invariant and the latter an external-facing precondition. The above example
treats the function `foo` as internal to the library implementing it, so that if
the function is ever called with an invalid `colour` then it's an internal bug
in the implementation and the implementation should be fixed; the user did
nothing wrong.

On the other hand, if the `foo` function were part of the library's public API,
then it would be possible for the user to call `foo` using an invalid `colour`
without violating any of the rules of C++. In this case, the macro could be
replaced with `ARENE_PRECONDITION` to communicate to the user that it's _the
user's_ responsibility to call this function with a valid argument, rather than
the implementation's responsibility to block off the code path.

## Violation handlers and contract violations {#violation-handlers-and-contract-violations}

If the condition for `ARENE_PRECONDITION()` or `ARENE_INVARIANT()` evaluates to
`false`, then we say that _the contract has been violated_, and a _violation
handler_ is invoked.

The violation handler writes the source file name and line number to the
standard error stream, along with the text of the condition or error message,
and then terminates the process by calling `std::abort`.

### Stacktrace Capture {#stacktrace-capture}

On supported platforms, a stacktrace is also included in the information printed
in the violation handler. The exact content is platform dependent, and the stack
trace may be symbolized, though the symbolized trace may be mangled names.

To explicitly disable stacktrace capture even if it is supported on a platform,
the bool configuration flag `//configs:capture_contract_stacktraces` can be used
on the command line.

```starlark
bazelisk test \
  --@arene-base//configs:capture_contract_stacktraces=false \
  <my-target>
```

The platforms for which stacktrace is currently supported are:

- Linux: stacktrace capture and symbolization are handled by the backtrace
  utilities in `<execinfo.h>`

## Differences between contract check macros and the `assert` macro {#differences-between-contract-check-macros-and-the-assert-macro}

It is common for developers to use the `assert` macro to check preconditions and
invariants, where there isn't explicit support for them.

If you use `assert` to specify a precondition or invariant, the output of a
failed assertion is unspecified, and does not necessarily make it clear what
condition was violated. It is also tied to the `NDEBUG` macro: if this macro is
defined (which is common for "release" builds) then the assertions are removed.
This means that errors are not checked in release builds, and can subsequently
propagate to undesired behaviour, or later crashes separated in time and space
from the point where the error occurred. Given this, conditions guarded by
`assert` cannot constitute part of the contract of a function, and must instead
be considered merely "helpful implementations of undefined behaviour for debug
builds".

Using `assert` also does not communicate intent: it does not document _why_ the
check is being made, or the larger context.

The Arene contract macros address these shortcomings: the name of the macro
documents the purpose of the check, and there is defined behaviour in all build
modes.

## Coverage reports and Death Tests {#coverage-reports-and-death-tests}

As `ARENE_PRECONDITION` is part of the contract of an API, the behavior of that
API in presence of a precondition violation will need to be covered by unit
tests. As `ARENE_PRECONDITION` causes the program to abort, this means that so
called "death tests," those where the program terminating is the expected
outcome, will have to be used. The GoogleTest unit testing frameworks supports
this kind of testing directly with the `ASSERT_DEATH` macro.

Unfortunately, coverage analysis tooling does not always cleanly support death
testing, and this may result in reports of uncovered branches for the negative
cases for `ARENE_PRECONDITION`. This behavior has been partially mitigated for
the following coverage tools:

- `gcov >= 3.1`: mitigation is automatic via detection of the `__GCOV__` define.
- `llvmcov`:
  - For versions `>= 18`, mitigation is automatic via detection of the
    `__LLVM_INSTR_PROFILE_GENERATE` define.
  - For versions `>= 3.5.0`, mitigation can be manually enabled by compiling
    with `ARENE_IS_LLVMCOV` defined.

For `ARENE_INVARIANT` the situation is altered; this is not part of the contract
of the system, it is essentally an "internal unit test" and thus cannot be
violated by any external means. At this time, there is no support provided for
allowing the failure branch.
