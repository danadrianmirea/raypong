# RayPong

A modern implementation of the classic Pong game built with [raylib](https://www.raylib.com/).

Play on itch.io: https://adrianmirea.itch.io/

## Features

- Classic Pong gameplay with smooth controls
- Multiple difficulty levels
- Score tracking
- Sound effects
- Clean, retro-inspired visuals
- CPU opponent with adjustable difficulty

## Controls

- **W/S** or **Up/Down Arrow Keys**: Move paddle up/down
- **P**: Pause game
- **ESC**: Exit game

## Building

### Prerequisites

- C++ compiler (GCC/MinGW recommended)
- raylib library
- CMake (version 3.10 or higher)

### Build Instructions

1. Clone the repository
2. Create a build directory: `mkdir build && cd build`
3. Generate build files: `cmake ..`
4. Build the project: `cmake --build .`
5. The executable will be generated as `game.exe` in the build directory

## Project Structure

- `game.cpp/h`: Main game logic and state management
- `paddle.cpp/h`: Player paddle implementation
- `cpupaddle.cpp/h`: CPU opponent implementation
- `ball.cpp/h`: Ball physics and behavior
- `globals.cpp/h`: Global game constants and settings
- `main.cpp`: Entry point and window setup

## License

This project is licensed under the terms specified in the LICENSE.txt file.


Big thanks to https://github.com/educ8s (https://www.youtube.com/@programmingwithnick) for making great Raylib tutorials on youtube.
