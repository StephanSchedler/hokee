#pragma once

#if (defined(__clang_major__) && __clang_major__ < 9) || (defined(_MSVC_VER) && _MSVC_VER < 1914)                 \
    || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 8)
#include <experimental/filesystem>
#else
#include <filesystem>
#endif

#if (defined(__clang_major__) && __clang_major__ < 9) || (defined(_MSVC_VER) && _MSVC_VER < 1914)                 \
    || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 8)
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif