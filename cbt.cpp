#include "cbt.h"
#include <iostream>
#include <iomanip>
#include <functional>

cbt::Result cbt::load_args(int argc, char *argv[])
{
    for (size_t i = 0; i < argc; i++)
    {
        cbt::args.push_back(argv[i]);
    }
    return Result::SUCCESS;
}

cbt::Result cbt::process_args()
{
    if (args.empty())
    {
        return cbt::Result::SUCCESS;
    }

    // Get cbt location
    cbt::Flags::cbt_location = get_next_arg();

    std::hash<std::string> hasher;
    std::string arg;

    while (!args.empty())
    {
        arg = get_next_arg();

        // Couldn't figure out a better way, so we're
        // hashing the string so we can use it in
        // a switch statement
        switch (cbt::hash_djb2a(arg))
        {
        case cbt::Hashes::help:
            cbt::Flags::help = true;
            break;
        case cbt::Hashes::run:
            cbt::Flags::run = true;
            break;

        default:
            cbt::log(cbt::LogType::WARNING, "Unknown Flag: " + arg);
            break;
        }
    }

    return cbt::Result::SUCCESS;
}

void cbt::log(LogType log_type, std::string text)
{
    switch (log_type)
    {
    case cbt::LogType::SUCCESS:
        if (cbt_config::console_colors)
            std::cout << "\x1B[32;1m";
        std::cout << "[SUCCESS]";
        break;
    case cbt::LogType::INFO:
        if (cbt_config::console_colors)
            std::cout << "\x1B[39;1m";
        std::cout << "[INFO]";
        break;
    case cbt::LogType::WARNING:
        if (cbt_config::console_colors)
            std::cout << "\x1B[33;1m";
        std::cout << "[WARNING]";
        break;
    case cbt::LogType::ERROR:
        if (cbt_config::console_colors)
            std::cout << "\x1B[31;1m";
        std::cout << "[ERROR]";
        break;
    default:
        break;
    }
    if (cbt_config::console_colors)
        std::cout << "\x1B[0m";
    std::cout << ' ' << text << std::endl;
}

std::string cbt::get_next_arg()
{
    if (args.empty())
    {
        return "";
    }

    std::string arg = args.front();
    cbt::args.pop_front();
    return arg;
}

cbt::Result cbt::build()
{
    std::string compile_command = cbt_config::cc + " -o" + cbt_config::target + ' ' + cbt_config::src;
    Result compile_result = (Result)std::system(compile_command.c_str());
    return compile_result;
}

void cbt::show_help()
{
    log(cbt::LogType::INFO, "Refer to the source code or the README.md");
}

int main(int argc, char *argv[])
{
    if (cbt::load_args(argc, argv) != cbt::Result::SUCCESS)
    {
        cbt::log(cbt::LogType::ERROR, "Failed to load args.");
        return 1;
    }

    if (cbt::process_args() != cbt::Result::SUCCESS)
    {
        cbt::log(cbt::LogType::ERROR, "Failed to process args.");
        return 1;
    }

    if (cbt::Flags::help)
    {
        cbt::show_help();
        return 0;
    }

    cbt::log(cbt::LogType::INFO, "Compiling...");

    if (cbt::build() != cbt::Result::SUCCESS)
    {
        cbt::log(cbt::LogType::ERROR, "Something went wrong!");
        return 1;
    }

    cbt::log(cbt::LogType::SUCCESS, "Successfully compiled!");

    if (cbt::Flags::run)
    {
        std::system(cbt_config::target.c_str());
        return 0;
    }
}