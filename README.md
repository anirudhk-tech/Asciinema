# Asciinema

A high-throughput, real-time video player that decodes video files, processes frames in parallel stages, and renders them as ASCII art in the terminal.

## Tech Stack

- C++17
- OpenCV 4.x
- ncurses

## Prerequisites

```bash
# macOS
brew install cmake opencv ncurses

# Linux (Ubuntu/Debian)
sudo apt install build-essential cmake libopencv-dev libncurses5-dev
```

## Build

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

```bash
./asciinema-player path/to/video.mp4
```

## Project Structure

```
asciinema/
├── CMakeLists.txt
├── include/asciinema/     # Header files
├── src/                   # Source files
├── tests/                 # Unit tests
├── docs/                  # Documentation
├── scripts/               # Build/utility scripts
└── samples/               # Sample videos
```

## License

MIT License - see [LICENSE](LICENSE)
# Asciinema
