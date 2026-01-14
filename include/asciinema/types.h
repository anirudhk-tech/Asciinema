#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace asciinema {

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::nanoseconds;

inline TimePoint now() { return Clock::now(); }

inline double to_ms(Duration d) {
    return std::chrono::duration<double, std::milli>(d).count();
}

using FrameId = uint64_t;

struct Dimensions {
    int cols;
    int rows;
    [[nodiscard]] int area() const { return cols * rows; }
};

// Ordered dark → light
constexpr char ASCII_RAMP[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
constexpr size_t ASCII_RAMP_SIZE = sizeof(ASCII_RAMP) - 1;

inline char pixel_to_char(uint8_t intensity) {
    return ASCII_RAMP[(intensity * (ASCII_RAMP_SIZE - 1)) / 255];
}

}  // namespace asciinema
