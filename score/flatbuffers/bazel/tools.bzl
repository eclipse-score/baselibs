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

"""Starlark rules for configuration file processing using flatc from FlatBuffers."""

def _serialize_buffer_impl(ctx):
    """Implementation of the serialize_buffer rule."""

    # Input files
    data_file = ctx.file.data
    schema_file = ctx.file.schema

    # Get the flatc compiler using absolute path from flatbuffers repository
    flatc = ctx.executable._flatc

    # Collect include directories from included .fbs files
    include_files = ctx.files.includes
    include_dirs = {files.dirname: True for files in include_files}

    # When converting JSON to binary, flatc generates a file named after the JSON file
    default_name = data_file.basename.replace(".json", ".bin")
    temp_subdir = "tmp_{}".format(ctx.label.name)
    generated_file = ctx.actions.declare_file("{}/{}".format(temp_subdir, default_name))
    out_bin = ctx.actions.declare_file(ctx.attr.output)

    # Options for flatc --binary: Convert JSON data to a binary FlatBuffer.
    # Options that apply only to other modes are not listed.
    # flatc reference: https://flatbuffers.dev/flatc/
    #
    # Options considered and their decisions:
    #
    # --binary (REQUIRED)
    #   Generate a binary FlatBuffer from a JSON data file using the provided schema.
    #
    # --strict-json (ENABLED)
    #   Require strict JSON compliance: field names must be enclosed in double quotes
    #   and trailing commas are not allowed. By default, flatc is lenient and accepts
    #   unquoted field names and trailing commas.
    #   DECISION: ENABLED - enforce files that a compliant to standard JSON tooling
    #   (validators, formatters, editors, linters).
    #
    # --force-defaults (NOT USED)
    #   Emit fields set to their default value in the binary output. By default,
    #   flatc omits default-valued fields to minimize binary size.
    #   DECISION: Not used - omitting defaults is the intended FlatBuffers behavior,
    #   reduces binary size, and consumers always read correct defaults via the
    #   generated accessor code.
    #
    # --size-prefixed (NOT USED)
    #   Interpret input/output buffers as size-prefixed. When used with --binary,
    #   the generated binary buffer is prefixed with its size.
    #   DECISION: Not used - our buffers are stored as standalone files with known
    #   sizes. Size-prefixed buffers add complexity on the consumer side without
    #   benefit for file-based configuration.
    #
    # --schema (NOT USED)
    #   Serialize the schema definition itself into a binary schema file (.bfbs)
    #   instead of serializing JSON data.
    #   DECISION: Not used - this rule serializes JSON data to binary buffers.
    #   Binary schema generation is a separate concern.
    #
    # --root-type T (NOT USED)
    #   Select or override the default root_type defined in the schema.
    #   DECISION: Not used - schemas should explicitly define their own root_type.
    #   Overriding it via the build rule could cause confusion between schema
    #   definition and build configuration.
    #
    # --flexbuffers (NOT USED)
    #   When used with --binary, generate schema-less FlexBuffer data instead of
    #   typed FlatBuffer data.
    #   DECISION: Not used - we use typed FlatBuffer schemas for configuration data.
    #   Schema-less FlexBuffers lose type safety and schema validation guarantees.
    #
    # --json-nested-bytes (NOT USED)
    #   Allow a nested_flatbuffer field to be parsed as a vector of bytes in JSON,
    #   which is unsafe unless checked by a verifier afterwards.
    #   DECISION: Not used - bypasses type safety for nested buffers. Not needed
    #   for standard configuration data.
    #
    # --allow-non-utf8 (NOT USED)
    #   Pass non-UTF-8 input through the parser instead of raising a parse error.
    #   DECISION: Not used - configuration data should contain valid UTF-8 strings.

    args = ctx.actions.args()
    args.add("--binary")
    args.add("--strict-json")
    for inc_dir in include_dirs:
        args.add("-I", inc_dir)
    args.add("-o", generated_file.dirname)
    args.add(schema_file.path)
    args.add(data_file.path)

    ctx.actions.run(
        inputs = [data_file, schema_file] + include_files,
        outputs = [generated_file],
        executable = flatc,
        arguments = [args],
        mnemonic = "FlatbuffersSerializeBuffer",
        progress_message = "Generating binary data from %s" % data_file.short_path,
    )

    # Symlink to the requested output name
    ctx.actions.symlink(output = out_bin, target_file = generated_file)

    return [DefaultInfo(files = depset([out_bin]))]

