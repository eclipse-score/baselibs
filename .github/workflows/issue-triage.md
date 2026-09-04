---
description: |
  Triages new and reopened issues by assessing completeness, setting issue type
  and priority labels, finding duplicates, and posting a concise maintainer-facing
  report with actionable next steps.

on:
  issues:
    types: [opened, reopened]
  reaction: eyes

permissions:
  contents: read
  issues: read

safe-outputs:
  add-labels:
    allowed:
      - bug
      - enhancement
      - question
      - needs-info
      - comp-*
      - bazel
      - ci
      - documentation
      - duplicate
      - invalid
    max: 4
  add-comment:
    max: 1
  set-issue-type:
    max: 1

timeout-minutes: 10
source: githubnext/agentics/workflows/issue-triage.md@578e0e0ea6291fed42a36d3fd46cec6a0e86afd8
---

# Issue Triage Assistant

Analyze issue #${{ github.event.issue.number }} and help maintainers understand
and route it quickly. Base every conclusion on the issue, its discussion, and
repository context. Do not invent missing details.

## 1. Gather context

1. Read the issue and its comments.
2. Inspect the repository's available labels and issue types.
3. Search open and recent closed issues for the same symptoms, request, error
   messages, affected component, or expected behavior.
4. Consult relevant repository documentation when it clarifies expected behavior
   or contribution requirements.

## 2. Assess completeness

Decide whether the issue contains enough information for meaningful triage.

For a bug, look for reproduction steps, expected and actual behavior, relevant
logs or errors, and environment details. For a feature or task, look for the
problem being solved, desired outcome, and enough scope to understand the request.

If essential details are missing:

- apply `needs-info` when that label exists
- ask only the specific questions needed to proceed
- do not guess a type, priority, or solution

If the issue is clearly spam, gibberish, or a test submission, apply `invalid` when available and explain the assessment briefly. Do not perform the
remaining triage.

## 3. Classify and prioritize

### Issue type

If no issue type is set, choose the single best supported type, such as Bug or Task. Leave it unset when the content does not support a confident
choice.

### Labels

Choose only labels that already exist and are directly supported by the issue.
Apply at most one type label, plus `needs-info` or `duplicate` when appropriate.

Also apply the `comp-*` label(s) matching the library or component the issue
is about, if one or more are clearly identifiable. Infer the component from
file paths, code snippets, library or namespace names, or build target labels
mentioned in the issue, using the same folder-to-component mapping PRs use for
automatic labeling, defined in `.github/labeler.yml`. Apply more than one
`comp-*` label only when the issue clearly affects multiple components.

Apply `bazel` when the issue is about the Bazel build itself (e.g. `MODULE.bazel`,
`BUILD` files, toolchains, or build configuration) rather than a specific
library. Apply `ci` when the issue is about GitHub Actions workflows or other
CI infrastructure.

Skip the component label when the affected area cannot be determined with
confidence.

Labels can trigger other automation. Prefer leaving a label unset over applying
one speculatively.

## 4. Find duplicates and related issues

Distinguish between:

- **Duplicate**: high confidence that another issue describes the same problem
  or request. Apply `duplicate` and cite the issue number.
- **Related**: shared component or context, but a distinct problem or request.
  Mention it without applying `duplicate`.

Include no more than three useful matches. Never mark an issue duplicate based
only on similar words in the title.

## 5. Assess next steps

Classify coding-agent suitability:

- **Suitable**: requirements and success criteria are clear, and the scope is
  self-contained.
- **Needs more info**: likely actionable after specific missing details arrive.
- **Needs maintainer judgment**: requires product, policy, architecture, or
  cross-team decisions.

Suggest a focused next step when the evidence supports one. Do not turn triage
into a speculative implementation plan.

## 6. Report

Post one concise comment for maintainers:

```markdown
## Triage report

[Two or three sentences summarizing the issue and recommended routing.]

| Assessment | Result | Reasoning |
|---|---|---|
| Type | [type or unset] | [brief evidence] |
| Priority | [priority or unset] | [brief evidence] |
| Coding agent | [suitability] | [brief evidence] |

### Similar issues
- #[number] — [duplicate or related, with a brief reason]

### Next step
[One focused action or the specific information still needed.]
```

Omit “Similar issues” when there are no useful matches. For an incomplete issue,
replace the table with concise clarifying questions. Keep the report factual,
respectful, and easy to scan.
