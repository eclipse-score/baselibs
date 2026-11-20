# Meyer singleton

This package is for code that needs a process-wide singleton whose static destruction order remains reliable when multiple threads initialize related static objects.
It is not a general replacement for an ordinary function-local static object.

## Why this utility exists

C++ serializes function-local static initialization.
If one thread is initializing a static object, another thread that reaches the same declaration waits for initialization to finish.
C++ also destroys static objects in the reverse of their completed initialization order.

The implementation works around observed GCC behavior tracked in [GCC bug 99613](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99613).
Consider two static objects, `X` and `Y`, where every thread observes `X` as initialized before `Y`.
GCC can still register them for destruction in an order that destroys `X` before `Y`.
If the destructor of `Y` accesses `X`, the process can crash because `X` is no longer alive.

Remove this workaround only after all supported GCC toolchains no longer exhibit the destruction-order issue and the concurrent lifetime tests confirm the replacement behavior.
