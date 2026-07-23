# ABI-dependent definitions

## Problem description

The C++ specification defines only the behavior that implementations should support,
not the internal layout of objects, for example, bellow we see how to popular C++
implementations represent the `std::string_view` type:

```
libstdc++ (used by GCC): [len][data]
libc++ (used by clang): [data][len]
```

That means that any FFI integration between Rust and C++ has to be aware of which
implementation of the standard lib is being used by the C++ code, otherwise mapping
would not match, and that could lead to Undefined Behavior (UB).

This crate aims to handle these ABI differences for stdlib types.

## Second-order problem definition

Imagine that one would have many of such types, and would like to support a wide range of implementations.
Clearly, the amount of work to maintain the compatibility would be non-trivial.

Therefore there's a second-order problem:
How to maintain the compatibility if the underlying ABIs change?

For tackling this problem, the following proposals have emerged:
1. Propose to the Rust community a sibling library to `libc`. This library currently exposes
   common C types to Rust, in a binary-compatible way. We could propose the creating of,
   say, `libcpp`, that would do the same for C++. A possible technical hurdle, though, is
   that such a lib would most likely need the functionality proposed for
   [`std::is_trivially_relocatable_v<T>`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1144r6.html), that was not yet accepted, not even for C++26.
   * Pros: Maintenance burden is shared with open-source community.
   * Cons: Some BMW-internal types might still need an extra layer of maintenance.
1. Introspect somehow into generated code using a particular version of the lib, and
   automatically detect it's layout, and then use code generation tools to create
   the glue code on compiling time.
   * Pros: Would avoid manual work for maintaining changes.
   * Cons: Requires investment to create and test such tooling (and tooling maintenance).
