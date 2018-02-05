# mini-yaml
[![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=master)](https://travis-ci.org/jimmiebergmann/mini-yaml)  
Single header YAML 1.0 C++11 serializer/deserializer.

## Quickstart
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
Yaml::Node root;
Yaml::Parse(root, "file.txt");

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
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/Yaml.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/Yaml.hpp)".
See [examples/FirstExample.cpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/examples/FirstExample.cpp) for additional examples.

## Build status
| Branch | Status |
| ------ | ------ |
| master | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=master)](https://travis-ci.org/jimmiebergmann/mini-yaml) |
| dev | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=dev)](https://travis-ci.org/jimmiebergmann/mini-yaml)|

## Todo
- Do more tests.
- Allow copy constructor of nodes.
- Serialize newlines in multi-line scalars.
- Properly serialize quotation marks.
- Ignore explicit typing(!!type) in parsing.
- Allow customization of serialization. Tab size, sequence item newline, etc..
- Parse tags.
- Parse flow sequences/maps.
- Parse complex keys.
- Parse sets.

