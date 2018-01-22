# mini-yaml
Single header YAML 1.0 C++11 serializer/deserializer.
Under development and currently not working.
## Features

##### Reader/deserializer
- Tabs are not allowed in line offsets, an parsing exception will be thrown.
- Parse multiple documents by calling Parse function once more. Document starts/ends are defined as("---" / "...").


## Known limitations
##### Reader/deserializer
- Quotation marks are not properly working in keys or scalar values.
- Flow sequences("[]")/maps("{}") are not available.


## Documentation
No documentation yet.

#### Deserialization
...

#### Serialization
...