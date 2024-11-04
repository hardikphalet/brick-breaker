# BB

A basic clone of the classic brick breaker game, written in C using the raylib framework. This project was a quick and straightforward recreation of my partner's favorite game, completed within a few hours. The primary goal was to deliver a fun and familiar experience rather than an optimized or polished codebase.

## Features
Classic Gameplay: Simple paddle and ball mechanics reminiscent of classic brick breaker games.
Rapid Development: Built quickly without extensive optimizations or complex packaging.
## Project Goals
This project aimed to provide a nostalgic gaming experience and was warmly received by its intended audience. While the code is rudimentary and lacks advanced organization, it met its purpose and brought joy to the player.

## Known Limitations
- No Code Optimizations: The code was written rapidly and may contain inefficiencies.
- Basic Structure: Minimal organization without modular or scalable packaging.

## How to Run

### Installing Raylib
To run this project, you’ll need to install the **raylib** library. For more information and installation options on other platforms, visit the [raylib GitHub page](https://github.com/raysan5/raylib).

On **macOS**, you can install **raylib** using Homebrew:
```shell
brew install raylib
```

### Building the Project
After installing **raylib**, you can build and run the project. The following steps assume you’re using **macOS**:

1. **Generate Build Files** using CMake:
    ```shell
    cmake .
    ```

2. **Compile the Project** using `make` with parallel jobs for faster building:
    ```shell
    make -j4
    ```

After compilation, you should be able to run the resulting executable.