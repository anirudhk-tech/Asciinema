#include "asciinema/renderer.h"

namespace asciinema {

    TerminalRenderer::TerminalRenderer() {
        win_ = initscr();
        cbreak();
        noecho();
        curs_set(0);
        nodelay(win_, TRUE);
        keypad(win_, TRUE);
        getmaxyx(win_, rows_, cols_);
    }

    TerminalRenderer::~TerminalRenderer() {
        endwin();
    }

    Dimensions TerminalRenderer::dimensions() const {
        return {cols_, rows_ - 2}; 
    }

    void TerminalRenderer::render(const ProcessedFrame& frame, RenderMode) {
        move(0, 0);
        const char* ptr = frame.char_grid.c_str();
        int y = 0, x = 0;
        while (*ptr) {
            if (*ptr == '\n') {
                y++;
                x = 0;
            } else {
                mvaddch(y, x++, *ptr);
            }
            ptr++;
        }
    }

    void TerminalRenderer::render_stats(const std::string& stats) {
        move(rows_ - 1, 0);
        clrtoeol();
        attron(A_REVERSE);
        printw(" %s ", stats.c_str());
        attroff(A_REVERSE);
    }

    void TerminalRenderer::clear() {
        erase();
    }

    void TerminalRenderer::refresh() {
        wnoutrefresh(win_);
        doupdate();
    }

} 
