# Software Design Description

The Memory component contains byte-order utilities, size and alignment information, and a PMR-aware ring buffer.
The utilities are invoked directly by their callers.
The ring buffer uses a caller-supplied polymorphic allocator and preserves its construction-time capacity.
