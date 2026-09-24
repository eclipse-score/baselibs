# Native dependency vendoring: hermetic sysroot vs. build-from-source

Context:`score_baselibs` repeatedly breaks when native third-party
libraries (`acl`, `libcap`, `valgrind`) are consumed as prebuilt Ubuntu `.deb`
archives — architecture mismatches, missing `-fPIC`, and no coverage for
non-Ubuntu targets (RedHat AutoSD, Elektrobit Linux). This document compares
three candidate approaches: **option 2** (hermetic sysroot/toolchain
bundling), **option 4** (build the dependency from source as a Bazel target),
and **option 5** (clean-room reimplementation). Option 4 has since been
implemented and validated for `acl`, so this comparison is grounded in real
implementation data, not just design discussion. Option 5 was considered
specifically to ask whether `acl`'s LGPL-2.1 license obligation could be
avoided altogether; it was not adopted (see its section below for why).

## The three options, in simple terms

2. **Hermetic sysroot** — bundle a prebuilt `acl` (and `libcap`, `valgrind`)
   into the same kind of toolchain/sysroot package this repo already uses for
   the GCC/QCC compilers, so it's version-controlled once, centrally.
4. **Build from source (implemented)** — download `acl`'s own source code and
   compile it ourselves, with our own toolchain and our own `-fPIC` flags,
   instead of trusting someone else's prebuilt binary.
5. **Clean-room reimplementation** — stop depending on `acl`'s code entirely;
   write our own implementation of the handful of ACL functions we actually
   call, directly against the well-documented kernel ACL format.

## Trade-off table

| Approach | Hermetic | License obligation | Effort | Distro coverage | Fixes root cause |
|---|---|---|---|---|---|
| 2. Hermetic sysroot | Yes | Applies — ships a compiled `acl` | High (cross-repo) | Best, if built for it | Yes |
| 4. Build from source (implemented) | Yes | Applies — ships a compiled `acl`, but made easy via `:acl_shared` (dynamic linking) | Medium-High | Best (arch/toolchain-driven) | Yes |
| 5. Clean-room reimplementation | Yes | None — no acl code, ours is Apache-2.0 | Very high, and ongoing forever | Best (our own code, any target) | Yes |

All three are hermetic and fix the root cause; only option 5 also removes the
LGPL license obligation, at the cost of writing and maintaining an ACL
implementation ourselves indefinitely.

## Option 2 — Hermetic sysroot/toolchain bundling

