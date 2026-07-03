# Findings: Evaluate llvm-cov coverage solution in baselibs (issue #289)

This is the outcome of the spike that ports the llvm-cov + justification coverage
solution from eclipse-score/communication#549 into baselibs. Scope: C++ only, on
Linux. Coverage runs over the whole `//score` tree; the justification mechanism
and the gcov comparison are illustrated on the `//score/bitmanipulation`
component.

## Summary

The solution was ported and works end to end in baselibs. All four acceptance
criteria are met:

1. llvm-cov produces a C++ coverage report on Linux.
2. The justification mechanism is validated with one justified exclusion that is
   correctly represented in the report.
3. llvm-cov output is compared against the existing `bazel coverage` (gcov) flow
   for the `bitmanipulation` component (below).
4. These findings are captured for reuse in the `score_tooling` module.

## What was added

- `quality/coverage/llvm_cov/` (ported verbatim from communication): `merger.py`
  (per-test profraw to profdata), `reporter.py` (aggregation + `llvm-cov
  show/export/report`), `justify.py` (resolves YAML + in-code markers to a
  manifest), `effective_coverage.py` (restyles justified lines and computes
  effective coverage), `filter_regexes.txt`, and their `BUILD`.
- `quality/coverage/coverage.bazelrc`: a dedicated `bl-cov-x86_64-linux` config
  that builds with the LLVM toolchain and source-based instrumentation
  (`--experimental_use_llvm_covmap`) and wires the merger/reporter generators.
- `quality/coverage/generate_coverage_html.sh` + `BUILD` target
  `//quality/coverage:generate_coverage_html` to extract the report, run
  justification, and produce HTML under `cpp_coverage/`.
- `quality/coverage/coverage_justifications.yaml`: the justification database.
- Build wiring: a pinned `score_baselibs_pip` hub for PyYAML
  (`requirements_lock.txt`, with hashes), `rules_shell` dev dependency,
  `.bazelrc` import of the coverage config, and `exports_files(["MODULE.bazel"])`
  used as a runfiles anchor by the reporter.
- CI: `.github/workflows/coverage_report.yml` now runs the llvm-cov flow instead
  of gcov + genhtml.

## How to run locally

```
bazel coverage --config=bl-cov-x86_64-linux -- //score/...
bazel run //quality/coverage:generate_coverage_html
# open cpp_coverage/index.html
```

## Whole-tree results (llvm-cov, LLVM 19.1.1, source-based)

Running over the whole `//score` tree (299 tests, with the four exclusions noted
below) produces:

| Metric          | Value                          |
|-----------------|--------------------------------|
| Lines           | raw 89.74% (29887/33303), effective 89.75% |
| Branches        | 73.27% (7263/9912)             |

Building the whole tree with clang required two adjustments in the coverage
config (both pre-existing warnings, not new): disabling the `warnings_as_errors`
feature (from the stub config) and the per-target `treat_warnings_as_errors`
feature (from `COMPILER_WARNING_FEATURES`), so that clang warnings such as
`-Wdeprecated-declarations` on deprecated APIs (`StringLiteral`, `LogStr`) do not
fail the coverage build.

Three tests pass under the existing gcov flow but fail under llvm-cov source-based
instrumentation and are excluded in the workflow:

- `aborts_upon_exception:abortsuponexception_lib_test` (and the existing
  `:abortsuponexception_toolchain_test`): abort/death tests, where the immediate
  `abort()` races with llvm-cov continuous-mode counter writeback.
- `safe_math/details:floating_point_environment_test`: the injected coverage
  counter code perturbs the floating-point status flags the test inspects.
- `os/linux/utils/test:network_interface_test`: reports a leaked mock object at
  the instrumentation-altered process exit path.

## Component results (bitmanipulation)

Used below for the justification demonstration and the gcov comparison:

| File                  | Line coverage | Branch coverage |
|-----------------------|---------------|-----------------|
| bit_manipulation.h    | 86.57%        | 100.00%         |
| bitmask_operators.h   | 100.00%       | 100.00%         |
| **Total**             | raw 90.29% (93/103), effective 91.26% | 100.00% (10/10) |

## Justification mechanism (AC #2)

The mechanism combines a YAML database with in-code markers:

- Single line: `// COV_JUSTIFIED <id>` on the uncovered line.
- Region: `// COV_JUSTIFIED_START <id>` ... `// COV_JUSTIFIED_STOP`.
- Each `<id>` resolves to an entry in `coverage_justifications.yaml` with a
  category (`defensive_programming`, `tool_false_positive`, `platform_specific`,
  `other`) and a reason.

