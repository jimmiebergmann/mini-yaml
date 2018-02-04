# mini-yaml
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
Node root;
Reader reader(root, "file.txt");

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

## Todo
- Add build and test automation.
- Allow copy constructor of nodes.
- Serialize multi-line scalars.
- Properly serialize quotation marks.
- Parse/serialize comments.
- Ignore explicit typing(!!type) in parsing.
- Allow customization of serialization. Tab size, sequence item newline, etc..
- Parse tags.
- Parse flow sequences/maps.
