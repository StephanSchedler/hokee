cmake_minimum_required(VERSION 3.1.0)
project(homebanking 
    VERSION 0.1.0 
    HOMEPAGE_URL https://github.com/StephanSchedler/Homebanking
    LANGUAGES CXX
)

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

    set(CMAKE_CXX_FLAGS "/W4 /nologo /EHsc /MP")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /DEBUG:FASTLINK /Ob0 /Zi /RTC1 /DDEBUG /D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|AppleClang|Clang")
    message(STATUS "GCC|AppleClang|Clang detected, adding compile flags")
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
    
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        add_compile_options(-O3)
    else()
        add_compile_options(-g)
    endif()
else()
    message(FATAL_ERROR "Compiler flags have not yet been defined for ${CMAKE_CXX_COMPILER_ID}")
endif()

# 3rd party
find_package(Threads REQUIRED)
find_package(fmt REQUIRED PATHS third_party/build/fmt/${CMAKE_BUILD_TYPE})

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
add_executable(homebanking src/main.cpp ${PROJECT_SOURCE_FILES})
add_executable(selftest src/selftest.cpp ${PROJECT_SOURCE_FILES})

target_link_libraries(homebanking Threads::Threads fmt::fmt)
target_link_libraries(selftest Threads::Threads fmt::fmt)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    target_link_libraries(homebanking stdc++fs)
    target_link_libraries(selftest stdc++fs)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(homebanking c++fs)
    target_link_libraries(selftest c++fs)
endif()

install(TARGETS homebanking RUNTIME DESTINATION ./bin)
install(TARGETS selftest RUNTIME DESTINATION ./bin)

enable_testing()
add_test(NAME Selftest 
         WORKING_DIRECTORY ../../testdata/input/
         COMMAND selftest)

