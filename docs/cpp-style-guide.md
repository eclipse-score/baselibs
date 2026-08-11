# C++ Style Conventions

Apply these when writing or reviewing C++. This document covers **style**: naming, file and API structure, and comments, i.e. how the code reads.

Mechanical formatting (indentation, spacing, brace placement, line wrapping, pointer alignment, the 120-column limit, namespace-close comments, preprocessor-directive indentation) is applied automatically by the repository's `.clang-format`. These conventions deliberately omit those rules so they stay focused on what clang-format cannot do: naming, structure, and semantics. Do not spend effort hand-formatting; run clang-format.

When you touch existing code, fix style within the lines you add or change; leave untouched surrounding lines alone rather than reformatting the whole file.

## Worked examples
The highest-frequency style fixes; the same reasoning generalizes to the rules below.

**Member naming:**
Before: `int m_count;`
After: `int count_;`

**Global constant:**
Before: `const int DAYS_IN_WEEK = 7;`
After: `constexpr int kDaysInAWeek{7};`

**Uniform initialization:**
Before: `std::string name = "abc";`
After: `std::string name{"abc"};`

## Naming
- Files and variables are `snake_case`. Namespaces are `snake_case` and follow the `score::...` hierarchy.
- Types (classes, structs, enums, aliases) are `PascalCase` with no underscores; even well-known acronyms are not all-caps (`XmlStreamReader`, not `XMLStreamReader`).
- Functions and methods are `PascalCase` (`MaxConcurrencyLevel()`).
- Class data members carry a trailing underscore (`memory_resource_`); struct members do not. Exception: `static constexpr`/`static const` class constants use the `k`-prefix constant rule below (`kMask`, not `mask_`).
- `k`-prefix every `const`/`constexpr` with static storage duration and a fixed value: namespace-scope constants and `static constexpr`/`static const` class members (`kDaysInAWeek`, `static constexpr std::uint8_t kMask{...};`). Optional for local `constexpr` variables. Enumerators also take the `k` prefix; prefer scoped enums (`enum class`), and only an enumerator in the global namespace embeds the type name (`kUrlTableErrors_Ok`). Mutable global variables are not used (MISRA 6.7.2 forbids them).
- Macros are a last resort (MISRA 19.0.2 bans function-like macros); when unavoidable, name them `UPPER_CASE_WITH_UNDERSCORES`.
- Be descriptive. Avoid abbreviations except ones that are well known outside the project (`url`, `dns`), and never abbreviate by dropping letters (`error_count`, not `error_cnt`). Types and variables are nouns; functions read as imperative verbs.
- Function parameter order is inputs, then inputs/outputs, then outputs.
- Non-type template parameters follow the variable convention; type template parameters are `PascalCase`, commonly suffixed `...Type`.

## Files and headers
- One class per translation unit: `class_name.h` / `class_name.cpp`, tests in `class_name_test.cpp`, mocks in `class_name_mock.h` / `class_name_mock.cpp`. Library folder names are `snake_case`.
- A file opens with the copyright header, then the include guard. `#pragma once` is not allowed.
- Include-guard name: the file path uppercased, `/` and `.` replaced by `_`, so `score/result/error.h` becomes `SCORE_RESULT_ERROR_H`.
- Use `"..."` for project headers and `<...>` for system and external headers, and include the file's own header first. clang-format sorts includes within each block.
- Definitions of inline functions belong in a header.

## Classes
- Declaration order: `public`, then `protected`, then `private`. Within each section: aliases/enums, constructors, destructor, methods, data members. `friend` declarations go in the `private` section. The `.cpp` defines members in the same order.
- List constructor initializers in member-declaration order, so run order matches the source and there is no `-Wreorder` warning.
- Interfaces do not use an `I` prefix: the interface keeps the plain name (`Executor`), the implementation takes an `Impl` suffix (`ExecutorImpl`), and the mock a `Mock` suffix (`ExecutorMock`). A pure interface declares a non-pure `virtual` destructor so deletion through the interface pointer is well defined.

## Idioms
Semantic and declaration rules that `.clang-format` cannot apply, so apply them yourself:
- Brace-initialize uniformly (`int x{3};`), not `int x = 3;`; clang-format will not convert `=` to braces.
- Declare only one pointer variable per line (`char* a;` then `char* b;`, never `char* a, *b;`).
- Do not wrap a `return` value in unnecessary parentheses.
- Prefer named constants over bare literals, `nullptr`, or `true`/`false` passed as arguments, so the call site explains itself.

## Comments
- `//` for ordinary comments, `///` for Doxygen; prefix Doxygen tags with `@` (`@brief`, `@param`, `@return`).
- On a public declaration, `@brief` is mandatory; add `@param`/`@return` when non-trivial. Document ownership transfer, whether arguments may be `nullptr`, the lifetime of retained references, thread-safety assumptions, and performance implications. A comment at the definition explains *how* the code works rather than repeating the declaration.
- Class data members get a short Doxygen comment describing their purpose and any sentinel values.
- Use `@todo` with the associated GitHub issue for non-trivial TODOs. Mark deprecated APIs with the C++ `[[deprecated("use NewApi instead")]]` attribute.
