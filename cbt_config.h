#pragma once
#include <string>

namespace cbt_config
{
    // Compilation
    std::string cc = "g++";
    std::string cflags = "";
    std::string ldflags = "";
    std::string src = "main.cpp";
    std::string target = "main.exe";

    // Enable console output colors
    bool console_colors = true;
};