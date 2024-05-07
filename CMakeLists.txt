cmake_minimum_required(VERSION 3.13)
project(asaphus_coding_challenge)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 QUIET)

# If Catch2 is not found, download and install it
if(NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.9
    )

    FetchContent_GetProperties(Catch2)
    if(NOT Catch2_POPULATED)
        FetchContent_Populate(Catch2)
        add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    endif()

    # Set Catch2 as a package
    set(Catch2_DIR ${catch2_BINARY_DIR})
    set(Catch2_FOUND TRUE)
endif()

enable_testing()

# Add executable
add_executable(${PROJECT_NAME} asaphus_coding_challenge.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)

# Set C++ standard
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

# Add test
add_test(NAME asaphus_coding_challenge_tests COMMAND ${PROJECT_NAME})
