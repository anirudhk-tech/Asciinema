#include "asciinema/decoder.h"
#include "asciinema/processor.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [OPTIONS] <video>\n\n"
              << "Options:\n"
              << "  -color    True color (24-bit) rendering\n"
              << "  -help     Show this message\n";
}

int main(int argc, char* argv[]) {
    using namespace asciinema;

    bool use_color = false;
    std::string video_path;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-color") == 0)
            use_color = true;
        else if (std::strcmp(argv[i], "-help") == 0 || std::strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-') {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            print_usage(argv[0]);
            return 1;
        } else {
            video_path = argv[i];
        }
    }

    if (video_path.empty()) {
        std::cerr << "Error: No video file specified\n\n";
        print_usage(argv[0]);
        return 1;
    }

    VideoDecoder decoder;
    if (!decoder.open(video_path)) {
        std::cerr << "Error: Could not open " << video_path << "\n";
        return 1;
    }

    RenderMode mode = use_color ? RenderMode::TrueColor : RenderMode::ASCII;
    FrameProcessor processor({160, 45}, mode);

    const char* mode_str = use_color ? "TrueColor" : "ASCII";
    std::cout << decoder.width() << "x" << decoder.height()
              << " @ " << decoder.fps() << " FPS [" << mode_str << "]\n\n";

    while (auto frame = decoder.next_frame()) {
        auto processed = processor.process(*frame);

        std::cout << "\033[0m\033[2J\033[H" << processed.char_grid << "\033[0m"
                  << "\nFrame " << processed.id << " | " << processed.latency_ms() << "ms\n";

        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(decoder.frame_delay_ms())));
    }

    return 0;
}
