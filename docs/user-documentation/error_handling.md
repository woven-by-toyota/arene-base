<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page errorhandling Error Handling and Reporting in Arene Base

<!-- markdownlint-enable MD041 -->

\arene_base uses 3 different mechanisms for reporting errors, depending on the
form of the error:

- [Return values](#return-values): The return type from the function encodes any
  information about an error. This is the most common form of error handling in
  \arene_base.
- [Throwing exceptions](#throwing-exceptions): Some functions throw C++
  exceptions to report errors. This is rarely used in \arene_base. It is only
  used for functions explicitly documented to throw, usually for consistency
  between \arene_base APIs and similar standard library APIs.
- [Process termination](#process-termination): _If_ a precondition violation or
  internal invariant violation is detected, the process is terminated. Such
  cases are either a logic error in the code using \arene_base, or a bug in the
  implementation of \arene_base itself.

## Process Termination {#process-termination}

Many \arene_base facilities have clearly-documented preconditions that must be
satisfied in order to use those facilities. They also may have internal
invariants that are maintained during correct operation. Failure to meet any
preconditions is considered a programming logic error in the calling code, and
the program must therefore be in an unanticipated state. Similarly, if any
internal invariant is broken, then there is a logic error in the calling code,
or in the \arene_base implementation, and the program must again be in an
unanticipated state.

Consequently, if \arene_base code detects violation of any preconditions or
internal invariants, the process is terminated. The program must be in an
unanticipated state, so the only safe action is to terminate, as the only
certain way to return the system to a defined state. See \ref contracts for
details, including how to use this mechanism in code outside \arene_base.

\note Not all preconditions are checked, as it may not be possible to do so, or
it may be too expensive. In such cases, precondition violations will not trigger
process termination and it is the responsibility of the caller to ensure
preconditions are satisfied _prior_ to calling the function.

\note Where a facility has a precondition that _is_ checked, \arene_base often
exposes facilities that would enable the calling code to check prior to using
the facility. For example, when accessing an element of an
`arene::base::inline_vector` using an index, the index must be less than the
size of the container; this can be checked beforehand by comparing the index to
the result of the `arene::base::inline_vector::size()` member function for that
vector.

## Throwing Exceptions {#throwing-exceptions}

In general, \arene_base does not throw exceptions as an error handling strategy.
However, it is used for consistency between \arene_base APIs and similar
standard library APIs, and in a small number of other cases. For example,
`arene::base::inline_vector::at()` will throw an exception (rather than
encounter a precondition violation) if the index is out of range.

All operations that throw exceptions are clearly documented as doing so.

\note Operations that do not throw themselves may still not be marked
`noexcept`, if they use standard library facilities that may throw, or if they
use user-provided facilities that may throw. Generic facilities will still pass
through any exception thrown by the operations performed on their input types if
those types throw.

## Return Values {#return-values}

Where a function may fail even if the inputs satisfy the preconditions, the most
common error handling strategy in \arene_base is to convey the error via the
return value of the function.

The form of the error indication depends on two aspects:

- Does the function have a return value other than the failure indication?
- Is a simple failed/succeeded indicator sufficient, or does the reason for the
  failure need enumerating?

### Functions with no other return value {#functions-with-no-other-return-value}

If the reason for the failure does not need enumerating, a function that can
fail will return a `bool`, with `true` for success and `false` for failure.

Where the reason does need enumerating, then an error code `enum` will be used
to specify the reasons for the failure. The return value for the function is
then `arene::base::result<void,the_error_code_enum>`, holding an empty value on
success, or the error code on failure.

### Functions with a return value on success {#functions-with-a-return-value-on-success}

For a function that returns a value of type `Foo` on success, if the reason for
the failure does not need enumerating, that function will return an
`arene::base::optional<Foo>` that holds a value on success, or
`arene::base::nullopt` on failure.

Where the reason does need enumerating, then an error code `enum` will be used
to specify the reasons for the failure. The return value for the function is
then `arene::base::result<Foo,the_error_code_enum>`, holding the successful
value on success, or the error code on failure.

### Constructors and Factory Functions {#constructors-and-factory-functions}

There are no direct return values from a constructor, so the only mechanisms
available to report errors are exceptions and process termination. For some
\arene_base classes where constructors may fail, if the class is `Foo`, then a
`Foo::try_construct` static member function is provided as a factory function,
in addition to the constructors. `Foo::try_construct` returns either an
`arene::base::optional<Foo>` or an `arene::base::result<Foo,some_error_code>` as
described [above](#functions-with-a-return-value-on-success). The result
contains the correctly-constructed `Foo` instance on success.
