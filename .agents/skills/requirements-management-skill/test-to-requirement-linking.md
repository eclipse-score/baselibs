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
8. Before moving to the next test, re-read this test's own assertions against the `Description`, `TestType`, and links you just wrote (see [Common Pitfalls](#common-pitfalls)). Nothing in the toolchain checks these values, so this self-check is the only gate until a human or a second review pass looks at it.

### Migrate legacy metadata

1. Determine what the test actually asserts and find the matching `comp_req__...`.
2. Replace legacy linkage with `FullyVerifies` or `PartiallyVerifies`.
3. Add missing `TestType`, `DerivationTechnique`, and `Description`.
4. Remove non-mandated metadata such as `ASIL` and `Priority`, including legacy `@req{...}` comment tags left over from Doxygen-based tracing.
5. Run the affected tests; the route is complete when no legacy metadata remains in the touched tests.

| Legacy pattern | Compliant replacement |
|----------------|-----------------------|
| `RecordProperty("Verifies", "...")` | `FullyVerifies`, or `PartiallyVerifies` if several tests together cover the need |
| `SCR-*` ID | The corresponding docs-as-code `comp_req__...` ID |
| Raw C++ symbol, e.g. `::score::json::ToJsonAny` | The `comp_req__...` need implemented by that symbol |
| `ASIL`, `Priority` | Remove; safety belongs on the requirement, not the test |
| `/// @req{ID}` comment above the test | Remove; replace with `FullyVerifies`/`PartiallyVerifies` metadata inside the test body |

## Common Pitfalls

These recur when migrating many similar tests in one pass, especially by reusing wording across tests instead of re-deriving each `Description` from that test's own assertions:

- **No real assertion.** A test that calls the code under test but has no `EXPECT_*`/`assert!` tied to the claimed behavior is not verification evidence, even if it is linked. Add the missing assertion or drop the link.
- **Reused/templated `Description` text.** Near-identical fixtures (e.g. a family of overflow/round-trip tests) tempt you to copy one test's description to the next. Re-check each one against its own inputs: which side (serialize vs. deserialize) receives the undersized/oversized buffer, whether fields are reordered vs. merely dropped/retained, whether a callback fires once per element or once with all elements.
- **Representative constants misclassified as `boundary-values`.** A fixture using round sizes like 100, 2048, 4096 bytes is usually exercising `equivalence-classes` (valid vs. invalid capacity), not a spec-stated boundary. Reserve `boundary-values` for a value tied to an actual documented limit, an off-by-one, or a 0/1/2-element collection.
- **`interface-test` under-used.** Parameter/error-handling checks across an API (rejecting an oversized length, a malformed buffer, an invalid handle) are `interface-test`, not `requirements-based`, even when they also happen to satisfy a `comp_req`.
- **Missing container linkage.** If a component treats `std::string` or another type as an automatically-iterable container, link the container requirement whenever a test exercises that type through the generic path, not just when it uses `std::vector` directly.
- **Ref-qualifier/move mismatch.** In a family of tests parameterized over ref-qualified overloads (lvalue, const-lvalue, rvalue, const-rvalue), a test named for the lvalue/const-lvalue overload but that calls `std::move(unit)` on its subject silently exercises the rvalue/const-rvalue overload instead. The test can still pass (e.g. the throw/abort behavior may be keyed on shared internal state rather than the overload called), which hides that the claimed overload is never actually verified. Check that each ref-qualifier variant's body matches its name: no `std::move()` for `&`/`const&` tests, `std::move()` only for `&&`/`const&&` tests.
- **Incomplete `noexcept`/compile-time-condition `Description`.** When a test's `Description` paraphrases a conditional `noexcept` (or another compound `enable_if`/type-trait expression), re-derive it from every conjunct in the actual code, not a simplified version. E.g. a defaulted move-assignment operator over a variant-like type typically needs both nothrow move-assignability AND nothrow move-constructibility of each alternative; describing only "nothrow move-assignable" is incomplete even if the test's `static_assert`s are otherwise correct. Note that not every composite trait needs restating this way: `std::is_nothrow_swappable` already subsumes nothrow move-construction (via the fallback `std::swap`'s own noexcept-specifier), so a `Description` that says "nothrow swappable" is already complete on its own.

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
