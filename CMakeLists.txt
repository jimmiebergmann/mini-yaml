# Function for grouping files. Used for filters in Visual Studio.
function(createSourceGroups files rootPath)
	foreach(_source IN ITEMS ${files} )
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${rootPath}" "${_source_path}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
endfunction(createSourceGroups)


project (mini-yaml)
cmake_minimum_required(VERSION 2.8.12)

# Options.
option(MINIYAML_ENABLE_COVERAGE "Enables coverage reporting." OFF)
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
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -fno-elide-constructors")
endif()


if(MINIYAML_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()


# Find files
set(mini_yaml_dir "${CMAKE_SOURCE_DIR}/yaml")
set(mini_yaml_test_dir "${CMAKE_SOURCE_DIR}/test")
set(mini_yaml_examples_dir "${CMAKE_SOURCE_DIR}/examples")
file(GLOB_RECURSE test_files "${mini_yaml_test_dir}/*.cpp" "${mini_yaml_test_dir}/*.hpp")
file(GLOB_RECURSE first_example_src "${mini_yaml_examples_dir}/first_example.cpp")


# ==============================================================================================================
#
#   First example
#
# ==============================================================================================================
#include_directories ("${mini_yaml_dir}")
#link_directories("${CMAKE_SOURCE_DIR}/lib")
#add_executable(first_example ${first_example_src} ${mini_yaml_files})

#set_target_properties( first_example
#  PROPERTIES
#  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
#  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
#  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
#)


# ==============================================================================================================
#
#   Test
#
# ==============================================================================================================
SET(gtest_force_shared_crt on CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/googletest
                 ${CMAKE_SOURCE_DIR}/vendor/googletest
                 EXCLUDE_FROM_ALL) # Add googletest directly to our build.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

add_executable(mini_yaml_tests ${test_files})

set_target_properties( mini_yaml_tests
  PROPERTIES
  OUTPUT_NAME_DEBUG "mini_yaml_tests_d"
  OUTPUT_NAME_RELEASE "mini_yaml_tests"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)

if(MSVC)
  if((${CMAKE_VERSION} VERSION_EQ "3.8.0") OR (${CMAKE_VERSION} VERSION_GREATER "3.8.0")) 
    set_property(TARGET mini_yaml_tests APPEND PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
  endif()
endif()

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(mini_yaml_tests PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(mini_yaml_tests "gcov")
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")

target_link_libraries(mini_yaml_tests gtest_main)