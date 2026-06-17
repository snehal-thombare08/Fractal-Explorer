# 🌀 Fractal Explorer

An interactive fractal renderer built with **C++** and **SFML 3.0** — explore infinite mathematical beauty with smooth zooming and panning!

![Fractal Explorer](https://raw.githubusercontent.com/snehal-thombare08/Fractal-Explorer/main/Screenshot%202026-06-17%20145149.png)

## ✨ Features

- **3 Fractal Types** — Mandelbrot Set, Julia Set, Burning Ship
- **Infinite zoom** — scroll to zoom toward any point on the fractal
- **Smooth panning** — drag to explore any region
- **Julia Set from mouse** — press J to generate Julia Set from cursor position
- **Psychedelic color palette** — smooth cycling colors based on iteration count
- **Multi-threaded rendering** — uses all CPU cores for fast rendering
- **Adjustable detail** — increase/decrease max iterations for more depth
- **Real-time coordinates** — see exact fractal position and zoom level

## 🚀 Download & Run

1. Download **FractalExplorer-v1.0-Windows.zip**
2. Extract all files
3. Run **FractalExplorer.exe**

## 🎮 Controls

| Key / Mouse | Action |
|-------------|--------|
| `Scroll` | Zoom in/out toward mouse |
| `LMB drag` | Pan around |
| `1` | Mandelbrot Set |
| `2` | Julia Set |
| `3` | Burning Ship |
| `J` | Julia Set from mouse position |
| `↑` | More detail (2x iterations) |
| `↓` | Less detail (faster render) |
| `+` / `-` | Zoom in / out |
| `R` | Reset view |
| `Esc` | Quit |

## 🧮 How It Works

Each pixel maps to a complex number. The program iterates the formula **z = z² + c** up to 256 times. Points that never escape to infinity are colored black (the set). Points that escape are colored based on how quickly they diverge — creating the psychedelic boundary patterns.

## 🛠️ Build from Source

**Requirements:** C++17, SFML 3.0, CMake, MinGW

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"
mingw32-make
```

## 🧰 Tech Stack

- **Language:** C++17
- **Graphics:** SFML 3.0
- **Rendering:** Multi-threaded CPU (std::thread)
- **Build:** CMake + MinGW
