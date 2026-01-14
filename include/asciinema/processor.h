#pragma once

#include "asciinema/frame.h"
#include "asciinema/types.h"

#include <opencv2/core/mat.hpp>

namespace asciinema {

enum class RenderMode { ASCII, TrueColor };

class FrameProcessor {
public:
    explicit FrameProcessor(Dimensions dims = {80, 24}, RenderMode mode = RenderMode::ASCII);

    void set_dimensions(Dimensions dims);
    [[nodiscard]] Dimensions dimensions() const;

    void set_render_mode(RenderMode mode);
    [[nodiscard]] RenderMode render_mode() const;

    [[nodiscard]] ProcessedFrame process(const RawFrame& frame);

private:
    Dimensions dims_;
    RenderMode mode_;
    cv::Mat resized_;
    cv::Mat grayscale_;
};

}  // namespace asciinema
