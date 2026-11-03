# Linux LLVM coverage

Generate a coverage report for the C++ and Rust libraries from the repository root:

```sh
./.github/tools/check_coverage_scope.sh
bazel coverage --config=llvm_cov --build_tests_only -- //score/... \
  -//score/language/safecpp/aborts_upon_exception/... \
  -//score/language/safecpp/safe_math/details:floating_point_environment_test \
  -//score/os/linux/utils/test:network_interface_test
COVERAGE_THRESHOLD=0 bazel run @score_coverage//:generate_coverage_html -- \
  --yaml tools/coverage/coverage_justifications.yaml \
  --testlogs-subdir score --archive-dir coverage-report
```

The LLVM config sets the Linux x86_64 platform and coverage toolchains itself.
Use it without `--config=score-linux-x86_64` to avoid registering the GCC toolchain after Clang.
The excluded tests and their reasons are documented in [the coverage workflow](../../.github/workflows/coverage_report.yml).
Use the full test selection above when generating a repository coverage report.

## View the results

Open `coverage_linux/index.html` for the detailed HTML report.
The `coverage-report/` directory contains the report, plain LCOV data in `coverage_report.dat`, test results, and justification results.

Check `coverage-report/unmapped_files.txt` for in-scope files without coverage data.
The scope check verifies that production targets are included or explicitly excluded; it does not verify that every source file was instrumented.

`COVERAGE_THRESHOLD=0` generates the report without enforcing a minimum percentage.
Set it to the required percentage to enforce a minimum effective line coverage after applying the justifications in `coverage_justifications.yaml`.

## Include coverage in the local documentation

The report command above writes the LCOV file to `coverage-report/coverage_report.dat`, where the documentation build reads it.
Build the documentation with:

```sh
bazel run //:docs
```

Open `_build/index.html` to view the documentation.
Component coverage summaries use raw LCOV measurements for each component's exported code files.
These summaries do not apply coverage justifications.

## CI reports

The Linux workflow generates coverage and passes the report to the documentation build.
Download the `<repository-name>_cpp_coverage_report` artifact from the workflow run to inspect the detailed report.
