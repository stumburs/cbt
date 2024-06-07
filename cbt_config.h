#pragma once
#include <string>
#include <vector>

namespace cbt_config
{
    using paths = std::vector<std::string>;
    using flags = std::vector<std::string>;

    // Compilation
    std::string cc = "g++";          // Your compiler of choice (cc, gcc, g++, clang, etc)
    paths src = {"main.cpp"};        // Your source files/folders of .c/.cpp files.
    flags cflags = {"-O3"};          // Any CFLAGS such as '-I./include -O3 -Wall -pedantic'
    flags ldflags = {};              // Any LDFLAGS such as '-L./lib -lm'
    std::string target = "main.exe"; // Your target executable name.

    // Features
    bool console_colors = true;       // Enable console output colors (not supported on all terminals)
    bool run_after_compiling = false; // Automatically run program after compiling
    bool timestamp_output = false;    // Add timestamps to cbt::Log messages

    // Custom commands
    std::string pre_build_command = "";  // Custom command to run before build
    std::string post_build_command = ""; // Custom command to run after build
    std::string post_run_command = "";   // Custom command to run after executing (if enabled)
};