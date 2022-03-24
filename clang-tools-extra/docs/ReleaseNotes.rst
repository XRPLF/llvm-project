====================================================
Extra Clang Tools |release| |ReleaseNotesTitle|
====================================================

.. contents::
   :local:
   :depth: 3

Written by the `LLVM Team <https://llvm.org/>`_

.. only:: PreRelease

  .. warning::
     These are in-progress notes for the upcoming Extra Clang Tools |version| release.
     Release notes for previous releases can be found on
     `the Download Page <https://releases.llvm.org/download.html>`_.

Introduction
============

This document contains the release notes for the Extra Clang Tools, part of the
Clang release |release|. Here we describe the status of the Extra Clang Tools in
some detail, including major improvements from the previous release and new
feature work. All LLVM releases may be downloaded from the `LLVM releases web
site <https://llvm.org/releases/>`_.

For more information about Clang or LLVM, including information about
the latest release, please see the `Clang Web Site <https://clang.llvm.org>`_ or
the `LLVM Web Site <https://llvm.org>`_.

Note that if you are reading this file from a Git checkout or the
main Clang web page, this document applies to the *next* release, not
the current one. To see the release notes for a specific release, please
see the `releases page <https://llvm.org/releases/>`_.

What's New in Extra Clang Tools |release|?
==========================================

Some of the major new features and improvements to Extra Clang Tools are listed
here. Generic improvements to Extra Clang Tools as a whole or to its underlying
infrastructure are described first, followed by tool-specific sections.

Major New Features
------------------

...

Improvements to clangd
----------------------

Inlay hints
^^^^^^^^^^^

Diagnostics
^^^^^^^^^^^

Semantic Highlighting
^^^^^^^^^^^^^^^^^^^^^

Compile flags
^^^^^^^^^^^^^

Hover
^^^^^

Code completion
^^^^^^^^^^^^^^^

Signature help
^^^^^^^^^^^^^^

Cross-references
^^^^^^^^^^^^^^^^

Objective-C
^^^^^^^^^^^

Miscellaneous
^^^^^^^^^^^^^

Improvements to clang-doc
-------------------------

The improvements are...

Improvements to clang-query
---------------------------

The improvements are...

Improvements to clang-rename
----------------------------

The improvements are...

Improvements to clang-tidy
--------------------------

New checks
^^^^^^^^^^

- New :doc:`bugprone-stringview-nullptr
  <clang-tidy/checks/bugprone-stringview-nullptr>` check.

  Checks for various ways that the ``const CharT*`` constructor of
  ``std::basic_string_view`` can be passed a null argument.

- New :doc:`abseil-cleanup-ctad
  <clang-tidy/checks/abseil-cleanup-ctad>` check.

  Suggests switching the initialization pattern of ``absl::Cleanup``
  instances from the factory function to class template argument
  deduction (CTAD), in C++17 and higher.

- New :doc:`bugprone-suspicious-memory-comparison
  <clang-tidy/checks/bugprone-suspicious-memory-comparison>` check.

  Finds potentially incorrect calls to ``memcmp()`` based on properties of the
  arguments.

- New :doc:`cppcoreguidelines-virtual-class-destructor
  <clang-tidy/checks/cppcoreguidelines-virtual-class-destructor>` check.

  Finds virtual classes whose destructor is neither public and virtual nor
  protected and non-virtual.

- New :doc:`hooks-account-buf-len
  <clang-tidy/checks/hooks-account-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-account-conv-buf-len
  <clang-tidy/checks/hooks-account-conv-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-account-conv-pure
  <clang-tidy/checks/hooks-account-conv-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-array-buf-len
  <clang-tidy/checks/hooks-array-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-burden-prereq
  <clang-tidy/checks/hooks-burden-prereq>` check.

  FIXME: add release notes.

- New :doc:`hooks-detail-buf-len
  <clang-tidy/checks/hooks-detail-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-detail-prereq
  <clang-tidy/checks/hooks-detail-prereq>` check.

  FIXME: add release notes.

- New :doc:`hooks-emit-buf-len
  <clang-tidy/checks/hooks-emit-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-emit-prereq
  <clang-tidy/checks/hooks-emit-prereq>` check.

  FIXME: add release notes.

- New :doc:`hooks-entry-point-recursion
  <clang-tidy/checks/hooks-entry-point-recursion>` check.

  FIXME: add release notes.

- New :doc:`hooks-entry-points
  <clang-tidy/checks/hooks-entry-points>` check.

  FIXME: add release notes.

- New :doc:`hooks-fee-prereq
  <clang-tidy/checks/hooks-fee-prereq>` check.

  FIXME: add release notes.

