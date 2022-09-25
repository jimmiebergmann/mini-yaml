project (mini-yaml-fuzz)
cmake_minimum_required(VERSION 2.8.12)

# Options.
set(MINIYAML_CPP_VERSION 20 CACHE STRING "Set c++ version.")

message("C++ language version: ${MINIYAML_CPP_VERSION}")

# C++ config.
set(CMAKE_CXX_STANDARD ${MINIYAML_CPP_VERSION})
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads)

# Force warnings as errors
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -fno-elide-constructors")
endif()

set(root_dir "${CMAKE_SOURCE_DIR}/..")
set(mini_yaml_dir "${root_dir}/yaml")
set(fuzz_dir "${root_dir}/fuzz")

include_directories ("${root_dir}")

add_executable(fuzz_sax_parse "${fuzz_dir}/fuzz_sax_parse.cpp")
target_link_libraries(fuzz_sax_parse PRIVATE -fsanitize=fuzzer,undefined,address)
target_compile_options(fuzz_sax_parse PRIVATE -fsanitize=fuzzer,undefined,address)
set_target_properties( fuzz_sax_parse
  PROPERTIES
  OUTPUT_NAME_DEBUG "fuzz_sax_parse_d"
  OUTPUT_NAME_RELEASE "fuzz_sax_parse"
  RUNTIME_OUTPUT_DIRECTORY "${root_dir}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${root_dir}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${root_dir}/bin"
)

add_executable(fuzz_sax_parse_reject_failure "${fuzz_dir}/fuzz_sax_parse_reject_failure.cpp")
target_link_libraries(fuzz_sax_parse_reject_failure PRIVATE -fsanitize=fuzzer,undefined,address)
target_compile_options(fuzz_sax_parse_reject_failure PRIVATE -fsanitize=fuzzer,undefined,address)
set_target_properties( fuzz_sax_parse_reject_failure
  PROPERTIES
  OUTPUT_NAME_DEBUG "fuzz_sax_parse_reject_failure_d"
  OUTPUT_NAME_RELEASE "fuzz_sax_parse_reject_failure"
  RUNTIME_OUTPUT_DIRECTORY "${root_dir}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${root_dir}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${root_dir}/bin"
)