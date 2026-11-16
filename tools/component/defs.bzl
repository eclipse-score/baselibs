# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************
"""Software components as Bazel package metadata (demo).

A component is declared once, as a metadata target::

    score_component(name = "component", id = "comp__baselibs_json", title = "JSON")
    package(default_package_metadata = [":component"])

Every target of that package then carries the component in its
``package_metadata`` attribute, a common attribute Bazel gives every rule.
Sub-packages repeat the ``package()`` line; a single target can override the
package default with its own ``package_metadata``.

Any rule or aspect can read the attribute, so the same declaration serves:

* documentation: ``score_component`` also declares the component's
  ``docs_bundle`` with the same ``id`` as ``primary_need_id``;
* coverage: ``score_component_files`` walks a dependency graph in the run's
  configuration and lists every source file per component;
* queries: ``bazel query 'attr(package_metadata, "//score/json:component", //...)'``
  lists a component's targets, e.g. its tests.

In the real thing the provider and the rules live in a small leaf module that
docs-as-code and the coverage tool both depend on; here they live in baselibs
so the demo is self-contained.
"""

load("@score_docs_as_code//:docs.bzl", "docs_bundle")

ScoreComponentInfo = provider(
    doc = "Identity of a software component, attached to targets as package metadata.",
    fields = {
        "id": "Need ID of the component in the process model, e.g. comp__baselibs_json.",
        "title": "Human-readable name.",
        "label": "Label of the metadata target, as a string.",
    },
)

def _score_component_impl(ctx):
    return [ScoreComponentInfo(
        id = ctx.attr.id,
        title = ctx.attr.title or ctx.attr.id,
        label = str(ctx.label),
    )]

_score_component = rule(
    implementation = _score_component_impl,
    attrs = {
        "id": attr.string(mandatory = True, doc = "Need ID of the component."),
        "title": attr.string(doc = "Human-readable name."),
    },
    doc = "A software component. Attach it to targets through package(default_package_metadata).",
)

def score_component(
        name,
        id,
        title = None,
        docs = None,
        code_targets = [],
        docs_source_dir = "docs",
        visibility = ["//visibility:public"],
        **kwargs):
    """Declare a software component and, optionally, its documentation bundle.

    Args:
      name: name of the metadata target (conventionally ``component``).
      id: the component's Need ID; also the bundle's ``primary_need_id``.
      title: human-readable name.
      docs: name of the ``docs_bundle`` to declare, or None for none.
      code_targets: passed through to ``docs_bundle``. Needed only until
        docs-as-code derives the file set from the metadata itself.
      docs_source_dir: ``source_dir`` of the bundle.
      visibility: visibility of both targets.
      **kwargs: further ``docs_bundle`` arguments.
    """
    _score_component(
        name = name,
        id = id,
        title = title,
        visibility = visibility,
        # The component does not belong to itself.
        package_metadata = [],
    )
    if docs:
        docs_bundle(
            name = docs,
            code_targets = code_targets,
            primary_need_id = id,
            source_dir = docs_source_dir,
            visibility = visibility,
            **kwargs
        )

# ---------------------------------------------------------------------------
# Files per component, derived from the dependency graph.

ComponentFilesInfo = provider(
    doc = "Transitive (component, file, owner) entries collected by the aspect.",
    fields = {"entries": "depset of structs with component (label string or None), id, path, owner"},
)

_SOURCE_ATTRS = ["srcs", "hdrs", "textual_hdrs"]
_DEP_ATTRS = ["deps", "implementation_deps", "srcs", "hdrs", "textual_hdrs"]

def _components_of(ctx):
    """The ScoreComponentInfo targets in this rule's package_metadata, if any."""
    attr = ctx.rule.attr
    metadata = []
    if hasattr(attr, "package_metadata"):
        metadata = attr.package_metadata
    elif hasattr(attr, "applicable_licenses"):
        metadata = attr.applicable_licenses
    return [t[ScoreComponentInfo] for t in metadata if ScoreComponentInfo in t]

def _component_files_aspect_impl(target, ctx):
    transitive = []
    for a in _DEP_ATTRS:
        for dep in getattr(ctx.rule.attr, a, []) or []:
            if type(dep) == "Target" and ComponentFilesInfo in dep:
                transitive.append(dep[ComponentFilesInfo].entries)

    direct = []
    testonly = getattr(ctx.rule.attr, "testonly", False)

    # Main repository only; test-only targets (tests, mocks) are traversed for
    # their deps but contribute no implementation files.
    if ctx.label.workspace_name == "" and not testonly:
        comps = _components_of(ctx)
        if len(comps) > 1:
            fail("{} belongs to several components: {}".format(ctx.label, [c.id for c in comps]))
        comp = comps[0] if comps else None
        for a in _SOURCE_ATTRS:
            for f in getattr(ctx.rule.files, a, []) or []:
                if f.is_source or f.owner.workspace_name == "":
                    direct.append(struct(
                        component = comp.label if comp else None,
                        id = comp.id if comp else None,
                        path = f.short_path,
                        owner = str(ctx.label),
                    ))
    return [ComponentFilesInfo(entries = depset(direct, transitive = transitive))]

component_files_aspect = aspect(
    implementation = _component_files_aspect_impl,
    attr_aspects = _DEP_ATTRS,
    doc = "Collects every main-repository source file with the component of its owner target.",
)

def _score_component_files_impl(ctx):
    by_component = {}
    for e in depset(transitive = [d[ComponentFilesInfo].entries for d in ctx.attr.deps]).to_list():
        key = e.id or "(unassigned)"
        if ctx.attr.component and e.component != str(ctx.attr.component.label):
            continue
        files = by_component.setdefault(key, {})
        files.setdefault(e.path, []).append(e.owner)

    lines = []
    for comp in sorted(by_component.keys()):
        lines.append("[%s]" % comp)
        for path in sorted(by_component[comp].keys()):
            lines.append("%s\t%s" % (path, ",".join(sorted(by_component[comp][path]))))
    out = ctx.actions.declare_file(ctx.label.name + ".txt")
    ctx.actions.write(out, "\n".join(lines) + "\n")

    out_json = ctx.actions.declare_file(ctx.label.name + ".json")
    ctx.actions.write(out_json, json.encode_indent({
        "version": 1,
        "components": [
            {
                "id": comp,
                "files": sorted(by_component[comp].keys()),
            }
            for comp in sorted(by_component.keys())
        ],
    }, indent = "  "))
    return [DefaultInfo(files = depset([out, out_json]))]

score_component_files = rule(
    implementation = _score_component_files_impl,
    attrs = {
        "deps": attr.label_list(
            aspects = [component_files_aspect],
            doc = "Entry points; the aspect follows deps, implementation_deps and sources.",
        ),
        "component": attr.label(
            providers = [ScoreComponentInfo],
            doc = "Restrict the output to this component; default: all, plus '(unassigned)'.",
        ),
    },
    doc = """Lists source files per component, as seen in the current configuration.

Writes ``<name>.txt`` (one ``[component]`` block, one ``path<TAB>owners`` line
per file) and ``<name>.json``. Files of targets without a component appear
under ``(unassigned)``. Build with ``--platforms`` to see the set for another
platform; ``select()`` resolves before the aspect runs.
""",
)
