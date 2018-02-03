# mini-yaml
Single header YAML 1.0 C++11 serializer/deserializer.

## Quick example
#### file.txt
```
key: foo bar
list:
  - hello world
  - integer: 123
    boolean: true
```
#### .cpp
```cpp
Node root;
Reader reader(root, data);

cout << root["key"].As<string>() << endl;
cout << root["list"][0].As<string>() << endl;
cout << root["list"][1]["integer"].As<int>() << endl;
cout << root["list"][1]["boolean"].As<bool>() << endl;
```
#### Output
```
foo bar
hello world
123
1
```

## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include <yaml.hpp>.
See [examples/FirstExample.cpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/examples/FirstExample.cpp) for additional examples.

## Features
##### Reader/deserializer
- Tabs are not allowed in line offsets, parsing exception will be thrown if detected.
- Parse multiple documents("---" / "...") by calling Parse function multiple times.

## Known limitations
- Unicode is not supported.
##### Reader/deserializer
- Colons inside quotation marks of keys will not work properly.
Example: "key:": value
- Flow sequences("[]")/maps("{}") are not supported.
- Explicit typing(!!type) is not supported.
- Tags are not supported.
- Comments are not supported.
##### Writer/serializer
- Not yet available.
