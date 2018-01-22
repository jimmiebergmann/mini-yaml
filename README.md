# mini-yaml
Single header YAML 1.0 C++11 serializer/deserializer.
Under development and currently not working.
## Features

##### Reader/deserializer
- Tabs are not allowed in line offsets, an parsing exception will be thrown.
- Parse multiple documents by calling Parse function once more. Document starts/ends are defined as("---" / "...").


## Known limitations
##### Reader/deserializer
- Quotation marks are seen as any character. Keys like "foo:"key: will not work. Reader will deserialize it as "foo".


## Documentation
No documentation yet.

#### Deserialization
...

#### Serialization
...