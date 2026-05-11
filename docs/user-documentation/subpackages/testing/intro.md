<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page testing testing: Helpers for Writing Robust Unit Tests

<!-- markdownlint-enable MD041 -->

The testing library provides facilities that make writing unit tests simpler and
more robust. It is divided into several sublibraries based on the testing
framework, as well as a sublibrary for facilities that are framework-agnostic.

## Subpackages for Testing {#subpackages-for-testing}

There is not a top-level target for all testing facilities. Instead, there are
individual targets for each testing framework.

- \subpage gtest
- \subpage property_based_testing
