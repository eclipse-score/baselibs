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
# Use this macro for loading internal targets.
# The `custom.bzl` files are not imported from the open-source repository.
# Therefore, targets defined here are not affected between syncs.
def load_custom_targets(name = "custom_targets"):
    pass

def load_custom_test_suites(name = "custom_test_suites"):
    return []
