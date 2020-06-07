cmake_minimum_required(VERSION 3.1.0)
project(HomeBanking VERSION 0.1.0)
enable_language(CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "MSVC detected, adding compile flags")
    #add_compile_options(/W4)

    if(CMAKE_BUILD_TYPE MATCHES "Release")
        add_compile_options(/O2)
    else()
        add_compile_options(/Od /DEBUG:FASTLINK)
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC detected, adding compile flags")
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
    
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        add_compile_options(-O3)
    else()
        add_compile_options(-g)
    endif()
else()
    message(FATAL_ERROR "Unkown compiler")
endif()

# 3rd party
find_package(Threads REQUIRED)
find_package(fmt REQUIRED PATHS third_party/build/fmt/${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE})

# Includes
include_directories(src)
include_directories(third_party)

# Src files
set(PROJECT_SOURCE_FILES 
    src/Application.cpp
    src/CsvParser.cpp
    src/CsvItem.cpp
    src/CsvWriter.cpp
    src/CsvDate.cpp
    src/CsvDatabase.cpp
    src/ReportGenerator.cpp
    src/CustomException.cpp
    src/Utils.cpp
)

# Execuables
add_executable(HomeBanking src/main.cpp ${PROJECT_SOURCE_FILES})
add_executable(selftest src/selftest.cpp ${PROJECT_SOURCE_FILES})

target_link_libraries(HomeBanking Threads::Threads fmt::fmt)
target_link_libraries(selftest Threads::Threads fmt::fmt)

install(TARGETS HomeBanking RUNTIME DESTINATION ./bin)
install(TARGETS selftest RUNTIME DESTINATION ./bin)

enable_testing()
add_test(NAME Selftest 
         WORKING_DIRECTORY ../../../testdata/input/
         COMMAND selftest)

