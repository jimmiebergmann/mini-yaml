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
Error checking is omitted, ~~see examples/quickstart_full.cpp for a real use case example~~.
```cpp
#include "yaml/yaml2.hpp"

auto result = yaml::dom::read_document_from_file<char>("file.yml");
if(!result) {
    return print_result(result);
}

auto& root = result.root_node;
std::cout << root["scalar"].as<std::string>() << "\n";
std::cout << root["list"][0].as<std::string>() << "\n";
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
#### Download repo 
Place [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project's include directory and simply #include "[yaml/yaml2.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/yaml2.hpp)".

#### Clone repo
```
git clone https://github.com/jimmiebergmann/mini-yaml.git

```

## Build status
Builds pass if all tests are successful and no memory leaks are found.

| Branch | Linux  | Windows | Fuzzing |
| ------ | ------ | ------- |----- |
| master | [![Github Linux Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Windows Master](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) | [![Github Fuzzing Master](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=master&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |
| dev    | [![Github Linux Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-linux.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![Github Windows Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-build-windows.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Dev](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/dev?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/dev) | [![Github Fuzzing Dev](https://img.shields.io/github/actions/workflow/status/jimmiebergmann/mini-yaml/github-fuzz.yml?branch=dev&label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-fuzz.yml) |

## Features
:heavy_check_mark: Supported :warning: Partial :x: Unsupported

|Feature | Support |
| ------ | ------ |
| sax reader | :heavy_check_mark: |
| dom reader | :heavy_check_mark: |
| dom_view reader | :x: |
| dom writer | :x: |
| unicode | :heavy_check_mark: |
| scalars | :heavy_check_mark: |
| maps | :heavy_check_mark: |
| sequences | :heavy_check_mark: |
| unicode | :heavy_check_mark: |
| scalar type conversions | :heavy_check_mark: |
| tags (!!type) | :x: |
| indentation indicator | :x: |
| flow scalars | :x: |
| flow maps | :x: |
| flow sequences | :x: |
| sets | :x: |
| anchors | :x: |
| complex keys | :x: |
| reader result printing | :x: |

### Note
One does not simply write a yaml parser from scratch.