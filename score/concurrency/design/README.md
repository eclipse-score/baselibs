# Design

The overall design goals are to provide an interface to execute
any C++ callable type in a parallel context. How this parallel context
looks like shall be then implementation specific. Meaning, we want to support
a runtime decision for the user which parallel context shall be used.

The following prose text describes the [Structural View](./structural_view.puml).

<img alt="Structural View" src="https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/eclipse-score/baselibs/refs/heads/main/score/concurrency/design/structural_view.puml">

## Tasks

One major functionality of our executors is the queuing of callables until they can be executed.
These callables need some kind of generic interface (type-erasure) that our generic
implementations can operate with them. Normally we could use `std::function` for this use-case.
Unfortunate `std::function` is allocating memory on the heap, which is prohibited in a safety context.
For this reason we introduced `SimpleTask` that is used to with a PMR-aware factory to enable usage without
heap allocations. Meaning, the storage will not be allocated in the heap, but wherever
the PMR is already allocated. In most cases this will be on the stack.

For users that require custom implementations of tasks we provide the interface `Task` that can be used to implement a
custom task. Basically, here the user has to take care himself that the implementation is PMR aware. By providing a
`shared_ptr` into the `Executor` we can ensure that the lifetime of the custom task is long enough.

## Executor

The `Executor` represents the overall interface. It offers three public methods to enqueue any
[callable](https://en.cppreference.com/w/cpp/named_req/Callable) for execution. The two
possibilities are `post` (Callable and a `std::shared_ptr<Task>`) which will ignore the callable return type and
can be seen as _fire and forget_.
The second possibility is `submit`, which respects the return type and wraps it in a `std::future`.
While it is possible to pass any kind of callable, we formulate a requirements towards its arguments.
Namely, that the first argument of the callable needs to be of kind `score::cpp::stop_token`. This is necessary
to enforce that every developer executing tasks in parallel, will think of how and if his thread
can be interrupted in case of a shutdown request. If he has only a super short running task that
cannot be stopped in between, he can ignore the argument. But then this is a specific decision instead
of the current situation where people just forget about this case. The `score::cpp::stop_token` parameter
will be provided by the `Executor` in the end. So the user does not need to care about a
respective `score::cpp::stop_source`.

At the end the goal is to repackage the provided callable as `Task` in the `do_post` and respective
`do_submit` function within the `Executor`. While then each implementation can decide how
it `enqueue`s and execute the single tasks.

Overall the idea of the `shutdown` function is to set all associated `score::cpp::stop_token`s mentioned
earlier to true. This is again an implementation detail of the underlying execution strategy.

## ThreadPool

The first execution strategy that shall be implemented is a plain `ThreadPool`. Upon creation
a specified number of threads will be created. Each thread will query the task queue within
the `ThreadPool` for the next task to execute.
The `enqueue` function will then put a task into the task pool which will act as FIFO-Queue.

Since the `ThreadPool` needs to do dynamic memory allocation for the FIFO-Queue, we make it
allocator aware. This way - if necessary e.g. in an ASIL-B context -, the use can override the default
HEAP allocation with any custom one.

## std::jthread aka score::cpp::jthread

For our implementation of the `Executor` and `ThreadPool` we will rely on a C++20 standard library feature
called [`std::jthread`](https://en.cppreference.com/w/cpp/thread/jthread).
The two main features of `std::jthread` are auto join support on destruction and the possibility to
request the thread to stop (aka `std::stop_token` handling). While we were first thinking of
implementing a custom API to handle the stop requests, the official C++ API seems most reasonable for
our use case (and will ensure future proofness).

Since we cannot rely on C++20 features right now, we use the implementation `score::cpp::jthread` within
[AMP](broken_link_g/swh/amp).

## SynchronizedQueue

The `SynchronizedQueue` provides the ability to use a synchronized queue for use from either one or multiple threads.
Queue can be used by one receiver, and update by multiple sender.
To use a queue, you need to set the queue size and the timeout that will be used by the receiver
when trying to read from the queue. If the queue remains empty within the specified timeout, an empty value is returned.
When the queue is full and the sender tries to write a new element to the queue,
the method `Push()` returns false, which indicates that the write attempt was unsuccessful.

## Timed Executor

There is regularly the use case to execute some task at a given time, or in a given period. To tackle this, we
introduced `DelayedTask` (specific time-point) and `PeriodicTask` (in an interval).
The `TimedExecutor` is another kind of `Executor` that takes care of `TimedTasks`. The basic idea is that a
`TimedTask` (e.g. like `PeriodicTask`) has another execution point. Which is why any implementation of the
`TimedExecutor` would query `GetNextExecutionPoint()` of a `TimedTask`. This way, we can decouple any different
implementation of `TimedTasks` from an `TimedExecutor`.

Currently, we have one implementation of the `TimedExecutor` interface, which is the `ConcurrentTimedExecutor`.
It implements the Earliest Start Time First scheduling algorithm. Basically, a user is able to provide a normal
`Executor`, like a `ThreadPool`. Then each possible concurrent thread is used to wait and execute `TimedTasks`.
