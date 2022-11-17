#ifndef TEST_HPP
#define TEST_HPP

#include <string>
#include <functional>
#include <map>
#include <unistd.h>
#include <termios.h>
#include <iostream>

using namespace std;

static map<string, function<void()>> tests;
static int passed = 0;

static int nb_tests = 0;
static int nb_passed = 0;

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

namespace color {
    const string black = "\033[30m";
    const string red = "\033[31m";
    const string green = "\033[32m";
    const string yellow = "\033[33m";
    const string blue = "\033[34m";
    const string magenta = "\033[35m";
    const string cyan = "\033[36m";
    const string white = "\033[37m";
    const string grey = "\033[90m";

    const string bg_black = "\033[40m";
    const string bg_red = "\033[41m";
    const string bg_green = "\033[42m";
    const string bg_yellow = "\033[43m";
    const string bg_blue = "\033[44m";
    const string bg_magenta = "\033[45m";
    const string bg_cyan = "\033[46m";
    const string bg_white = "\033[47m";
    const string bg_grey = "\033[100m";

    const string reset = "\033[0m";
    const string bold = "\033[1m";
    const string underline = "\033[4m";
    const string blink = "\033[5m";
    const string inverse = "\033[7m";
}

namespace utils {
    inline string move_up(uint n = 1) {
        return "\033[" + to_string(n) + "A";
    }

    inline string move_down(uint n = 1) {
        return "\033[" + to_string(n) + "B";
    }

    inline string move_left(uint n = 1) {
        return "\033[" + to_string(n) + "D";
    }

    inline string move_right(uint n = 1) {
        return "\033[" + to_string(n) + "C";
    }

    typedef enum {
        EOL = 0,
        BOL = 1,
        LINE = 2
    } clear_mode;

    inline string clear_line(clear_mode mode) {
        return "\033[" + to_string(mode) + "K";
    }

    inline string hide_cursor() {
        return "\033[?25l";
    }

    inline string show_cursor() {
        return "\033[?25h";
    }

    inline void enable_raw_mode() {
        struct termios term{};
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
    }

    inline void disable_raw_mode() {
        struct termios term{};
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= (ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
    }
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

bool run_tests() {
    utils::enable_raw_mode();
    cout << utils::hide_cursor();
    cout << "Run " << tests.size() << " tests" << endl;
    passed = 0;

    for (const auto &test: tests) {
        cout << color::bg_blue << " " << test.first << " " << color::reset << endl;

        // Init
        nb_tests = 0;
        nb_passed = 0;

        // Run
        test.second();

        // Resume
        if (nb_tests == nb_passed) {
            cout << color::bg_green << " " << nb_passed << "/" << nb_tests << " passed " << color::reset << endl;
            passed++;
        } else {
            cout << color::bg_red << " " << nb_passed << "/" << nb_tests << " passed " << color::reset << endl;
        }
    }

    // Resume
    cout << endl;
    bool result = passed == tests.size();
    if (result) {
        cout << color::bg_green << " " << passed << "/" << tests.size() << " passed " << color::reset << endl;
    } else {
        cout << color::bg_red << " " << passed << "/" << tests.size() << " passed " << color::reset << endl;
    }
    cout << utils::show_cursor();
    utils::disable_raw_mode();

    return result;
}

#define describe(name, test) \
    tests[name] = test;

#define it(name, test)                                                \
    cout << color::cyan << "  " << name << color::reset;              \
    cout << utils::move_left(1000);                                   \
    nb_tests++                                                        \
    if (test()) {                                                     \
        cout << color::green << "  " << name << color::reset << endl; \
        nb_passed++;                                                  \
    } else {                                                          \
        cout << color::red << "  " << name << color::reset << endl;   \
    }

#endif // TEST_HPP
