#!/usr/bin/env python3

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

# fmt: off

N = 64

for x in range(N, 0, -1):
    strange = [str(t) for t in range(1, x + 1)]
    print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
    print("// Macros of Field names and can't use parenthesis.")
    print("// coverity[autosar_cpp14_a16_0_1_violation]")
    print("// coverity[autosar_cpp14_m16_0_6_violation]")
    print("// coverity[autosar_cpp14_m16_3_1_violation]")
    print("#define STRUCT_VISITABLE{}(S, F{}) \\".format(x, ", F".join(strange)))
    print("    STRUCT_VISITABLE_FULL_DEFINITION(S, \\")
    print("    ::bmw::common::visitor::detail::pack_values(#F{}), s.F{}) \\".format(", #F".join(strange), ", s.F".join(strange)))
    print("")

strange = [str(t) for t in range(1, N + 1)]
print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("// Macros for pass number of Arguments.")
print("// Macros are used to pass length of args and can't use parenthesis")
print("// coverity[autosar_cpp14_a16_0_1_violation]")
print("// coverity[autosar_cpp14_m16_0_6_violation]")
print("#define GET_NTH_ARG(_{}, N, ...) N".format(", _".join(strange)))

print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("// Macros for counting number of args.")
print("// Macros are used to pass count args length and can't use parenthesis")
print("// coverity[autosar_cpp14_a16_0_1_violation]")
print("// coverity[autosar_cpp14_m16_0_6_violation]")
print("#define COUNT_VARARGS(...) GET_NTH_ARG(__VA_ARGS__, {}, 0)".format(", ".join(strange[::-1])))

print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("// Macros for Concatenation.")
print("// Macros are used to concatenate names and can't use parenthesis")
print("// coverity[autosar_cpp14_a16_0_1_violation]")
print("// coverity[autosar_cpp14_m16_0_6_violation]")
print("#define DDAD_CONCATENATE2(A, B) A##B")

print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("// Macros for Concatenation.")
print("// Macros are used to concatenate names and can't use parenthesis")
print("// coverity[autosar_cpp14_a16_0_1_violation]")
print("// coverity[autosar_cpp14_m16_0_6_violation]")
print("#define DDAD_CONCATENATE(A, B) DDAD_CONCATENATE2(A, B)")
print("")
print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("// Macros for Concatenation.")
print("// Macros are used to concatenate names and can't use parenthesis")
print("// coverity[autosar_cpp14_a16_0_1_violation]")
print("// coverity[autosar_cpp14_m16_0_6_violation]")
print("#define STRUCT_VISITABLE(S, ...) DDAD_CONCATENATE(STRUCT_VISITABLE, COUNT_VARARGS(__VA_ARGS__))(S, __VA_ARGS__)")
