// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @example examples/scope_guard_examples.cpp
/// @brief Examples for the scope_guard subpackage documentation
///

//! [include_header]
#include "arene/base/scope_guard.hpp"
//! [include_header]

#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "arene/base/functional.hpp"

namespace {

//! [non_raii_interface]
/// @brief A handle to some opaque system resource.
struct resource_handle {
  int value;
};

/// @brief Allocates a system resource.
/// @return resource_handle A handle to the allocated resource.
auto allocate_resource() noexcept -> resource_handle;

/// @brief Frees an already allocated resource.
/// @param handle The handle to the resource to be freed.
/// @post The resource pointed to by @c handle is freed.
void free_resource(resource_handle handle) noexcept;
//! [non_raii_interface]

auto operator<<(std::ostream& lhs, resource_handle const& rhs) -> std::ostream& {
  return lhs << "resource_handle{value=" << rhs.value << '}';
}

//! [basic_usage]
/// @brief Some operation that might throw.
/// @param value A value needed for the operator.
/// @param handle A handle to the resource to perform the operation on.
/// @throws std::out_of_range if @c value is less than 0.
auto might_throw(int value, resource_handle handle) -> int;

/// @brief Example of using scope_guard directly
/// @param value The value to pass to @c might_throw
void basic_usage(int value) {
  resource_handle const handle = allocate_resource();
  auto const always_free = arene::base::on_scope_exit([&handle]() noexcept { free_resource(handle); });
  might_throw(value, handle);
}
//! [basic_usage]

//! [named_raii_helper_func]
/// @brief Creates a scope guard which will free the supplied resource.
/// @param handle The resource to free on exit.
/// @return A @c scope_guard which will call @c free_resource on @c handle .
auto free_on_exit(resource_handle const& handle) {
  return arene::base::on_scope_exit([&handle]() noexcept { free_resource(handle); });
}

/// @brief Example of using a named scope_guard helper.
/// @param value The value to pass to @c might_throw.
void named_usage(int value) {
  resource_handle const handle = allocate_resource();
  auto const cleanup = free_on_exit(handle);
  might_throw(value, handle);
}
//! [named_raii_helper_func]

//! [invoke_now_and_cancel]
/// @brief Some function that stores the resource to use later
/// @param handle The handle to store.
void store_handle_for_later(resource_handle handle);

/// @brief Example of using the @c invoke_now and @c cancel functionality.
/// @param value If < 0, frees the handle immediately, otherwise it stores it for later.
void maybe_release_early(int value) {
  resource_handle const handle = allocate_resource();
  auto cleanup = free_on_exit(handle);
  if (value < 0) {
    std::cout << "Cleaning up immediately...\n";
    cleanup.invoke_now();
  } else {
    store_handle_for_later(handle);
    // We have to call cancel to avoid freeing the resource on scope exit.
    cleanup.cancel();
  }
}
//! [invoke_now_and_cancel]

auto allocate_resource() noexcept -> resource_handle {
  static int resource_value{};
  resource_handle handle{resource_value++};
  std::cout << "allocated resource: " << handle << '\n';
  return handle;
}
void free_resource(resource_handle handle) noexcept { std::cout << "freeing resource: " << handle << '\n'; }
auto might_throw(int const value, resource_handle) -> int {
  if (value < 0) {
    throw std::out_of_range{"invalid value"};
  }
  return value + 2;
}
void store_handle_for_later(resource_handle handle) { std::cout << "Storing " << handle << " for later.\n"; }

}  // namespace

auto main() -> int {
  std::cout << "enter value: ";
  int value{};
  std::cin >> value;
  std::cout << "\nBasic Usage...\n";
  try {
    basic_usage(value);
  } catch (std::exception const& e) {
    std::cerr << "basic_usage exited via exception: " << e.what() << '\n';
  }
  std::cout << "\nNamed Usage...\n";
  try {
    named_usage(value);
  } catch (std::exception const& e) {
    std::cerr << "named_usage exited via exception: " << e.what() << '\n';
  }

  std::cout << "\nInvoke Now/Cancel Usage...\n";
  maybe_release_early(value);
  return 0;
}
