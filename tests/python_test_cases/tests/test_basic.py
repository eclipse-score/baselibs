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

from pathlib import Path
from typing import Any

import pytest
from testing_utils import LogContainer, ScenarioResult

from .common import CitScenario, ResultCode, create_json_file

try:
    from attribute_plugin import add_test_properties  # type: ignore[import-untyped]
except ImportError:
    # Define no-op decorator if attribute_plugin is not available (outside bazel)
    # Keeps IDE debugging functionality
    def add_test_properties(*args, **kwargs):
        def decorator(func):
            return func  # No-op decorator

        return decorator


@add_test_properties(
    partially_verifies=["comp_req__json__deserialization"],
    test_type="requirements-based",
    derivation_technique="requirements-analysis",
)
class TestSmoke(CitScenario):
    """
    Smoke test for basic JSON deserialization.
    """

    @pytest.fixture(scope="class")
    def scenario_name(self, *_, **__) -> str:
        return "basic.basic"

    @pytest.fixture(scope="class")
    def json_data(self) -> dict[str, Any]:
        return {"string_key": "value", "number": 42}

    @pytest.fixture(scope="class")
    def json_file(self, temp_dir: Path, json_data) -> Path:
        json_path = temp_dir / "data.json"
        create_json_file(json_path, json_data)
        return json_path

    @pytest.fixture(scope="class")
    def test_config(self, json_file: Path) -> dict[str, Any]:
        return {"test_logic": {"json_path": str(json_file)}}

    def test_returncode_ok(self, results: ScenarioResult):
        assert results.return_code == ResultCode.SUCCESS

    def test_string_value(self, logs_target: LogContainer, json_data):
        log = logs_target.find_log("string_key")
        assert log.string_key == json_data["string_key"]

    def test_number_value(self, logs_target: LogContainer, json_data):
        log = logs_target.find_log("number")
        assert log.number == json_data["number"]
