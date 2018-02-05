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

// Print all scalars.
cout << root["key"].As<string>() << endl;
cout << root["list"][0].As<string>() << endl;
cout << root["list"][1]["integer"].As<int>() << endl;
cout << root["list"][1]["boolean"].As<bool>() << endl;

// Iterate second sequence node.
for(auto it = root[1].Begin(); it != root[1].End(); it++)
{
    cout << (*it).first << ": " << (*it).second.As<string>() << endl;
}
```
#### Output
```
foo bar
hello world
123
1
integer: 123
boolean: true
```

## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/Yaml.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/Yaml.hpp)".
See [examples/FirstExample.cpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/examples/FirstExample.cpp) for additional examples.

## Build status
Builds are passed if all tests are good and no memory leaks were found.

| Branch | Status |
| ------ | ------ |
| master | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=master)](https://travis-ci.org/jimmiebergmann/mini-yaml) |
| dev | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=dev)](https://travis-ci.org/jimmiebergmann/mini-yaml)|

## Todo
- Serialize newlines in multi-line scalars.
- Properly serialize quotation marks.
- Ignore explicit typing(!!type) in parsing.
- Allow customization of serialization. Tab size, sequence item newline, etc..
- Parse tags.
- Parse flow sequences/maps.
- Parse complex keys.
- Parse sets.

