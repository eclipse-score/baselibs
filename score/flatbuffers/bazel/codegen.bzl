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

"""Starlark rules for generating source files from FlatBuffer schemas using flatc."""

def _generate_cpp_impl(ctx):
    """Implementation of the generate_cpp rule."""

    # Input .fbs file
    fbs_file = ctx.file.schema

    # Get the flatc compiler using absolute path from flatbuffers repository
    flatc = ctx.executable._flatc

    # flatc generates <basename>_generated.h by default
    # Use temporary subdirectory based on target to avoid conflicts
    default_name = fbs_file.basename.replace(".fbs", "_generated.h")
    temp_subdir = "tmp_{}".format(ctx.label.name)
    generated_file = ctx.actions.declare_file("{}/{}".format(temp_subdir, default_name))
    out_header = ctx.actions.declare_file(ctx.attr.output)

    # Generate the header file
    # flatc C++ Code Generation Options reference: https://flatbuffers.dev/flatc/
    #
    # Options considered and their decisions:
    #
    # --cpp (REQUIRED)
    #   Generate C++ code from FlatBuffer schema
    #
    # --cpp-std c++11 (EXPLICITLY SET TO C++11)
    #   Use C++11 code generation. Alternatives: c++0x (legacy), c++17 (experimental)
    #   DECISION: c++11 explicitly set to avoid experimental features and ensure reproducible builds
    #
    # --scoped-enums (ENABLED)
    #   Use C++11 style scoped enums (enum class). Adds type safety.
    #   DECISION: ENABLED by default - improves type safety by preventing accidental enum value
    #   collisions, enforces explicit enum scope qualification, and prevents implicit integer
    #   conversions.
    #
    # --gen-mutable (OUT OF SCOPE)
    #   Generate non-const accessors for in-place mutation of FlatBuffers.
    #   DECISION: Out of scope for current use case - read-only access is currently intended.
    #   Mutation support can be added in future if needed for extended use cases.
    #
    # --gen-object-api (OUT OF SCOPE)
    #   Generate object-based API for convenient construction/mutation.
    #   DECISION: Out of scope for current use case - read-only buffer access is currently intended.
    #   This option relies on heap allocation for object construction, which conflicts with
    #   potential future heap-free buffer building requirements in safety-critical systems.
    #
    # --gen-name-strings (NOT USED)
    #   Generate GetFullyQualifiedName() methods for table structs (e.g., "my_component.demo.MyComponentConfig").
    #   DECISION: Not used - provides table type name reflection which is not needed for read-only buffer access.
    #
    # --gen-all (NOT USED)
    #   Generate code for all included schema files, not just current one.
    #   DECISION: Not used - this rule generates C++ headers only for the specified schema.
    #   Multi-schema code generation is out of scope; use separate rule instances for dependencies.
    #
    # --no-includes (NOT USED)
    #   Don't generate include statements for included schemas.
    #   DECISION: Not used - includes are standard practice for dependent schemas
    #
    # --cpp-include (NOT USED)
    #   Add custom #include in generated file (e.g., --cpp-include "my_custom_include.h").
    #   DECISION: Not used - can be added once necessary

    args = ctx.actions.args()
    args.add("--cpp")
    args.add("--cpp-std", "c++11")
    args.add("--scoped-enums")
    args.add("-o", generated_file.dirname)
    args.add(fbs_file.path)

    ctx.actions.run(
        inputs = [fbs_file],
        outputs = [generated_file],
        executable = flatc,
        arguments = [args],
        mnemonic = "FlatbuffersCompile",
        progress_message = "Generating C++ header from %s" % fbs_file.short_path,
    )

    # Symlink to the requested output name
    ctx.actions.symlink(output = out_header, target_file = generated_file)

    return [DefaultInfo(files = depset([out_header]))]

generate_cpp = rule(
    implementation = _generate_cpp_impl,
    attrs = {
        "schema": attr.label(
            allow_single_file = [".fbs"],
            mandatory = True,
            doc = "The .fbs FlatBuffer schema file to compile",
        ),
        "output": attr.string(
            mandatory = True,
            doc = "The name of the generated C++ header file",
        ),
        "_flatc": attr.label(
            default = "@flatbuffers//:flatc",
            executable = True,
            cfg = "exec",
            doc = "The flatc compiler (absolute path from flatbuffers repository)",
        ),
    },
    doc = """Generates a C++ header file from a FlatBuffer schema (.fbs) file.
    
    This rule uses the flatc compiler from the @flatbuffers repository with
    absolute paths, making it usable outside of this repository.
    
    Example:
        generate_cpp(
            name = "demo_flatbuffer",
            schema = "demo.fbs",
            output = "demo_config.h",
        )
    """,
)
