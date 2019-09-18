# mini-yaml

![version](https://img.shields.io/badge/version-v0.1.0-blue) [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=master)](https://github.com/jimmiebergmann/mini-yaml#build-status) [![Build status](https://ci.appveyor.com/api/projects/status/xi37d7i9t7m0xsj1/branch/master?svg=true)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) [![codecov](https://codecov.io/gh/jimmiebergmann/mini-yaml/branch/master/graph/badge.svg)](https://codecov.io/gh/jimmiebergmann/mini-yaml)  [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
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
auto root = yaml::parse_file("file.txt");

// Get scalar values.
root["key"].as<std::string>(); // "foo bar"
root["list"][0].as<std::string>(); // "hello world"
root["list"][1]["integer"].as<int>(); // 123
root["list"][1]["boolean"].as<bool>(); // true
root["list"][1]["boolean"].as<std::string>(); // "true"

// Iterate second sequence item.
auto & item = root[1];
for(auto it = item.begin(); it != item.end(); it++)
{
    (*it).first; // "integer" / "boolean"
    (*it).second.as<string>(); // "123" / "true"
}
```

See  [Best practice](https://github.com/jimmiebergmann/mini-yaml#best-practice).

## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/yaml.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/Yaml.hpp)".
See [examples/first_example.cpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/examples/first_example.cpp) for additional examples.

## Best practice
Always use references when accessing node content, if not intended to make a copy. Modifying copied node wont affect the original node content.  
See example:
```cpp
yaml::node root;

yaml::node & ref = root;  // The content of "root" is not being copied.
ref["key"] = "value";     // Modifying "root" node content.

yaml::node copy = root;   // The content of "root" is copied to "copy".
                          // Slow operation if "root" contains a lot of content.
copy["key"] = "value";    // Modifying "copy"'s content. "root" is left untouched.
```

## Build status
Builds are passed if all tests are good and no memory leaks were found.

| Branch | Linux | Windows |
| ------ | ------ | ---- |
| master | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=master)](https://travis-ci.org/jimmiebergmann/mini-yaml) |  [![Build status](https://ci.appveyor.com/api/projects/status/xi37d7i9t7m0xsj1/branch/master?svg=true)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) |
| dev | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=dev)](https://travis-ci.org/jimmiebergmann/mini-yaml)|  [![Build status](https://ci.appveyor.com/api/projects/status/xi37d7i9t7m0xsj1/branch/develop?svg=true)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/develop) |

## ~~Todo~~
- ~~Parse/serialize tags(!!type).~~
- ~~Parse anchors.~~
- ~~Parse flow sequences/maps.~~
- ~~Parse complex keys.~~
- ~~Parse sets.~~

