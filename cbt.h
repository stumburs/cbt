#pragma once
#include "cbt_config.h"
#include <deque>
#include <functional>

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
    }

    Result load_args(int argc, char *argv[]);
    Result process_args();
    void log(LogType log_type, std::string text);
    std::string get_next_arg();
    Result build();
    void show_help();
    Result run(const std::string &path);
};