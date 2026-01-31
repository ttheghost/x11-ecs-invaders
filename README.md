# X11 ECS Invaders

A Space Invaders clone built from scratch in C using pure X11 (Xlib) and an Entity Component System (ECS) architecture.

## The Architecture

This project was built to understand Data-Oriented Design and low-level Linux graphics programming.

* **Language:** C17
* **Graphics:** Xlib (Double Buffered to prevent flickering)
* **Architecture:** Entity Component System (ECS)
  * **Entities:** Just an ID (index in an array).
  * **Components:** Structure of Arrays (SoA) for cache efficiency (Position, Velocity, Sprite, etc.).
  * **Systems:** Stateless logic loops (Movement, Collision, Rendering, AI)

## How to Build

You need the X11 development headers installed.

**Debian/Ubuntu:**

```bash
sudo apt install libx11-dev
sudo apt install libxpm-dev
```

**Fedora/CentOS**

```bash
sudo dnf install libXpm-devel
```

### Build & Run:

```bash
gcc -o space_invaders src/main.c src/x11_wrapper.c src/ecs.c -lX11
./space_invaders
```

### Build & Run (CMake):

```bash
cmake -B build
cmake --build build
./build/space_invaders
```

## Controls
* **Left / Right Arrow:** Move Ship
* **Space:** Fire
* **ESC:** Quit
