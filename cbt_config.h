#pragma once
#include <string>

namespace cbt_config
{
    // Compilation
    std::string cc = "g++";
    std::string src = "main.cpp";
    std::string cflags = "-O3";
    std::string ldflags = "";
    std::string target = "main.exe";

    // Enable console output colors
    bool console_colors = true;
};