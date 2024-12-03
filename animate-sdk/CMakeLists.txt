# Project: AnimateSDK

# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h source/*.hpp)
file(GLOB_RECURSE SOURCES source/*.cpp)

set(TARGET AnimateSDK)

add_library(${TARGET} STATIC)	
add_library(Adobe::Animate ALIAS ${TARGET})
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "AnimateSDK")

# add *.h and *.cpp files
target_sources(${TARGET} PRIVATE ${HEADERS} ${SOURCES})

# include directories
target_include_directories(${TARGET} PUBLIC source)

# setup project
wk_project_setup(${TARGET})

file(GLOB_RECURSE PUBLIC_HEADERS include/*.h)
target_sources(${TARGET} PRIVATE ${PUBLIC_HEADERS})
target_include_directories(${TARGET} PUBLIC 
    include
)

add_definitions(
    -DUNICODE -D_UNICODE
)

target_link_libraries(${TARGET} PUBLIC 
    wk::core
)