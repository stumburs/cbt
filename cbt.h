#pragma once
#include "cbt_config.h"
#include <deque>
#include <functional>
#include <chrono>

namespace cbt
{
    enum class Result
    {
        SUCCESS,
        FAIL,
    };

    enum class LogType
    {
        SUCCESS,
        INFO,
        WARNING,
        ERROR,
    };

    std::deque<std::string> args;

    namespace Flags
    {
        std::string cbt_location = "";
        bool help = false;
        bool run = false;
        bool show_compile_command = false;
        bool clean = false;
    }

    constexpr size_t hash_djb2a(const std::string_view sv)
    {
        size_t hash{5381};
        for (auto c : sv)
        {
            hash = ((hash << 5) + hash) ^ c;
        }
        return hash;
    }

    namespace Hashes
    {
        constexpr size_t help = hash_djb2a("-help");
        constexpr size_t run = hash_djb2a("-r");
        constexpr size_t show_compile_command = hash_djb2a("-cmd");
        constexpr size_t clean = hash_djb2a("-clean");
    }

    std::string compile_command = "";

    // Timing
    std::chrono::time_point<std::chrono::high_resolution_clock> build_start, build_end, run_start, run_end;

    Result load_args(int argc, char *argv[]);
    Result process_args();
    void log(LogType log_type, const std::string &text);
    std::string get_next_arg();
    Result build();
    void show_help();
    void show_compile_command();
    std::string create_compile_command();
    Result run_cmd(const std::string &path);
    std::vector<std::string> get_all_src_files(const std::vector<std::string> &directories);
    Result clean();
    void show_time();

    // Helper functions
    std::string add_space_if_not_empty(const std::string &str)
    {
        return str.empty() ? "" : ' ' + str;
    }
};