1. Before adopting a new lib, performing an ABI compliance check (see section [How to Detect ABI Breaks](#How_to_Detect_ABI_Breaks)), and assess the impact on possible internally-maintained
   mapping. Then, given the impact, decide if we would like to make the necessary changes
   in-house or hire external workforce.
1. Collaborating new types to [CXX project](https://cxx.rs/) project directly, as this is
   the glueing layer that we are using for the FFI interoperability. However, Markus commented
   that the process can take too much time, as he received feeback about a proposal he made
   to this project only after one and a half year.

### Assessing the size of the problem

On practice the problem would only occurr when:
* Adopting a completely new inplementation of stdlib (very unlikely);
* Changing toolchain versions *and* having incompatible implementations (very infrequent);
* If the number of mapped types would be high (unknown, currently we know we need
std::string_view and std::variant).

As adopting completely new implementation is a bit unforeseeable for the time being,
let's dwelve into changes to the currently used ones.

#### ABI breaking changes on the history of GNU's libstdc++

Here’s a detailed summary of all known **incompatible ABI changes in GNU libstdc++ across GCC versions**, based on official documentation and historical notes:

1.  **GCC 3.x (2002–2003)**
    *   **Event:** Switch to the **Itanium C++ ABI**.
    *   **Impact:** Complete ABI break from older GCC 2.x releases. All binaries had to be recompiled.
    *   **Reason:** Adoption of industry-standard ABI for name mangling, vtables, RTTI, and exception handling.
    *   **Mitigation:** None; full rebuild required.    [\[gcc.gnu.org\]](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html)

***

2.  **GCC 5.1 (2015)**
    *   **Event:** Introduction of **dual ABI** for C++11.
    *   **Impact:** `std::string` and `std::list` changed layout to comply with C++11 requirements (e.g., short string optimization, constant-time `size()` for `list`).
    *   **Reason:** C++11 standard forbids copy-on-write strings and requires different semantics for containers.
    *   **Mitigation:** Environment variable `_GLIBCXX_USE_CXX11_ABI` allows choosing old vs new ABI for compatibility.
    *   **Breakage:** Passing `std::string` or `std::list` between code compiled with different ABI settings causes undefined behavior.    [\[askubuntu.com\]](https://askubuntu.com/questions/770358/how-should-i-handle-abi-incompatability-between-gcc-4-9-and-gcc-5), [\[stackoverflow.com\]](https://stackoverflow.com/questions/67839008/please-explain-the-c-abi)

***

3.  **GCC 7.x (2017)**
    *   **Event:** Changes in `std::function` and `std::allocator` internals for performance and standard compliance.
    *   **Impact:** Minor ABI break for some template instantiations, especially when mixing old/new DSOs.
    *   **Mitigation:** Mostly backward-compatible due to symbol versioning, but edge cases exist.    [\[maskray.me\]](https://maskray.me/blog/2023-06-25-c++-standard-library-abi-compatibility)

***

4.  **GCC 11.x (2021)**
    *   **Event:** Adjustments for C++20 features (e.g., `std::span`, ranges).
    *   **Impact:** Some internal changes in debug mode and allocator propagation could break ABI in rare cases.
    *   **Mitigation:** Symbol versioning reduces most issues, but mixing DSOs compiled with different major versions can fail.    [\[maskray.me\]](https://maskray.me/blog/2023-06-25-c++-standard-library-abi-compatibility)

***

##### **General Policy**

*   **Forward compatibility:** Since GCC 3.4, libstdc++ tries to maintain forward compatibility (old binaries work with newer libstdc++).
*   **Backward compatibility:** Not guaranteed; new binaries may fail with older libstdc++.
*   **Symbol versioning:** Introduced to allow coexistence of old and new symbols in the same shared library.
*   **Dual ABI:** Only for `std::string` and `std::list` since GCC 5.    [\[gcc.gnu.org\]](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html), [\[stackoverflow.com\]](https://stackoverflow.com/questions/67839008/please-explain-the-c-abi)

#### ABI breaking changes on the history of LLVM's libc++

Here’s the **analysis of ABI-breaking changes in LLVM’s libc++ implementation**, similar to what we did for GNU libstdc++:

1.  **Initial Design (libc++ inception, \~2011)**
    *   **Event:** libc++ introduced as a new standard library implementation for C++11 and beyond.
    *   **Impact:** No compatibility with older libstdc++ ABIs; a clean break was intentional to support modern features like rvalue references and short string optimization.
    *   **Reason:** Performance and correctness goals required abandoning Copy-On-Write strings and other legacy layouts.    [\[releases.llvm.org\]](https://releases.llvm.org/18.1.8/projects/libcxx/docs/index.html)

***

2.  **ABI Versioning Mechanism (Introduced early, still active)**
    *   **Event:** libc++ introduced **ABI versioning** via `LIBCXX_ABI_VERSION` (default = 1 for stable ABI, 2 for experimental ABI).
    *   **Impact:** Users can opt into ABI-breaking changes by building with `LIBCXX_ABI_VERSION=2` or `LIBCXX_ABI_UNSTABLE`.
    *   **Reason:** Allows incremental adoption of performance improvements without breaking existing binaries.
    *   **Mitigation:** ABI flags like `_LIBCPP_ABI_ALTERNATE_STRING_LAYOUT`, `_LIBCPP_ABI_OPTIMIZED_FUNCTION`, etc., are gated behind version macros.    [\[releases.llvm.org\]](https://releases.llvm.org/18.1.8/projects/libcxx/docs/DesignDocs/ABIVersioning.html), [\[releases.llvm.org\]](https://releases.llvm.org/21.1.0/projects/libcxx/docs/ABIGuarantees.html)

***

3.  **LLVM 19 → LLVM 20 (March 2025)**
    *   **Event:** **Container layout changes** for `std::unordered_map`, `std::unordered_set`, `std::deque`, and others when using empty allocators or comparators.
    *   **Impact:** Size and layout of these containers changed due to replacing “compressed pair” emulation with `[[no_unique_address]]`.
    *   **Reason:** Performance and conformance improvements.
    *   **Breakage:** Any type embedding these containers alongside empty allocators saw size changes, breaking ABI.
    *   **Mitigation:** Planned rollback in LLVM 21 for Clang builds; GCC builds remain on LLVM 20 ABI until compiler fixes land.    [\[github.com\]](https://github.com/llvm/llvm-project/issues/154985)

***

4.  **LLVM 21 (August 2025)**
    *   **Event:** **std::deque size changed** (example: from 48 to 56 bytes in certain allocator configurations).
    *   **Impact:** Mixing LLVM 21 and earlier versions in DSOs caused layout mismatches.
    *   **Reason:** Internal restructuring for allocator handling and optimizations.    [\[github.com\]](https://github.com/llvm/llvm-project/issues/154146)


##### **ABI Flags That Cause Breakage**

*   `_LIBCPP_ABI_ALTERNATE_STRING_LAYOUT` → Changes `std::string` internal buffer alignment.
*   `_LIBCPP_ABI_OPTIMIZED_FUNCTION` → Restructures `std::function` for performance.
*   `_LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION` → Shrinks `std::variant` index type.
*   `_LIBCPP_ABI_NO_ITERATOR_BASES` → Removes iterator base classes, affecting padding.
    These are opt-in via vendor configuration or unstable ABI builds.    [\[releases.llvm.org\]](https://releases.llvm.org/21.1.0/projects/libcxx/docs/ABIGuarantees.html)


##### **General Policy**

*   **Stable ABI:** Default (`LIBCXX_ABI_VERSION=1`) aims to keep layout and symbol stability across releases.
*   **Unstable ABI:** Opt-in for aggressive optimizations and new layouts.
*   **Symbol versioning:** libc++ does **not** use symbol versioning like libstdc++, so mixing DSOs compiled with different ABI versions is unsafe.    [\[releases.llvm.org\]](https://releases.llvm.org/18.1.8/projects/libcxx/docs/DesignDocs/ABIVersioning.html), [\[maskray.me\]](https://maskray.me/blog/2023-06-25-c++-standard-library-abi-compatibility)

##### **Summary Table**

| LLVM Version | ABI Change               | Notes                  |
| ------------ | ------------------------ | ---------------------- |
| Initial      | Clean break              | New library for C++11  |
| ABI v2       | Experimental ABI         | Opt-in via build flags |
| 19 → 20      | Container layout changes | `unordered_*`, `deque` |
| 21           | `std::deque` size change | Allocator-related      |


#### **How to Detect ABI Breaks**

*   Use tools like **ABI Compliance Checker** or **abi-laboratory.pro** for detailed reports.    [\[lvc.github.io\]](https://lvc.github.io/abi-compliance-checker/)

Also, the PR introduces unit tests that aim to break if there are incompatibilities between
the representation provided by the lib and the internal mapping expected for it.
Both the object sizes and structure is asserted by them, so there's high chance that by
just running the unit tests against a new toolchain would already be enough to detect
incompatible changes.

#### **Which further types could be needed to maintain**

Currently we identified the need for the following types (already provided on this PR):
* `std::string_view`
* `std::variant`

Also, the open-source framework [CXX project](https://cxx.rs/) that we are using writing
glue code between the languages already provides facilities for the following types:
* String — rust::String
* &str — rust::Str
* String — rust::String
* &[T], &mut [T] — rust::Slice<T>
* CxxString — std::string
* Box<T> — rust::Box<T>
* UniquePtr<T> — std::unique_ptr<T>
* SharedPtr<T> — std::shared_ptr<T>
* Vec<T> — rust::Vec<T>
* CxxVector<T> — std::vector<T>
* *mut T, *const T raw pointers
* Function pointers
* Result<T>

With the difference that this last `Result<T>` is a way of C++ code to deal with Rust Result
objects, whereas what has been provided internally was a way to turn a BMW Result type into
a Rust Result type, such that Rust code can use the return value of C++ code transparently.

It's, of course, hard to predict all types that might ever be needed, but an intuition can be
constructed that it will limit itself to a reasonably small set of semi-primitive types,
otherwise it would just make more sense to treat the types as opaque and interoperate through
pointers.

A more concrete assessment could be made by analysing a small set of AAS applications and
their interactions with BMW-provided framework, such that one could build a picture of which
concrete types one would want to manipulate directly from Rust code and which ones could be
treated as handles.

### Strategy options for Rust and C++ interop

After consulting with the Rust comunity (see [this thread](https://users.rust-lang.org/t/rust-c-ffi-suggestions/136749/13)),
these are the suggested options (in order of consideration):

#### Boxing (default)

For low throughput and no-latency critical aspects, one can always create a Box and pass
the pointer to the other side.

If in high throughput or low latency scenarios, please consider one of the options bellow.

#### Use bindgen to import C++ objects

Works for "comparably simple types" (e.g. std::string_view), and there's no cost of maintenance.

If the type cannot be mapped using bindgen, then consider one of the options bellow.

#### Adapt C++ type to be FFI compatible

Pros: ABI is fully controllable.
Cons: Requires breaking ABI of existing type (which is not a problem as we always rebuild the
whole set of dependencies).

#### Create a C bridge type and use it as intermediary type

Pros: ABI is fully controllable.
Cons: Requires extra convertion efforts. Performance penalty on object copy.

#### Hand-written binding

Pros: No performance or memory overhead.
Cons: Maintenance cost if adding a new incompatible standard library, which is rare.

### Discarded strategies

For the sake of completeness, these are the strategies that have been considered, but would
not work for our setup:

#### Using Crubit

Not recomended before the first quarter of 2026 as it's not usable except for Google projects,
and focuses only on LLVM.

#### Extending CXX

Benevolent dictatorship policy might lead to rejection of necessary features.
Forking would mean even more maintenance.

#### Zngur

Works in the same direction as adapting the types to be FFI-compatible.
Has a lightweight IDE where types can be defined, and it generates versions for C++ and Rust
(in a way that is FFI compatible).
It's focus is exposing Rust structures to C++, so it could be lacking on the C++ to Rust direction.
Might become a possibility in the future (further analysis would be required).
