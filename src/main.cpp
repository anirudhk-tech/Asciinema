#include "asciinema/decoder.h"
#include "asciinema/processor.h"
#include "asciinema/renderer.h"
#include "asciinema/types.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <csignal>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static volatile bool running = true;

void signal_handler(int) { running = false; }

asciinema::Dimensions get_terminal_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_col, static_cast<int>(w.ws_row - 2)};
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

    VideoDecoder decoder;
    if (!decoder.open(video_path)) {
        std::cerr << "Error: Could not open " << video_path << "\n";
        return 1;
    }

    signal(SIGINT, signal_handler);

    RenderMode mode = use_color ? RenderMode::TrueColor : RenderMode::ASCII;
    const char* mode_str = use_color ? "TrueColor" : "ASCII";

    if (use_color) {
        Dimensions dims = get_terminal_size();
        FrameProcessor processor(dims, mode);

        std::cout << "\033[?25l\033[?1049h";

        while (running) {
            auto frame = decoder.next_frame();
            if (!frame) break;

            auto processed = processor.process(*frame);

            std::cout << "\033[H" << processed.char_grid << "\033[0m";
            
            std::cout << "\033[" << (dims.rows + 1) << ";1H\033[7m"
                      << " Frame " << processed.id << "/" << decoder.total_frames()
                      << " | " << processed.latency_ms() << "ms"
                      << " | " << mode_str << " | Ctrl+C=quit \033[0m" << std::flush;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(decoder.frame_delay_ms())));
        }

        // Restore terminal
        std::cout << "\033[?1049l\033[?25h" << std::flush;

    } else {
        // ASCII mode: use ncurses
        TerminalRenderer renderer;
        Dimensions dims = renderer.dimensions();
        FrameProcessor processor(dims, mode);

        while (running) {
            auto frame = decoder.next_frame();
            if (!frame) break;

            auto processed = processor.process(*frame);

            renderer.clear();
            renderer.render(processed, mode);

            std::ostringstream stats;
            stats << "Frame " << processed.id << "/" << decoder.total_frames()
                  << " | " << processed.latency_ms() << "ms"
                  << " | " << mode_str << " | q=quit";
            renderer.render_stats(stats.str());
            renderer.refresh();

            int ch = getch();
            if (ch == 'q' || ch == 'Q') break;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(decoder.frame_delay_ms())));
        }
    }

    return 0;
}
