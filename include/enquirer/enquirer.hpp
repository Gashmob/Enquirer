/**
 *     === Enquirer ===
 * Created by Kevin Traini
 *      Under GPL-3.0
 * -----------------------
 */
#ifndef ENQUIRER_HPP
#define ENQUIRER_HPP

#define ENQUIRER_VERSION "0.1.0"

#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <sstream>

namespace enquirer {

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Utilities

    namespace color {
        const std::string black = "\033[30m";
        const std::string red = "\033[31m";
        const std::string green = "\033[32m";
        const std::string yellow = "\033[33m";
        const std::string blue = "\033[34m";
        const std::string magenta = "\033[35m";
        const std::string cyan = "\033[36m";
        const std::string white = "\033[37m";
        const std::string grey = "\033[90m";

        const std::string reset = "\033[0m";
        const std::string bold = "\033[1m";
        const std::string underline = "\033[4m";
        const std::string blink = "\033[5m";
        const std::string inverse = "\033[7m";
    }

    namespace utils {
        /**
         * Move the cursor n times upward
         */
        inline std::string move_up(uint n = 1) {
            return "\033[" + std::to_string(n) + "A";
        }

        /**
         * Move the cursor n times downward
         */
        inline std::string move_down(uint n = 1) {
            return "\033[" + std::to_string(n) + "B";
        }

        /**
         * Move the cursor n times to the left
         */
        inline std::string move_left(uint n = 1) {
            return "\033[" + std::to_string(n) + "D";
        }

        /**
         * Move the cursor n times to the right
         */
        inline std::string move_right(uint n = 1) {
            return "\033[" + std::to_string(n) + "C";
        }

        typedef enum {
            EOL = 0,
            BOL = 1,
            LINE = 2
        } clear_mode;

        /**
         * Clear entire line
         */
        inline std::string clear_line(clear_mode mode) {
            return "\033[" + std::to_string(mode) + "K";
        }

        /**
         * Hide the cursor
         */
        inline std::string hide_cursor() {
            return "\033[?25l";
        }

        /**
         * Show the cursor
         */
        inline std::string show_cursor() {
            return "\033[?25h";
        }

        /**
         * Clear line then print the question
         */
        inline void print_question(const std::string &question,
                                   const std::string &symbol = color::cyan + "? ",
                                   const std::string &input = color::grey + "› ") {
            std::cout << clear_line(LINE);
            std::cout << symbol
                      << color::reset << question
                      << " " << input
                      << color::reset;
        }

        /**
         * Print the question as it's answered
         */
        inline void print_answer(const std::string &question) {
            print_question(question, color::green + "✔ ", color::grey + "· ");
        }

        inline void enable_raw_mode() {
            struct termios term;
            tcgetattr(STDIN_FILENO, &term);
            term.c_lflag &= ~(ECHO | ICANON);
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
        }

        inline void disable_raw_mode() {
            struct termios term;
            tcgetattr(STDIN_FILENO, &term);
            term.c_lflag |= (ECHO | ICANON);
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
        }

        inline std::string ltrim(const std::string &str) {
            size_t start = str.find_first_not_of(' ');
            return (start == std::string::npos) ? "" : str.substr(start);
        }

        inline std::string rtrim(const std::string &str) {
            size_t end = str.find_last_not_of(' ');
            return (end == std::string::npos) ? "" : str.substr(0, end + 1);
        }

        /**
         * Trim whitespace from both sides of a string
         */
        inline std::string trim(const std::string &str) {
            return rtrim(ltrim(str));
        }

        /**
         * Split a string around a delimiter
         */
        inline std::vector<std::string> split(const std::string &str, const char delim) {
            std::vector<std::string> result;
            std::stringstream ss(str);
            std::string item;
            while (std::getline(ss, item, delim)) {
                result.push_back(trim(item));
            }

            return result;
        }
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Auth

    std::pair<std::string, std::string> auth(const std::string &id_prompt = "Username",
                                             const std::string &pw_prompt = "Password") {
        return {}; // TODO
    }

