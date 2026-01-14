#include "asciinema/pipeline.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace asciinema {

namespace {
    Dimensions get_terminal_size() {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return {w.ws_col, static_cast<int>(w.ws_row - 2)};
    }
}

Pipeline::Pipeline(size_t decode_queue_size, size_t render_queue_size)
    : decode_queue_(decode_queue_size)
    , render_queue_(render_queue_size)
{}

Pipeline::~Pipeline() {
    stop();
}

bool Pipeline::start(const std::string& video_path, RenderMode mode) {
    if (running_) return false;

    if (!decoder_.open(video_path)) return false;

    mode_ = mode;
    dims_ = get_terminal_size();
    processor_ = FrameProcessor(dims_, mode_);
    running_ = true;

    decode_thread_ = std::thread(&Pipeline::decode_loop, this);
    process_thread_ = std::thread(&Pipeline::process_loop, this);
    render_thread_ = std::thread(&Pipeline::render_loop, this);

    return true;
}

void Pipeline::stop() {
    if (!running_) return;

    running_ = false;
    decode_queue_.stop();
    render_queue_.stop();

    if (decode_thread_.joinable()) decode_thread_.join();
    if (process_thread_.joinable()) process_thread_.join();
    if (render_thread_.joinable()) render_thread_.join();
}

void Pipeline::decode_loop() {
    double frame_delay = decoder_.frame_delay_ms();
    auto next_frame_time = std::chrono::steady_clock::now();

    while (running_) {
        auto frame = decoder_.next_frame();
        if (!frame) {
            decoder_.reset();
            continue;
        }

        if (!decode_queue_.try_push(std::move(*frame))) {
            stats_.frames_dropped++;
        } else {
            stats_.frames_decoded++;
        }

        next_frame_time += std::chrono::milliseconds(static_cast<int>(frame_delay));
        std::this_thread::sleep_until(next_frame_time);
    }
}

void Pipeline::process_loop() {
    while (running_) {
        RawFrame raw = decode_queue_.pop();
        if (!raw.valid()) continue;

        ProcessedFrame processed = processor_.process(raw);

        if (!render_queue_.try_push(std::move(processed))) {
            stats_.frames_dropped++;
        } else {
            stats_.frames_processed++;
        }
    }
}

void Pipeline::render_loop() {
    if (mode_ == RenderMode::TrueColor) {
        std::cout << "\033[?25l\033[?1049h";
    }

    TerminalRenderer* renderer = nullptr;
    if (mode_ == RenderMode::ASCII) {
        renderer = new TerminalRenderer();
    }

    while (running_) {
        ProcessedFrame frame = render_queue_.pop();
        if (!frame.valid()) continue;

        stats_.frames_rendered++;

        std::ostringstream stats_str;
        stats_str << "D:" << stats_.frames_decoded 
                  << " P:" << stats_.frames_processed
                  << " R:" << stats_.frames_rendered
                  << " Drop:" << stats_.frames_dropped
                  << " Q:" << decode_queue_.size() << "/" << decode_queue_.capacity()
                  << " | " << frame.latency_ms() << "ms"
                  << " | q=quit";

        if (mode_ == RenderMode::TrueColor) {
            std::cout << "\033[H" << frame.char_grid << "\033[0m";
            std::cout << "\033[" << (dims_.rows + 1) << ";1H\033[7m " 
                      << stats_str.str() << " \033[0m" << std::flush;
        } else {
            renderer->clear();
            renderer->render(frame, mode_);
            renderer->render_stats(stats_str.str());
            renderer->refresh();

            int ch = getch();
            if (ch == 'q' || ch == 'Q') {
                running_ = false;
                break;
            }
        }
    }

    if (mode_ == RenderMode::TrueColor) {
        std::cout << "\033[?1049l\033[?25h" << std::flush;
    }

    delete renderer;
}

}
