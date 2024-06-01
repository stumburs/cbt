# cbt (**C** **B**uild **T**ool)

Simple C and C++ build tool.

## What is it?

cbt is a basic build tool/system for C/C++ where you only require a C++ compiler - no build system required like Make, CMake, etc. It's vaguely inspired by [nobuild](https://github.com/tsoding/nobuild), and not quite as useful as this tool was primarly made for fun and learning.

## Quick Start

1.  Acquire [`cbt.cpp`](./cbt.cpp), [`cbt.h`](./cbt.h) and [`cbt_config.h`](./cbt_config.h) in whatever way you like.
2.  Put these files into your project directory, preferably the root.
3.  Modify the [`cbt_config.h`](./cbt_config.h) file to set up the "build system".

    ```cpp
    #pragma once
    #include <string>
    #include <vector>

    namespace cbt_config
    {
        using paths = std::vector<std::string>;
        using flags = std::vector<std::string>;

        // Compilation
        std::string cc = "g++";                   // Your compiler of choice (cc, gcc, g++, clang, etc)
        paths src = {"main.cpp", "./other_srcs"}; // Your source files/folders of .c/.cpp files.
        flags cflags = {"-I./include", "-O3"};    // Any CFLAGS such as '-I./include -O3 -Wall -pedantic'
        flags ldflags = {"-L./lib"};              // Any LDFLAGS such as '-L./lib -lm'
        std::string target = "main.exe";          // Your target executable name.

        // Enable console output colors
        bool console_colors = true;
    };
    ```

> [!NOTE]
> `paths` recursively interates through all directories within given folders, however `flags`, does not support this functionality yet. If your include folder has nested directories, you have to specify each one.

1.  Compile the build tool.

    ```console
    g++ -o cbt ./cbt.cpp -std=c++20
    ```

> [!NOTE]
> As this project is written in C++, you have to use a C++ compiler to build it, even if you want to use the tool for C projects. Furthermore, you must use C++20.

5.  Run the build tool to build your project.

    ```console
    ./cbt
    ```

## Flags

| Flag    | Description                            | Example       |
| ------- | -------------------------------------- | ------------- |
| `-help` | Displays help information              | `./cbt -help` |
| `-cmd`  | Displays the constructed build command | `./cbt -cmd`  |
| `-r`    | Runs the project after compiling       | `./cbt -r`    |

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## License

This project is licensed under the [MIT License](LICENSE).
