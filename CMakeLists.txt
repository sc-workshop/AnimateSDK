cmake_minimum_required(VERSION 3.22)

# workspace
project(animate-sdk)

# includes
include(FetchContent)
include(cmake/dependencies.cmake)
include(WkJson)
include(WkLocale)

# projects
add_subdirectory(animate-sdk)
