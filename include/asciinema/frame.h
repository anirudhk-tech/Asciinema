#pragma once

#include "asciinema/types.h"

#include <opencv2/core/mat.hpp>
#include <string>
#include <utility>

namespace asciinema {

    struct RawFrame {
        FrameId id;
        TimePoint timestamp;
        cv::Mat image;

        RawFrame() : id(0), timestamp{}, image{} {}

        RawFrame(FrameId frame_id, TimePoint ts, cv::Mat img)
            : id(frame_id)
            , timestamp(ts)
            , image(std::move(img))
        {}

        [[nodiscard]] bool valid() const {
            return !image.empty();
        }

        RawFrame(RawFrame&&) = default;
        RawFrame& operator=(RawFrame&&) = default;

        RawFrame(const RawFrame&) = default;
        RawFrame& operator=(const RawFrame&) = default;
    };

    struct ProcessedFrame {
        FrameId id;
        TimePoint timestamp;
        std::string char_grid;
        Dimensions dimensions;

        ProcessedFrame() : id(0), timestamp{}, char_grid{}, dimensions{} {}

        ProcessedFrame(FrameId frame_id, TimePoint ts, std::string grid, Dimensions dims)
            : id(frame_id)
            , timestamp(ts)
            , char_grid(std::move(grid))
            , dimensions(dims)
        {}

        [[nodiscard]] bool valid() const {
            return !char_grid.empty();
        }

        [[nodiscard]] Duration latency() const {
            return now() - timestamp;
        }

        [[nodiscard]] double latency_ms() const {
            return to_ms(latency());
        }

        ProcessedFrame(ProcessedFrame&&) = default;
        ProcessedFrame& operator=(ProcessedFrame&&) = default;

        ProcessedFrame(const ProcessedFrame&) = default;
        ProcessedFrame& operator=(const ProcessedFrame&) = default;
    };
}