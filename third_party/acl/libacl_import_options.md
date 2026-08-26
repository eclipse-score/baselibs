# Native dependency vendoring: hermetic sysroot vs. build-from-source

Context:`score_baselibs` repeatedly breaks when native third-party
libraries (`acl`, `libcap`, `valgrind`) are consumed as prebuilt Ubuntu `.deb`
archives — architecture mismatches, missing `-fPIC`, and no coverage for
non-Ubuntu targets (RedHat AutoSD, Elektrobit Linux). Four candidate approaches
were evaluated; this document compares the two "hermetic" ones in detail:
**option 2** (hermetic sysroot/toolchain bundling) and **option 4** (build the
dependency from source as a Bazel target). Option 4 has since been implemented
and validated for `acl`, so this comparison is grounded in real implementation
data, not just design discussion.

## Full trade-off table (all four options, for reference)

| Approach | Hermetic | Effort | Distro coverage | Fixes root cause |
|---|---|---|---|---|
| 1. System-installed | No | Low | Requires per-image setup | No |
| 2. Hermetic sysroot | Yes | High (cross-repo) | Best, if built for it | Yes |
| 3. Parameterized vendoring | Partial | Medium | Still manual per distro | Partially |
| 4. Build from source | Yes | Medium-High | Best (arch/toolchain-driven) | Yes |

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

## Side-by-side, informed by the completed `acl` implementation

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
