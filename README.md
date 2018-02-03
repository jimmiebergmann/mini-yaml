# mini-yaml
Single header YAML 1.0 C++11 serializer/deserializer.
Under development and currently not working.
## Features

##### Reader/deserializer
- Tabs are not allowed in line offsets, parsing exception will be thrown if detected.
- Parse multiple documents by calling Parse function once more.
Document starts/ends are defined as("---" / "...").


## Known limitations
- Unicode is not supported.
##### Reader/deserializer
- Colons inside quotation marks of keys will not work properly.
Example: "key:": value
- Flow sequences("[]")/maps("{}") are not supported.
- Explicit typing(!!type) is not supported.
- Tags are not supported.
- Comments are not supported.


## Documentation
See examples/FirstExample.cpp

#### Deserialization
...

#### Serialization
...
