#include "cbt.h"
#include <iostream>

cbt::Result cbt::build()
{
    std::string compile_command = cbt_config::cc + " -o" + cbt_config::target + ' ' + cbt_config::src;
    Result compile_result = (Result)std::system(compile_command.c_str());
    return compile_result;
}

int main(int argc, char *argv[])
{
    std::cout << "Compiling..." << std::endl;

    cbt::Result compile_result = cbt::build();

    if (compile_result != cbt::Result::SUCCESS)
    {
        std::cout << "Something went wrong!" << std::endl;
    }

    std::cout << "Compiled successfully!" << std::endl;
}