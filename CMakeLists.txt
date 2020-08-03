cmake_minimum_required(VERSION 3.1.0)
project(hokee 
    VERSION 1.0.0
    DESCRIPTION "Automated Housekeeping Book"
    HOMEPAGE_URL "https://github.com/StephanSchedler/hokee"
    LANGUAGES CXX
)

configure_file(src/hokee.cmake.h hokee.h)

if(WIN32)
    message(STATUS "WIN32 detected, configure CPack generator(s)")
    set(CPACK_GENERATOR ZIP 7Z)
elseif(UNIX)
    if(APPLE)
        message(STATUS "APPLE detected, configure CPack generator(s)")
        set(CPACK_GENERATOR TGZ) #DragNDrop #Bundle
        #set(CPACK_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
        #set(CPACK_BUNDLE_PLIST "Info.plist")
        #set(CPACK_BUNDLE_ICON "hokee.png")
    else()
        message(STATUS "UNIX detected, configure CPack generator(s)")
        set(CPACK_GENERATOR TGZ DEB RPM)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
        set(CPACK_PACKAGE_CONTACT "schedler@paderborn.com")
    endif()
endif()
include(CPack)

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

    set(CMAKE_CXX_FLAGS "/W4 /nologo /EHsc /MP /D_CRT_SECURE_NO_WARNINGS")
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
include_directories(${PROJECT_BINARY_DIR})

# Src files
set(PROJECT_SOURCE_FILES
    src/csv/CsvParser.cpp
    src/csv/CsvConfig.cpp
    src/csv/CsvItem.cpp
    src/csv/CsvWriter.cpp
    src/csv/CsvFormat.cpp
    src/csv/CsvDate.cpp
    src/csv/CsvDatabase.cpp
    src/csv/CsvRules.cpp 
    src/html/HtmlGenerator.cpp
    src/html/HtmlElement.cpp
    src/html/HtmlText.cpp
    src/Application.cpp
    src/Settings.cpp
    src/InternalException.cpp
    src/UserException.cpp
    src/Utils.cpp
    src/HttpServer
)

# Images
configure_file(third_party/small-n-flat/png/24/notepad.png ${PROJECT_BINARY_DIR}/images/24-notepad.png COPYONLY)
configure_file(third_party/small-n-flat/png/24/wrench-screwdriver.png ${PROJECT_BINARY_DIR}/images/24-wrench-screwdriver.png COPYONLY)
configure_file(third_party/small-n-flat/png/24/folder.png ${PROJECT_BINARY_DIR}/images/24-folder.png COPYONLY)
configure_file(third_party/small-n-flat/png/48/money.png ${PROJECT_BINARY_DIR}/images/48-money.png COPYONLY)
configure_file(third_party/small-n-flat/png/48/file-text.png ${PROJECT_BINARY_DIR}/images/48-file-text.png COPYONLY) 
configure_file(third_party/small-n-flat/png/48/sign-check.png ${PROJECT_BINARY_DIR}/images/48-sign-check.png COPYONLY)
configure_file(third_party/small-n-flat/png/48/sign-delete.png ${PROJECT_BINARY_DIR}/images/48-sign-delete.png COPYONLY)
configure_file(third_party/small-n-flat/png/48/sign-warning.png ${PROJECT_BINARY_DIR}/images/48-sign-warning.png COPYONLY)
configure_file(third_party/small-n-flat/png/48/sign-error.png ${PROJECT_BINARY_DIR}/images/48-sign-error.png COPYONLY)  
configure_file(third_party/small-n-flat/png/48/sign-sync.png ${PROJECT_BINARY_DIR}/images/48-sign-sync.png COPYONLY)  
configure_file(third_party/small-n-flat/png/48/file-excel.png ${PROJECT_BINARY_DIR}/images/48-file-excel.png COPYONLY)   
configure_file(third_party/small-n-flat/png/48/file-exe.png ${PROJECT_BINARY_DIR}/images/48-file-exe.png COPYONLY)   
configure_file(third_party/small-n-flat/png/48/cogs.png ${PROJECT_BINARY_DIR}/images/48-cogs.png COPYONLY)    
configure_file(third_party/small-n-flat/png/48/search.png ${PROJECT_BINARY_DIR}/images/48-search.png COPYONLY) 
configure_file(third_party/small-n-flat/png/48/box-full.png ${PROJECT_BINARY_DIR}/images/48-box-full.png COPYONLY)
configure_file(third_party/small-n-flat/png/48/box-in.png ${PROJECT_BINARY_DIR}/images/48-box-in.png COPYONLY)  
configure_file(third_party/small-n-flat/png/48/envelope-letter.png ${PROJECT_BINARY_DIR}/images/48-envelope-letter.png COPYONLY)  
configure_file(third_party/small-n-flat/png/96/box.png ${PROJECT_BINARY_DIR}/images/96-box.png COPYONLY)
configure_file(third_party/small-n-flat/png/96/sign-ban.png ${PROJECT_BINARY_DIR}/images/96-sign-ban.png COPYONLY)
configure_file(src/images/sign-delete2.png ${PROJECT_BINARY_DIR}/images/48-sign-delete2.png COPYONLY)
configure_file(src/images/sign-warning2.png ${PROJECT_BINARY_DIR}/images/48-sign-warning2.png COPYONLY)

