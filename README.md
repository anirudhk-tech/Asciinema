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
# Using Make wrapper
make build

# Or using CMake directly
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

```bash
./build/asciinema-player [OPTIONS] <video_path>
```

### Options

| Option | Description |
|--------|-------------|
| `-color` | Use true color (24-bit grayscale) rendering instead of ASCII characters |
| `-help` | Show usage information |

### Render Modes

**ASCII Mode** (default)
- Uses characters like `@#8&OC=+;:.` to represent brightness levels
- Works on any terminal
- Lower visual fidelity

**True Color Mode** (`-color`)
- Uses 24-bit ANSI escape codes for smooth grayscale
- Requires a terminal with true color support (iTerm2, VS Code, modern Terminal.app)
- Much higher visual quality

### Examples

```bash
# Play video in ASCII mode
./build/asciinema-player video.mp4

# Play video in true color mode (better quality)
./build/asciinema-player -color video.mp4

# Using make
make run VIDEO=samples/test.mp4                    # ASCII mode
./build/asciinema-player -color samples/test.mp4   # True color mode
```

## Project Structure

```
asciinema/
├── CMakeLists.txt
├── Makefile              # Build wrapper
├── include/asciinema/    # Header files
│   ├── types.h           # Core types and constants
│   ├── frame.h           # Frame data structures
│   ├── decoder.h         # Video decoder
│   └── processor.h       # Frame processor
├── src/                  # Source files
│   ├── main.cpp          # Entry point
│   ├── decoder.cpp       # Video decoder implementation
│   └── processor.cpp     # Frame processor implementation
├── tests/                # Unit tests
├── docs/                 # Documentation
├── scripts/              # Build/utility scripts
└── samples/              # Sample videos
```

## License

MIT License - see [LICENSE](LICENSE)
