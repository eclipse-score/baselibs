---
name: release-notes-skill
description: Prepare or verify the human-readable "Notable Changes" section of an eclipse-score/baselibs GitHub release.
disable-model-invocation: true
---

# Release Notes

Use this skill for a specific release tag.

## Gather the release scope

1. Load the raw release body and metadata: `GH_PAGER= gh release view <tag> --repo eclipse-score/baselibs --json body,isDraft,name,publishedAt,tagName`. Use the header's `Origin Release Tag` and `Release Commit Hash` when present. Complete when the release tag and unformatted body are known.
2. Extract every unique PR number from the release body's `What's Changed` list. This list is the release scope, including for a published release. If the list is absent or has no PR links, report that the scope cannot be determined from the release and stop. Do not reconstruct it from `git log` or a comparison view.
3. Fetch every in-scope PR with `GH_PAGER= gh pr view <number> --repo eclipse-score/baselibs --json number,title,body,author,url`. Complete when every in-scope PR has a title, body, author, and URL from GitHub.
4. Classify every in-scope PR using the criteria below. For a PR that is notable but whose title and body do not support a precise statement, inspect its diff before writing about it. Complete when every notable bullet is supported by PR metadata or a diff.
5. Find the two published releases immediately before the target release. For a draft, use the two most recent published releases. Use `GH_PAGER= gh release list --repo eclipse-score/baselibs --exclude-drafts --limit 10 --json tagName,publishedAt` to identify them, increasing the limit when the target's history is not included. Retrieve their bodies with `GH_PAGER= gh release view <tag> --repo eclipse-score/baselibs --json body,tagName`. Match their section granularity and identify any prior release promise fulfilled by the target PRs. Complete when the editorial precedent and prior-release follow-up have both been considered.

## Produce the result

### Draft

Write the complete release body for the user. Preserve every existing non-`Notable Changes` section verbatim, and GitHub-generated `What's Changed` list. Replace only the `Notable Changes` section. Complete when the proposed body retains all source content outside that section and every notable PR is represented accurately.

### Verify

Do not modify the release. Report a missing `Notable Changes` section, or each unsupported, inaccurate, missing, or misleading statement with the relevant PR URL and a corrected statement. State clearly when the section is supported by the in-scope PRs. Complete when every existing Notable Changes claim has been checked against its source PR.

### Update or publish

First provide the proposed complete body. Change GitHub only after the user explicitly asks for the update or publication and confirms the mutation immediately before it is made. Complete when the requested GitHub operation reports success and the release body is retrieved again to confirm the intended content.

## Classifying changes: notable vs. skip

- **Skip**: CI/workflow-only PRs, dependency-bump PRs (author `eclipse-score-bot` / `renovate`), Bazel visibility-only changes, requirements/doc-only PRs, internal repo-sync or agent-tooling PRs, and anything that nets to zero versus the previous release (e.g. a revert immediately undone by its own revert).
- **Notable**: breaking API/behavior changes and deprecations, new public features or libraries, user-visible bug fixes, and anything that continues or completes a change the previous release's notes flagged as upcoming.

## Style rules for Notable Changes

- Headings use ATX style (`#`, `##`, `###`).
- Link to PRs/issues with the bare URL, e.g. `https://github.com/eclipse-score/baselibs/pull/NNN`. GitHub auto-links and shortens these; do not use `[text](url)` markdown link syntax.
- Use `-` for bullets and backticks for code identifiers/paths.
- Group bullets under `###` subheadings as needed, typically `Breaking changes & deprecations`, `New features`, `Bug fixes` — plus an optional themed `###` (or a short intro paragraph before the first subheading) for a single cross-release effort such as a migration or multi-release deprecation.

## Inspecting diffs: `git` vs `gh` vs the web

- Prefer plain `git` for anything already in your local clone's history: `git log`, `git show`, `git diff`.
- Use `GH_PAGER= gh pr diff` for an in-scope PR whose diff is not available locally.
- Only fetch github.com pages through a browser/web tool as a last resort.
- Prefix every `gh` invocation with `GH_PAGER=` so it never opens an interactive pager.
