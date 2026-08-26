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

# Use Dockerfile to get dependabot version bumps after new image is released
FROM ghcr.io/eclipse-score/devcontainer:v1.4.1

# graphviz provides the 'dot' executable required by PlantUML when rendering
# diagrams during `bazel run //:live_preview` / docs builds.
RUN apt-get update \
    && apt-get install -y --no-install-recommends graphviz \
    && rm -rf /var/lib/apt/lists/*

# The base image only ships the POSIX/C locales. Sphinx calls
# locale.setlocale(LC_ALL, '') at startup, which aborts with
# "unsupported locale setting" if LANG/LC_ALL point to a locale (e.g.
# en_US.UTF-8) that isn't generated. C.UTF-8 is always available and is
# enough for a UTF-8 capable build.
ENV LANG=C.UTF-8 \
    LC_ALL=C.UTF-8
