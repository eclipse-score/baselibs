---
name: requirements-management-skill
description: >-
   Requirements traceability for S-CORE baselibs Sphinx-Needs. Use when authoring or reviewing comp_req/aou_req requirements, looking up external feat_req/stkh_req needs, validating version-pinned links and safety consistency. Use also when authoring/reviewing/migrating C++ and Rust unit tests as those always need comp_req links and verification metadata.
---

# Requirements Management Skill

## Routes

### Author or edit `comp_req` / `aou_req`

1. Edit `score/<component>/docs/requirements/index.rst`.
2. Use a stable lower-snake-case ID: `comp_req__<component>__<short_name>` or `aou_req__<component>__<short_name>`.
3. Write the body following [Requirement Wording](#requirement-wording); split non-atomic requirements into separate needs.
4. Set every required attribute from [Requirement Attributes](#requirement-attributes), including `:version:`.
5. For `comp_req`, pin every `:derived_from:`, `:satisfied_by:`, and `:covers:` target as `<target_id>[version==N]`.
6. Check the linked target exists and the pinned version matches the authoritative need data.
7. Validate with `bazel run //:docs`; the route is complete when the docs build has no metamodel, link, or schema errors caused by the change.

### Review `comp_req` / `aou_req`

1. Locate the Bazel output base with `bazel info output_base`.
2. Read the upstream inspection checklist `*score_process_description+/process/folder_templates/platform/features/feature_name/requirements/req_inspection.rst`. No component-level checklist exists; apply the feature-level one to `comp_req` and `aou_req`.
3. Check each new or changed requirement against every checklist item and report findings by item ID, e.g. `REQ_02_03`.
4. The route is complete when every checklist item is answered for every requirement under review.

### Look up external `feat_req` / `stkh_req`

1. Do not grep for external feature or stakeholder requirement sources in this repo; they are imported from `score_platform` needs JSON.
2. Locate the Bazel output base with `bazel info output_base`.
3. Read the relevant `*score_platform+/needs_json/.../needs.json` file for `id`, `title`, `content`, `status`, `version`, and links.
4. For process-level needs such as `wp__requirements_comp`, read the parallel `*score_process+/needs_json/.../needs.json`.
5. The route is complete when each referenced need ID, version, safety level, and upstream trace is accounted for.

### Link tests to requirements

1. Read [`test-to-requirement-linking.md`](test-to-requirement-linking.md) before authoring, reviewing, or migrating test metadata.
2. Link C++ gtest and Rust unit tests to the `comp_req__...` they verify using the mandated metadata: `PartiallyVerifies` / `FullyVerifies`, `TestType`, `DerivationTechnique`, and `Description`.
3. The route is complete when every affected test has correct metadata and the requirement it verifies exists.

## Requirement Types, Traceability & Naming

Traceability runs `stkh_req --derived_from-> feat_req --derived_from-> comp_req`. `stkh_req` and `feat_req` are external; `comp_req` and `aou_req` are authored here. `aou_req` has no `derived_from` because it is an assumption, not a derived requirement.

| Type | `:id:` pattern | Origin | Traceability |
|------|----------------|--------|--------------|
| Stakeholder (`stkh_req`) | `stkh_req__baselibs__...` | External (`score_platform` needs JSON) | Root |
| Feature (`feat_req`) | `feat_req__baselibs__<name>` | External (`score_platform` needs JSON) | `derived_from` >=1 `stkh_req` |
| Component (`comp_req`) | `comp_req__<component>__<name>` | **Authored here** | `derived_from` >=1 `feat_req`; `satisfied_by` the `comp__...` |
| Assumption of Use (`aou_req`) | `aou_req__<component>__<name>` | **Authored here** | No `derived_from`; linked to by a requirement via `covers` |

Other IDs: requirements document `doc__<component>_requirements`; architecture component `comp__baselibs_<component>`.

- The directive title is a short noun phrase; put `shall`, `must`, `will`, and other normative wording in the body.
- Use `:status: invalid` with a TODO instead of linking to a `feat_req` that does not exist yet.
- Do not rename an existing `:id:` casually; downstream links and `:need:` references depend on it.

## Requirement Wording

- Follow the formulation template `gd_temp__req_formulation`, defined in `*score_process_description+/process/process_areas/requirements_engineering/guidance/requirements_templates.rst` under the Bazel output base.
- Keep each requirement atomic (checklist item `REQ_02_03`): it states one behavior that one positive test case can verify.
- Split a body that joins independent behaviors with "and" / "or", enumerates several capabilities, or contains a bullet list of features into one need per behavior, each with its own ID.
- Keep a requirement non-atomic only when splitting would make it harder to understand.

## Requirement Attributes

### Authored need attributes

| Option | Required | Values / Notes |
|--------|----------|----------------|
| directive title | Yes | Short noun phrase, no `shall` / `must` / `will` |
| `:id:` | Yes | Stable ID matching the need type pattern |
| `:reqtype:` | Yes | `Functional` \| `Interface` \| `Process` \| `Non-Functional` |
| `:safety:` | Yes | `QM` \| `ASIL_B` |
| `:security:` | Yes | `YES` \| `NO` |
| `:status:` | Yes | `valid` \| `valid_inspected` \| `invalid`; see [Requirement Status](#requirement-status) |
| `:version:` | Yes | Whole number, starting at `1`; bump on material change |
| body (content) | Yes | Normative text for `comp_req`; integrator/user assumption for `aou_req` |

### `comp_req`-only attributes

| Option | Required | Values / Notes |
|--------|----------|----------------|
| `:derived_from:` | Yes | Comma-separated `feat_req__...` IDs, **each version-pinned** as `feat_req__...[version==N]` |
| `:satisfied_by:` | Recommended | Implementing `comp__...[version==N]` |
| `:covers:` | Optional | `aou_req__...[version==N]` this requirement covers |
| `:reqcovered:` / `:testcovered:` | Optional | `YES` \| `NO` |
| `:tags:` | Optional | Comma-separated tags |

`aou_req` has no type-specific required attributes and no `:derived_from:` link.

## Safety Classification (ASIL)

`safety` is either `QM` or `ASIL_B`. A `QM` requirement may not be `derived_from` an `ASIL_B` requirement; re-check the chain whenever you change safety.

## Need Versioning

Every need has a whole-number `:version:`. Bump it only on a material change: a change of the body's meaning, safety, security, or of what a link traces to.
Do not bump for non-material changes, such as typo and formatting fixes, title changes, or retargeting a link to a renamed or re-versioned need without changing what it traces to.
A non-material change must not cascade: keep the need's version, so pins to it stay valid and their owners need no edits.
Document needs (`doc__...`) that no link pins can be bumped whenever their content changes.

Every link value is written as `<target_id>[version==N]`, where `N` is the current version of the target need:

```rst
:derived_from: feat_req__baselibs__containers_library[version==2]
:satisfied_by: comp__baselibs_containers[version==1]
```

- Pin every target in every link attribute, including `:derived_from:`, `:satisfied_by:`, `:realizes:`, `:covers:`, `:fulfils:`, `:includes:`, `:included_by:`, `:complies:`, and `:mitigated_by:`.
- For comma-separated links, pin each ID individually.
- On unresolved-link failures, check the target version as well as the target ID.

## Requirement Status

- `valid`: the requirement is in force but has not passed a requirements inspection. Use it for every new requirement.
- `valid_inspected`: the requirement passed a human requirements inspection according to the S-CORE process, and the human inspector filled out the inspection checklist.
  This status is relevant for ASIL code, including this repository's `ASIL_B` requirements.
  An automated or AI-assisted review or a successful documentation build does not replace the human inspection or justify setting this status.
  Keep this status as long as the requirement text is unchanged, including when only its links, title, or version change.
- `invalid`: the requirement is not in force, for example while its `feat_req` does not exist yet.

Reset `valid_inspected` to `valid` only when the requirement text changes materially, since that requires a new inspection.

## Directory Structure

Author `comp_req` / `aou_req` in each component's `requirements/index.rst`.
Component docs live next to the component source and are mounted under `baselibs/components/<component>/` in the built documentation:

```
score/<component>/docs/
├── index.rst                 # component document node (doc__<component>)
├── requirements/index.rst    # comp_req + aou_req  <- edit here
└── architecture/index.rst    # component architecture (comp__..., interfaces)
```

Each file opens with a `.. document::` need, then groups `comp_req` and `aou_req` directives under local headings such as Functional Requirements, Non-Functional Requirements, and Assumptions of Use.

## Building & Validating

```bash
bazel run //:docs
```

A build failure reporting a missing/invalid option, an unknown link target, or a broken reference means a requirement violates the metamodel and must be fixed.

If validation behaves unexpectedly, inspect the authoritative metamodel in the Bazel output base: run `bazel info output_base`, then read `*score_metamodel*/metamodel.yaml` under that directory.
