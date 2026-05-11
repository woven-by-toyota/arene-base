<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page scope_guard scope_guard: An RAII Helper

<!-- markdownlint-enable MD041 -->

The `scope_guard` sub-package provides a facility for ensuring that a cleanup
function is run when a scope is exited by whatever means.

The public header is

\snippet docs/examples/scope_guard_examples.cpp include_header

The Bazel target is

```text
//:scope_guard
```

## Guaranteeing Cleanup {#guaranteeing-cleanup}

A common problem when interacting with externally managed resources (files,
allocated memory, etc) is ensuring that resources are correctly released, or
other cleanup correctly done, when a function or scope exits in an unexpected
fashion. Most commonly, this "unexpected exit" comes from an exception being
thrown, but it can also be due to a `break` or `return` statement leading to the
normal cleanup code being skipped.

In C++, this is generally handled via classes whose constructors acquire the
resources, and destructors release them. This pattern is generally called
_Resource Aqusition Is Initialization_, or RAII. However, there are situations
where creating a dedicated class to implement this pattern is overly complex,
and simply invoking a function to perform the cleanup operation is all that is
needed. The `scope_guard` sub-package provides a mechanism to assist with this
in the form of the `arene::base::scope_guard` class template, and the associated
`arene::base::on_scope_exit` factory function.

`arene::base::scope_guard` has a single template parameter: the type of an
invocable to store, which must satisfy `is_nothrow_invocable_r_v<void, Func>`.
The constructor then constructs an instance of that invocable from the supplied
constructor argument, and the destructor invokes it. This means that an instance
of `arene::base::scope_guard` created at local scope will run the supplied
invocable when the scope exits, whether that is because the execution continued
off the end of the scope, or the scope was exited via a use of `break`,
`continue`, `return` or `throw`.

### Using `at_scope_exit` Directly {#using-at_scope_exit-directly}

Assume a resource management API like the following:

\snippet docs/examples/scope_guard_examples.cpp non_raii_interface

The caller needs to ensure that there are matched calls to the allocation and
free functions, else the resource leaks. The following example shows using
`at_scope_exit` directly to ensure that the free function is always called, even
if the scope is exited via an exception.

\snippet docs/examples/scope_guard_examples.cpp basic_usage

### Creating a Named Helper {#creating-a-named-helper}

If a particular form of guard will be used repeatedly, instead of calling
`at_scope_exit` directly, a factory function can be made to give the action a
descriptive name and hide the implementation details. The previous example
refactored in this manner would look like this:

\snippet docs/examples/scope_guard_examples.cpp named_raii_helper_func

### Invoking The Guard Early Or Canceling It {#invoking-the-guard-early-or-canceling-it}

If the function needs to be invoked prior to destroying the `guard` object, the
`arene::base::scope_guard::invoke_now` member function can be used.
Alternatively, if the function should not be called, then the
`arene::base::scope_guard::cancel` function can be used. This can be useful if
the resource is being passed to somewhere else that will correctly clean up when
it is done:

\snippet docs/examples/scope_guard_examples.cpp invoke_now_and_cancel