    bool auth(std::function<bool(std::pair<std::string, std::string>)> &predicate,
              const std::string &id_prompt = "Username",
              const std::string &pw_prompt = "Password") {
        return false; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Autocomplete

    std::string autocomplete(const std::string &question,
                             const std::string choices[] = {},
                             int limit = 10) {
        return ""; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Confirm

    bool confirm(const std::string &question,
                 bool default_value = false) {
        // Print question
        utils::print_question(question);

        // Print choices
        bool confirmed = default_value;
        std::cout << (confirmed ? "Yes" : "No");

        // Get answer
        char current;
        utils::enable_raw_mode();
        std::cout << utils::hide_cursor();
        while (std::cin.get(current)) {
            bool previous = confirmed;
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    break;
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 68) { // Left
                            confirmed = true;
                        } else if (current == 67) { // Right
                            confirmed = false;
                        }
                    }
                }
            }

            // Redraw choices
            std::cout << utils::move_left(previous ? 3 : 2)
                      << utils::clear_line(utils::EOL)
                      << (confirmed ? "Yes" : "No");
        }
        std::cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << (confirmed ? color::green : color::red) << (confirmed ? "Yes" : "No") << color::reset << std::endl;

        return confirmed;
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Form

    std::map<std::string, std::string> form(const std::string &question,
                                            const std::string inputs[]) {
        return {}; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Input

    std::string input(const std::string &question,
                      const std::string &default_value = "") {
        // Print question
        utils::print_question(question);

        // Print default value
        std::cout << color::grey << default_value << color::reset;
        std::cout << utils::move_left(default_value.size());

        // Get answer
        std::string answer;
        char current;
        utils::enable_raw_mode();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    std::cout << std::endl;
                    break;
                } else if (current == 127) { // Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                        std::cout << utils::move_left(1);
                        std::cout << utils::clear_line(utils::EOL);
                    }
                } else if (current == 9) { // Tab
                    if (answer == default_value.substr(0, answer.length())) {
                        std::cout << default_value.substr(answer.length());
                        answer = default_value;
                    }
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else { // 'Normal' character
                answer += current;
                std::cout << current;
            }

            // Check default_value
            if (answer == default_value.substr(0, answer.length())) {
                std::cout << color::grey << default_value.substr(answer.length()) << color::reset;
                if (answer != default_value) {
                    std::cout << utils::move_left(default_value.size() - answer.size());
                }
            } else if (answer != default_value) {
                std::cout << utils::clear_line(utils::EOL);
            }
        }
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_up()
                  << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << color::cyan << answer << color::reset << std::endl;

        return answer;
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Invisible

