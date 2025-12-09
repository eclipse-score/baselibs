# Component Integration Tests

For general information check [main README.md file](../README.md).

## Setup

Create `venv`, activate and install dependencies:

```bash
python -m venv <REPO_ROOT>/.venv
source <REPO_ROOT>/.venv/bin/activate
pip install -r <REPO_ROOT>/tests/python_test_cases/requirements.txt
```

## Usage

Set current working directory to the following:

```bash
cd <REPO_ROOT>/tests/python_test_cases
```

### Run tests

Basic run:

```bash
pytest .
```

Run with additional flags:

```bash
pytest -vsx . -k <PATTERN> --build-scenarios
```

- `-v` - increase verbosity.
- `-s` - show logs (disable capture).
- `-k <PATTERN>` - run tests matching the pattern.
- `--build-scenarios` - build Rust test scenarios before execution.

Run tests repeatedly:

```bash
pytest . --count <VALUE> --repeat-scope session -x
```

- `--count <VALUE>` - number of repeats.
- `--repeat-scope session` - scope of repeat.
- `-x` - exit on first error.

Refer to `pytest` manual for `pytest` specific options.
Refer to `conftest.py` for test suite specific options.

### Create HTML report

To generate HTML report use:

```bash
pytest -v . --build-scenarios --self-contained-html --html report.html --traces <VALUE>
```

- `--self-contained-html` - generate self contained HTML file.
- `--html report.html` - HTML report output path.
- `--traces <VALUE>` - verbosity of traces in output and HTML report - "none", "target" or "all".

> Traces are collected using `stdout`.
> Setting `--capture` flag (including `-s`) might cause traces to be missing from HTML report.

### Bazel execution

Run all Component Integration Tests:

```bash
bazel test //:cit_tests --config=cit
```

When the dependencies in [requirements.txt](python_test_cases/requirements.txt) file are manually modified, the user should invoke command and commit changes:

```bash
bazel run //tests/python_test_cases:requirements.update
```

In order to update all dependencies use:

```bash
bazel run //tests/python_test_cases:requirements.update -- --upgrade
```

## Standalone execution of test scenarios

Test scenarios can be run independently from `pytest`.

### C++ Scenarios

Set current working directory to the following:

```bash
cd <REPO_ROOT>/tests/cpp_test_scenarios
```

List all available scenarios:

```bash
bazel run //tests/cpp_test_scenarios:cpp_test_scenarios --config=cit -- --list-scenarios
```

Run specific test scenario:

```bash
bazel run //tests/cpp_test_scenarios:cpp_test_scenarios --config=cit -- --name <TEST_GROUP>.<TEST_SCENARIO> --input <TEST_INPUT>
```

Example:

```bash
bazel run //tests/cpp_test_scenarios:cpp_test_scenarios --config=cit -- --name basic.basic --input '{"test_logic":{"json_path":"data.json"}}'
```

Run test scenario executable directly:

```bash
<REPO_ROOT>/bazel-bin/tests/cpp_test_scenarios/cpp_test_scenarios --name basic.basic --input '{"kvs_parameters":{"instance_id":0}}'
```

Run with GDB:

```bash
bazel build //tests/cpp_test_scenarios:cpp_test_scenarios --config=cit -c dbg --strip never
gdb --args <REPO_ROOT>/bazel-bin/tests/cpp_test_scenarios/cpp_test_scenarios --name <TEST_GROUP>.<TEST_SCENARIO> --input '{"test_logic":{"json_path":"data.json"}}'
```
