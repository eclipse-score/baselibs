#!/usr/bin/env bash

bazel run @score_tooling//format_checker:rustfmt_with_policies
