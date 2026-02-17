# JSON

- [JSON](#json)
  - [Usage](#usage)
    - [Bazel target](#bazel-target)
    - [Loading a JSON document from a file](#loading-a-json-document-from-a-file)
    - [Accessing elements from a JSON object](#accessing-elements-from-a-json-object)
    - [Accessing elements from a JSON list](#accessing-elements-from-a-json-list)
    - [Accessing numbers from a JSON document](#accessing-numbers-from-a-json-document)
    - [Accessing strings from a JSON document](#accessing-strings-from-a-json-document)
    - [Simplified usage](#simplified-usage)
    - [Declarative parsing of JSON data](#declarative-parsing-of-json-data)
  - [Design](#design)
  - [Constraints](#constraints)
  - [Requirements](#requirements)
  - [Assumptions of Use](#assumptions-of-use)
  - [Selecting base library](#selecting-base-library)

This JSON library is designed as an abstraction layer which can switch to using
other parsers/serializers under the hood. At the moment it uses vaJson from Vector for parsing,
which is ASIL D certified. For serialization this library uses a custom implementation.

This library requires to be ASIL B certified, so it can be used in other ASIL B
certified components.

Mid-Term goal is that all JSON usages within the platform are based on this
abstraction and use-cases that are not yet covered are extended.

## Usage

### Bazel target

This library offers the Bazel target `//platform/aas/lib/json`, which is uniquely ASIL B certified. However, this certification is valid only when the target utilizes the vaJson parser from Vector, [selectable via a feature flag](#selecting-base-library).

Additionally, there is [another](#declarative-parsing-of-json-data) Bazel target, `//platform/aas/lib/json:json_serializer`, which is **not** ASIL B certified.

### Loading a JSON document from a file

```c++
#include "score/json/json_parser.h"

score::json::JsonParser json_parser_obj;
auto const root = json_parser_obj.FromFile("score/json/examples/example.json");
if (!root.has_value())
{
    score::mw::log::LogError() << "Failed to load json: " << root.error();
    return root.error();
}
```

### Accessing elements from a JSON object

In `score::json`, JSON objects are represented by `std::unordered_map`.
Thus you can use a JSON object like you would use `std::unordered_map`.

Here is an excerpt from the [example code](./examples/json_object.cpp) for JSON objects:

```c++
// Access the root element of the document as an object/dictionary type.
auto const& obj_result = root.value().As<score::json::Object>();
if (!obj_result.has_value())
{
    score::mw::log::LogError() << "Error: root element is not an object.";
    return root.error();
}
auto const& obj = obj_result.value().get();

// Iterating over a dictionary elements with key and value
for (auto const& element : obj)
{
    if (element.second.As<std::string>().has_value())
    {
        score::mw::log::LogError() << element.first.GetAsStringView()                          // key
                                 << ": " << element.second.As<std::string>().value().get();  // value
    }
}

// JSON objects be used just like an std::unordered_map.
auto const foo = obj.find("key");
if (foo == obj.end())
{
    score::mw::log::LogError() << "Failed to find 'key' in object.";
    return root.error();
}
auto const foo_value = foo->second.As<bool>();
if (foo_value.has_value())
{
    score::mw::log::LogError() << "key: " << foo_value.value();
}
```

### Accessing elements from a JSON list

In `score::json`, a JSON list is represented by a `std::vector`. However, note
that a JSON list may contain elements with heterogeneous types. Here is an
excerpt from the [example code](./examples/json_list.cpp) for JSON lists:

```c++
// Check if the root object contains a List under key "my_array".
const auto my_array_iter = obj.find("my_array");
if (my_array_iter == obj.end())
{
    score::mw::log::LogError() << "Error: could not find my_array.";
    return root.error();
}

// Check that my_array is in fact a JSON list/array.
const auto my_array_result = my_array_iter->second.As<score::json::List>();
if (!my_array_result.has_value())
{
    score::mw::log::LogError() << "Error: my_array is not a list.";
    return root.error();
}

// Iterate over my_array
for (const auto& element : my_array_result.value().get())
{
    const auto integer = element.As<std::int64_t>();
    if (integer.has_value())
    {
        score::mw::log::LogError() << integer.value();
    }

    const auto& message = element.As<std::string>();
    if (message.has_value())
    {
        score::mw::log::LogError() << message.value().get();
    }

    const auto null = element.As<score::json::Null>();
    if (null.has_value())
    {
        score::mw::log::LogError() << "Null";
    }

    const auto pi = element.As<float>();
    if (null.has_value())
    {
        score::mw::log::LogError() << pi.value();
    }
}
```

### Accessing numbers from a JSON document

`score::json` supports the following arithmetic types:

- Unsigned: `bool`, `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`
- Signed: `int8_t`, `int16_t`, `int32_t`, `int64_t`
- Floating point: `float`, `double`

An element can be converted into the requested type if it can be represented
identically in that type without loss of information. Otherwise an `kWrongType`
error will be returned, e.g. in case of a precision loss. To access the value
you can use the `As<ArithmeticType>()` method on instances of `score::json::Any`
and `score::json::Number`:

```c++
const auto integer = element.As<std::int64_t>();
if (integer.has_value())
{
    score::mw::log::LogError() << integer.value();
}
```

Please see tests and design for more detailed usage.

### Accessing strings from a JSON document

There are two ways to get a string from an instance of `Any`. The first is to
get it as `std::reference_wrapper<const std::string>` and then use `get()` is to
access the underlying string:

```c++
const auto str = element.As<std::string>();
if (str.has_value())
{
    score::mw::log::LogError() << integer.value().get();
}
```

The alternative is getting it directly as `std::string_view`:

```C++
std::string_view str = element.As<std::string_view>();
```

**Note** that the lifetime of any returned `reference_wrapper` and `string_view`
is bound by the lifetime of the JSON `root` object, that is returned e.g. by
`json::FromFile()`. We have to put the content to another owner if we want to
access the value after the destruction of the `root` instance.

### Simplified usage

Finding attributes of an object requires a sequence of repetitive steps.
Specially when dealing with nested objects, the boilerplate code resulting from
the successive searches and conversions can get in the way of readability.

Thus an optional layer of syntax-sugar is provided by sub-package "getters",
that aims to avoid the repetitive code. For example, suppose one wants to
retrieve the property `widget.geometry.size.width` from a JSON with a structure
similar to:

```json
{
  ...
  "widget": {
    "color": "blue",
    "geometry": {
      "shape": "rectangle",
      "size": {
        "heigth": 10,
        "width": 20,
        "unit": "px"
      },
      ...
    },
    ...
  }
  ...
}
```

Using the syntax-sugar layer, one could accomplish it as:

```c++
Result<std::uint64_t> GetWidgetWidth(json::Object& main_object)
{
    const auto& widget = GetAttribute<json::Object>(main_object, "widget");
    const auto& geometry = GetAttribute<json::Object>(widget.value(), "geometry");
    const auto& size = GetAttribute<json::Object>(geometry.value(), "size");
    return GetAttribute<std::uint64_t>(size.value(), "width");
}
```

See the [unit tests](./getters_test.cpp) for details.

### Declarative parsing of JSON data

An alternative to navigating the tree of json objects is to declare C++ structs
that resemble the scheme of the JSON data and converting the top-level `json::Any`
into the declared type hierarchy and vice versa. This can be done using the
functions `FromJsonAny` and `ToJsonAny` from the `json` namespace.

The serialization and deserialization of the data is achieved through static
reflection. Therefore, serializing data will lead to a compilation error in
case the data structure is not serializable to JSON. Converting from JSON,
however, might lead to runtime errors because the JSON data might not match the
declared data structure. Therefore, `FromJsonAny` returns a `Result<T>`,
indicating whether deserialization was successful. Please also note that, for
deserialization, `json::Any` is handed over by-value, so that its data can
be moved-from where possible.

The following example demonstrates the usage of the declarative JSON parser:

```c++
#include <static_reflection_with_serialization/visitor/visit_as_struct.h>

#include "score/json/json_serializer.h"

using score::json::_json;

struct Size
{
    std::uint64_t width{};
    std::uint64_t height{};
    std::string unit{};
};

STRUCT_VISITABLE(Size, width, height, unit)

void SerializeToJsonAny()
{
    Size size{10, 20, "px"};
    auto json_any = score::json::ToJsonAny(size);
    // json_any is now a json::Any object with the following structure:
    // {
    //     "width": 10,
    //     "height": 20,
    //     "unit": "px"
    // }
}

void DeserializeFromJson()
{
    auto json = R"({
        "width": 10,
        "height": 20,
        "unit": "px"
    })"_json;

    auto size = score::json::FromJsonAny<Size>(json);
    if (size.has_value())
    {
        // size.value() is now a Size object with the following values:
        // width = 10
        // height = 20
        // unit = "px"
    }
    else
    {
        // size.error() contains the reason why deserilization failed
    }
}
```

One reason why deserialization fails is because of missing fields in the JSON.
If a field is optional, and you want to tolerate missing fields, you may wrap
the field type into an `std::optional` to indicate that deserialization should
not fail if the field is missing. If there is no data for that field, its value
will be `std::nullopt`.

Currently, the following data types can be serialized to and from JSON:

* std::string
* bool
* Any arithmetic type (int, float, double, etc.)
* std::vector of any JSON-serializable type
* Any visitable struct that consists of JSON-serializable types
* std::optional of any JSON-serializable type

## Design

[Detailed design](detailed_design/README.md)

## Constraints

We have a couple of constraints with this library. We avoid implementing these
since they would require high amount of work. Please be aware and note:

- No support for key duplicates (we will always take the first)
- No support for empty keys
- No support for online schema validation (we expect that this is done offline)
- Only support for JSON Standard [RFC-8259](https://datatracker.ietf.org/doc/html/rfc8259)

## Requirements

For the most up to date information on Requirements please follow the provided links to CodeBeamer.

Adaptive Platform SW Safety Requirements
- [The users of the vaJson ibrary SHALL guarantee the integrity of the data used to initialize the `amsr::json::JsonData` buffer.](broken_link_c/issue/6576406)

[SW Component Requirements](broken_link_c/tracker/566325?workingSetId=-1&layout_name=document&subtreeRoot=5310849)
- The JSON-Library shall support at least the functional set of RFC-8259.
- The JSON-Library shall not change or manipulate the data that is read from a filesystem.

vaJson Requirements (taken from the safety manual)
- vaJson shall provide a service to check the well-formedness of JSON data.
- vaJson shall provide a service to parse JSON data according to RFC8259.
- vaJson shall provide a service to listen to events for every parsed JSON item.

## Assumptions of Use

For the most up to date information on AoUs please follow the provided links to CodeBeamer or have a look into the respective safety manual.

[AoUs for this library](broken_link_c/tracker/938306?view_id=-2&layout_name=document&subtreeRoot=16556079)
- Avoid using real number values if rounding errors have safety impact
- Filesystem Integrity Responsibility Lies with the User

This library provides the functionallity to read and write JSON files/data from and to the filesystem however this functionallity is considerd ASIL QM as this library itself does not do any filesystem integrity checks. The responsibility of filesystem integrity lies within the user (see AoU).

[AoUs for vaJson (taken from the safety manual)](broken_link_c/tracker/940190?view_id=-11&layout_name=document&subtreeRoot=6204897)
- The user of MICROSAR Adaptive Safe shall ensure that IoIntegrityStream is used
when initializing vaJson from an input stream where safety requirements are assumed.
- The user of MICROSAR Adaptive Safe shall ensure the integrity of the character
buffer used to initialize vaJson where safety requirements are assumed.
- The user of MICROSAR Adaptive Safe shall ensure the integrity of the file when
initializing vaJson from a path to the file where safety requirements are assumed.

This library does not provide a way for the user to to initialize vaJson from an input stream.

## Selecting base library

This library supports usage of vector json library and nhlomann json library. By default vector json library is used.

In order to make use of nlohmann json library, feature flag needs to be set. Please see an example below.

bazel test --config=spp_host_clang //score/json/... --//platform/aas/lib/json:base_library="nlohmann"

nlohmann json library do not supports hexadecimal. As it is not part of json standard.
