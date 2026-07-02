# Procedural 2D Console Physics Engine & Retro Game

A high-performance, **cross-platform 2D console physics simulation** and arcade engine written in pure, standard **C**. This software models particle kinematics, collision boundaries, and vector-bounce calculations rendered dynamically in standard ASCII format.

Bypassing platform-specific dependencies like `<windows.h>`, the system features custom conditional abstraction layers (`#ifdef`) that support zero-dependency builds on **macOS**, **Linux**, and **Windows**.

## 🚀 Key Architectural Engineering Highlights

* **Cross-Platform Compatibility Engine:** Replaced high-overhead, proprietary system APIs with POSIX-standard terminal interfaces (`termios.h`, `fcntl.h`) to allow non-blocking input listener routines (`kbhit()`) and real-time clock loops (`usleep()`) to function seamlessly on macOS Terminal and Linux Shells.

* **Vector Collision Physics:** Implemented a robust 2D bounding-box and line collision intersection formula. The engine tracks dynamic coordinates inside memory buffers, performing high-speed calculations to update angle vectors when interacting with moving paddles or static game boundaries.

* **Zero-Flicker ASCII Rendering:** Utilizes specialized terminal control escape sequences (`\033[H`) to move the terminal writing head back to origin rather than completely flashing the standard display buffer. This creates smooth, tear-free animations.

* **Non-Blocking Key Reading:** Bypasses blocked system input sequences. Users can control operations via up and down keys without halting the engine's real-time physics clock.

## 📂 Project Structure

* `procedural_2d_game_engine.c` — The complete physics and simulation routine.

## 💻 Compilation & Local Execution

This engine compiles out of the box using any standard C compiler (`gcc`, `clang`, or standard MSVC environments).

### Running Natively on Mac (macOS Terminal)

1. Open your Mac Terminal.

2. Navigate directly to your local project directory.
