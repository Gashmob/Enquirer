/**
 * MIT License
 *
 * Copyright (c) 2023 Kevin Traini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ENQUIRER_HPP
#define ENQUIRER_HPP

#define ENQUIRER_VERSION "1.0.2"

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

namespace enquirer {

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Utilities

    namespace color {
        const std::string black   = "\033[30m";
        const std::string red     = "\033[31m";
        const std::string green   = "\033[32m";
        const std::string yellow  = "\033[33m";
        const std::string blue    = "\033[34m";
        const std::string magenta = "\033[35m";
        const std::string cyan    = "\033[36m";
        const std::string white   = "\033[37m";
        const std::string grey    = "\033[90m";

        const std::string reset     = "\033[0m";
        const std::string bold      = "\033[1m";
        const std::string underline = "\033[4m";
        const std::string blink     = "\033[5m";
        const std::string inverse   = "\033[7m";
    }// namespace color

    namespace utils {
        auto move_up(unsigned int n = 1) -> std::string;

        auto move_left(unsigned int n = 1) -> std::string;

        typedef enum {
            EOL  = 0,
            BOL  = 1,
            LINE = 2
        } clear_mode;

        auto clear_line(clear_mode mode) -> std::string;

        auto hide_cursor() -> std::string;

        auto show_cursor() -> std::string;

        auto print_question(const std::string &question,
                            const std::string &symbol = color::cyan + color::bold + "? ",
                            const std::string &input  = color::grey + color::bold + "› ") -> void;

        auto print_answer(const std::string &question) -> void;

        auto enable_raw_mode() -> void;

        auto disable_raw_mode() -> void;
    }// namespace utils

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Auth

    auto auth(const std::string &id_prompt = "Username",
              const std::string &pw_prompt = "Password",
              char mask                    = '*') -> std::pair<std::string, std::string>;

    auto auth(const std::function<bool(const std::pair<std::string, std::string> &)> &predicate,
              const std::string &id_prompt = "Username",
              const std::string &pw_prompt = "Password",
              char mask                    = '*') -> bool;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Autocomplete

    auto autocomplete(const std::string &question,
                      const std::vector<std::string> &choices = {},
                      unsigned int limit                      = 10) -> std::string;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Confirm

    auto confirm(const std::string &question,
                 bool default_value = false) -> bool;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Form

    auto form(const std::string &question,
              const std::vector<std::string> &inputs) -> std::map<std::string, std::string>;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Input

    auto input(const std::string &question,
               const std::string &default_value = "") -> std::string;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Invisible

    auto invisible(const std::string &question) -> std::string;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // List

    auto list(const std::string &question) -> std::vector<std::string>;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // MultiSelect

    auto multi_select(const std::string &question,
                      const std::vector<std::string> &choices) -> std::vector<std::string>;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Number

    template<typename N,
             typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
    auto number(const std::string &question) -> N {
        // Print question
        utils::print_question(question);

        // Get answer
        std::string answer;
        char current;
        utils::enable_raw_mode();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    std::cout << std::endl;
                    break;
                } else if (current == 127) {// Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                        std::cout << utils::move_left(1);
                        std::cout << utils::clear_line(utils::EOL);
                    }
                } else if (current == 27) {// Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else if (isdigit(current) ||
                       (current == '.' && answer.find('.') == std::string::npos) ||
                       ((current == '+' || current == '-') && answer.empty())) {// 'Normal' character
                answer += current;
                std::cout << current;
            }
        }
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_up()
                  << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << color::cyan << answer << color::reset << std::endl;

        // Convert answer to number type N
        std::stringstream ss(answer);
        N number;
        ss >> number;

        return number;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Password

    auto password(const std::string &question,
                  char mask = '*') -> std::string;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Quiz

    auto quiz(const std::string &question,
              const std::vector<std::string> &choices,
              const std::string &correct) -> bool;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Slider

    template<typename N,
             typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
    auto slider(const std::string &question,
                N min_value,
                N max_value,
                N step,
                N initial_value) -> N {
        // Print question
        utils::print_question(question);

        struct winsize w {};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        const unsigned int width = std::min((unsigned int) ((max_value - min_value) / step),
                                            (unsigned int) w.ws_col - 7);// 7 is for < > # and 2 spaces each side
        const N swidth           = (max_value - min_value) / ((N) width);

        // Print value
        N value = initial_value;
        std::cout << std::endl
                  << "   "
                  << std::string((width / 2) - (std::to_string(value).length() / 2), ' ')
                  << color::bold << value << color::reset
                  << std::endl;

        // Print slider
        std::cout << "  " << color::cyan << color::bold << "<" << color::reset;
        for (N i = 0; i <= (N) width; i++) {
            N l = min_value + (i * swidth);
            N r = min_value + ((i + 1) * swidth);
            if (value >= l && value < r) {
                std::cout << color::cyan << color::bold << "#" << color::reset;
            } else {
                std::cout << color::grey << "-" << color::reset;
            }
        }
        std::cout << color::cyan << color::bold << ">" << color::reset;

        // Get answer
        char current;
        utils::enable_raw_mode();
        std::cout << utils::hide_cursor();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    break;
                } else if (current == 27) {// Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 67) {// Right
                            value = std::min(value + step, max_value);
                        } else if (current == 68) {// Left
                            value = std::max(value - step, min_value);
                        }
                    }
                }
            }

            // Redraw slider
            std::cout << utils::clear_line(utils::LINE)
                      << utils::move_up() << utils::clear_line(utils::LINE)
                      << utils::move_left(1000);
            std::cout << "   "
                      << std::string((width / 2) - (std::to_string(value).length() / 2), ' ')
                      << color::bold << value << color::reset << std::endl;
            std::cout << "  "
                      << color::cyan << color::bold << "<" << color::reset;
            for (unsigned int i = 0; i <= width; i++) {
                N l = min_value + (i * swidth);
                N r = min_value + ((i + 1) * swidth);
                if (value >= l && value < r) {
                    std::cout << color::cyan << color::bold << "#" << color::reset;
                } else {
                    std::cout << color::grey << "-" << color::reset;
                }
            }
            std::cout << color::cyan << color::bold << ">" << color::reset;
        }
        std::cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::clear_line(utils::LINE)
                  << utils::move_up() << utils::clear_line(utils::LINE)
                  << utils::move_up() << utils::clear_line(utils::LINE)
                  << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << color::cyan << value << color::reset << std::endl;

        return value;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Select

    auto select(const std::string &question,
                const std::vector<std::string> &choices) -> std::string;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Toggle

    auto toggle(const std::string &question,
                const std::string &enable,
                const std::string &disable,
                bool default_value = false) -> bool;
}// namespace enquirer

#endif//ENQUIRER_HPP
