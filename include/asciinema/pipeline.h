#pragma once

#include "asciinema/decoder.h"
#include "asciinema/frame.h"
#include "asciinema/metrics.h"
#include "asciinema/processor.h"
#include "asciinema/queue.h"
#include "asciinema/renderer.h"

#include <atomic>
#include <string>
#include <thread>

namespace asciinema {

class Pipeline {
public:
    Pipeline(size_t decode_queue_size = 16, size_t render_queue_size = 8);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    bool start(const std::string& video_path, RenderMode mode);
    void stop();
    bool is_running() const { return running_; }

    const Metrics& metrics() const { return metrics_; }
    size_t decode_queue_depth() const { return decode_queue_.size(); }
    size_t render_queue_depth() const { return render_queue_.size(); }

private:
    void decode_loop();
    void process_loop();
    void render_loop();

    std::atomic<bool> running_{false};
    RenderMode mode_{RenderMode::ASCII};

    VideoDecoder decoder_;
    FrameProcessor processor_;
    Dimensions dims_;

    BoundedQueue<RawFrame> decode_queue_;
    BoundedQueue<ProcessedFrame> render_queue_;

    std::thread decode_thread_;
    std::thread process_thread_;
    std::thread render_thread_;

    Metrics metrics_;
};

}
