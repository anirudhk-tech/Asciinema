# Asciinema

A high-performance, multithreaded video-to-terminal renderer built in modern C++17. Decodes video files in real-time, processes frames through a concurrent pipeline, and renders them as ASCII art or true-color output directly in your terminal.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-green.svg)
![ncurses](https://img.shields.io/badge/ncurses-supported-orange.svg)

## Features

- **Real-time video playback** at original FPS
- **Two rendering modes**: ASCII characters or 24-bit true color
- **3-thread concurrent pipeline** with lock-free frame passing
- **Configurable backpressure**: choose between frame dropping or blocking
- **Live performance metrics**: FPS, latency percentiles (p50/p95), queue depth
- **Graceful degradation**: automatic frame dropping under load

## Architecture

```mermaid
flowchart LR
    subgraph DecodeStage [Decode Thread]
        D[OpenCV VideoCapture]
    end
    
    subgraph Q1 [BoundedQueue]
        Queue1["RawFrame\ncap: 16"]
    end
    
    subgraph ProcessStage [Process Thread]
        P["Grayscale\nResize\nChar Map"]
    end
    
    subgraph Q2 [BoundedQueue]
        Queue2["ProcessedFrame\ncap: 8"]
    end
    
    subgraph RenderStage [Render Thread]
        R["ncurses\nANSI escape"]
    end
    
    D --> Queue1 --> P --> Queue2 --> R
```

### Concurrency Model

```mermaid
sequenceDiagram
    participant Dec as Decode Thread
    participant Q1 as Queue 1
    participant Proc as Process Thread
    participant Q2 as Queue 2
    participant Rend as Render Thread
    
    loop Every Frame
        Dec->>Q1: try_push(RawFrame)
        Note over Q1: mutex lock
        Q1-->>Proc: pop() blocks if empty
        Proc->>Proc: process()
        Proc->>Q2: try_push(ProcessedFrame)
        Q2-->>Rend: pop() blocks if empty
        Rend->>Rend: render to terminal
    end
```

### Thread Synchronization

```mermaid
stateDiagram-v2
    [*] --> Empty
    Empty --> HasItems: push()
    HasItems --> HasItems: push() / pop()
    HasItems --> Empty: pop() last item
    HasItems --> Full: push() to capacity
    Full --> HasItems: pop()
    
    note right of Empty: Consumer waits on not_empty
    note right of Full: Producer waits on not_full
```

### Flow Control Strategies

| Strategy | Flag | Behavior | Trade-off |
|----------|------|----------|-----------|
| **Frame Dropping** | default | `try_push()` returns immediately | Smooth playback, may skip frames |
| **Backpressure** | `-bp` | `push()` blocks until space | No frame loss, may slow down |

## Tech Stack

- **C++17** — structured bindings, `std::optional`, `if constexpr`
- **OpenCV 4.x** — video decoding, image processing
- **ncurses** — terminal UI, flicker-free rendering
- **POSIX Threads** — `std::thread`, `std::mutex`, `std::condition_variable`

## Building

### Prerequisites

```bash
# macOS
brew install cmake opencv ncurses

# Ubuntu/Debian
sudo apt install build-essential cmake libopencv-dev libncurses5-dev
```

### Compile

```bash
make build          # Standard build
make release        # Optimized build (-O3)
make debug          # Debug symbols
```

## Usage

```bash
./build/asciinema-player [OPTIONS] <video_file>
```

### Options

| Flag | Description |
|------|-------------|
| `-color` | Enable 24-bit true color rendering |
| `-bp` | Enable backpressure (block when queue full) |
| `-help` | Show usage information |

### Examples

```bash
# ASCII mode (works on any terminal)
make run VIDEO=video.mp4

# True color mode (requires modern terminal)
make run VIDEO=video.mp4 COLOR=1

# Backpressure mode (no frame drops)
make run VIDEO=video.mp4 COLOR=1 BP=1
```

## Performance Metrics

The stats bar displays real-time performance data:

```
FPS D:30 P:30 R:30 | Lat 8.2/12.5ms | Drop 0 | Frames 1847 | Q:4/16 | DROP
```

```mermaid
flowchart LR
    subgraph Metrics
        D["D:30\nDecode FPS"]
        P["P:30\nProcess FPS"]
        R["R:30\nRender FPS"]
        L["8.2/12.5ms\np50/p95 latency"]
        DR["Drop 0\nDropped frames"]
        F["Frames 1847\nTotal rendered"]
        Q["Q:4/16\nQueue depth"]
        S["DROP\nStrategy"]
    end
```

## Project Structure

```mermaid
flowchart TB
    subgraph Headers ["include/asciinema/"]
        types.h --> frame.h
        frame.h --> decoder.h
        frame.h --> processor.h
        processor.h --> renderer.h
        frame.h --> queue.h
        queue.h --> pipeline.h
        metrics.h --> pipeline.h
        decoder.h --> pipeline.h
        processor.h --> pipeline.h
        renderer.h --> pipeline.h
    end
    
    subgraph Sources ["src/"]
        main.cpp --> pipeline.cpp
        decoder.cpp
        processor.cpp
        renderer.cpp
    end
```

```
terminal-vision/
├── include/asciinema/
│   ├── types.h         # Core types, time utilities
│   ├── frame.h         # RawFrame, ProcessedFrame
│   ├── decoder.h       # VideoDecoder (OpenCV wrapper)
│   ├── processor.h     # FrameProcessor (image → chars)
│   ├── renderer.h      # TerminalRenderer (ncurses)
│   ├── queue.h         # BoundedQueue<T> (thread-safe)
│   ├── pipeline.h      # Pipeline orchestrator
│   └── metrics.h       # FPS counter, latency tracker
├── src/
│   ├── main.cpp        # Entry point, CLI parsing
│   ├── decoder.cpp
│   ├── processor.cpp
│   ├── renderer.cpp
│   └── pipeline.cpp
├── CMakeLists.txt
├── Makefile
└── README.md
```

## Key Implementation Details

### Thread-Safe Bounded Queue

```cpp
template <typename T>
class BoundedQueue {
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_empty_, not_full_;
    
    void push(T item) {
        std::unique_lock lock(mutex_);
        not_full_.wait(lock, [&] { return queue_.size() < capacity_; });
        queue_.push(std::move(item));
        not_empty_.notify_one();
    }
};
```

### Move Semantics for Zero-Copy Frame Transfer

```cpp
RawFrame(FrameId id, TimePoint ts, cv::Mat img)
    : id(id), timestamp(ts), image(std::move(img)) {}
```

### Latency Tracking with Percentiles

```cpp
double p95() const {
    std::vector<double> sorted(samples_.begin(), samples_.end());
    std::sort(sorted.begin(), sorted.end());
    return sorted[sorted.size() * 95 / 100];
}
```

## What I Learned

- **Concurrent programming**: mutexes, condition variables, atomics
- **Producer-consumer patterns**: bounded queues, backpressure strategies
- **Modern C++17**: move semantics, RAII, `std::optional`, type aliases
- **Real-time systems**: frame timing, latency measurement, graceful degradation
- **Terminal graphics**: ANSI escape codes, ncurses double-buffering

## License

MIT License