Extend (or create) a hermetic multi-arch sysroot — similar to how
`score_gcc_x86_64_toolchain`/`score_gcc_aarch64_toolchain` are already pulled in
via `score_bazel_cpp_toolchains` in [MODULE.bazel](../../MODULE.bazel#L18-L33) —
that ships `libacl`, `libcap`, `valgrind` headers/libs for each target arch as
part of the toolchain package, instead of a separate ad-hoc `download_deb` per
library.

- **Pros:** Fully hermetic and reproducible; single point of version control
  (the sysroot artifact); naturally extends the toolchain infrastructure
  already in place for GCC/QCC.
- **Cons:** Requires building/maintaining that sysroot artifact (likely
  upstream in `score_bazel_cpp_toolchains` or a new repo) — work isn't only in
  this repo; slower to iterate; still needs a strategy per distro if
  AutoSD/Elektrobit need different library builds (e.g. musl vs glibc,
  different ABI).

## Option 4 — Build the dependency from source as a Bazel target

Vendor the upstream *source* (e.g. `acl`/`libcap` release tarballs) and compile
them with Bazel's own toolchain/`-fPIC` flags directly, instead of consuming
prebuilt distro binaries at all.

- **Pros:** Fully hermetic, arch-agnostic (any target the toolchain supports),
  immune to distro packaging quirks (no more "was this `.deb` built with
  `-fPIC`" issues); consistent story across Ubuntu/AutoSD/Elektrobit since none
  of them are involved at build time.
- **Cons:** Higher upfront effort (write BUILD files for each source tree,
  handle their native build systems/autoconf quirks); becomes something this
  repo now owns and must patch/update over time; still need libc/system
  headers (e.g. `sys/capability.h`) to match target consistently.

### How Option 4 avoids a `libattr` dependency

Upstream Ubuntu's `libacl1-dev` (used by the old `.deb`-based approach)
declares a package-manager-level dependency on `libattr1-dev`, but that was
never modeled explicitly as a Bazel dependency in this repo — it was simply
bundled inside the downloaded `.deb`. Vendoring `acl` from source instead
requires being explicit about which of its ~40 files actually need `libattr`,
so the dependency can be scoped or dropped deliberately.

Of the vendored `acl-2.4.0` source, only two files reference `libattr` at all:
`libacl/perm_copy_fd.c` and `libacl/perm_copy_file.c`, both of which
`#include <attr/error_context.h>` with `ERROR_CONTEXT_MACROS` defined first —
that makes `libattr`'s `error()`/`quote()`/`quote_free()` helpers expand to
macros at compile time rather than symbols resolved at link time, so even
upstream's own `perm_copy_*` never dynamically link against `libattr.so`.
`libacl/acl_delete_def_file_at.c` has no `attr/` reference at all; it is
grouped with `perm_copy_fd.c`/`perm_copy_file.c` in the exclusion list purely
because it is unused, not because it needs `libattr`.

Since `score/os/acl_impl.cpp` never calls `perm_copy_fd`, `perm_copy_file`, or
`acl_delete_def_file_at`, all three files are simply left out of `ACL_SRCS` in
[acl_sources.bzl](acl_sources.bzl) (see the exclusion note at the top of
[acl.BUILD](acl.BUILD)). Every other vendored file — including the `_at`
fd-relative variants and their `libmisc` compat shims — talks to the kernel
directly via `getxattr`/`setxattr`(`_at`), with no `libattr` involvement.

Net effect: the `:acl` cc_library has no reference to `attr/error_context.h`
and no Bazel dependency on any `libattr` target (none exists in this repo).

**Constraint:** this is enforced only by omission, not by an automated check.
`check_config_drift.py`/`:config_drift_test` verifies `config.h` macro
coverage and flags GPL-only (vs. LGPL) file headers, but it does not
specifically fail the build if a future change re-adds `perm_copy_fd.c` or
`perm_copy_file.c` to `ACL_SRCS`. Re-adding either file would silently
reintroduce a `libattr` header dependency, and — if the code copying non-ACL
extended attributes were later extended to call `libattr`'s linkable API
(e.g. `attr_copy_file`/`attr_copy_fd`) rather than just its header macros — a
real link-time dependency, requiring `libattr` to be vendored or declared
alongside `acl` at that point.

## Option 5 — Clean-room reimplementation

Stop depending on `acl`'s code at all. `score/os/acl_impl.cpp` only calls a
small, fixed set of functions (`acl_get/set_fd`, `acl_get_file`,
`acl_get/create_entry`, `acl_get/set_tag_type`, `acl_get/set_qualifier`,
`acl_get/add_perm`, `acl_get_permset`, `acl_clear_perms`, `acl_calc_mask`,
`acl_valid`, `acl_to_text`, `acl_free`). These are thin wrappers around a
well-documented, fixed kernel format: `getxattr`/`setxattr` on
`system.posix_acl_access`/`_default` with a simple binary struct, plus
POSIX.1e's plain-text ACL grammar for `acl_to_text`. Writing an independent
implementation against that public format (without reading `acl`'s own
source) avoids depending on `acl`'s code at all.

- **Pros:** No `acl` code anywhere, so no LGPL obligation; our own code can be
  Apache-2.0 like the rest of this repo; fully hermetic and fixes the root
  cause the same way option 4 does (we compile it, with our own toolchain).
- **Cons:** By far the highest effort of any option, and it doesn't end at
  first implementation — we'd own correctness and maintenance of an ACL
  library forever, including edge cases `acl` has presumably already found
  and fixed over its ~20-year history. Real risk of subtle behavioral
  divergence from the reference implementation. Not pursued for `acl` given
  the scope of this repo's actual need (a handful of functions used by one
  OSAL wrapper).

## Side-by-side, informed by the completed `acl` implementation

The comparison below stays focused on the two options that are hermetic *and*
fix the root cause without requiring us to reimplement `acl` ourselves
(option 5 wasn't pursued — see above).

| | **Option 2: Hermetic sysroot** | **Option 4: Build from source** |
|---|---|---|
| **Where the work lives** | Upstream infra repo (`score_bazel_cpp_toolchains` or a new toolchain-artifact repo) — outside `score_baselibs`' control | Entirely inside `score_baselibs` (`third_party/acl/*`, [MODULE.bazel](../../MODULE.bazel)) |
| **Hermeticity** | Fully hermetic, but only once the sysroot artifact itself is built and versioned somewhere | Fully hermetic — verified directly: `integrity` SRI sha256 pin on the tarball, compiled by the exact `score_gcc_*_toolchain` already used for everything else |
| **Arch coverage** | One artifact per arch, built once, reused everywhere — no compile step in this repo | Recompiled per target/arch by Bazel automatically; validated on both `bl-x86_64-linux` and `bl-aarch64-linux` (build + test, aarch64 via qemu) |
| **Distro coverage (Ubuntu/AutoSD/Elektrobit)** | Best *if* the sysroot is built for it — but if AutoSD/Elektrobit need a different libc/ABI (e.g. musl vs glibc), multiple sysroot variants are needed, which is ongoing infra work | Source-vendoring sidesteps distro packaging entirely — same source, same toolchain, any Linux target; no distro branch needed |
| **Fixes root cause?** | Yes, but the "no `-fPIC` / arch mismatch" bug class only disappears once the sysroot-building pipeline is disciplined about it — a new failure surface (sysroot build) replaces the old one (distro `.deb`) | Yes, directly — the toolchain that compiles `score_baselibs` itself also compiles the dependency, so PIC/ABI can never drift from the rest of the build |
| **Effort (upfront)** | High, but concentrated: build/patch autoconf once when authoring the sysroot | Medium-high per library, and iterative — the `acl` implementation needed 4 build-fail/investigate/fix round-trips: (1) a combined patch file mis-parsed by Bazel's native patcher, (2) `-Wcast-qual`/`-Werror` failures from stricter warnings, (3) an `EXPORT` macro left undefined (normally stripped by upstream's own `make install`), (4) undefined symbols from transitively-required `_at`/`libmisc` files not obvious from a first read of `Makemodule.am` |
| **Effort (ongoing)** | Low per-repo (consume the sysroot like today's toolchain deps), but real: someone has to rebuild/republish the sysroot on every `acl`/`libcap`/`valgrind` version bump | This repo now owns the vendoring: any upstream `acl` update means re-auditing the file list/`config.h`/patches again |
| **Cross-repo blast radius** | High — changes to a shared toolchain artifact affect every consumer of `score_bazel_cpp_toolchains`, so it needs broader review/rollout | Zero — fully contained to `score_baselibs`, no coordination needed with other repos |
| **Debuggability** | Sysroot is a black box from this repo's perspective; if a symbol/header is wrong, you're often blocked on another team/repo | Every failure surface (missing symbol, missing header, macro) is visible and fixable locally, as demonstrated end-to-end for `acl` |
| **Best fit** | Scales better if *many* repos/targets need the same set of native libs (shared infra investment pays off) | Scales better for a small, `score_baselibs`-specific set of libraries (`acl`, `libcap`) where a shared sysroot would be overkill |
