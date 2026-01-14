#pragma once

#include "asciinema/frame.h"
#include "asciinema/types.h"

#include <opencv2/videoio.hpp>
#include <optional>
#include <string>

namespace asciinema {

class VideoDecoder {
public:
    VideoDecoder() = default;
    ~VideoDecoder() = default;

    VideoDecoder(const VideoDecoder&) = delete;
    VideoDecoder& operator=(const VideoDecoder&) = delete;
    VideoDecoder(VideoDecoder&&) = default;
    VideoDecoder& operator=(VideoDecoder&&) = default;

    [[nodiscard]] bool open(const std::string& path);
    [[nodiscard]] bool is_open() const;
    void close();

    [[nodiscard]] std::optional<RawFrame> next_frame();

    [[nodiscard]] double fps() const;
    [[nodiscard]] double frame_delay_ms() const;
    [[nodiscard]] int64_t total_frames() const;
    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;
    [[nodiscard]] int64_t current_position() const;

    void seek(int64_t frame_number);
    void reset();

private:
    cv::VideoCapture capture_;
    std::string path_;
    FrameId next_frame_id_ = 0;
    double fps_ = 0.0;
    int64_t total_frames_ = 0;
    int width_ = 0;
    int height_ = 0;
};

} 
