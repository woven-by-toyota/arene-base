// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @example examples/subpackages/optional.cpp
/// @brief Examples for the optional subpackage documentation
///

//! [include_header]
#include "arene/base/optional.hpp"
//! [include_header]
#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>

#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/string_view.hpp"

namespace examples {

namespace ab = ::arene::base;

//! [init_access]
/// @brief Shows off initialization of an optional and accessing the value.
void initialize_and_access() {
  // A default constructed optional is unengaged. This is the same as an optional constructed from nullopt
  ab::optional<std::string> const empty_word;

  // You can test if an optional has a value explicitly. An unengaged optional is "falsy".
  if (!empty_word.has_value()) {
    std::cout << "The word was empty!";
  }

  // It can be initialized with anything convertible to the held type. Doing so constructs an engaged optional.
  ab::optional<std::string> const word{"One ring"};

  // You can cast an optional to bool to test for it holding a value. An engaged optional is "truthy".
  if (word) {
    // The value can be accessed by explicit call or dereference operator.
    std::cout << word.value() << " to rule them all.\n";
    std::cout << *word << " to find them.\n";

    // The -> operator can be used similarly to a pointer, to access the value's members directly.
    std::cout << "word size is: " << word->size() << '\n';
  }
}
//! [init_access]

//! [disengage]
/// @brief Demonstrate how to disengage an optional.
void disengage() {
  // Initialize an optional int with value 42.
  ab::optional<int> opt_int = 42;

  // Disengage the optional int with arene::base::nullopt
  opt_int = ab::nullopt;

  // Disengage the optional int with {}
  opt_int = {};

  // Disengage the optional int with member reset()
  opt_int.reset();

  // Disengage the optional int by assigment from a default constructed (and hence disengaged) optional
  opt_int = ab::optional<int>{};
}
//! [disengage]

//! [as_return]
/// @brief A very basic secret protector.
class locked_secrets {
 public:
  /// @brief constructs a secret protected by a password.
  explicit locked_secrets(std::string secret, std::string password)
      : password_(std::move(password)),
        secret_(std::move(secret)) {}

  /// @brief Attempts to unlock the secret.
  /// @param[in] password The password to try
  /// @return ab::optional<std::string> If the password was not correct, @c nullopt , else the held secret.
  ab::optional<std::string> unlock(ab::string_view password) const {
    if (password == password_) {
      return secret_;
    }
    return ab::nullopt;
  }

 private:
  /// the password
  std::string password_;
  /// the secret
  std::string secret_;
};
//! [as_return]

//! [as_param]
/// @brief A function which calls a processing function in a loop, with an optional timeout.
/// @tparam Func The type of the function to call in the loop. Returns false to indicate the loop should terminate.
/// @param[in] func The function to call in the loop.
/// @param[in] timeout The maximum amount of time to try to execute the operation. Defaults to never timing out.
template <typename Func>
void call_in_loop(Func func, ab::optional<std::chrono::milliseconds> timeout = ab::nullopt) {
  auto const start = std::chrono::steady_clock::now();
  auto const has_timed_out = [&timeout, &start]() {
    return timeout ? (std::chrono::steady_clock::now() - start) >= *timeout : false;
  };
  bool should_continue = true;
  while (should_continue && !has_timed_out) {
    should_continue = func();
  }
}
//! [as_param]

//! [as_config]
/// @brief configures the execution properties
class execution_config {
 public:
  /// The maximum number of times to execute the function. If nullopt there is no execution limit.
  ab::optional<std::size_t> max_execution_count;
  /// The maximum duration to execute the function. If nullopt there is no timeout.
  ab::optional<std::chrono::milliseconds> max_execution_time;
};

/// @brief A function which calls a processing function in a loop, with configurable termination conditions.
///
/// @tparam Func The type of the function to call in the loop. Returns false to indicate the loop should terminate.
/// @param[in] func The function to call in the loop.
/// @param[in] config Optional configuration of termination conditions.
template <typename Func>
void call_in_a_loop(Func func, execution_config const& config = {}) {
  if (config.max_execution_count) {
    std::size_t remaining = *config.max_execution_count;
    call_in_a_loop(
        [&remaining, &func]() {
          if (remaining != 0) {
            --remaining;
            return func();
          }
          return false;
        },
        config.max_execution_time
    );
  } else {
    call_in_a_loop(func, config.max_execution_time);
  }
}
//! [as_config]

//! [as_differed]
/// @brief Stores the largest seen value
template <typename T>
class find_max_value {
 public:
  /// @brief Observes a value, and retains it if is larger than the largest value seen.
  /// @param[in] value The value to observe.
  /// @post If @c !max() or @c *max()<value , then @c max() will be an optional containing @c value .
  void operator()(T value) noexcept {
    // optional supports safe passthrough comparison; a null optional compares less than all values.
    if (max_value_ < value) {
      max_value_ = value;
    }
  }

  constexpr ab::optional<T> max() const noexcept { return max_value_; }

 private:
  ab::optional<T> max_value_;
};
//! [as_differed]

//! [as_lifetime_management]
/// @brief some optional extended state information
class extended_state {
 public:
  explicit extended_state(ab::string_view data) { std::cout << "parsing '" << data << "' to produce extended state\n"; }
  ~extended_state() { std::cout << "destroying extended state\n"; }
  extended_state(extended_state const&) = default;
  extended_state(extended_state&&) = default;
  extended_state& operator=(extended_state const&) = default;
  extended_state& operator=(extended_state&&) = default;
};

/// @brief A simple class which leverages an optional member to enable extending the state used to perform an operation
/// after it is constructed without needing dynamic allocation.
class extendable {
 public:
  /// @brief enables the extended operation
  /// @post calling @c do_operation() will perform extended functionality.
  void enable_extended_operation(ab::string_view data) {
    // emplace constructs the object as if via direct construction, so there is no copy/move.
    extended_state_.emplace(data);
  }

