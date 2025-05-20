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

# Use the official Ubuntu 24.04 as the base image
FROM ubuntu:24.04

# Set the DEBIAN_FRONTEND to noninteractive to avoid prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Set labels
LABEL maintainer="Private"
LABEL version="0.0.1"
LABEL description="S-CORE baselibs test Docker image"

# Install required dependencies (without prompts)
RUN apt-get update && apt-get install -y \
    build-essential \
    docker.io \
    gnupg \
    graphviz \
    lld \
    g++ \
    python3 \
    zip \
    unzip \
    clang \
    curl \
    sudo \
    git \
    software-properties-common \
    openjdk-11-jdk \
    ca-certificates \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Install lib
RUN wget http://security.ubuntu.com/ubuntu/pool/universe/n/ncurses/libtinfo5_6.3-2ubuntu0.1_amd64.deb \
    && apt install ./libtinfo5_6.3-2ubuntu0.1_amd64.deb

# Download and install Bazelisk
RUN curl -Lo /usr/local/bin/bazel https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64 \
    && chmod +x /usr/local/bin/bazel

# Create bazelisk cache directory
RUN mkdir /bazelisk-cache

# Pre-install bazel
ENV BAZELISK_HOME=/bazelisk-cache
RUN USE_BAZEL_VERSION=8.2.1 bazel version
RUN USE_BAZEL_VERSION=latest bazel version

RUN chmod -R 777 /bazelisk-cache

# Setup non-root user
ADD create_user.sh /sbin/create_user.sh

ENTRYPOINT ["/sbin/create_user.sh"]

# Set the working directory
WORKDIR /workspace