Validation: one justification (`upper-half-byte-constexpr-only`, category
`tool_false_positive`) was added on `score::platform::Byte::UpperHalfByte()`. That
accessor *is* covered by the unit test `Extract.UpperHalfByteFromAByte`, but the
test calls it only in a constexpr context (`constexpr auto upperHalfByte =
input.UpperHalfByte();`), so the body is evaluated at compile time and never
executed at runtime. llvm-cov source-based coverage instruments runtime execution
only, so it reports the line as uncovered. This is a genuine tool false positive,
exactly the case the justification mechanism is meant for. `justify.py` resolved
exactly one justified line, which lifted line coverage from raw 90.29% to
effective 91.26%. In the HTML report the line is rendered with the
`justified-line` style (yellow) and a tooltip carrying the id and reason.

## Comparison: llvm-cov vs `bazel coverage` (gcov) for bitmanipulation (AC #3)

Existing flow: `bazel coverage --config=bl-x86_64-linux` (GCC + gcov,
`--combined_report=lcov`), then `genhtml`.

| Aspect                | gcov (existing)                              | llvm-cov (this spike)                          |
|-----------------------|----------------------------------------------|------------------------------------------------|
| Overall line coverage | 100% (50/50 reported)                        | raw 90.29% (93/103), effective 91.26%          |
| Branch coverage       | "no data found"                              | 100% (10/10)                                   |
| Per-file rates        | nonsensical: `bit_manipulation.h` 135%, `bitmask_operators.h` 36.8%, functions 0.0% | consistent per-line/region mapping             |
| Uncovered inline/constexpr code | not counted (only emitted lines instrumented) | fully mapped via covmap, so untested inline accessors show as uncovered |
| constexpr-only code paths | not surfaced (no runtime emission)       | reported as uncovered (runtime instrumentation only); needs justification |
| Source rendering      | genhtml                                      | `llvm-cov show` HTML + justification overlay   |
| Justification support | none                                         | YAML + in-code markers, effective coverage     |

Key takeaway: gcov reports a misleadingly clean 100% because untested inline and
constexpr methods in the headers are never emitted into the test binaries and so
are never instrumented, while lcov aggregation over multiple test binaries even
produces impossible per-file rates above 100%. llvm-cov source-based coverage
maps all source regions (including never-instantiated inline functions),
surfaces those genuine gaps, and provides reliable branch coverage. This makes
llvm-cov substantially more trustworthy for header-heavy, template/constexpr C++
such as baselibs.

A caveat that surfaced during this spike: llvm-cov reports `constexpr` functions
that are only ever evaluated at compile time (for example accessors used solely
in `constexpr` test expressions) as uncovered, because nothing executes them at
runtime. These are real, tested code paths but appear as gaps. The justification
mechanism (`tool_false_positive`) is the right tool to mark them; alternatively,
adding a runtime call in a test would cover them naturally. score_tooling should
expect this pattern in constexpr-heavy code and document it.

## Recommendations for score_tooling reuse

- Adopt the llvm-cov flow for C++ coverage; it is more accurate than the gcov
  flow for the inline/template-heavy code in baselibs and provides branch data.
- A dedicated coverage config is required here because baselibs is config-driven
  and its default platform is GCC-pinned. The `bl-cov-x86_64-linux` config
  registers only the LLVM toolchain to avoid `extra_toolchains` priority
  conflicts; replicate this pattern per repo rather than overloading the global
  `coverage` keyword.
- Whole-tree coverage requires disabling the warnings-as-errors features
  (`warnings_as_errors` and `treat_warnings_as_errors`) for the coverage build,
  since clang surfaces pre-existing deprecation warnings that would otherwise
  break compilation.
- Some test classes are incompatible with llvm-cov source-based instrumentation
  (abort/death tests, floating-point-environment tests, and a mock-leak case).
  They must be excluded from the coverage run or adapted; the regular gcov flow
  tolerates them. score_tooling should provide a documented exclusion list.
- `justify.py` needs PyYAML. It is added as a pinned, hashed pip dependency
  (`score_baselibs_pip`). If full hermeticity is a hard requirement, consider a
  vendored YAML parser or a JSON justification format to avoid the pip fetch.
- `--dynamic_mode=off` is needed for source-based coverage; expect larger static
  links and longer cold builds.
- Source filtering is done with `--ignore-filename-regex` in the
  merger/reporter (`filter_regexes.txt`), not Bazel `instrumentation_filter`.
  Centralize this list when scaling beyond one component.
- Note the gcov-based `coverage_termination_handler` used by baselibs test
  macros for death tests; verify interaction when expanding the llvm-cov scope
  to components that rely on it.
- Scale-up: extend the target pattern from `//score/bitmanipulation/...` to more
  components incrementally, watching build time and the effective-coverage
  threshold (currently 100%, which fails the build on any unjustified gap).
