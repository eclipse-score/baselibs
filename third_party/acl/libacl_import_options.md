# Native dependency vendoring: hermetic sysroot vs. build-from-source

Context:`score_baselibs` repeatedly breaks when native third-party
libraries (`acl`, `libcap`, `valgrind`) are consumed as prebuilt Ubuntu `.deb`
archives — architecture mismatches, missing `-fPIC`, and no coverage for
non-Ubuntu targets (RedHat AutoSD, Elektrobit Linux). Six candidate approaches
were evaluated in total; this document compares the two "hermetic" ones in
detail: **option 2** (hermetic sysroot/toolchain bundling) and **option 4**
(build the dependency from source as a Bazel target). Option 4 has since been
implemented and validated for `acl`, so this comparison is grounded in real
implementation data, not just design discussion. Options 5 and 6 were
considered later, specifically to ask whether `acl`'s LGPL-2.1 license
obligation could be avoided altogether; neither was adopted (see their
sections below for why).

## All six options, in simple terms

1. **System-installed** — don't vendor or build `acl` at all; write our own
   small header declaring the functions we need, and link against whatever
   `libacl.so` already happens to be installed on the machine at runtime.
2. **Hermetic sysroot** — bundle a prebuilt `acl` (and `libcap`, `valgrind`)
   into the same kind of toolchain/sysroot package this repo already uses for
   the GCC/QCC compilers, so it's version-controlled once, centrally.
3. **Parameterized vendoring** — keep downloading a prebuilt `.deb` like
   before, but make the URL/arch/checksum a parameter instead of hardcoding
   two separate `deb()` rules, so adding a new arch/distro is a small
   change instead of a copy-paste.
4. **Build from source (implemented)** — download `acl`'s own source code and
   compile it ourselves, with our own toolchain and our own `-fPIC` flags,
   instead of trusting someone else's prebuilt binary.
5. **Clean-room reimplementation** — stop depending on `acl`'s code entirely;
   write our own implementation of the handful of ACL functions we actually
   call, directly against the well-documented kernel ACL format.
6. **Shell out to system CLI tools** — don't link against any ACL library at
   all; run the system's `setfacl`/`getfacl`/`chacl` programs as separate
   processes and read their text output.

## Full trade-off table (all six options)

| Approach | Hermetic | License obligation | Effort | Distro coverage | Fixes root cause |
|---|---|---|---|---|---|
| 1. System-installed | No | None — nothing of acl's is ever shipped | Low | Requires per-image setup | No |
| 2. Hermetic sysroot | Yes | Applies — ships a compiled `acl` | High (cross-repo) | Best, if built for it | Yes |
| 3. Parameterized vendoring | Partial | Applies — ships a compiled `acl` | Medium | Still manual per distro | Partially |
| 4. Build from source (implemented) | Yes | Applies — ships a compiled `acl`, but made easy via `:acl_shared` (dynamic linking) | Medium-High | Best (arch/toolchain-driven) | Yes |
| 5. Clean-room reimplementation | Yes | None — no acl code, ours is Apache-2.0 | Very high, and ongoing forever | Best (our own code, any target) | Yes |
| 6. Shell out to CLI tools | No | None — separate process, nothing linked | Low-Medium | Requires per-image setup | No |

Only options 2, 4, and 5 are both hermetic *and* fix the root cause; of those,
only option 5 also removes the license obligation, at the cost of writing and
maintaining an ACL implementation ourselves indefinitely. Options 1 and 6
remove the obligation but bring back the "does this image already have a
working ACL implementation" problem this whole effort started from — see
their sections below.

## Option 1 — System-installed

Don't vendor or compile any `acl` code in this repo at all. Write a small,
original header declaring the handful of `acl_*` functions
`score/os/acl_impl.cpp` needs, and link against `-lacl` so the actual
`libacl.so` comes from whatever is already on the target machine at runtime.

- **Pros:** No `acl` code is ever distributed by `score_baselibs`, so there is
  no LGPL obligation to discharge.
- **Cons:** Only works if the target image already has a working, correctly
  built `libacl.so` — not guaranteed for AutoSD/Elektrobit, and **not true at
  all for QNX**, which has no such package. This is the same "requires
  per-image setup" gap SWP-278650 was filed over, so it does not fix the root
  cause.

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

## Option 3 — Parameterized vendoring

Keep downloading a prebuilt `.deb`, as before, but parameterize the
url/architecture/checksum instead of hand-writing a separate `deb()` rule per
arch (as `acl-deb`/`acl-deb-aarch64` used to be). Adding a new arch or distro
becomes a small config change instead of a copy-pasted rule.

- **Pros:** Lower effort than options 2/4; keeps using distro-provided
  binaries, so no need to compile `acl` ourselves.
- **Cons:** Still consumes someone else's prebuilt binary, so the underlying
  "was this built with `-fPIC`" risk from SWP-278650 isn't actually removed,
  just made easier to patch when it recurs; still needs a distro-specific URL
  for every target (no coverage for QNX, which has no `.deb` at all); still
  ships a compiled `acl`, so the LGPL obligation is unchanged from today.

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

## Option 6 — Shell out to system CLI tools

Don't link against any ACL library at all. Run the system's
`setfacl`/`getfacl`/`chacl` binaries as separate processes (`fork`/`exec`) and
parse their text output instead of calling into `libacl` directly.

- **Pros:** Invoking a separate, unmodified program via `exec` (rather than
  linking it into our process) is generally understood to fall outside
  GPL/LGPL's linking obligations, so this removes the license question
  entirely; no compiling/linking of `acl` code means no PIC concerns either.
- **Cons:** Same "does the image already have this" gap as option 1 — these
  CLI tools don't exist on QNX and aren't guaranteed on minimal
  AutoSD/Elektrobit images, so it doesn't fix the root cause. Also a poor fit
  for a low-level OSAL primitive: process-spawn-per-ACL-call overhead and
  fragile text parsing are hard to justify here, independent of the licensing
  question. Ruled out on engineering grounds alone.

## Side-by-side, informed by the completed `acl` implementation

The comparison below predates options 5 and 6 and stays focused on the two
options that are actually hermetic *and* fix the root cause without requiring
us to reimplement `acl` ourselves (option 5 wasn't pursued, and option 6 isn't
hermetic at all — see above).

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
