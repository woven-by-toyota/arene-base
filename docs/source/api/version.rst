.. Copyright 2026, Toyota Motor Corporation
..
.. SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

version
#######

The ``<arene/version.hpp>`` header and related are filled with macro checks and definitions to control configuration and use of the arene.base libraries. You can use some of the publicly-documented functionality below to ease version checking and other things in your code base, and make the checks less prone to decay and failure. If you would like to see the reasoning behind this, check :doc:`the configuration implementation page </implementation/macros and configuration>`.

Arene Version
-------------

.. doxygenvariable:: arene::base::version_major

.. doxygenvariable:: arene::base::version_minor

.. doxygenvariable:: arene::base::version_patch

.. doxygenvariable:: arene::base::version

Feature Checking
----------------

.. doxygendefine:: ARENE_ON

.. doxygendefine:: ARENE_OFF

.. doxygendefine:: ARENE_ON_BY_DEFAULT

.. doxygendefine:: ARENE_OFF_BY_DEFAULT

.. doxygendefine:: ARENE_IS_ON

.. doxygendefine:: ARENE_IS_ON_RAW

.. doxygendefine:: ARENE_IS_OFF

.. doxygendefine:: ARENE_IS_OFF_RAW

.. doxygendefine:: ARENE_IS_ON_BY_DEFAULT

.. doxygendefine:: ARENE_IS_ON_BY_DEFAULT_RAW

.. doxygendefine:: ARENE_IS_OFF_BY_DEFAULT

.. doxygendefine:: ARENE_IS_OFF_BY_DEFAULT_RAW

.. doxygendefine:: ARENE_GUARANTEE_INTERNAL_DEFINITION
