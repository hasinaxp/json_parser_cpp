# JSON Parser and Serializer Documentation
`json_parser.hpp` is a simple header-only library to work with jsons in c++.

## Overview

The JSON parser supports the following JSON types:
- **Null**
- **Boolean** (true/false)
- **Number** (integer and floating-point)
- **String**
- **Array**
- **Object**

It provides functionality to:
- Parse JSON strings into a `json_t` object.
- Serialize a `json_t` object back into a JSON string.

---

## Features

### Supported JSON Types
- **Enumeration**: `JsonType` for JSON data types.
  ```cpp
  enum JsonType {
      JSON_ERROR = -1,
      JSON_NULL,
      JSON_BOOL,
      JSON_NUMBER,
      JSON_STRING,
      JSON_ARRAY,
      JSON_OBJECT
  };
  ```

### `json_t` Structure
The `json_t` structure represents JSON data and includes:
- `type`: JSON data type.
- `keys`: For objects, holds keys; for primitives, holds the value as a string.
- `values`: For objects/arrays, holds child `json_t` values.

Methods:
- `to_bool`, `to_int`, `to_double`, `to_string`: Convert JSON data to native types.
- Overloaded operators (`[]`, `=`) for accessing and modifying JSON objects/arrays.

### Parsing Functions
- `parse_json(const std::string&)`: Parses a JSON string into a `json_t` object.

### Serialization
- `to_string(const json_t&, bool compact, int indent, int current_indent)`: Converts a `json_t` object to a JSON string.

---

## Example Usage

### Parsing JSON Strings
```cpp
#include "json_parser.h"

int main() {
    sp::json_t json = sp::parse_json("{\"key\": \"value\", \"number\": 42}");
    std::cout << json["key"].to_string() << std::endl; // Outputs: value
    std::cout << json["number"].to_int() << std::endl;  // Outputs: 42
    return 0;
}
```

### Creating JSON Objects
```cpp
sp::json_t json = sp::create_json_object();
json["name"] = "John Doe";
json["age"] = 30;

sp::json_t hobbies = sp::create_json_array();
hobbies.push_back("Reading");
hobbies.push_back("Cycling");

json["hobbies"] = hobbies;
std::cout << sp::to_string(json, false, 2) << std::endl;
```

### Serializing JSON
```cpp
sp::json_t json = sp::parse_json("[1, true, \"test\"]");
std::cout << sp::to_string(json, true) << std::endl; // Outputs: [1,true,"test"]
```

---

## License
This implementation is open-source and can be used or modified freely.


