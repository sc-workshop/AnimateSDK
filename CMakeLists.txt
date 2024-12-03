cmake_minimum_required(VERSION 3.22)

# workspace
project(animate-sdk)

set(WK_SERIALIZATION_JSON  ON)

# includes
include(FetchContent)
include(cmake/dependencies.cmake)

# projects
add_subdirectory(animate-sdk)
