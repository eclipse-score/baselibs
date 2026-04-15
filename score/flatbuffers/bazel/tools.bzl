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
    args.add("-o", generated_file.dirname)
    args.add(schema_file.path)
    args.add(data_file.path)

    ctx.actions.run(
        inputs = [data_file, schema_file],
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
        )
    """,
)

def _serialize_multiple_buffers_impl(ctx):
    """Implementation of the serialize_multiple_buffers rule."""

    # Input schema file
    schema_file = ctx.file.schema

    # Get the flatc compiler using absolute path from flatbuffers repository
    flatc = ctx.executable._flatc

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
        args.add("-o", generated_file.dirname)
        args.add(schema_file.path)
        args.add(data_file.path)

        ctx.actions.run(
            inputs = [data_file, schema_file],
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
        )
    """,
)

def _generate_json_schema_impl(ctx):
    """Implementation of the generate_json_schema rule."""

    # Input files
    schema_file = ctx.file.schema

    # Get the flatc compiler using absolute path from flatbuffers repository
    flatc = ctx.executable._flatc

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
    args.add("-o", generated_file.dirname)
    args.add(schema_file.path)

    ctx.actions.run(
        inputs = [schema_file],
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
        "_flatc": attr.label(
            default = "@flatbuffers//:flatc",
            executable = True,
            cfg = "exec",
            doc = "The flatc compiler (absolute path from flatbuffers repository)",
        ),
    },
    doc = """Generates a JSON schema from a FlatBuffer schema.

    Example:
        generate_json_schema(
            name = "demo_schema",
            schema = "demo.fbs",
            output = "demo.schema.json",
        )
    """,
)