serialize_buffer = rule(
    implementation = _serialize_buffer_impl,
    attrs = {
        "data": attr.label(
            allow_single_file = [".json"],
            mandatory = True,
            doc = "The JSON data file to convert to binary buffer",
        ),
        "schema": attr.label(
            allow_single_file = [".fbs"],
            mandatory = True,
            doc = "The .fbs FlatBuffer schema file that defines the data structure",
        ),
        "output": attr.string(
            mandatory = True,
            doc = "The name of the generated binary data file (should end with .bin)",
        ),
        "includes": attr.label_list(
            allow_files = [".fbs"],
            default = [],
            doc = "Additional .fbs files required to resolve include directives in the schema.",
        ),
        "_flatc": attr.label(
            default = "@flatbuffers//:flatc",
            executable = True,
            cfg = "exec",
            doc = "The flatc compiler (absolute path from flatbuffers repository)",
        ),
    },
    doc = """Generates a binary buffer file from a JSON data file using a FlatBuffer schema.

    Example:
        serialize_buffer(
            name = "demo_data",
            data = "demo_data.json",
            schema = "demo.fbs",
            output = "demo_data.bin",
            includes = ["some_other.fbs"],
        )
    """,
)

def _serialize_multiple_buffers_impl(ctx):
    """Implementation of the serialize_multiple_buffers rule."""

    # Input schema file
    schema_file = ctx.file.schema

    # Get the flatc compiler using absolute path from flatbuffers repository
    flatc = ctx.executable._flatc

    # Collect include directories from included .fbs files
    include_files = ctx.files.includes
    include_dirs = {files.dirname: True for files in include_files}

    # Parse the data files dict and process each data file
    output_files = []

    for data_key, data_value in ctx.attr.data_dict.items():
        # data_key is a Target; extract the single File via .files.to_list()[0] (safe since allow_files = [".json"]).
        data_file = data_key.files.to_list()[0]

        # When converting JSON to binary, flatc generates a file named after the JSON file
        default_name = data_file.basename.replace(".json", ".bin")
        temp_subdir = "tmp_{}_{}".format(ctx.label.name, data_file.basename.replace(".json", "").replace(".", "_"))
        generated_file = ctx.actions.declare_file("{}/{}".format(temp_subdir, default_name))
        out_bin = ctx.actions.declare_file(data_value)

        # Options for flatc --binary: Convert JSON data to a binary FlatBuffer.
        # Refer to the options and decisions in the _serialize_buffer_impl function.

        args = ctx.actions.args()
        args.add("--binary")
        args.add("--strict-json")
        for inc_dir in include_dirs:
            args.add("-I", inc_dir)
        args.add("-o", generated_file.dirname)
        args.add(schema_file.path)
        args.add(data_file.path)

        ctx.actions.run(
            inputs = [data_file, schema_file] + include_files,
            outputs = [generated_file],
            executable = flatc,
            arguments = [args],
            mnemonic = "FlatbuffersSerializeMultipleBuffers",
            progress_message = "Generating binary data from %s" % data_file.short_path,
        )

        # Symlink to the requested output name
        ctx.actions.symlink(output = out_bin, target_file = generated_file)

        output_files.append(out_bin)

    return [DefaultInfo(files = depset(output_files))]

serialize_multiple_buffers = rule(
    implementation = _serialize_multiple_buffers_impl,
    attrs = {
        "schema": attr.label(
            allow_single_file = [".fbs"],
            mandatory = True,
            doc = "The .fbs FlatBuffer schema file that defines the data structure",
        ),
        "data_dict": attr.label_keyed_string_dict(
            allow_files = [".json"],
            mandatory = True,
            doc = "A mapping of input JSON file paths to output binary buffer file paths (e.g., {':demo_data.json': 'demo_data.bin', ':subdir/demo_data2.json': 'subdir/demo_data2.bin'})",
        ),
        "includes": attr.label_list(
            allow_files = [".fbs"],
            default = [],
            doc = "Additional .fbs files required to resolve include directives in the schema.",
        ),
        "_flatc": attr.label(
            default = "@flatbuffers//:flatc",
            executable = True,
            cfg = "exec",
            doc = "The flatc compiler (absolute path from flatbuffers repository)",
        ),
    },
    doc = """Generates multiple binary buffer files from JSON data files using a FlatBuffer schema.

    Example:
        serialize_multiple_buffers(
            name = "demo_data",
            schema = "demo.fbs",
            data_dict = {
                "demo_data.json": "demo_data.bin",
                "subdir/demo_data2.json": "subdir/demo_data2.bin",
            },
            includes = ["some_other.fbs"],
        )
    """,
)

