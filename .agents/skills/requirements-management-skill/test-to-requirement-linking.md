# Test-to-Requirement Linking

Use this reference when linking, migrating, or reviewing C++ gtest and Rust unit tests against baselibs `comp_req__...` needs. Python tests are out of scope for this skill.

## Routes

### Define or update a test link

1. Identify the `comp_req__<component>__...` that the test verifies; author the requirement first if it does not exist.
2. Decide whether the test alone verifies the requirement (`FullyVerifies`) or contributes partial coverage (`PartiallyVerifies`).
3. Choose `TestType` and `DerivationTechnique` from [Metadata Reference](#metadata-reference).
4. Add a concise `Description` that states objective, input, and expected outcome.
5. Record the metadata using the language-specific mechanism below.
6. Run the narrow test target, usually `bazel test --config=bl-x86_64-linux //score/<component>/...`; the route is complete when the test passes and the referenced requirement exists.

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
- Linked unit tests point at existing `comp_req__...` needs, not feature or stakeholder requirements.
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
| `TestType` | Yes | `requirements-based`, `interface-test`, `fault-injection`, or `resource-usage` |
| `DerivationTechnique` | Yes | One of the values below |

`DerivationTechnique` is chosen from how the test case was derived:

| Use | When the test is derived from... |
|-----|--------------------------------|
| `requirements-analysis` | The requirement's normative statement |
| `design-analysis` | Design or architecture rather than requirement text |
| `boundary-values` | Limits, overflow, out-of-bounds, zero, min/max, saturation, off-by-one |
| `equivalence-classes` | One representative of a class with equivalent behavior |
| `fuzz-testing` | Randomized or generated inputs |
| `error-guessing` | Reasoning about likely faults without a formal derivation |
| `explorative-testing` | Ad-hoc manual exploration |

When several techniques apply, choose the one that best explains why this specific case was written. A max-value or overflow case is `boundary-values` even if the feature came from requirements analysis.

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
