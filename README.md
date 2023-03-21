# mini-yaml

![version](https://img.shields.io/badge/version-v0.1.0-blue) [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT) [![Github Linux Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) [![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) [![codecov](https://codecov.io/gh/jimmiebergmann/mini-yaml/branch/master/graph/badge.svg)](https://codecov.io/gh/jimmiebergmann/mini-yaml)  
Single header YAML C++ >= 11 sax/dom reader.

## Quickstart
### file.yml
```
scalar: foo bar
list:
  - hello world
  - integer: 123
    boolean: true
```
### .cpp
Error checking is omitted, see quickstart_full.yml for a real example.
```cpp
#include "yaml/yaml2.hpp"

auto result = yaml::dom::read_document_from_file("file.yml");
if(!result) {
    return yaml::print_result(result);
}

auto& root = result.root_node;
std::cout << root.as_object()["scalar"].as_scalar().as_string() << "\n";
std::cout << root.as_object()["list"].as_array()[0].as_scalar().as_string() << "\n";
std::cout << root.as_object()["list"].as_array()[1].as_object()["integer"].as_scalar().as_string() << "\n";
std::cout << root.as_object()["list"].as_array()[1].as_object()["boolean"].as_scalar().as_string() << "\n";

```
### Output
```
foo bar
hello world
123
true
```
## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/yaml2.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/yaml2.hpp)".

## Build status
Builds pass if all tests are successful and no memory leaks are found.

| Branch | Linux  | Windows | Fuzzing |
| ------ | ------ | ------- |----- |
| master | [![Github Linux Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Windows Master](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) | [![Github Fuzz Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |
| dev    | [![Github Linux Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Dev](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/dev?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/dev) | [![Github Fuzz Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |

## Missing features (TODO)
- quoted strings
- yaml::dom_view
- indentation indicator
- tags(!!type)
- anchors
- flow sequences/maps
- complex keys
- sets


##### Note
One does not simply write a yaml parser from scratch.