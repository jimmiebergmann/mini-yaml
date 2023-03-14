# mini-yaml

![version](https://img.shields.io/badge/version-v0.1.0-blue) [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT) [![Github Linux Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) [![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) [![codecov](https://codecov.io/gh/jimmiebergmann/mini-yaml/branch/master/graph/badge.svg)](https://codecov.io/gh/jimmiebergmann/mini-yaml)  
Single header YAML 1.0 C++ >= 11 serializer/deserializer.

## Quickstart
#### file.yml
```
key: foo bar
list:
  - hello world
  - integer: 123
    boolean: true
```
#### .cpp
```cpp
#include "yaml/yaml2.hpp"

auto result = yaml::dom::read_document_from_file("file.yml");
if(result != yaml::read_result_code::success) {
    return yaml::print_result(result);
}

...
```

## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/yaml2.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/yaml2.hpp)".

## Build status
Builds pass if all tests are successful and no memory leaks are found.

| Branch | Linux  | Windows | Fuzz |
| ------ | ------ | ------- |----- |
| master | [![Github Linux Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Windows Master](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) | [![Github Fuzz Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |
| dev    | [![Github Linux Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Dev](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/dev?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/dev) | [![Github Fuzz Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |

## Missing features (TODO)
- quoted strings
- yaml::dom
- yaml::dom_view
- indentation indicator
- sequences
- tags(!!type)
- anchors
- flow sequences/maps
- complex keys
- sets