def _inject_buffer_version_impl(ctx):
    """Implementation of the _inject_buffer_version rule.

    Runs inject_buffer_version.py to inject major/minor version fields into a JSON file.
    The output is a patched copy of the input JSON, suitable for passing to
    serialize_buffer as the data source.
    """
    data_file = ctx.file.data
    patched_json = ctx.actions.declare_file(ctx.attr.output)

    patch_args = ctx.actions.args()
    patch_args.add("--input", data_file.path)
    patch_args.add("--output", patched_json.path)
    patch_args.add("--major", str(ctx.attr.major_version))
    patch_args.add("--minor", str(ctx.attr.minor_version))

    ctx.actions.run(
        inputs = [data_file],
        outputs = [patched_json],
        executable = ctx.executable._inject_buffer_version,
        arguments = [patch_args],
        mnemonic = "PatchBufferVersion",
        progress_message = "Patching version into %s" % data_file.short_path,
    )

    return [DefaultInfo(files = depset([patched_json]))]

_inject_buffer_version = rule(
    implementation = _inject_buffer_version_impl,
    attrs = {
        "data": attr.label(
            allow_single_file = [".json"],
            mandatory = True,
            doc = "The JSON data file to patch with version information.",
        ),
        "output": attr.string(
            mandatory = True,
            doc = "The name of the patched JSON output file.",
        ),
        "major_version": attr.int(
            mandatory = True,
            doc = "Major version number to inject. Must be in [0, 65535] (uint16_t).",
        ),
        "minor_version": attr.int(
            mandatory = True,
            doc = "Minor version number to inject. Must be in [0, 65535] (uint16_t).",
        ),
        "_inject_buffer_version": attr.label(
            default = "//score/flatbuffers/bazel:inject_buffer_version",
            executable = True,
            cfg = "exec",
            doc = "The inject_buffer_version helper script for JSON version injection.",
        ),
    },
    doc = "Injects major/minor version fields into a JSON file for use with serialize_versioned_buffer.",
)

def serialize_versioned_buffer(
        name,
        data,
        schema,
        output,
        buffer_major_version,
        buffer_minor_version,
        includes = [],
        **kwargs):
    """Injects version information into a JSON file and serializes it to a binary FlatBuffer.

    This macro composes _inject_buffer_version and serialize_buffer: it first injects the
    given major/minor version into the JSON data file, then passes the patched
    JSON to serialize_buffer for compilation.

    Args:
        name: Target name.
        data: The JSON data file label to version-patch and serialize.
        schema: The .fbs FlatBuffer schema file label. The schema must include
            buffer_version.fbs manually (e.g. `include "buffer_version.fbs";`).
        output: The name of the generated binary output file (should end with .bin).
        buffer_major_version: Major version to inject (uint16_t, must be > 0 to have effect).
        buffer_minor_version: Minor version to inject (uint16_t).
        includes: Additional .fbs files required to resolve include directives in the schema.
            @score_baselibs//score/flatbuffers/common:buffer_version.fbs is always added
            automatically and must not be listed here.
        **kwargs: Additional arguments forwarded to serialize_buffer (e.g. visibility, tags).

    Example:
        serialize_versioned_buffer(
            name = "demo_data",
            data = "demo_data.json",
            schema = "demo.fbs",
            output = "demo_data.bin",
            includes = ["some_other.fbs"],
            buffer_major_version = 2,
            buffer_minor_version = 3,
        )
    """
    patched_name = name + "_patched_json"
    patched_output = name + "_patched.json"

    _inject_buffer_version(
        name = patched_name,
        data = data,
        output = patched_output,
        major_version = buffer_major_version,
        minor_version = buffer_minor_version,
        **kwargs
    )

    serialize_buffer(
        name = name,
        data = ":" + patched_name,
        schema = schema,
        output = output,
        includes = ["@score_baselibs//score/flatbuffers/common:buffer_version.fbs"] + includes,
        **kwargs
    )

def serialize_multiple_versioned_buffers(
        name,
        data_dict,
        schema,
        buffer_major_version,
        buffer_minor_version,
        includes = [],
        **kwargs):
    """Injects version information into multiple JSON files and serializes them to binary FlatBuffers.

    This macro composes _inject_buffer_version and serialize_multiple_buffers: for each entry in
    data_dict it creates an _inject_buffer_version target that injects the given major/minor version,
    then passes all patched JSON files to serialize_multiple_buffers.

    Args:
        name: Target name.
        data_dict: A dict mapping input JSON file labels to output binary file paths.
        schema: The .fbs FlatBuffer schema file label. The schema must include
            buffer_version.fbs manually (e.g. `include "buffer_version.fbs";`).
        buffer_major_version: Major version to inject into every buffer (uint16_t).
        buffer_minor_version: Minor version to inject into every buffer (uint16_t).
        includes: Additional .fbs files required to resolve include directives in the schema.
            @score_baselibs//score/flatbuffers/common:buffer_version.fbs is always added
            automatically and must not be listed here.
        **kwargs: Additional arguments forwarded to serialize_multiple_buffers.

    Example:
        serialize_multiple_versioned_buffers(
            name = "demo_data",
            schema = "demo.fbs",
            data_dict = {
                "demo_data.json": "demo_data.bin",
                "subdir/demo_data2.json": "subdir/demo_data2.bin",
            },
            includes = ["some_other.fbs"],
            buffer_major_version = 1,
            buffer_minor_version = 0,
        )
    """
    patched_data_dict = {}

    for data_label, output_path in data_dict.items():
        # Derive a stable target name from the output path
        safe_key = output_path.replace("/", "_").replace(".", "_")
        patched_name = name + "_patched_" + safe_key
        patched_output = name + "_patched_" + safe_key + ".json"

        _inject_buffer_version(
            name = patched_name,
            data = data_label,
            output = patched_output,
            major_version = buffer_major_version,
            minor_version = buffer_minor_version,
            **kwargs
        )

        patched_data_dict[":" + patched_name] = output_path

    serialize_multiple_buffers(
        name = name,
        schema = schema,
        data_dict = patched_data_dict,
        includes = ["@score_baselibs//score/flatbuffers/common:buffer_version.fbs"] + includes,
        **kwargs
    )

