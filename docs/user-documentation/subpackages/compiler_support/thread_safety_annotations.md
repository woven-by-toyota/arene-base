<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 MD013-->
<!-- prettier-ignore -->
\page compiler_support_thread_safety_annotations Thread Safety Analysis Annotations

<!-- markdownlint-enable MD041 -->

[Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html)
(TSA hereafter) is a form of static analysis supported by the clang compiler
which, through use of explicit user annotations, allows the compiler to detect
many types of common concurrency defects statically at compile time.

\note TSA is part of the clang compiler itself, and is enabled via the
`-Wthread-safety` compiler flag.

The annotations are implemented in the form of macros which are used to decorate
class definitions, function/member-function declarations, and variable/member
declarations. This _domain specific language_ is based on the notion of
"capabilities" that guard resources, and then correctly annotating which
functions interact with which capabilities.

\note Currently, only `clang` supports TSA. However, the attributes **are safe
to use with other compilers** in production settings; they turn into no-ops for
unsupported compilers. Because the attributes provide a declarative syntax for
documenting the concurrency model of a system, it is therefore recommended to
use them even if `clang` is not the primary compiler for the project.

## Capabilities Guard Resources {#capabilities-guard-resources}

The fundamental axiom of Thread Safety Analysis is that of the _capability_,
which protects _resources_. _Resources_ can be global variables, members, or
functions/methods, and a calling thread may not access a _resource_ unless it
holds the required _capability_. This conceptual model, and TSA's analysis in
general, is based on the concepts of
[Capability Based Security](https://en.wikipedia.org/wiki/Capability-based_security).

In practice, there are specifically annotated C++ types which may act as
_capabilities_. A mutex is the most basic example of a type which may act as a
capability; if a resource is said to be _guarded by_ the mutex, then attempting
to access the resource without the mutext being locked (without _acquiring_ the
capability) will result in a compile-time error.

This is only a brief summary of _capabilities_ and the analysis model for TSA,
please see the full
[documentation](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#basic-concepts-capabilities)
for more details.

## Annotating Resources As Being Guarded By Capabilities {#annotating-resources-as-being-guarded-by-capabilities}

There are two primary annotations for declaring _resources_ as being _guarded_
by _capabilities_:

|          Annotation          |         Annotation Applied To         |                                                                            Description                                                                            |
| :--------------------------: | :-----------------------------------: | :---------------------------------------------------------------------------------------------------------------------------------------------------------------: |
|  `ARENE_TSA_GUARDED_BY(x)`   |       Global Variables/Members        |                                             Annotates a variable/member as as being _guarded_ by the named capability                                             |
| `ARENE_TSA_PT_GUARDED_BY(x)` | Pointer-Like Global Variables/Members | Annotates the _content pointed to_ by a pointer-like variable/member as as being _guarded_ by the named capability. The pointer-like entity itself is not guarded |

Their usage is straightforward, as demonstrated in the following example:

```cpp
struct synchronized {
    arene::base::mutex mtx;
    int a ARENE_TSA_GUARDED_BY(mtx);
    int* b ARENE_TSA_PT_GUARDED_BY(mtx);
}

synchronized data;
data.a = 0; // this is a compiler error
data.mtx.lock(); // acquired the needed capability
data.a = 0; // Ok!
data.mtx.unlock(); // release the needed capability

int indirected = 1;
data.b = &indirected // this is ok, because PT_GUARDED_BY doesn't guard the pointer-value
*data.b = 0; // this is a compiler error, as we access resource pointed at
data.mtx.lock(); // acquired the needed capability
*data.b = 0; // Ok!
data.mtx.unlock(); // release the needed capability
```

## Annotating Functions/Methods As Requiring/Excluding Capabilities {#annotating-functionsmethods-as-requiringexcluding-capabilities}

A common issue in concurrent programming is deadlock: a thread enters a function
which _acquires_ exclusive access to a _capability_ in its body, and then before
_releasing_ the capability, enters another function which attempts to _acquire_
the capability again, becoming forever blocked on itself.

TSA provides annotations for function/method bodies that allow deadlock
detection to be pushed to compile time, by declaring that a capability is
_required_ to be held, or _excluded_ from being held, in order to enter the
function/method:

|            Annotation            | Annotation Applied To |                                                                       Description                                                                       |
| :------------------------------: | :-------------------: | :-----------------------------------------------------------------------------------------------------------------------------------------------------: |
|    `ARENE_TSA_REQUIRES(...)`     |   Functions/Methods   | Annotates a function/method as _requiring_ the calling thread to hold **exclusive** access to the named capabilities _before entering_ and _after exit_ |
| `ARENE_TSA_REQUIRES_SHARED(...)` |   Functions/Methods   |  Annotates a function/method as _requiring_ the calling thread to hold **shared** access to the named capabilities _before entering_ and _after exit_   |
|    `ARENE_TSA_EXCLUDES(...)`     |   Functions/Methods   |          Annotates a function/method as _excluding_ the calling thread from holding the named capabilities _before entering_ and _after exit_           |

Put into simple terms: a function/method annotated with
`ARENE_TSA_REQUIRES[_SHARED]` is declaring it **will not** attempt to _acquire_
the _capabilities_ that _guard_ the _resources_ it might access during its
execution. Whereas one annotated with `ARENE_TSA_EXCLUDES` is declaring it
**will** _acquire_ the _capabilities_ that _guard_ the _resources_ it attempts
to access during its execution. An example is shown below:

```cpp
class deadlock_safe {
public:
  // This is the public, thread-safe entrypoint to updating the message.
  // It is going to acquire the update_guard_ capability, and thus _excludes_ the
  // caller from already holding it.
  void update_message(std::string msg) ARENE_TSA_EXCLUDES(update_guard_) {
    // the capability is acquired, it will be freed when the scope exits
    auto lckg = arene::base::lock_guard(update_guard_);
    // the internal, non-thread-safe implementation is called
    update_message_impl(std::move(msg));
  }

private:
    // This is the internal, non-thread-safe entrypoint to updating the message.
    // It is not going to attempt to _acquire_ the update_guard_ capability, and
    // thus _requires_ the caller to already hold it.
    void update_message_impl(std::string msg) ARENE_TSA_REQUIRES(update_guard_) {
        msg_ = std::move(msg);
    }

  // The capability that guards updating content, in this case a mutex.
  arene::base::mutex update_guard_;

  // msg_ is guarded by update_guard_, so it must be locked to access msg_.
  std::string msg_ ARENE_TSA_GUARDED_BY(update_guard_);
};
```

This two-step design of having a public, _capability excluding_ method that
simply acquires the capability and then dispatches to a private, _capability
requiring_ implementation becomes a common pattern to avoid deadlock while
allowing functionality reuse; other methods which also hold `update_guard_` can
safely call `update_message_impl()` without fear of deadlock or data races. If
`update_message()` is incorrectly called while already holding `update_guard_`,
a compiler error is raised instead of deadlock resulting. Similarly, if
`update_message_impl()` is called without holding `update_guard_`, a compiler
error is raised instead of introducing a data race.

\note In this example, even if `update_message_impl()` was not annotated with
`ARENE_TSA_REQUIRES(update_guard_)`, a compiler error would still be issued if
the calling thread did not hold `update_guard_`; this is because `msg_` is
annotated as `TSA_GUARDED_BY(update_guard_)`. However this compiler error would
point to the wrong place: it would point to the access of the guarded resource,
`msg_ = std::move(msg)`. This isn't where the actual defect is though, since
`update_message_impl()` is supposed to be able to access the resource. With the
`ARENE_TSA_REQUIRES(update_guard_)` annotation, the compiler error points to the
correct place: the call to `update_message_impl()` without holding the needed
`update_guard_` _capability_.

## Suppressing TSA Diagnostics {#suppressing-tsa-diagnostics}

There are occasions where due to [known limitations](#known-limitations-of-tsa)
in TSA, a function/method may need to be excluded from TSA analysis. This can be
achieved using the `ARENE_TSA_NO_THREAD_SAFETY_ANALYSIS` annotation, like so:

```cpp
void hits_known_limitation() ARENE_TSA_NO_THREAD_SAFETY_ANALYSIS {
  // do something that cannot be validated by TSA as safe.
}
```

This excludes the function from TSA analysis _entirely_.

\note Unlike other TSA annotations, `ARENE_TSA_NO_THREAD_SAFETY_ANALYSIS` is
part of the **implementation** rather than the interface of a function/method.
It thus should be placed with the definition -in a `.cpp` file for non-inline
code- rather than the declaration.

## Known Limitations Of TSA {#known-limitations-of-tsa}

There are a number of limitations in the current implementation of TSA, beyond
the most obvious of currently only being part of the clang compiler and required
explicitly annotated _capabilities_ to work. A bulleted summary list follows,
for full descriptions of each please see the
[official documentation](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#limitations):

### [Lexical Scoping Limitations](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#lexical-scope) {#lexical-scoping-limitations}

TSA annotations on methods are parsed at the same time as method bodies, and
thus are subject to the same lexical scoping limitations with respect to
use-before-declaration when referencing a named _capability_.

### [Private Mutexes](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#private-mutexes) {#private-mutexes}

TSA annotations respect visibility restrictions. A private mutex member cannot
be named in an annotation in a context that would not normally have visibility
to that member. The `ARENE_TSA_RETURN_CAPABILITY(x)` annotation can provide a
workaround, see the linked documentation for details.

### [No conditionally held locks](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#no-conditionally-held-locks) {#no-conditionally-held-locks}

TSA's analysis is not path-sensitive to branches dependant on runtime state, so
situations where a capability may be _conditionally_ held at a given point in a
program may cause spurious diagnostics.

\note Remember that GoogleTest's `ASSERT_XXX` macros `return` on failure. This
can result in unexpectedly hitting the conditional locking limitations in unit
tests if the _release_ of a capability is done only if an assert passes, rather
than unconditionally via a _scoped capability_ tied to the scope containing the
assert.

### [No checking inside ctors/dtors](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#no-checking-inside-constructors-and-destructors) {#no-checking-inside-ctorsdtors}

As ctors and dtors can generally assumed to be executing in the context of a
single thread for a given instance, and typically have to initialize or destroy
guarded resources, all ctors and dtors are implicitly treated as if annotated
with `ARENE_TSA_NO_THREAD_SAFETY_ANALYSIS`.

### [No inlining](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#no-inlining) {#no-inlining}

TSA is strictly intra-procedural in the same way as type-checking: it relies
purely on the declared attributes of variables/members/methods/functions, it
does not examine their implementations. This can cause issues when type-erasure
or similar indirection hides the nessisary calls to _acquire_ or _release_ a
given capability: for example if a callback is consumed as
`arene::base::function_ref<void() const>`, and that callback _acquires_ or
_releases_ a capability referenced by the outer scope, TSA will not see this
happen and spurious warnings may result.

\note For interfaces which consume a generic `template<typename Func>` or
similar, and thus do _not_ type erase the signature of the callback, annotating
the input lambda as appropriate will "do the right thing." Annotations on
lambdas look like this:
`[mtx_, resource_]() ARENE_TSA_EXCLUDES(mtx_) { /* acquire mtx, use resource */ }`

### [No alias analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#no-alias-analysis) {#no-alias-analysis}

Other than inside a `SCOPED_CAPABILITY`, TSA cannot see through pointer aliases
to capabilities to understand they are equivalent:

```cpp
arene::base::mutex mtx;
int guarded ARENE_TSA_GUARDED_BY(mtx) = 0;
auto* alias = &mtx;
alias->lock(); // actually locks mtx, but TSA can't see that alias == mtx.
guarded = 1; // compiler error, capability mtx is not seen as held.
```

### [Order of capability acquisition is unimplemented](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#acquired-before-and-acquired-after-are-currently-unimplemented) {#order-of-capability-acquisition-is-unimplemented}

TSA defines attributes for establishing an order that _capabilities_ must be
_acquired_, but they are currently unimplemented in the actual analysis. To
avoid confusion and a false sense of security, \arene_base has not exposed them.

### No Support For Recursive Mutexes {#no-support-for-recursive-mutexes}

There is currently no mechanism to support recursive _capability_ _acquisition_.
While this limitation is not stated explicitly in the documentation, it is
likely due to the same reasons that conditional locking is not supported:
recursion must necessarily be conditional to not recurse endlessly, and that
condition is runtime state dependent.

That said, explicit recursion is against MISRA/AutoSAR guidelines, and nearly
all other need for recursive mutexes can be avoided by following the
public/private indirection pattern mentioned in the
[function annotation](#annotating-functionsmethods-as-requiringexcluding-capabilities)
section.

### No Direct Support For `unique_lock` {#no-direct-support-for-unique_lock}

There are several limitations that combine to make it impossible to correctly
annotate `std::unique_lock` or an equivalent type to work with TSA:

1. A type cannot be both a _capability_ and a _scoped capability_ at the same
   time.
2. Even if it could, the ["no aliases"](#no-alias-analysis) limitations means
   TSA cannot follow _capabilities_ which are moved, as this functionally gives
   them a new name. `std::unique_lock` effectively _is_ an alias for a mutex;
   that's its reason d'etre. You can move them, swap them, re-assign them. All
   of these are _runtime_ state considerations that TSA simply cannot follow
   based on _static_ program information.
3. Less importantly, there is no support for
   [recursive mutexes](#no-support-for-recursive-mutexes), and
   `std::unique_lock` allows recursive mutexes.

This can make `std::condition_variable` difficult to use correctly under TSA, as
`std::unique_lock` is generally the best practice for the lock type to pass to
it. There are two possible workarounds:

- Do not use `std::unique_lock`. In this case, due to the lack of
  [inlining](#no-inlining), the lambda passed to `std::condition_variable` must
  be annotated as `ARENE_TSA_REQUIRES(mtx)`, where `mtx` is the _capability_
  which _guards_ the _resources_ accessed in the lambda.
- Use `arene::base::scoped_lock_assertion` to manually set TSA into the correct
  state without actually changing the lock state:

  ```cpp
  arene::base::mutex mtx;
  int value ARENE_TSA_GUARDED_BY(mtx);
  std::condition_variable cv;

  int compute_value() {
    std::unique_lock<arene::base::mutex> lock(mtx);
    arene::base::scoped_lock_assertion<arene::base::mutex> assertion(mtx);
    cv.wait(lock, [](){
      arene::base::scoped_lock_assertion<arene::base::mutex> assertion(mtx);
      return value != 0;
    })
    return 100 / value;
  }
  ```

### No direct support for `std::lock` and `scoped_lock` {#no-direct-support-for-std-lock-and-scoped_lock}

The ["no aliases"](#no-alias-analysis) limitation also prevents reasonably
annotating `std::lock` and `std::scoped_lock`. Unfortunately users will have to
fall back to individual `arene::base::lock_guard` usages. The need for the
deadlock prevention mechanism of `std::lock` is lessened by the fact that TSA
would have already detected the deadlock at compile time.

## Annotating Types As Providing A Capability {#annotating-types-as-providing-a-capability}

\note Users should rarely need to introduce their own capability types.
\arene_base provides pre-annotated implementations of common concurrency
primitives in its \ref mutex_subpackage mutex subpackage. Prefer those where
possible.

There are 11 annotations that are used to declare a type as providing a
capability:

|               Annotation                | Annotation Applied To |                                                                                                               Description                                                                                                                |
| :-------------------------------------: | :-------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
|        `ARENE_TSA_CAPABILITY(x)`        |        Classes        |                                                                                               Annotates a class as providing a capability                                                                                                |
|      `ARENE_TSA_SCOPED_CAPABILITY`      |        Classes        |                                                                                      Annotates a class as providing a capability tied to a _scope_                                                                                       |
|        `ARENE_TSA_ACQUIRE(...)`         |   Functions/Methods   |                                                            Annotates a function/method as acquiring **exclusive** access to the named capability(s) before the function exits                                                            |
|     `ARENE_TSA_ACQUIRE_SHARED(...)`     |   Functions/Methods   |                                                             Annotates a function/method as acquiring **shared** access to the named capability(s) before the function exits                                                              |
|      `ARENE_TSA_TRY_ACQUIRE(...)`       |   Functions/Methods   | Annotates a function/method as one which will attempt to acquire **exclusive** access to the named capability(s) before the function exits. The return value shall be a boolean indicating success or failure to acquire the capability. |
|   `ARENE_TSA_TRY_ACQUIRE_SHARED(...)`   |   Functions/Methods   |  Annotates a function/method as one which will attempt to acquire **shared** access to the named capability(s) before the function exits. The return value shall be a boolean indicating success or failure to acquire the capability.   |
|        `ARENE_TSA_RELEASE(...)`         |   Functions/Methods   |                                                            Annotates a function/method as releasing **exclusive** access to the named capability(s) before the function exits                                                            |
|     `ARENE_TSA_RELEASE_SHARED(...)`     |   Functions/Methods   |                                                             Annotates a function/method as releasing **shared** access to the named capability(s) before the function exits                                                              |
|    `ARENE_TSA_RELEASE_GENERIC(...)`     |   Functions/Methods   |                                                     Annotates a function/method as releasing access to the named capability(s), regardless of exclusivity, before the function exits                                                     |
|    `ARENE_TSA_ASSERT_CAPABILITY(x)`     |   Functions/Methods   |             Annotates a function/method as one which will check if the calling thread has **exclusive** access to the named capability(s) before entering the function, and if it does not would cause program abandonment.              |
| `ARENE_TSA_ASSERT_SHARED_CAPABILITY(x)` |   Functions/Methods   |               Annotates a function/method as one which will check if the calling thread has **shared** access to the named capability(s) before entering the function, and if it does not would cause program abandonment.               |

The usage of these annotations is best demonstrated through a simple example,
which decorates the interface of a basic mutex that supports exclusive and
shared access, taken from the
[TSA documentation](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#mutex-h):

```cpp
class ARENE_TSA_CAPABILITY("mutex") mutex {
public:
  // Acquire/lock this mutex exclusively.  Only one thread can have exclusive
  // access at any one time.  Write operations to guarded data require an
  // exclusive lock.
  void lock() ARENE_TSA_ACQUIRE();

  // Acquire/lock this mutex for read operations, which require only a shared
  // lock.  This assumes a multiple-reader, single writer semantics.  Multiple
  // threads may acquire the mutex simultaneously as readers, but a writer
  // must wait for all of them to release the mutex before it can acquire it
  // exclusively.
  void reader_lock() ARENE_TSA_ACQUIRE_SHARED();

  // Release/unlock an exclusive mutex.
  void unlock() ARENE_TSA_RELEASE();

  // Release/unlock a shared mutex.
  void reader_unlock() ARENE_TSA_RELEASE_SHARED();

  // Generic unlock, can unlock exclusive and shared mutexes.
  void generic_unlock() ARENE_TSA_RELEASE_GENERIC();

  // Try to acquire the mutex.  Returns true on success, and false on failure.
  bool try_lock() ARENE_TSA_TRY_ACQUIRE(true);

  // Try to acquire the mutex for read operations.
  bool reader_try_lock() ARENE_TSA_TRY_ACQUIRE_SHARED(true);

  // Assert that this mutex is currently held by the calling thread.
  void assert_held() ARENE_TSA_ASSERT_CAPABILITY(this);

  // Assert that is mutex is currently held for read operations.
  void assert_reader_held() ARENE_TSA_ASSERT_SHARED_CAPABILITY(this);

  // For negative capabilities.
  const Mutex& operator!() const { return *this; }
};
```

A _scoped capability_ is one which provides RAII semantics, in a manner similar
to `std::lock_guard`. _scoped_capabilities_ generally _acquire_ a _capability_
in their constructor, and _release_ it in their destructor. The most basic
example looks like this:

```cpp
// A very basic lock gard
template<typename Mutex>
class ARENE_TSA_SCOPED_CAPABILITY lock_guard{

public:
    // Acquire exclusive access to the mutex in the ctor.
    lock_guard(Mutex* mtx) ARENE_TSA_ACQUIRE(mtx): mtx_(mtx) {
        mtx_->lock();
    }

    // Releases exclusive access to the mutex in the dtor. We don't need to
    // specify the capability because ARENE_TSA_SCOPED_CAPABILITY implicitly
    // binds all acquired capabilities to `this`.
    ~lock_guard() ARENE_TSA_RELEASE() {
        mtx_->unlock();
    }

private:
    Mutex* mtx_;
};
```
