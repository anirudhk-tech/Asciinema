#include "asciinema/processor.h"

#include <opencv2/imgproc.hpp>
#include <sstream>

namespace asciinema {

FrameProcessor::FrameProcessor(Dimensions dims, RenderMode mode)
    : dims_(dims), mode_(mode) {}

void FrameProcessor::set_dimensions(Dimensions dims) { dims_ = dims; }
Dimensions FrameProcessor::dimensions() const { return dims_; }

void FrameProcessor::set_render_mode(RenderMode mode) { mode_ = mode; }
RenderMode FrameProcessor::render_mode() const { return mode_; }

ProcessedFrame FrameProcessor::process(const RawFrame& frame) {
    cv::resize(frame.image, resized_, cv::Size(dims_.cols, dims_.rows));

    std::ostringstream oss;

    if (mode_ == RenderMode::TrueColor) {
        // Full RGB color output
        cv::Mat rgb;
        if (resized_.channels() == 4)
            cv::cvtColor(resized_, rgb, cv::COLOR_BGRA2BGR);
        else
            rgb = resized_;

        for (int y = 0; y < rgb.rows; ++y) {
            const uint8_t* row = rgb.ptr<uint8_t>(y);
            for (int x = 0; x < rgb.cols; ++x) {
                int b = row[x * 3];
                int g = row[x * 3 + 1];
                int r = row[x * 3 + 2];
                oss << "\033[48;2;" << r << ";" << g << ";" << b << "m \033[0m";
            }
            oss << '\n';
        }
    } else {
        // ASCII grayscale
        if (resized_.channels() == 3)
            cv::cvtColor(resized_, grayscale_, cv::COLOR_BGR2GRAY);
        else if (resized_.channels() == 4)
            cv::cvtColor(resized_, grayscale_, cv::COLOR_BGRA2GRAY);
        else
            grayscale_ = resized_;

        for (int y = 0; y < grayscale_.rows; ++y) {
            const uint8_t* row = grayscale_.ptr<uint8_t>(y);
            for (int x = 0; x < grayscale_.cols; ++x)
                oss << pixel_to_char(row[x]);
            oss << '\n';
        }
    }

    return ProcessedFrame(frame.id, frame.timestamp, oss.str(), dims_);
}

} 
