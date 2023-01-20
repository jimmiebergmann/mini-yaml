# mini-yaml

![version](https://img.shields.io/badge/version-v0.1.0-blue) [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT) [![GitHub Linux Master](https://img.shields.io/github/workflow/status/jimmiebergmann/mini-yaml/linux/master?label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions) [![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) [![codecov](https://codecov.io/gh/jimmiebergmann/mini-yaml/branch/master/graph/badge.svg)](https://codecov.io/gh/jimmiebergmann/mini-yaml)  
Single header YAML 1.0 C++ >= 11 serializer/deserializer.

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
// TODO
```

## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/yaml2.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/yaml2.hpp)".

## Build status
Builds are passed if all tests are successful and no memory leaks were found.

| Branch | Linux  | Windows |
| ------ | ------ | ------- |
| master | [![GitHub Linux Master](https://img.shields.io/github/workflow/status/jimmiebergmann/mini-yaml/linux/master?label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![GitHub Windows Master ](https://img.shields.io/github/workflow/status/jimmiebergmann/mini-yaml/windows/master?label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Windows Master](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/master) |
| dev    | [![GitHub Windows Dev](https://img.shields.io/github/workflow/status/jimmiebergmann/mini-yaml/linux/dev?label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-linux.yml) | [![GitHub Windows Dev ](https://img.shields.io/github/workflow/status/jimmiebergmann/mini-yaml/windows/dev?label=Github&logo=Github)](https://github.com/jimmiebergmann/mini-yaml/actions/workflows/github-build-windows.yml) [![AppVeyor Dev](https://img.shields.io/appveyor/ci/jimmiebergmann/mini-yaml/dev?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/mini-yaml/branch/dev) |  

## Missing features (TODO)
- yaml::dom
- yaml::dom_view
- indentation indicator
- sequences
- tags(!!type)
- anchors
- flow sequences/maps
- complex keys
- sets