    std::string invisible(const std::string &question) {
        // Print question
        utils::print_question(question);

        // Get answer
        std::string answer;
        char current;
        utils::enable_raw_mode();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    std::cout << std::endl;
                    break;
                } else if (current == 127) { // Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                        std::cout << utils::move_left(1);
                        std::cout << utils::clear_line(utils::EOL);
                    }
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else { // 'Normal' character
                answer += current;
            }
        }
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_up()
                  << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << std::endl;

        return answer;
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// List

    std::vector<std::string> list(const std::string &question) {
        // Print question
        utils::print_question(question);

        // Get answer
        std::string answer;
        char current;
        utils::enable_raw_mode();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    std::cout << std::endl;
                    break;
                } else if (current == 127) { // Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                        std::cout << utils::move_left(1);
                        std::cout << utils::clear_line(utils::EOL);
                    }
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else { // 'Normal' character
                answer += current;
                std::cout << current;
            }
        }
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_up()
                  << utils::move_left(1000);
        utils::print_answer(question);
        auto items = utils::split(answer, ',');
        for (auto it = items.begin(); it != items.end(); it++) {
            std::cout << color::cyan << *it << color::reset;
            if (it + 1 != items.end()) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;

        return items;
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// MultiSelect

    std::vector<std::string> multi_select(const std::string &question,
                                          const std::vector<std::string> &choices) {
        // Print question
        utils::print_question(question);
        std::cout << std::endl;

        uint selected = 0;
        bool choice[choices.size()];
        for (uint i = 0; i < choices.size(); i++) {
            choice[i] = false;
        }

        // Print choices
        for (uint i = 0; i < choices.size(); i++) {
            if (choice[i]) {
                std::cout << color::green << "✔ " << color::reset;
            } else {
                std::cout << color::grey << "✔ " << color::reset;
            }
            if (i == selected) {
                std::cout << color::cyan << color::underline << choices[i] << color::reset << std::endl;
            } else {
                std::cout << choices[i] << std::endl;
            }
        }

        // Get answer
        char current;
        utils::enable_raw_mode();
        std::cout << utils::hide_cursor();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    break;
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 65 && selected > 0) { // Up
                            selected--;
                        } else if (current == 66 && selected < choices.size() - 1) { // Down
                            selected++;
                        } else if (current == 67) { // Right
                            choice[selected] = true;
                        } else if (current == 68) { // Left
                            choice[selected] = false;
                        }
                    }
                }
            }

            // Redraw choices
            std::cout << utils::move_up(choices.size())
                      << utils::move_left(1000);
            for (uint i = 0; i < choices.size(); i++) {
                if (choice[i]) {
                    std::cout << color::green << "✔ " << color::reset;
                } else {
                    std::cout << color::grey << "✔ " << color::reset;
                }
                if (i == selected) {
                    std::cout << color::cyan << color::underline << choices[i] << color::reset << std::endl;
                } else {
                    std::cout << choices[i] << std::endl;
                }
            }
        }
        std::cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        for (uint i = 0; i < choices.size(); i++) { // Clear choices
            std::cout << utils::move_up() << utils::clear_line(utils::EOL);
        }
        std::cout << utils::move_up()
                  << utils::move_left(1000);
        utils::print_answer(question);
        std::vector<std::string> items;
        for (uint i = 0; i < choices.size(); i++) {
            if (choice[i]) {
                items.push_back(choices[i]);
            }
        }
        for (auto it = items.begin(); it != items.end(); it++) {
            std::cout << color::cyan << *it << color::reset;
            if (it + 1 != items.end()) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;

        return items;
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Number

    template<typename N>
    typename std::enable_if<std::is_arithmetic<N>::value>::type number(const std::string &question) {
        return 0; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Password

    std::string password(const std::string &question,
                         char mask = '*') {
        return ""; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Quiz

    bool quiz(const std::string &question,
              const std::string choices[],
              const std::string &correct) {
        return false; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Slider

    template<typename N>
    typename std::enable_if<std::is_arithmetic<N>::value>::type slider(const std::string &question,
                                                                       N min_value,
                                                                       N max_value,
                                                                       N step,
                                                                       N initial_value) {
        return 0; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Select

    std::string select(const std::string &question,
                       const std::vector<std::string> &choices) {
        // Print question
        utils::print_question(question);
        std::cout << std::endl;

        uint choice = 0;

        // Print choices
        for (uint i = 0; i < choices.size(); i++) {
            if (i == choice) {
                std::cout << color::cyan << color::bold << "> " << color::reset
                          << color::cyan << color::underline << choices[i] << color::reset << std::endl;
            } else {
                std::cout << "  " << choices[i] << std::endl;
            }
        }

        // Get answer
        char current;
        utils::enable_raw_mode();
        std::cout << utils::hide_cursor();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    break;
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 65 && choice > 0) { // Up
                            choice--;
                        } else if (current == 66 && choice < choices.size() - 1) { // Down
                            choice++;
                        }
                    }
                }
            }

            // Redraw choices
            std::cout << utils::move_up(choices.size())
                      << utils::move_left(1000);
            for (uint i = 0; i < choices.size(); i++) {
                if (i == choice) {
                    std::cout << color::cyan << color::bold << "> " << color::reset
                              << color::cyan << color::underline << choices[i] << color::reset << std::endl;
                } else {
                    std::cout << "  " << choices[i] << std::endl;
                }
            }
        }
        std::cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        for (uint i = 0; i < choices.size(); i++) { // Clear choices
            std::cout << utils::move_up() << utils::clear_line(utils::EOL);
        }
        std::cout << utils::move_up()
                  << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << color::cyan << choices[choice] << color::reset << std::endl;

        return choices[choice];
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Toggle

    bool toggle(const std::string &question,
                const std::string &enable,
                const std::string &disable) {
        return false; // TODO
    }
}

#endif //ENQUIRER_HPP
