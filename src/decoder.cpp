#include "asciinema/decoder.h"

#include <opencv2/videoio.hpp>

namespace asciinema {

bool VideoDecoder::open(const std::string& path) {
    close();

    if (!capture_.open(path)) return false;

    path_ = path;
    fps_ = capture_.get(cv::CAP_PROP_FPS);
    total_frames_ = static_cast<int64_t>(capture_.get(cv::CAP_PROP_FRAME_COUNT));
    width_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    height_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));

    if (fps_ <= 0.0) fps_ = 30.0;

    return true;
}

bool VideoDecoder::is_open() const { return capture_.isOpened(); }

void VideoDecoder::close() {
    if (capture_.isOpened()) capture_.release();
    path_.clear();
    next_frame_id_ = 0;
    fps_ = 0.0;
    total_frames_ = 0;
    width_ = 0;
    height_ = 0;
}

std::optional<RawFrame> VideoDecoder::next_frame() {
    if (!capture_.isOpened()) return std::nullopt;

    cv::Mat image;
    capture_ >> image;

    if (image.empty()) return std::nullopt;

    return RawFrame(next_frame_id_++, now(), std::move(image));
}

double VideoDecoder::fps() const { return fps_; }

double VideoDecoder::frame_delay_ms() const {
    return fps_ > 0.0 ? 1000.0 / fps_ : 33.33;
}

int64_t VideoDecoder::total_frames() const { return total_frames_; }
int VideoDecoder::width() const { return width_; }
int VideoDecoder::height() const { return height_; }

int64_t VideoDecoder::current_position() const {
    return capture_.isOpened()
        ? static_cast<int64_t>(capture_.get(cv::CAP_PROP_POS_FRAMES))
        : 0;
}

void VideoDecoder::seek(int64_t frame_number) {
    if (capture_.isOpened())
        capture_.set(cv::CAP_PROP_POS_FRAMES, static_cast<double>(frame_number));
}

void VideoDecoder::reset() {
    seek(0);
    next_frame_id_ = 0;
}

}  
