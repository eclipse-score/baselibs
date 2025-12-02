/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#pragma once

#include <optional>
#include <string>

#include "scenario.hpp"

class BasicScenario final : public Scenario
{
public:
    ~BasicScenario() final = default;

    std::string name() const final;

    void run(const std::string &input) const final;
};