- New :doc:`hooks-field-add-buf-len
  <clang-tidy/checks/hooks-field-add-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-field-buf-len
  <clang-tidy/checks/hooks-field-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-field-del-buf-len
  <clang-tidy/checks/hooks-field-del-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-float-arith-pure
  <clang-tidy/checks/hooks-float-arith-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-float-compare-pure
  <clang-tidy/checks/hooks-float-compare-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-float-int-pure
  <clang-tidy/checks/hooks-float-int-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-float-manip-pure
  <clang-tidy/checks/hooks-float-manip-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-float-one-pure
  <clang-tidy/checks/hooks-float-one-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-float-pure
  <clang-tidy/checks/hooks-float-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-func-addr-taken
  <clang-tidy/checks/hooks-func-addr-taken>` check.

  FIXME: add release notes.

- New :doc:`hooks-guard-called
  <clang-tidy/checks/hooks-guard-called>` check.

  FIXME: add release notes.

- New :doc:`hooks-guard-in-for
  <clang-tidy/checks/hooks-guard-in-for>` check.

  FIXME: add release notes.

- New :doc:`hooks-guard-in-while
  <clang-tidy/checks/hooks-guard-in-while>` check.

  FIXME: add release notes.

- New :doc:`hooks-hash-buf-len
  <clang-tidy/checks/hooks-hash-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-keylet-buf-len
  <clang-tidy/checks/hooks-keylet-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-param-buf-len
  <clang-tidy/checks/hooks-param-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-param-set-buf-len
  <clang-tidy/checks/hooks-param-set-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-raddr-conv-buf-len
  <clang-tidy/checks/hooks-raddr-conv-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-raddr-conv-pure
  <clang-tidy/checks/hooks-raddr-conv-pure>` check.

  FIXME: add release notes.

- New :doc:`hooks-reserve-limit
  <clang-tidy/checks/hooks-reserve-limit>` check.

  FIXME: add release notes.

- New :doc:`hooks-slot-hash-buf-len
  <clang-tidy/checks/hooks-slot-hash-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-slot-keylet-buf-len
  <clang-tidy/checks/hooks-slot-keylet-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-slot-limit
  <clang-tidy/checks/hooks-slot-limit>` check.

  FIXME: add release notes.

- New :doc:`hooks-slot-sub-limit
  <clang-tidy/checks/hooks-slot-sub-limit>` check.

  FIXME: add release notes.

- New :doc:`hooks-slot-type-limit
  <clang-tidy/checks/hooks-slot-type-limit>` check.

  FIXME: add release notes.

- New :doc:`hooks-state-buf-len
  <clang-tidy/checks/hooks-state-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-transaction-hash-buf-len
  <clang-tidy/checks/hooks-transaction-hash-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-transaction-slot-limit
  <clang-tidy/checks/hooks-transaction-slot-limit>` check.

  FIXME: add release notes.

- New :doc:`hooks-validate-buf-len
  <clang-tidy/checks/hooks-validate-buf-len>` check.

  FIXME: add release notes.

- New :doc:`hooks-verify-buf-len
  <clang-tidy/checks/hooks-verify-buf-len>` check.

  FIXME: add release notes.

- New :doc:`misc-misleading-identifier <clang-tidy/checks/misc-misleading-identifier>` check.

  Reports identifier with unicode right-to-left characters.

- New :doc:`readability-container-data-pointer
  <clang-tidy/checks/readability-container-data-pointer>` check.

  Finds cases where code could use ``data()`` rather than the address of the
  element at index 0 in a container.

- New :doc:`readability-identifier-length
  <clang-tidy/checks/readability-identifier-length>` check.

  Reports identifiers whose names are too short. Currently checks local
  variables and function parameters only.

- New :doc:`bugprone-shared-ptr-array-mismatch <clang-tidy/checks/bugprone-shared-ptr-array-mismatch>` check.

  Finds initializations of C++ shared pointers to non-array type that are initialized with an array.

New check aliases
^^^^^^^^^^^^^^^^^

Changes in existing checks
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Fixed a false positive in :doc:`readability-non-const-parameter
  <clang-tidy/checks/readability-non-const-parameter>` when the parameter is referenced by an lvalue

- Fixed a crash in :doc:`readability-const-return-type
  <clang-tidy/checks/readability-const-return-type>` when a pure virtual function
  overrided has a const return type. Removed the fix for a virtual function.

Removed checks
^^^^^^^^^^^^^^

Improvements to include-fixer
-----------------------------

The improvements are...

Improvements to clang-include-fixer
-----------------------------------

The improvements are...

Improvements to modularize
--------------------------

The improvements are...

Improvements to pp-trace
------------------------

The improvements are...

Clang-tidy Visual Studio plugin
-------------------------------
