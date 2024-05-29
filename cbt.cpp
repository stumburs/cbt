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
        std::cout << "\x1B[32m[SUCCESS]\033[0m\t";
        break;
    case cbt::LogType::INFO:
        std::cout << "[INFO]\t";
        break;
    case cbt::LogType::WARNING:
        std::cout << "\x1B[33m[WARNING]\033[0m\t";
        break;
    case cbt::LogType::ERROR:
        std::cout << "\x1B[31m[ERROR]\033[0m\t";
        break;
    default:
        break;
    }
    std::cout << std::setw(20) << text << std::endl;
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
    cbt::Result load_args_result = cbt::load_args(argc, argv);
    cbt::Result process_args_result = cbt::process_args();

    if (cbt::Flags::help)
    {
        cbt::show_help();
        return 0;
    }

    cbt::log(cbt::LogType::INFO, "Compiling...");

    cbt::Result compile_result = cbt::build();

    if (compile_result != cbt::Result::SUCCESS)
    {
        cbt::log(cbt::LogType::ERROR, "Something went wrong!");
    }

    cbt::log(cbt::LogType::SUCCESS, "Successfully compiled!");
}