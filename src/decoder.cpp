#include "asciinema/decoder.h"

#include <opencv2/videoio.hpp>

namespace asciinema {
    bool VideoDecoder::open(const std::string& path) {
        close();

        if (!capture_.open(path)) {
            return false;
        }

        path_ = path;
        fps_ = capture_.get(cv::CAP_PROP_FPS);
        total_frames_ = static_cast<int64_t>(capture_.get(cv::CAP_PROP_FRAME_COUNT));
        width_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
        height_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));

        if (fps_ <= 0.0) {
            fps_ = 30.0;
        }

        return true;
    }

    bool VideoDecoder::is_open() const {
        return capture_.isOpened();
    }

    void VideoDecoder::close() {
        if (capture_.isOpened()) {
            capture_.release();
        }
        path_.clear();
        fps_ = 0.0;
        total_frames_ = 0;
        width_ = 0;
        height_ = 0;
        next_frame_id_ = 0;
    }

    std::optional<RawFrame> VideoDecoder::next_frame() {
        if (!capture_.isOpened()) {
            return std::nullopt;
        }

        cv::Mat image;

        capture_ >> image;

        if (image.empty()) {
            return std::nullopt;
        }

        RawFrame frame(
            next_frame_id_++,
            now(),
            std::move(image)
        );

        return frame;
    }

    double VideoDecoder::fps() const {
        return fps_;
    }

    double VideoDecoder::frame_delay_ms() const {
        if (fps_ <= 0.0) return 33.33;
        return 1000.0 / fps_;
    }

    int64_t VideoDecoder::total_frames() const {
        return total_frames_;
    }

    int VideoDecoder::width() const {
        return width_;
    }

    int VideoDecoder::height() const {
        return height_;
    }

}