#include "asciinema/pipeline.h"

#include <csignal>
#include <cstring>
#include <iostream>

static asciinema::Pipeline* g_pipeline = nullptr;

void signal_handler(int) {
    if (g_pipeline) g_pipeline->stop();
}

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

    Pipeline pipeline;
    g_pipeline = &pipeline;

    signal(SIGINT, signal_handler);

    RenderMode mode = use_color ? RenderMode::TrueColor : RenderMode::ASCII;

    if (!pipeline.start(video_path, mode)) {
        std::cerr << "Error: Could not start pipeline for " << video_path << "\n";
        return 1;
    }

    while (pipeline.is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
