cmake_minimum_required(VERSION 3.11)
project(cpp03-optional)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "${Boost_INCLUDE_DIRS}")

add_executable(cpp03-optional optional_test.cpp optional.cpp)

enable_testing()

add_executable(optional_test optional_test.cpp optional.cpp)
add_test(
        NAME optional_test
        COMMAND $<TARGET_FILE:optional_test>
)
set_property(TEST optional_test PROPERTY LABELS optional)

