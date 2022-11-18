/**
 *     === Enquirer ===
 * Created by Kevin Traini
 *      Under GPL-3.0
 * -----------------------
 */
#ifndef ENQUIRER_HPP
#define ENQUIRER_HPP

#define ENQUIRER_VERSION "1.0.0"

#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <termios.h>
#include <sstream>
#include <sys/ioctl.h>
#include <cmath>
#include <algorithm>
#include <unistd.h>

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
                                   const std::string &symbol = color::cyan + color::bold + "? ",
                                   const std::string &input = color::grey + color::bold + "› ") {
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
            print_question(question, color::green + color::bold + "✔ ", color::grey + color::bold + "· ");
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

        /**
         * Return src filtered by the predicate
         */
        inline std::vector<std::string> filter(const std::vector<std::string> &src,
                                               const std::function<bool(const std::string &)> &predicate) {
            std::vector<std::string> result;

            for (const auto &item: src) {
                if (predicate(item)) {
                    result.push_back(item);
                }
            }

            return result;
        }

        /**
         * Check if src begin with prefix
         */
        inline bool begin_with(const std::string &src, const std::string &prefix) {
            return src.find(prefix) == 0;
        }

        /**
         * Complete src with fill until it reaches the length
         */
        inline std::string lfill(const std::string &src, const size_t width, const char fill = ' ') {
            if (src.length() >= width) {
                return src;
            }

            return std::string(width - src.length(), fill) + src;
        }

        /**
         * Returns the size of longest string in the vector
         */
        inline uint max_size(const std::vector<std::string> &strs) {
            uint max = 0;
            for (const auto &str: strs)
                if (str.length() > max)
                    max = str.length();

            return max;
        }
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Auth

    std::pair<std::string, std::string> auth(const std::string &id_prompt = "Username",
                                             const std::string &pw_prompt = "Password",
                                             char mask = '*') {
        // Print inputs
        std::vector<std::string> inputs = {id_prompt, pw_prompt};
        uint width = std::max(id_prompt.length(), pw_prompt.length());
        uint line = 0;
        std::pair<std::string, std::string> answers = std::make_pair("", "");
        utils::print_question(color::cyan + utils::lfill(id_prompt, width), color::grey + "⊙ ");
        std::cout << std::endl;
        utils::print_question(utils::lfill(pw_prompt, width), color::grey + "⊙ ");
        std::cout << std::endl;
        std::cout << utils::move_up(2) << utils::move_right(width + 5);

        // Get answers
        char current;
        utils::enable_raw_mode();
        while (std::cin.get(current)) {
            uint previous = line;
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    if (!answers.first.empty() && !answers.second.empty()) {
                        break;
                    } else {
                        line = answers.first.empty() ? 0 : 1;
                    }
                } else if (current == 127) { // Backspace
                    if (line == 0 && !answers.first.empty())
                        answers.first.pop_back();
                    else if (line == 1 && !answers.second.empty())
                        answers.second.pop_back();

                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 65) { // Up
                            line = (line == 0) ? 1 : 0;
                        } else if (current == 66) { // Down
                            line = (line == 1) ? 0 : 1;
                        }
                    }
                }
            } else { // 'Normal' character
                if (line == 0)
                    answers.first += current;
                else
                    answers.second += current;

            }

            // Redraw inputs
            std::cout << utils::move_left(1000) << (previous == 0 ? "" : utils::move_up());
            std::cout << utils::clear_line(utils::EOL);
            if (line == 0) {
                utils::print_question(color::cyan + utils::lfill(id_prompt, width), (answers.first.empty() ?
                                                                                     color::grey + "⊙ " :
                                                                                     color::green + "⦿ "));
                std::cout << answers.first << std::endl;
                utils::print_question(utils::lfill(pw_prompt, width), (answers.second.empty() ?
                                                                       color::grey + "⊙ " :
                                                                       color::green + "⦿ "));
                std::cout << std::string(answers.second.length(), '*') << std::endl;
            } else {
                utils::print_question(utils::lfill(id_prompt, width), (answers.first.empty() ?
                                                                       color::grey + "⊙ " :
                                                                       color::green + "⦿ "));
                std::cout << answers.first << std::endl;
                utils::print_question(color::cyan + utils::lfill(pw_prompt, width), (answers.second.empty() ?
                                                                                     color::grey + "⊙ " :
                                                                                     color::green + "⦿ "));
                std::cout << std::string(answers.second.length(), '*') << std::endl;
            }
            std::cout << utils::move_up(inputs.size() - line)
                      << utils::move_right(width + 5 + (line == 0 ?
                                                        answers.first.length() :
                                                        answers.second.length()));
        }

        // Print resume
        std::cout << utils::move_left(1000) << (line == 0 ? "" : utils::move_up());
        std::cout << utils::clear_line(utils::EOL);
        utils::print_question(utils::lfill(id_prompt, width), (answers.first.empty() ?
                                                               color::grey + "⊙ " :
                                                               color::green + "⦿ "));
        std::cout << answers.first << std::endl;
        utils::print_question(utils::lfill(pw_prompt, width), (answers.second.empty() ?
                                                               color::grey + "⊙ " :
                                                               color::green + "⦿ "));
        std::cout << std::string(answers.second.length(), '*') << std::endl;

        return {answers.first, answers.second};
    }

    bool auth(const std::function<bool(const std::pair<std::string, std::string> &)> &predicate,
              const std::string &id_prompt = "Username",
              const std::string &pw_prompt = "Password",
              char mask = '*') {
        return predicate(auth(id_prompt, pw_prompt, mask));
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Autocomplete

    std::string autocomplete(const std::string &question,
                             const std::vector<std::string> &choices = {},
                             uint limit = 10) {
        // Print question
        utils::print_question(question);

        std::vector<std::string> current_choices;
        int choice = -1;

        // Get answer
        std::string answer;
        char current;
        utils::enable_raw_mode();
        while (std::cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    break;
                } else if (current == 127) { // Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                    }
                } else if (current == 9 && choice != -1) { // Tab
                    if (answer == current_choices[choice].substr(0, answer.length())) {
                        answer = std::string(current_choices[choice]);
                    }
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 65) { // Up
                            choice = (choice == 0) ? (int) current_choices.size() - 1 : choice - 1;
                        } else if (current == 66) { // Down
                            choice = (choice == std::min(limit, (uint) current_choices.size()) - 1) ? 0 : choice + 1;
                        }
                    }
                }
            } else { // 'Normal' character
                answer += current;
            }

            // Erase previous choices
            std::cout << utils::move_down(std::min(limit, (uint) current_choices.size()) + 1)
                      << utils::move_left(1000);
            for (uint i = 0; i < current_choices.size() && i < limit; i++) {
                std::cout << utils::clear_line(utils::EOL)
                          << utils::move_up();
            }
            std::cout << utils::move_up();
            // Draw completion
            current_choices = utils::filter(choices, [=](const std::string &item) {
                return utils::begin_with(item, answer);
            });
            choice = std::max(0, std::min(choice, (int) current_choices.size() - 1));
            std::cout << utils::move_left(1000) << utils::clear_line(utils::LINE);
            utils::print_question(question);
            std::cout << answer;
            if (!current_choices.empty()) {
                std::cout << color::grey << current_choices[choice].substr(answer.length()) << color::reset;
            }
            std::cout << std::endl;
            for (uint i = 0; i < current_choices.size() && i < limit; i++) {
                std::cout << utils::clear_line(utils::EOL);
                if (i == choice) {
                    std::cout << color::cyan << color::underline << current_choices[i] << color::reset << std::endl;
                } else {
                    std::cout << current_choices[i] << std::endl;
                }
            }
            std::cout << utils::move_up(std::min(limit, (uint) current_choices.size()) + 1)
                      << utils::move_left(1000) << utils::move_right(question.length() + answer.length() + 5);
        }
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_down(std::min(limit, (uint) current_choices.size()) + 1)
                  << utils::move_left(1000);
        for (uint i = 0; i < current_choices.size() && i < limit; i++) {
            std::cout << utils::clear_line(utils::EOL)
                      << utils::move_up();
        }
        std::cout << utils::move_up()
                  << utils::move_left(1000) << utils::clear_line(utils::LINE);
        utils::print_answer(question);
        std::cout << color::cyan << answer << color::reset << std::endl;

        return answer;
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
                                            const std::vector<std::string> &inputs) {
        if (inputs.empty()) {
            return {};
        }

        // Print question
        utils::print_question(question);
        std::cout << std::endl;

        // Print inputs
        uint width = utils::max_size(inputs);
        uint line = 0;
        std::map<std::string, std::string> answers;
        for (uint i = 0; i < inputs.size(); i++) {
            answers[inputs[i]] = "";
            if (i == line) {
                utils::print_question(color::cyan + utils::lfill(inputs[i], width), color::grey + "⊙ ");
            } else {
                utils::print_question(utils::lfill(inputs[i], width), color::grey + "⊙ ");
            }
            std::cout << std::endl;
        }
        std::cout << utils::move_up(inputs.size()) << utils::move_right(width + 5);

        // Get answers
        char current;
        utils::enable_raw_mode();
        while (std::cin.get(current)) {
            uint previous = line;
            if (iscntrl(current)) {
                if (current == 10) { // Enter
                    if (std::all_of(answers.begin(), answers.end(),
                                    [](const std::pair<std::string, std::string> &item) {
                                        return !item.second.empty();
                                    })) {
                        break;
                    } else {
                        line = std::distance(answers.begin(),
                                             std::find_if(answers.begin(), answers.end(),
                                                          [](const std::pair<std::string, std::string> &item) {
                                                              return item.second.empty();
                                                          }));
                    }
                } else if (current == 127) { // Backspace
                    if (!answers[inputs[line]].empty()) {
                        answers[inputs[line]].pop_back();
                    }
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 65) { // Up
                            line = (line == 0) ? inputs.size() - 1 : line - 1;
                        } else if (current == 66) { // Down
                            line = (line == inputs.size() - 1) ? 0 : line + 1;
                        }
                    }
                }
            } else { // 'Normal' character
                answers[inputs[line]] += current;
            }

            // Redraw inputs
            std::cout << utils::move_left(1000) << (previous == 0 ? "" : utils::move_up(previous));
            for (uint i = 0; i < inputs.size(); i++) {
                std::cout << utils::clear_line(utils::EOL);
                std::string indicator = (answers[inputs[i]].empty() ?
                                         color::grey + "⊙ " :
                                         color::green + "⦿ ");
                if (i == line) {
                    utils::print_question(color::cyan + utils::lfill(inputs[i], width), indicator);
                } else {
                    utils::print_question(utils::lfill(inputs[i], width), indicator);
                }
                std::cout << answers[inputs[i]] << std::endl;
            }
            std::cout << utils::move_up(inputs.size() - line)
                      << utils::move_right(width + 5 + answers[inputs[line]].length());
        }

        // Print resume
        std::cout << utils::move_left(1000) << (line == 0 ? "" : utils::move_up(line))
                  << utils::move_up()
                  << utils::clear_line(utils::EOL);
        utils::print_answer(question);
        std::cout << std::endl;
        for (const auto &input: inputs) {
            std::cout << utils::clear_line(utils::EOL);
            utils::print_question(utils::lfill(input, width), color::green + "⦿ ");
            std::cout << answers[input] << std::endl;
        }

        return answers;
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
                std::cout << color::bold << color::green << "✔ " << color::reset;
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
                        if (current == 65) { // Up
                            selected = (selected == 0) ? choices.size() - 1 : selected - 1;
                        } else if (current == 66) { // Down
                            selected = (selected == choices.size() - 1) ? 0 : selected + 1;
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
                    std::cout << color::bold << color::green << "✔ " << color::reset;
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

    template<typename N,
            typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
    N number(const std::string &question) {
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
            } else if (std::isdigit(current) || current == '.') { // 'Normal' character
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

    std::string password(const std::string &question,
                         char mask = '*') {
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
                std::cout << mask;
            }
        }
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_up()
                  << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << color::cyan << std::string(answer.size(), mask) << color::reset << std::endl;

        return answer;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Quiz

    bool quiz(const std::string &question,
              const std::vector<std::string> &choices,
              const std::string &correct) {
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
                        if (current == 65) { // Up
                            choice = (choice == 0) ? choices.size() - 1 : choice - 1;
                        } else if (current == 66) { // Down
                            choice = (choice == choices.size() - 1) ? 0 : choice + 1;
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
        bool result = (choices[choice] == correct);
        std::cout << (result ? color::green : color::red) << choices[choice] << color::reset << std::endl;

        return result;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Slider

    template<typename N,
            typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
    N slider(const std::string &question,
             N min_value,
             N max_value,
             N step,
             N initial_value) {
        // Print question
        utils::print_question(question);

        struct winsize w{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        const uint width = std::min((uint) ((max_value - min_value) / step),
                                    (uint) w.ws_col - 7); // 7 is for < > # and 2 spaces each side
        const N swidth = (max_value - min_value) / ((N) width);

        // Print value
        N value = initial_value;
        std::cout << std::endl << "   "
                  << std::string((width / 2) - (std::to_string(value).length() / 2), ' ')
                  << color::bold << value << color::reset
                  << std::endl;

        // Print slider
        std::cout << "  " << color::cyan << color::bold << "<" << color::reset;
        for (N i = 0; i <= width; i++) {
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
                if (current == 10) { // Enter
                    break;
                } else if (current == 27) { // Escape
                    std::cin.get(current);
                    if (current == 91) {
                        std::cin.get(current);
                        if (current == 67) { // Right
                            value = std::min(value + step, max_value);
                        } else if (current == 68) { // Left
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
            for (uint i = 0; i <= width; i++) {
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
                        if (current == 65) { // Up
                            choice = (choice == 0) ? choices.size() - 1 : choice - 1;
                        } else if (current == 66) { // Down
                            choice = (choice == choices.size() - 1) ? 0 : choice + 1;
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
                const std::string &disable,
                bool default_value = false) {
        // Print question
        utils::print_question(question);

        // Print choices
        bool toggled = default_value;
        std::cout << (toggled ? color::cyan + color::underline : "") << enable << color::reset << "/"
                  << (toggled ? "" : color::cyan + color::underline) << disable << color::reset;

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
                        if (current == 68) { // Left
                            toggled = true;
                        } else if (current == 67) { // Right
                            toggled = false;
                        }
                    }
                }
            }

            // Redraw choices
            std::cout << utils::move_left(enable.length() + disable.length() + 1)
                      << utils::clear_line(utils::EOL)
                      << (toggled ? color::cyan + color::underline : "") << enable << color::reset << "/"
                      << (toggled ? "" : color::cyan + color::underline) << disable << color::reset;
        }
        std::cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        std::cout << utils::move_left(1000);
        utils::print_answer(question);
        std::cout << (toggled ? color::green : color::red) << (toggled ? enable : disable) << color::reset << std::endl;

        return toggled;
    }
}

#endif //ENQUIRER_HPP
