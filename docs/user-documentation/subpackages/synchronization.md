<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 MD013-->
<!-- prettier-ignore -->
\page synchronization synchronization: Facilities For Synchronization In Concurrent Systems

<!-- markdownlint-enable MD041 MD013-->

Arene Base provides facilities for synchronizing operations across threads.

There is no singular public header for this subpackage, as each facility is
bespoke to a given usecase and there is unlikely to be overlap between usage.

The Bazel target is

```text
//:synchronization
```

## Introduction {#introduction}

The `synchronization` subpackage provides backports of
[`std::latch`](https://en.cppreference.com/w/cpp/thread/latch) and
[`std::barrier`](https://en.cppreference.com/w/cpp/thread/barrier), as well as
additional facilities for synchronizing operations between threads.

Note: use of these synchronization mechanisms may lead a thread to **block**.
Care should be taken to ensure this is permitted by the relevant coding
standard.

## Latches {#latches}

`arene::base::latch` is a backport of
[`std::latch`](https://en.cppreference.com/w/cpp/thread/latch). It is a single
use latching synchronization object. The constructor takes the number of events
needed to trigger the latch.

Threads then call the `count_down` or `arrive_and_wait` member functions to
decrease the count.

Threads can call `wait` or `arrive_and_wait` member functions to wait for the
count to reach zero. This is a **blocking** wait that **cannot** be interrupted.
When the count reaches zero, all waiting threads are unblocked, and can return
from their `wait` or `arrive_and_wait` calls. Calls to `wait` after the count is
already zero return immediately.

For example, you can use a latch to wait for background processing to complete.

```{.cpp}
void foo(){
  unsigned const thread_count=...;
  arene::base::latch done(thread_count);
  my_data data[thread_count];
  std::vector<std::thread> threads;
  // ensure we join the threads, even if an exception is thrown
  auto cleanup = arene::base::on_scope_exit([&] {
    for(auto& t: threads){
      t.join();
    }
  });

  threads.reserve(thread_count);
  for(unsigned i=0;i<thread_count;++i)
    threads.push_back(std::thread([&,i]{
      data[i]=make_data(i);
      done.count_down();
      do_more_stuff();
    }));
  done.wait();
  process_data();
}
```

The call to `done.wait()` will block until all the threads have executed
`done.count_down()`.

Header:

```{.cpp}
#include <arene/base/synchronization/latch.hpp>
```

## Barriers {#barriers}

`arene::base::barrier` is a backport of
[`std::barrier`](https://en.cppreference.com/w/cpp/thread/barrier). It is a
"loop" synchronization primitive, used to synchronize a number of threads
involved in a macro task with repeated synchronization points. The "loop" may
involve one or more loop statements such as `for` or `while`, or may be a
"conceptual" loop where each thread involved in the task performs a series of
steps with synchronization points between.

The template parameter is the type of the completion function that is executed
at the end of each loop iteration; if it is omitted, then a default do-nothing
completion function is used.

The constructor takes a count of threads that will need to _arrive_ at the
barrier each loop iteration, and an optional completion function, which must be
of the specified type. If the completion function is omitted, then a
default-constructed instance is used instead; this will result in an error if
the type is not default-constructible.

Each loop iteration, threads _arrive_ at the barrier, and wait for it to be
signalled. When the specified number of threads have _arrived_, the completion
function is invoked, waiting threads are woken, and the barrier is reset for the
next loop iteration.

Threads _arrive_ at the barrier by calling one of the following functions:

- `arene::base::barrier::arrive`: This _arrives_ at the barrier and returns an
  `arene::base::barrier::arrival_token` which must later be passed to
  `arene::base::barrier::wait` by that thread in order to wait for the loop
  iteration to end. If the loop iteration has already ended before the call to
  `arene::base::barrier::wait` then the call will return immediately.
- `arene::base::barrier::arrive_and_drop`: This _arrives_ at the barrier, but
  does not wait for the loop iteration to end, and the thread may no longer
  _arrive_ at the barrier in future iterations: the thread count is permanently
  decreased by one.
- `arene::base::barrier::arrive_and_wait`: This is equivalent to calling
  `b.wait(b.arrive())` for a `arene::base::barrier` instance named `b`.

The thread that _arrives_ at the barrier last, causing the remaining thread
count to decrease to 0, executes the **completion function**, then wakes all
waiting threads blocked in `wait` or `arrive_and_wait`. Any threads that have
called `arrive` but not `wait` will not block when they call `wait`. The counter
is then reset and threads can begin arriving for the next loop iteration.

A thread may pass a numerical value to `arrive`, aggregating arrivals from
multiple threads. The total number of arrivals during a given loop iteration
must not exceed the current thread count of the barrier object; too many
arrivals is a [precondition violation](\ref contracts)

\warning A thread which calls `arrive` must be careful to not access any
resources which require synchronization with the completion function between the
call to `arrive` and the call to `wait`, as the completion function will be
invoked based on the number of threads that _arrive_, not the number of threads
that _wait_. Failure to maintain this assumption will result in data races.

The example below shows a task being split across multiple worker threads, which
synchronize using an `arene::base::barrier`:

```{.cpp}
void finish_task(); // the completion function of the barrier

void worker_thread(
  arene::base::barrier<arene::base::inline_function<void()>>& barrier,
std::atomic<bool>& stop, unsigned index){
  while(!stop){
    do_stuff(index);
    barrier.arrive_and_wait();
  }
}

void split_work(unsigned num_threads){
  arene::base::barrier<arene::base::inline_function<void()>> bar(num_threads, finish_task);

  std::vector<std::thread> threads;
  // ensure we join the threads, even if an exception is thrown
  auto cleanup = arene::base::on_scope_exit([&] {
    for(auto& thread: threads){
      thread.join();
    }
  });
  threads.reserve(num_threads);
  std::atomic<bool> stop{false};
  for(unsigned index = 0; index < num_threads; ++index){
    threads.push_back(std::thread{[&bar, &stop, index] {
      worker_thread(bar, stop, index);
    }});
  }
  do_main_processing();
  stop = true;
}
```

Here, the worker threads each do their portion of the work, and then wait for
the others to finish before `finish_task` is invoked, and then all the threads
start work on the next iteration. The main thread does whatever processing it
needs to do, and then signals that the background threads should stop, and waits
for them to finish.

Header:

```{.cpp}
#include <arene/base/synchronization/barrier.hpp>
```

## Manual Reset Events {#manual-reset-events}

`arene::base::manual_reset_event` is a low level synchronization object. It has
two states: _signalled_ and _not signalled_. Initially it is _not signalled_.

When it is _not signalled_, then any threads that call
`arene::base::manual_reset_event::wait` will block until it is _signalled_.
Calling `arene::base::manual_reset_event::signal` will change the state to
_signalled_, and wake any threads blocked in `wait`.

When the event is _signalled_, then any threads that call `wait` will return
immediately.

Calling `arene::base::manual_reset_event::reset` will change the state from
_signalled_ to _not signalled_. If `reset` is called immediately after `signal`,
then it is possible that some of the waiting threads will not have woken, and
will thus miss the signal, and remain blocked. If this is undesirable, then an
additional mechanism should be used to ensure that all threads have woken before
calling `reset`.

Once the event has been changed back to _non signalled_, threads that call
`wait` will now block again.

Header:

```{.cpp}
#include <arene/base/synchronization/manual_reset_event.hpp>
```