  /// @brief disables the extended operation
  /// @post calling @c do_operation() will not perform extended functionality.
  void disable_extended_operation() {
    // reset() calls the destructor of the held object. You could also assign nullopt to it to have the same effect.
    extended_state_.reset();
  }

  /// @brief Does an operation which may be extended by calling @c enable_extended_operation()
  void do_operation() const {
    std::cout << "performing operation in extendable... ";
    if (extended_state_) {
      std::cout << "with extended state...";
    }
    std::cout << "done!\n";
  }

 private:
  /// The extended state information, if it has been provided.
  ab::optional<extended_state> extended_state_;
};
//! [as_lifetime_management]

//! [value_or]
/// @brief Attempts to unlock the door using the unlock function from the return value example.
void try_to_open_the_door(ab::string_view password) {
  auto const door = locked_secrets{"enter, friend", "mellon"};
  std::cout << "I speak '" << password
            << "', the door replies: " << door.unlock(password).value_or("nothing, and remains shut. Tricky dwarves.")
            << '\n';
}
//! [value_or]

//! [value_or_else]
/// @brief A door opener class which keeps track of how many times the door has tried to be unlocked.
class door_opener {
 public:
  /// @brief Attempts to unlock the door using the unlock function from the return value example.
  void try_to_open_the_door(ab::string_view password) {
    std::cout << "I speak '" << password << "', the door replies: "
              << door_.unlock(password).value_or_else([this]() { return generate_failure_reply(); }) << '\n';
    ++try_count_;
  }

 private:
  /// the number of tries
  std::size_t try_count_ = 0;
  /// the secret door to unlock
  locked_secrets door_{"enter, friend", "mellon"};
  /// @brief produces a reply for failure based on the number of tries.
  /// @return A reply dependent on @c try_count_ .
  ab::string_view generate_failure_reply() const {
    if (try_count_ < 2) {
      return "Try again!";
    }
    if (try_count_ >= 2 && try_count_ < 10) {
      return "Still wrong, you're persistent, aren't you?";
    }
    return "Seriously, give it up already!";
  }
};
//! [value_or_else]

//! [and_then]
/// @brief Performs safe integer division
/// @param[in] num the numerator
/// @param[in] den the denominator
/// @return ab::optional<std::int64_t> if @c den==0 , @c nullopt, else @c num/den
ab::optional<std::int64_t> safe_divide(std::int64_t num, std::int64_t den) {
  if (den != 0) {
    return num / den;
  }
  return ab::nullopt;
}

/// @brief Filters out values below a given threshold
/// @param[in] value The value to filter
/// @param[in] threshold The threshold of the filter
/// @return ab::optional<std::int64_t> If @c value>=threshold , @c value , else @c nullopt .
ab::optional<std::int64_t> filter(std::int64_t value, std::int64_t threshold) {
  if (value >= threshold) {
    return value;
  }
  return ab::nullopt;
}

/// @brief Performs safe division of the input values, then filters them by a given threshold.
/// @param[in] num the numerator
/// @param[in] den the denominator
/// @param[in] threshold The threshold of the filter
/// @return ab::optional<std::int64_t> @c num/den if both of the following are true: @c den!=0 and
///         @c num/den>=threshold . Otherwise @c nullopt
ab::optional<std::int64_t> save_divide_and_filter(std::int64_t num, std::int64_t den, std::int64_t threshold) {
  return safe_divide(num, den).and_then([threshold](std::int64_t value) { return filter(value, threshold); });
}
//! [and_then]

//! [or_else]
/// @brief A forgiving door guard who eventually always opens the door.
class forgiving_door_guard {
 public:
  /// @brief Attempts to unlock the door using the unlock function from the return value example.
  /// @param[in] password the password to try unlocking with.
  void try_to_open_the_door(ab::string_view password) {
    // after try_count_ == the_password.size() attempts, the door always unlocks no matter what the user guessed.
    auto const maybe_unlocked =
        door_.unlock(password).or_else([this]() { return door_.unlock(the_password.substr(0, try_count_)); });

    std::cout << "I speak '" << password << "', the door replies: " << maybe_unlocked.value_or("try again!") << '\n';
    ++try_count_;
  }

 private:
  /// the number of tries
  std::size_t try_count_ = 0;
  /// the password
  static constexpr ab::string_view the_password = "mellon";
  /// the secret door to unlock
  locked_secrets door_{"enter, friend", the_password};
};
//! [or_else]

//! [transform]
/// @brief Normalizes the input value by converting it into floating-point as if the input represented milli-units.
/// @param[in] value The value to normalize
/// @return double The input value, as floating point value, divided by @c std::milli::den .
///
double normalize(std::int64_t value) { return static_cast<double>(value) / std::milli::den; }

/// @brief Performs safe division of the input values, then normalizes the result into a floating point quantity as if
///        it represented milli-units.
/// @param[in] num the numerator
/// @param[in] den the denominator
/// @return ab::optional<double> if @c den==0 , @c nullopt, else equivalent to @c normalize(num/den) .
ab::optional<double> safe_divide_and_normalize(std::int64_t num, std::int64_t den) {
  return safe_divide(num, den).transform(normalize);
}
//! [transform]

}  // namespace examples
