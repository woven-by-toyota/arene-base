<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compiler_support compiler_support: Facilities For Compiler-Provided Interfaces

<!-- markdownlint-enable MD041 -->

The `compiler_support` subpackage provides facilities for interacting with the
preprocessor, as well as platform and compiler-dependant features in a
coding-standards-compliant manner. The public bazel target is:

```text
//:compiler_support
```

There is no singular public header for this subpackage, as each facility is
bespoke to a given usecase and there is unlikely to be overlap between usage.
The following general categories of facilities are provided:

- \subpage compiler_support_attributes
- \subpage compiler_support_const_evaluation
- \subpage compiler_support_diagnostics
- \subpage compiler_support_hardware_queries
- \subpage compiler_support_platform_queries
- \subpage compiler_support_preprocessor
- \subpage compiler_support_sanitizers
- \subpage compilersupportexpect
- \subpage compiler_support_cpp14_inline_variable
