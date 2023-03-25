# mini-yaml

![version](https://img.shields.io/badge/version-v0.1.0-blue) [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT) [![Github Linux Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) [![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) [![codecov](https://codecov.io/gh/jimmiebergmann/mini-yaml/branch/master/graph/badge.svg)](https://codecov.io/gh/jimmiebergmann/mini-yaml)  
Single header YAML C++ >= 11 sax/dom reader.

## Quickstart
### file.yml
```yaml
scalar: foo bar
list:
  - hello world
  - boolean: true
    integer: 123
    floating point: 2.75
```
### .cpp
Error checking is omitted, see quickstart_full.yml for a real use case example.
```cpp
#include "yaml/yaml2.hpp"

auto result = yaml::dom::read_document_from_file("file.yml");
if(!result) {
    return print_result(result);
}

auto& root = result.root_node;
std::cout << root["scalar"].as_string() << "\n";
std::cout << root["list"][0].as_string() << "\n";
std::cout << root["list"][1]["boolean"].as<bool>() << "\n";
std::cout << root["list"][1]["integer"].as<int>() << "\n";
std::cout << root["list"][1]["floating point"].as<float>() << "\n";

```
### Output
```
foo bar
hello world
true
123
2.75
```
## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/yaml2.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/yaml2.hpp)".

## Build status
Builds pass if all tests are successful and no memory leaks are found.

| Branch | Linux  | Windows | Fuzzing |
| ------ | ------ | ------- |----- |
| master | [![Github Linux Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Windows Master](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) | [![Github Fuzzing Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |
| dev    | [![Github Linux Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Dev](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/dev?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/dev) | [![Github Fuzzing Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |

## Missing features (TODO)
- quoted strings
- yaml::dom_view
- indentation indicator
- tags(!!type)
- anchors
- flow sequences/maps
- complex keys
- sets

### Note
One does not simply write a yaml parser from scratch.