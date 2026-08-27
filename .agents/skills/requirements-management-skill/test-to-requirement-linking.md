# Test-to-Requirement Linking

Use this reference when linking, migrating, or reviewing C++ gtest and Rust unit tests against baselibs `comp_req__...` needs. Python tests are out of scope for this skill.

## Routes

### Define or update a test link

1. Identify the `comp_req__<component>__...` that the test verifies; author the requirement first if it does not exist.
2. Decide whether the test alone verifies the requirement (`FullyVerifies`) or contributes partial coverage (`PartiallyVerifies`).
3. Choose `TestType` and `DerivationTechnique` from [Metadata Reference](#metadata-reference).
4. Add a concise `Description` that states objective, input, and expected outcome.
5. Record the metadata using the language-specific mechanism below.
6. Run the narrow test target, usually `bazel test --config=bl-x86_64-linux //score/<component>/...`, to confirm the test still passes.
7. Run `bazel run //:docs` to confirm the linked requirement ID resolves. A passing test does not prove this: `RecordProperty`/`record_property` values are untyped strings with no compile-time or test-runtime check against the metamodel, so a typoed or removed ID only surfaces as a docs build warning or error. The route is complete when both the test passes and the docs build reports no unresolved-link or missing-property issue for it.

### Migrate legacy metadata

1. Determine what the test actually asserts and find the matching `comp_req__...`.
2. Replace legacy linkage with `FullyVerifies` or `PartiallyVerifies`.
3. Add missing `TestType`, `DerivationTechnique`, and `Description`.
4. Remove non-mandated metadata such as `ASIL` and `Priority`.
5. Run the affected tests; the route is complete when no legacy metadata remains in the touched tests.

| Legacy pattern | Compliant replacement |
|----------------|-----------------------|
| `RecordProperty("Verifies", "...")` | `FullyVerifies`, or `PartiallyVerifies` if several tests together cover the need |
| `SCR-*` ID | The corresponding docs-as-code `comp_req__...` ID |
| Raw C++ symbol, e.g. `::score::json::ToJsonAny` | The `comp_req__...` need implemented by that symbol |
| `ASIL`, `Priority` | Remove; safety belongs on the requirement, not the test |

### Review test linkage

Check every touched test case:

- It records `TestType`, `DerivationTechnique`, `Description`, and exactly one linkage style: `FullyVerifies` or `PartiallyVerifies`.
- Linked unit tests point at existing `comp_req__...` needs only, never at `aou_req__...`, `feat_req__...`/`stkh_req__...`.
- `FullyVerifies` is used only when this test alone covers the requirement; otherwise use `PartiallyVerifies`.
- The assertions exercise the linked requirement's normative behavior.
- No legacy `Verifies`, `ASIL`, `Priority`, `SCR-*`, or raw-symbol requirement links remain.
- The PR approver is not an author of the change.

## Metadata Reference

Every test case that links to requirements shall carry these metadata properties. Multiple IDs are written as one comma-separated string, e.g. `"comp_req__json__number_parsing, comp_req__json__whitespace"`.

| Metadata key | Required | Meaning |
|--------------|----------|---------|
| `FullyVerifies` | One of `FullyVerifies` / `PartiallyVerifies` is mandatory | Requirement/design/interface ID(s) fully covered by this single test |
| `PartiallyVerifies` | See above | Requirement/design/interface ID(s) partially covered by this test |
| `Description` | Yes, non-empty | Objective, inputs, expected outcome; add environment or event sequence only when relevant |
| `TestType` | Yes | One of the values below |
| `DerivationTechnique` | Yes | One of the values below |

`TestType` is chosen from what aspect of the test subject is being exercised:

| Use | When the test is exercising... |
|-----|--------------------------------|
| `requirements-based` | Whether the unit fulfils a specific requirement's normative statement, and does not exhibit unintended additional functionality |
| `interface-test` | Correctness of data and control passed across an interface: parameter passing between units, data format/encoding, protocol/sequencing, or how a communication error is handled; for unit tests this covers internal interfaces, external interfaces belong at integration/feature level |
| `fault-injection` | The unit's detection and handling of a deliberately induced fault: a corrupted value, an error forced from a mocked or wrapped dependency, or injected data corruption, verifying a safety mechanism reacts correctly, not a plain out-of-range argument |
| `resource-usage` | Whether resource consumption, e.g. execution time, memory or stack usage, or communication bandwidth, stays within its specified budget, including under worst-case or exhausted conditions |

`DerivationTechnique` is chosen from how the test case was derived: 

| Use | When the test is derived from... |
|-----|--------------------------------|
| `requirements-analysis` | The requirement's normative statement, both its nominal behavior (input x yields output y) and, where the requirement implies it, the negative/off-spec behavior it does not spell out explicitly |
| `design-analysis` | Design or architecture rather than requirement text |
| `equivalence-classes` | A representative value from a partition of valid/invalid inputs or outputs, grouped either because the spec treats them alike or because they yield the same functional result; pick this only once boundary values are ruled out |
| `boundary-values` | A value at, or one step inside/outside, a partition's edge: minimum, maximum, zero, an off-by-one position, a sequence's first/last element, a collection holding zero/one/two items, or a numeric limit stated in the requirement text (e.g. "up to 64 bits"); also covers forcing an *output* to its own limit, not just an input; wins over `equivalence-classes` whenever a value could be read either way |
| `fuzz-testing` | Randomized or generated inputs |
| `error-guessing` | An atypical value, combination, or timing (e.g. rapid or concurrent calls) added from tester experience, lessons-learned, or FMEA findings on top of the systematically derived set, not from partitioning the spec itself |
| `explorative-testing` | Ad-hoc manual exploration |

## Description

Keep `Description` to one or two sentences. It must be self-contained enough to state objective, input, and expected outcome, but it should not restate every visible value from the test body.

Good:

```text
"Check that tuple_to_array converts a tuple into a std::array of matching size and element order."
"Check that strip_trailing_spaces clamps end=16 on an 11-character string and returns 16."
```

Avoid descriptions that only repeat the test name, or mechanical `Objective` / `Input` / `Expected` prose that mirrors the code without adding information.

## C++ gtest

Use `RecordProperty`. Put common fixture-wide metadata in `SetUp()` only when the whole fixture shares the same values; keep per-test linkage and `Description` inside each test case.

```cpp
class MyComponentTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        RecordProperty("TestType", "requirements-based");
        RecordProperty("DerivationTechnique", "requirements-analysis");
    }
};

TEST_F(MyComponentTest, GivenValidInput_WhenParsed_ThenReturnsExpectedValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__number_parsing");
    RecordProperty("Description", "Check that parsing a valid integer literal yields its numeric value.");

    EXPECT_EQ(/* ... */);
}
```

For a free `TEST(...)`, record every property inside the test body:

```cpp
TEST(MyComponentTest, GivenFullQueue_WhenPush_ThenReturnsFalse)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__bounded_queue");
    RecordProperty("Description", "Check that pushing into a full queue returns false instead of blocking.");
    EXPECT_FALSE(/* ... */);
}
```

Do not create a fixture just to deduplicate mixed metadata. Repeating `TestType` and `DerivationTechnique` across free tests is acceptable.

## Rust

Use `#[record_property("Key", "Value")]` from the `test_properties` crate on each `#[test]` function.

```rust
use test_properties::record_property;

#[record_property("PartiallyVerifies", "comp_req__containers__dynamic_array")]
#[record_property("Description", "Check that pushing beyond capacity returns an error instead of growing.")]
#[record_property("TestType", "requirements-based")]
#[record_property("DerivationTechnique", "boundary-values")]
#[test]
fn given_full_vector_when_push_then_returns_error() {
    assert!(/* ... */);
}
```

The `test_properties` crate must be an approved dependency. If it is not available, ask before adding or inventing a substitute. In-crate `#[cfg(test)]` unit tests map to unit testing; tests under a crate's `tests/` directory map to component/feature integration testing.