# Execuables
add_executable(hokee src/hokee.cpp ${PROJECT_SOURCE_FILES})
add_executable(rules_match_test tests/rules_match_test.cpp ${PROJECT_SOURCE_FILES})
add_executable(rules_add_test tests/rules_add_test.cpp ${PROJECT_SOURCE_FILES})

target_link_libraries(hokee Threads::Threads fmt::fmt)
target_link_libraries(rules_match_test Threads::Threads fmt::fmt)
target_link_libraries(rules_add_test Threads::Threads fmt::fmt)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    target_link_libraries(hokee stdc++fs)
    target_link_libraries(rules_match_test stdc++fs)
    target_link_libraries(rules_add_test stdc++fs)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(hokee c++fs)
    target_link_libraries(rules_match_test c++fs)
    target_link_libraries(rules_add_test c++fs)
endif()

install(TARGETS hokee RUNTIME DESTINATION ./bin)
install(TARGETS rules_match_test RUNTIME DESTINATION ./bin)
install(TARGETS rules_add_test RUNTIME DESTINATION ./bin)
if(CMAKE_EXPORT_COMPILE_COMMANDS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    install(FILES ${PROJECT_BINARY_DIR}/compile_commands.json DESTINATION .)
endif()

install(DIRECTORY ${PROJECT_BINARY_DIR}/images DESTINATION .)
install(DIRECTORY favicon DESTINATION ./images)

enable_testing()
add_test(NAME rules_match_test COMMAND rules_match_test)
add_test(NAME rules_add_test COMMAND rules_add_test)
install(FILES tests/test_data/rules_match_test.csv DESTINATION ./test_data/rules_match_test)
install(FILES tests/test_data/ABC/Account_123456790_2020_1.csv DESTINATION ./test_data/rules_match_test/input/ABC)
install(FILES tests/test_data/ABC/Account_123456790_2020_2.csv DESTINATION ./test_data/rules_match_test/input/ABC)
install(FILES tests/test_data/ABC/format.ini DESTINATION ./test_data/rules_match_test/input/ABC)
install(FILES tests/test_data/input_add_test.csv DESTINATION ./test_data/rules_add_test)
install(FILES tests/test_data/rules_add_test.csv DESTINATION ./test_data/rules_add_test)
install(FILES tests/test_data/ABC/Account_123456790_2020_1.csv DESTINATION ./test_data/rules_add_test/input/ABC)
install(FILES tests/test_data/ABC/Account_123456790_2020_2.csv DESTINATION ./test_data/rules_add_test/input/ABC)
install(FILES tests/test_data/ABC/format.ini DESTINATION ./test_data/rules_add_test/input/ABC)
