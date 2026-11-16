# Components as Bazel package metadata (demo)

This branch shows one way to define a software component once, in Bazel, so
that documentation, coverage and plain queries all read the same definition.

## The idea

Bazel gives every rule a common attribute `package_metadata`. Its default is
the package's `default_package_metadata`, so one line in a `BUILD` file tags
every target of that package. Rules and aspects can read it. This is the
mechanism rules_license uses to attach license and SBOM provenance to targets.
A component is the same kind of fact: a constant attached to a target.

```python
load("@score_baselibs//tools/component:defs.bzl", "score_component")

package(default_package_metadata = [":component"])

score_component(
    name = "component",
    id = "comp__baselibs_json",      # the Need ID in the process model
    title = "JSON",
    docs = "docs",                   # declares the docs_bundle with the same id
    code_targets = [":json", ":json_serializer"],
)
```

- Sub-packages repeat the `package()` line with the parent's label
  (`//score/json:component`). The default does not inherit downwards.
- A mixed package names a second component and points sub-packages or single
  targets at it: `score/json/internal/parser/vajson` and
  `score/json/internal/writer/vajson` carry `//score/json:vajson_component`.
- A target can opt out: `//score/json:mock` sets `package_metadata = []`
  because test doubles are not implementation.
- The attribute is nonconfigurable: no `select()` on ownership. Platform
  differences come from `select()` in `deps` and `srcs`, which resolve before
  any aspect runs, so a component's file set per platform is derived, never
  declared.

## What reads it

| Consumer | How |
| --- | --- |
| Queries | `bazel query 'attr(package_metadata, "//score/json:component", //score/...)'` lists the component's targets; wrap in `kind(".*_test rule", ...)` for its tests. |
| Files per component | `bazel build //tools/component:component_files` runs an aspect over the entry points and writes `component_files.txt` / `.json`: every main-repository source file grouped by the component of its owner target, plus `(unassigned)`. Build with `--platforms=...` for another platform. |
| Documentation | `score_component(docs = ...)` declares the `docs_bundle` with `primary_need_id = id`. `code_targets` stays until docs-as-code reads the metadata itself. |
| Coverage of one component | `tools/component/coverage.sh //score/json:component` selects the tests by metadata, runs `bazel coverage` on them, generates the usual report and joins the LCOV with the component's file list. |

## What this replaces, eventually

- The hand-maintained `SCOPE_DEPS` list in `tools/coverage/BUILD`: scope
  becomes "everything that carries a component", and a lint fails any package
  under `score/` without one.
- `code_targets` on every `docs_bundle`: docs-as-code can derive the file set
  from the same aspect, and `primary_need_id` is the component id.
- The file-selection mismatch between the two tools: both would read one
  attribute on one target instead of walking dependencies with their own rules.

## Where it should live

Providers are identity-checked, so both tools must load the same definition.
`defs.bzl` belongs in a small leaf module without dependencies, which
baselibs, docs-as-code and the coverage tool all depend on. It is in baselibs
here only to keep the demo self-contained.
