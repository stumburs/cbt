#include "cbt.h"
#include <iostream>
#include <iomanip>
#include <functional>
#include <filesystem>
#include <sstream>

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
        case cbt::Hashes::show_compile_command:
            cbt::Flags::show_compile_command = true;
            break;
        case cbt::Hashes::clean:
            cbt::Flags::clean = true;
            break;

        default:
            cbt::log(cbt::LogType::WARNING, "Unknown Flag: " + arg);
            break;
        }
    }

    return cbt::Result::SUCCESS;
}

void cbt::log(LogType log_type, const std::string &text)
{
    if (cbt_config::timestamp_output)
    {
        auto current_timestamp = []()
        {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::ostringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
            return ss.str();
        };

        std::cout << "[" << current_timestamp() << "] ";
    }

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
    if (std::system(cbt::compile_command.c_str()) != 0)
    {
        return cbt::Result::FAIL;
    }
    return cbt::Result::SUCCESS;
}

void cbt::show_help()
{
    log(cbt::LogType::INFO, "Refer to the source code or the README.md");
}

void cbt::show_compile_command()
{
    log(cbt::LogType::INFO, "Compile command: " + cbt::compile_command);
}

std::string cbt::create_compile_command()
{
    std::string cmd = "";
    cmd += cbt_config::cc;

    // Add srcs
    for (const auto &file : cbt::get_all_src_files(cbt_config::src))
    {
        cmd += add_space_if_not_empty(file);
    }

    // Add CFLAGS
    for (const auto &cflag : cbt_config::cflags)
    {
        cmd += add_space_if_not_empty(cflag);
    }

    // Add LDFLAGS
    for (const auto &ldflag : cbt_config::ldflags)
    {
        cmd += add_space_if_not_empty(ldflag);
    }

    cmd += " -o " + cbt_config::target;

    return cmd;
}

cbt::Result cbt::run_cmd(const std::string &cmd)
{
    if (std::system(cmd.c_str()) != 0)
    {
        return cbt::Result::FAIL;
    }
    return cbt::Result::SUCCESS;
}

std::vector<std::string> cbt::get_all_src_files(const std::vector<std::string> &directories)
{
    std::vector<std::string> extensions = {".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hxx"};
    std::vector<std::filesystem::path> paths;

    for (const auto &dir : directories)
    {
        if (!std::filesystem::exists(dir))
        {
            cbt::log(cbt::LogType::WARNING, std::string("Couldn't find path: \'" + dir + '\''));
            continue;
        }
        if (std::filesystem::is_directory(dir))
        {
            for (std::filesystem::recursive_directory_iterator i(dir), end; i != end; ++i)
            {
                if (!std::filesystem::is_directory(i->path()))
                {
                    if (std::find(extensions.begin(), extensions.end(), i->path().extension().string()) != extensions.end())
                    {
                        paths.push_back(i->path().string());
                    }
                }
            }
        }
        else
        {
            if (std::find(extensions.begin(), extensions.end(), dir.substr(dir.find_last_of("."))) != extensions.end())
            {
                paths.push_back(dir);
            }
        }
    }

    // Remove duplicate paths
    std::sort(paths.begin(), paths.end());
    paths.erase(std::unique(paths.begin(), paths.end()), paths.end());

    // Convert paths to strings
    std::vector<std::string> str_paths;
    for (const auto &dir : paths)
    {
        str_paths.push_back(dir.string());
    }
    return str_paths;
}

cbt::Result cbt::clean()
{
    if (std::filesystem::remove(cbt_config::target))
    {
        cbt::log(cbt::LogType::SUCCESS, "Removed \'" + cbt_config::target + "\'");
        return cbt::Result::SUCCESS;
    }
    return cbt::Result::FAIL;
}

void cbt::show_time()
{
    auto build_time = std::chrono::duration_cast<std::chrono::milliseconds>(cbt::build_end - cbt::build_start);
    auto run_time = std::chrono::duration_cast<std::chrono::milliseconds>(cbt::run_end - cbt::run_start);

    cbt::log(cbt::LogType::INFO, "Build: " + std::to_string(build_time.count()) + "ms");
    cbt::log(cbt::LogType::INFO, "Execution: " + std::to_string(run_time.count()) + "ms");
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

    if (cbt::Flags::clean)
    {
        if (cbt::clean() != cbt::Result::SUCCESS)
        {
            cbt::log(cbt::LogType::ERROR, "Failed to remove \'" + cbt_config::target + "\'");
            return 1;
        }
        return 0;
    }

    cbt::compile_command = cbt::create_compile_command();

    if (cbt::Flags::show_compile_command)
    {
        cbt::show_compile_command();
        return 0;
    }

    // Run pre-build command
    if (!cbt_config::pre_build_command.empty())
    {
        if (cbt::run_cmd(cbt_config::pre_build_command) != cbt::Result::SUCCESS)
        {
            cbt::log(cbt::LogType::ERROR, "Failed to execute pre-build command: \'" + cbt_config::pre_build_command + "\'");
            return 1;
        }
    }

    cbt::log(cbt::LogType::INFO, "Compiling...");

    if (cbt_config::time)
    {
        cbt::build_start = std::chrono::high_resolution_clock::now();
    }

    if (cbt::build() != cbt::Result::SUCCESS)
    {
        cbt::log(cbt::LogType::ERROR, "Something went wrong!");
        return 1;
    }

    if (cbt_config::time)
    {
        cbt::build_end = std::chrono::high_resolution_clock::now();
    }

    // Run post-build command
    if (!cbt_config::post_build_command.empty())
    {
        if (cbt::run_cmd(cbt_config::post_build_command) != cbt::Result::SUCCESS)
        {
            cbt::log(cbt::LogType::ERROR, "Failed to execute post-build command: \'" + cbt_config::post_build_command + "\'");
            return 1;
        }
    }

    cbt::log(cbt::LogType::SUCCESS, "Successfully compiled!");

    if (cbt::Flags::run || cbt_config::run_after_compiling)
    {
        if (cbt_config::time)
        {
            cbt::run_start = std::chrono::high_resolution_clock::now();
        }
        if (cbt::run_cmd(cbt_config::target) != cbt::Result::SUCCESS)
        {
            cbt::log(cbt::LogType::ERROR, "Failed to execute program: " + cbt_config::target);
            return 1;
        }

        if (cbt_config::time)
        {
            cbt::run_end = std::chrono::high_resolution_clock::now();
            cbt::show_time();
        }

        // Run post-run command
        if (!cbt_config::post_run_command.empty())
        {
            if (cbt::run_cmd(cbt_config::post_run_command) != cbt::Result::SUCCESS)
            {
                cbt::log(cbt::LogType::ERROR, "Failed to execute post-run command: \'" + cbt_config::post_run_command + "\'");
                return 1;
            }
        }
    }

    return 0;
}