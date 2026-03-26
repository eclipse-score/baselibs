# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
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

    args = ctx.actions.args()
    args.add("--binary")
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

    # Get files and output paths (they're in the same order from the dict)
    data_files = ctx.files.data_dict
    output_paths = list(ctx.attr.data_dict.values())

    for data_file, output_path in zip(data_files, output_paths):
        # When converting JSON to binary, flatc generates a file named after the JSON file
        default_name = data_file.basename.replace(".json", ".bin")
        temp_subdir = "tmp_{}_{}".format(ctx.label.name, data_file.basename.replace(".json", "").replace(".", "_"))
        generated_file = ctx.actions.declare_file("{}/{}".format(temp_subdir, default_name))
        out_bin = ctx.actions.declare_file(output_path)

        args = ctx.actions.args()
        args.add("--binary")
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
