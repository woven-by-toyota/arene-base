<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page intrusive intrusive: Intrusive Containers

<!-- markdownlint-enable MD041 -->

The `intrusive` sub-package provides containers that use fields in the stored
elements themselves to link them into the data structure, thus avoid dynamic
memory allocation.

The public headers are

```{.cpp}
#include "arene/base/intrusive/queue.hpp"
#include "arene/base/intrusive/priority_queue.hpp"
```

The Bazel target is

```text
//:intrusive
```

## Intrusive vs. Non-Intrusive Containers {#intrusive-vs-non-intrusive-containers}

<!-- prettier-ignore -->
Most containers, such as those in the standard library or in other libraries
such as [`inline_container`](\ref inline_container) are **non-intrusive**. This
means that they internally manage memory for the elements they hold, and consume
_copies_ of elements when they are inserted. In general, any element can be
placed into a _non-intrusive_ container without modification. This style of
container is thus easy to use and difficult to use wrong, but comes with the
tradeoff of needing to copy elements into and out of it, as well as needing to
allocate memory on construction and on growth if the container supports growing.

In comparison, an **intrusive** container does not manage any storage for
holding its elements. Instead, the elements themselves are modified with
additional content, colloquially called _hooks_, needed to support linking
relationships in the intrusive container. This is what gives this category of
containers their name; they are _intrusive_ into the definition of the elements
that will be inserted into them. When an element is inserted into an intrusive
container, these hooks are set. When it is removed, they are cleared. Intrusive
containers thus do not require copying (or moving) elements, nor do they require
allocating additional storage for them.

The advantages of an intrusive container are:

- The intrusive container itself has a very small memory footprint; generally
  only a pointer to the first element and a size.
- The container may grow unboundedly as elements are inserted without allocating
  memory.
- They can generally have strong exception guarantees (possibly being completely
  `noexcept`) which would not be possible with a container that needs to
  allocate storage for elements or copy elements into it.
- They can work with elements that can be nether moved nor copied.

The downsides of intrusive containers are:

- The requirement to intrusively modify the definition of an element's type in
  order to be used with an intrusive container. This also increases the memory
  footprint of the element type even if it's not used inside a container.
- The lifetime of elements are now managed outside of the control of the
  container; users have to be very careful to ensure an element is never
  destroyed while it still belongs to a container.
- As intrusive containers generally cannot reason about the relative layout in
  memory of their elements, they cannot leverage contiguous memory assumptions
  to efficiently provide random-access to elements. However, they do generally
  have better iteration performance than equivalent non-contiguous ,
  non-intrusive containers as they only need a single level of indirection (the
  elements _are_ the nodes, rather than the nodes point to elements)
- Intrusive containers are move-only: they cannot be copied or assigned to. This
  is because there is no reasonable definition for "copying" a container that
  does not own the storage for its own elements.

\warning It bears repeating: once inserted in a container, an element **must
not** be moved or destroyed until it has been removed from that container, as
the links will connect to that **specific instance** of the element.

## Adding Intrusive Container Hooks To Elements {#adding-intrusive-container-hooks-to-elements}

In general, the _hooks_ needed for insertion into an intrusive container are
added to elements via inheritance; each container type has a base class which
elements derive from, and this injects private members into the type. For
example, the _hook_ class for `arene::base::intrusive::queue` is
`arene::base::intrusive::singly_linked_node`.

In addition, these _hook_ classes consume an optional template parameter: `Tag`,
which is a simple _tag-type_. The `Tag` is used to allow an element to be
inserted into multiple different intrusive containers of the same type at the
same time. If `Tag` is omitted, the _default tag_ is assumed. Similarly, each
intrusive container type has an optional `Tag` template parameter, to specify
which elements may be inserted into it. If it is omitted, the _default tag_ is
used.

Please see the descriptions of the individual container types below for
additional details and examples.

\note As you may have guessed, this means that the memory footprint of the
element grows for _each unique `Tag`_ that is used to inject hooks. Only use
non-default tags if you are absolutely sure you need them.

## The Intrusive Containers {#the-intrusive-containers}

### Queue {#queue}

`arene::base::intrusive::queue` is a _queue_ data structure: you push elements
on the back with `arene::base::intrusive::queue::push`, pop them off the front
via `arene::base::intrusive::queue::pop`, and the only elements access is to the
head an tail of the queue via `arene::base::intrusive::queue::front` and
`arene::base::intrusive::queue::back`, respectively.

Elements must derive from the _hook_ class
`arene::base::intrusive::singly_linked_node`.

```{.cpp}
class some_data : public arene::base::intrusive::singly_linked_node<> {
  int value_{0};
 public:
  constexpr some_data(int value = 0) : value_{value} {}
  constexpr int value() const noexcept { return value_; }
};

void foo() {
  some_data abc(42);
  some_data def(99);
  some_data ghi(-123);

  arene::base::intrusive::queue<some_data> queue;

  queue.push(abc);
  queue.push(def);
  queue.push(ghi);

  while(!queue.empty()){
    auto& next = queue.front();
    std::cout<<"val = "<<next.value();
    queue.pop();
  }
}
```

Here, `some_data` derives from `arene::base::intrusive::singly_linked_node` in
order to provide the necessary fields for linking into the queue. `foo` then
creates 3 instances of the data element, pushes them onto the queue, and then
pops them off the queue in a loop.

### An Intrusive Priority Queue {#an-intrusive-priority-queue}

`arene::base::intrusive::priority_queue` is a is a max-heap-tree-based intrusive
priority queue data structure. It forms a logical priority queue, allowing
elements to be added/removed to the priority queue and providing access to the
element with the highest priority (depending on the comparator). The default
comparator is `std::less`, but another comparator could be provided to use a
different ordering; if `comp(a,b)` is `true` then `a` has _lower_ priority than
`b`.

Elements can be pushed in any order with
`arene::base::intrusive::priority_queue::push`, and popped in priority order
with `arene::base::intrusive::priority_queue::pop`. Also, elements in the queue
can be erased (with `arene::base::intrusive::priority_queue::erase`). Only the
"top" element, with the highest priority, can be accessed directly, using
`arene::base::intrusive::priority_queue::top`.

Elements must derive from the _hook_ class
`arene::base::intrusive::binary_tree_node`.

The following example shows elements being removed in _priority order_, despite
being pushed in a different order.

```{.cpp}
struct some_data : public arene::base::intrusive::binary_tree_node<> {
    some_data(int value) : value_(value) {}
    int value_;

    constexpr friend bool operator<(const some_data& lhs,
                                    const some_data& rhs) noexcept {
        return lhs.value_ < rhs.value_;
    }
};

arene::base::intrusive::priority_queue<some_data> pq;
std::vector<some_data> elements{
  some_data{1},some_data{2},some_data{3},some_data{4}};

pq.push(nodes[3]); // Push in random order
pq.push(nodes[1]);
pq.push(nodes[0]);
pq.push(nodes[2]);


ASSERT_EQ(queue.top().value_, 4); // The highest priority is 4
queue.pop(); // Remove elements with the highest priority
ASSERT_EQ(queue.top().value_, 3);
queue.pop();
queue.erase(node2); // Erase the node2
ASSERT_EQ(queue.top().value_, 1); // Now the highest priority is 1
```

Note that `node` itself is still valid after being removed with `erase`, it is
just no longer in the queue. Iterators to other elements also remain valid.