def _generate_json_schema_impl(ctx):
    """Implementation of the generate_json_schema rule."""

    # Input files
    schema_file = ctx.file.schema

    # Get the flatc compiler using absolute path from flatbuffers repository
    flatc = ctx.executable._flatc

    # Collect include directories from included .fbs files
    include_files = ctx.files.includes + [ctx.file._buffer_version_fbs]
    include_dirs = {f.dirname: True for f in include_files}

    # When generating JSON schema, flatc generates a file named after the schema file
    default_name = schema_file.basename.replace(".fbs", ".schema.json")
    temp_subdir = "tmp_{}".format(ctx.label.name)
    generated_file = ctx.actions.declare_file("{}/{}".format(temp_subdir, default_name))
    out_schema = ctx.actions.declare_file(ctx.attr.output)

    # Options for flatc --jsonschema: Generate a JSON Schema from a FlatBuffer schema.
    # Options that apply only to other modes are not listed.
    # flatc reference: https://flatbuffers.dev/flatc/
    #
    # Options considered and their decisions:
    #
    # --jsonschema (REQUIRED)
    #   Generate a JSON Schema file from the FlatBuffer schema definition.
    #
    # --root-type T (NOT USED)
    #   Override the root_type declared in the schema. Determines which table
    #   becomes the top-level object in the generated JSON Schema.
    #   DECISION: Not used - the root_type must be declared in the schema itself,
    #   keeping the JSON Schema output self-contained and independent of build
    #   rule parameters.

    args = ctx.actions.args()
    args.add("--jsonschema")
    for inc_dir in include_dirs:
        args.add("-I", inc_dir)
    args.add("-o", generated_file.dirname)
    args.add(schema_file.path)

    ctx.actions.run(
        inputs = [schema_file] + include_files,
        outputs = [generated_file],
        executable = flatc,
        arguments = [args],
        mnemonic = "FlatbuffersJsonSchema",
        progress_message = "Generating JSON schema from %s" % schema_file.short_path,
    )

    # Symlink to the requested output name
    ctx.actions.symlink(output = out_schema, target_file = generated_file)

    return [DefaultInfo(files = depset([out_schema]))]

generate_json_schema = rule(
    implementation = _generate_json_schema_impl,
    attrs = {
        "schema": attr.label(
            allow_single_file = [".fbs"],
            mandatory = True,
            doc = "The .fbs FlatBuffer schema file to generate JSON schema from",
        ),
        "output": attr.string(
            mandatory = True,
            doc = "The name of the generated JSON schema file (should end with .schema.json)",
        ),
        "includes": attr.label_list(
            allow_files = [".fbs"],
            default = [],
            doc = "Additional .fbs files required to resolve include directives in the schema.",
        ),
        "_flatc": attr.label(
            default = "@flatbuffers//:flatc",
            executable = True,
            cfg = "exec",
            doc = "The flatc compiler (absolute path from flatbuffers repository)",
        ),
        "_buffer_version_fbs": attr.label(
            default = "@score_baselibs//score/flatbuffers/common:buffer_version.fbs",
            allow_single_file = [".fbs"],
            doc = "Automatically included buffer_version.fbs for common buffer version support.",
        ),
    },
    doc = """Generates a JSON schema from a FlatBuffer schema.

    @score_baselibs//score/flatbuffers/common:buffer_version.fbs is always included
    automatically. The schema must include buffer_version.fbs manually if it uses
    the common buffer version (e.g. `include "buffer_version.fbs";`).

    Example:
        generate_json_schema(
            name = "demo_schema",
            schema = "demo.fbs",
            output = "demo.schema.json",
            includes = ["some_other.fbs"],
        )
    """,
)
