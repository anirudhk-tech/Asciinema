#pragma once

#include "asciinema/frame.h"
#include "asciinema/processor.h"

#include <ncurses.h>
#include <string>

namespace asciinema {

    class TerminalRenderer {
        public:
            TerminalRenderer();
            ~TerminalRenderer();

            TerminalRenderer(const TerminalRenderer&) = delete;
            TerminalRenderer& operator=(const TerminalRenderer&) = delete;

            [[nodiscard]] Dimensions dimensions() const;

            void render(const ProcessedFrame& frame, RenderMode mode);
            void render_stats(const std::string& stats);
            void clear();
            void refresh();

        private:
            WINDOW* win_;
            int rows_;
            int cols_;
    };

} 
