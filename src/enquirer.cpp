/**
 * MIT License
 *
 * Copyright (c) 2024-Present Kevin Traini
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
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

using namespace std;

namespace enquirer {

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Utilities

    namespace color {
        const string black   = "\033[30m";
        const string red     = "\033[31m";
        const string green   = "\033[32m";
        const string yellow  = "\033[33m";
        const string blue    = "\033[34m";
        const string magenta = "\033[35m";
        const string cyan    = "\033[36m";
        const string white   = "\033[37m";
        const string grey    = "\033[90m";

        const string reset     = "\033[0m";
        const string bold      = "\033[1m";
        const string underline = "\033[4m";
        const string blink     = "\033[5m";
        const string inverse   = "\033[7m";
    }// namespace color

    namespace utils {
        auto move_up(unsigned int n = 1) -> string {
            return "\033[" + to_string(n) + "A";
        }

        auto move_down(unsigned int n = 1) -> string {
            return "\033[" + to_string(n) + "B";
        }

        auto move_left(unsigned int n = 1) -> string {
            return "\033[" + to_string(n) + "D";
        }

        auto move_right(unsigned int n = 1) -> string {
            return "\033[" + to_string(n) + "C";
        }

        typedef enum {
            EOL  = 0,
            BOL  = 1,
            LINE = 2
        } clear_mode;

        auto clear_line(clear_mode mode) -> string {
            return "\033[" + to_string(mode) + "K";
        }

        auto hide_cursor() -> string {
            return "\033[?25l";
        }

        auto show_cursor() -> string {
            return "\033[?25h";
        }

        auto print_question(const string &question,
                            const string &symbol = color::cyan + color::bold + "? ",
                            const string &input  = color::grey + color::bold + "› ") -> void {
            cout << clear_line(LINE);
            cout << symbol
                 << color::reset << question
                 << " " << input
                 << color::reset;
        }

        auto print_answer(const string &question) -> void {
            print_question(question, color::green + color::bold + "✔ ", color::grey + color::bold + "· ");
        }

        auto enable_raw_mode() -> void {
            struct termios term {};
            tcgetattr(STDIN_FILENO, &term);
            term.c_lflag &= ~(ECHO | ICANON);
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
        }

        auto disable_raw_mode() -> void {
            struct termios term {};
            tcgetattr(STDIN_FILENO, &term);
            term.c_lflag |= (ECHO | ICANON);
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
        }

        auto ltrim(const string &str) -> string {
            auto start = str.find_first_not_of(' ');
            return (start == string::npos) ? "" : str.substr(start);
        }

        auto rtrim(const string &str) -> string {
            auto end = str.find_last_not_of(' ');
            return (end == string::npos) ? "" : str.substr(0, end + 1);
        }

        auto trim(const string &str) -> string {
            return rtrim(ltrim(str));
        }

        auto split(const string &str, const char delim) -> vector<string> {
            vector<string> result;
            stringstream ss(str);
            string item;
            while (getline(ss, item, delim)) {
                result.push_back(trim(item));
            }

            return result;
        }

        auto filter(const vector<string> &src,
                    const function<bool(const string &)> &predicate) -> vector<string> {
            vector<string> result;

            for (const auto &item: src) {
                if (predicate(item)) {
                    result.push_back(item);
                }
            }

            return result;
        }

        auto begin_with(const string &src, const string &prefix) -> bool {
            return src.find(prefix) == 0;
        }

        auto lfill(const string &src, const size_t width, const char fill = ' ') -> string {
            if (src.length() >= width) {
                return src;
            }

            return string(width - src.length(), fill) + src;
        }

        auto max_size(const vector<string> &strs) -> unsigned int {
            unsigned int max = 0;
            for (const auto &str: strs)
                if (str.length() > max)
                    max = str.length();

            return max;
        }
    }// namespace utils

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Auth

    auto auth(const string &id_prompt = "Username",
              const string &pw_prompt = "Password",
              char mask               = '*') -> pair<string, string> {
        // Print inputs
        vector<string> inputs        = {id_prompt, pw_prompt};
        unsigned int width           = max(id_prompt.length(), pw_prompt.length());
        unsigned int line            = 0;
        pair<string, string> answers = make_pair("", "");
        utils::print_question(color::cyan + utils::lfill(id_prompt, width), color::grey + "⊙ ");
        cout << endl;
        utils::print_question(utils::lfill(pw_prompt, width), color::grey + "⊙ ");
        cout << endl;
        cout << utils::move_up(2) << utils::move_right(width + 5);

        // Get answers
        char current;
        utils::enable_raw_mode();
        while (cin.get(current)) {
            unsigned int previous = line;
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    if (!answers.first.empty() && !answers.second.empty()) {
                        break;
                    } else {
                        line = answers.first.empty() ? 0 : 1;
                    }
                } else if (current == 127) {// Backspace
                    if (line == 0 && !answers.first.empty())
                        answers.first.pop_back();
                    else if (line == 1 && !answers.second.empty())
                        answers.second.pop_back();

                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 65) {// Up
                            line = (line == 0) ? 1 : 0;
                        } else if (current == 66) {// Down
                            line = (line == 1) ? 0 : 1;
                        }
                    }
                }
            } else {// 'Normal' character
                if (line == 0)
                    answers.first += current;
                else
                    answers.second += current;
            }

            // Redraw inputs
            cout << utils::move_left(1000) << (previous == 0 ? "" : utils::move_up());
            cout << utils::clear_line(utils::EOL);
            if (line == 0) {
                utils::print_question(color::cyan + utils::lfill(id_prompt, width), (answers.first.empty() ? color::grey + "⊙ " : color::green + "⦿ "));
                cout << answers.first << endl;
                utils::print_question(utils::lfill(pw_prompt, width), (answers.second.empty() ? color::grey + "⊙ " : color::green + "⦿ "));
                cout << string(answers.second.length(), mask) << endl;
            } else {
                utils::print_question(utils::lfill(id_prompt, width), (answers.first.empty() ? color::grey + "⊙ " : color::green + "⦿ "));
                cout << answers.first << endl;
                utils::print_question(color::cyan + utils::lfill(pw_prompt, width), (answers.second.empty() ? color::grey + "⊙ " : color::green + "⦿ "));
                cout << string(answers.second.length(), mask) << endl;
            }
            cout << utils::move_up(inputs.size() - line)
                 << utils::move_right(width + 5 + (line == 0 ? answers.first.length() : answers.second.length()));
        }

        // Print resume
        cout << utils::move_left(1000) << (line == 0 ? "" : utils::move_up());
        cout << utils::clear_line(utils::EOL);
        utils::print_question(utils::lfill(id_prompt, width), (answers.first.empty() ? color::grey + "⊙ " : color::green + "⦿ "));
        cout << answers.first << endl;
        utils::print_question(utils::lfill(pw_prompt, width), (answers.second.empty() ? color::grey + "⊙ " : color::green + "⦿ "));
        cout << string(answers.second.length(), mask) << endl;

        return {answers.first, answers.second};
    }

    auto auth(const function<bool(const pair<string, string> &)> &predicate,
              const string &id_prompt = "Username",
              const string &pw_prompt = "Password",
              char mask               = '*') -> bool {
        return predicate(auth(id_prompt, pw_prompt, mask));
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Autocomplete

    auto autocomplete(const string &question,
                      const vector<string> &choices = {},
                      unsigned int limit            = 10) -> string {
        // Print question
        utils::print_question(question);

        vector<string> current_choices;
        int choice = -1;

        // Get answer
        string answer;
        char current;
        utils::enable_raw_mode();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    break;
                } else if (current == 127) {// Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                    }
                } else if (current == 9 && choice != -1) {// Tab
                    if (answer == current_choices[choice].substr(0, answer.length())) {
                        answer = string(current_choices[choice]);
                    }
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 65) {// Up
                            choice = (choice == 0) ? (int) current_choices.size() - 1 : choice - 1;
                        } else if (current == 66) {// Down
                            choice = (choice == (int) min(limit, (unsigned int) current_choices.size()) - 1) ? 0 : choice + 1;
                        }
                    }
                }
            } else {// 'Normal' character
                answer += current;
            }

            // Erase previous choices
            cout << utils::move_down(min(limit, (unsigned int) current_choices.size()) + 1)
                 << utils::move_left(1000);
            for (unsigned int i = 0; i < current_choices.size() && i < limit; i++) {
                cout << utils::clear_line(utils::EOL)
                     << utils::move_up();
            }
            cout << utils::move_up();
            // Draw completion
            current_choices = utils::filter(choices, [=](const string &item) {
                return utils::begin_with(item, answer);
            });
            choice          = max(0, min(choice, (int) current_choices.size() - 1));
            cout << utils::move_left(1000) << utils::clear_line(utils::LINE);
            utils::print_question(question);
            cout << answer;
            if (!current_choices.empty()) {
                cout << color::grey << current_choices[choice].substr(answer.length()) << color::reset;
            }
            cout << endl;
            for (unsigned int i = 0; i < current_choices.size() && i < limit; i++) {
                cout << utils::clear_line(utils::EOL);
                if ((int) i == choice) {
                    cout << color::cyan << color::underline << current_choices[i] << color::reset << endl;
                } else {
                    cout << current_choices[i] << endl;
                }
            }
            cout << utils::move_up(min(limit, (unsigned int) current_choices.size()) + 1)
                 << utils::move_left(1000) << utils::move_right(question.length() + answer.length() + 5);
        }
        utils::disable_raw_mode();

        // Print resume
        cout << utils::move_down(min(limit, (unsigned int) current_choices.size()) + 1)
             << utils::move_left(1000);
        for (unsigned int i = 0; i < current_choices.size() && i < limit; i++) {
            cout << utils::clear_line(utils::EOL)
                 << utils::move_up();
        }
        cout << utils::move_up()
             << utils::move_left(1000) << utils::clear_line(utils::LINE);
        utils::print_answer(question);
        cout << color::cyan << answer << color::reset << endl;

        return answer;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Confirm

    auto confirm(const string &question,
                 bool default_value = false) -> bool {
        // Print question
        utils::print_question(question);

        // Print choices
        bool confirmed = default_value;
        cout << (confirmed ? "Yes" : "No");

        // Get answer
        char current;
        utils::enable_raw_mode();
        cout << utils::hide_cursor();
        while (cin.get(current)) {
            bool previous = confirmed;
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    break;
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 68) {// Left
                            confirmed = true;
                        } else if (current == 67) {// Right
                            confirmed = false;
                        }
                    }
                }
            }

            // Redraw choices
            cout << utils::move_left(previous ? 3 : 2)
                 << utils::clear_line(utils::EOL)
                 << (confirmed ? "Yes" : "No");
        }
        cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        cout << utils::move_left(1000);
        utils::print_answer(question);
        cout << (confirmed ? color::green : color::red) << (confirmed ? "Yes" : "No") << color::reset << endl;

        return confirmed;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Form

    auto form(const string &question,
              const vector<string> &inputs) -> map<string, string> {
        if (inputs.empty()) {
            return {};
        }

        // Print question
        utils::print_question(question);
        cout << endl;

        // Print inputs
        unsigned int width = utils::max_size(inputs);
        unsigned int line  = 0;
        map<string, string> answers;
        for (unsigned int i = 0; i < inputs.size(); i++) {
            answers[inputs[i]] = "";
            if (i == line) {
                utils::print_question(color::cyan + utils::lfill(inputs[i], width), color::grey + "⊙ ");
            } else {
                utils::print_question(utils::lfill(inputs[i], width), color::grey + "⊙ ");
            }
            cout << endl;
        }
        cout << utils::move_up(inputs.size()) << utils::move_right(width + 5);

        // Get answers
        char current;
        utils::enable_raw_mode();
        while (cin.get(current)) {
            unsigned int previous = line;
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    if (all_of(answers.begin(), answers.end(),
                               [](const pair<string, string> &item) {
                                   return !item.second.empty();
                               })) {
                        break;
                    } else {
                        line = distance(answers.begin(),
                                        find_if(answers.begin(), answers.end(),
                                                [](const pair<string, string> &item) {
                                                    return item.second.empty();
                                                }));
                    }
                } else if (current == 127) {// Backspace
                    if (!answers[inputs[line]].empty()) {
                        answers[inputs[line]].pop_back();
                    }
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 65) {// Up
                            line = (line == 0) ? inputs.size() - 1 : line - 1;
                        } else if (current == 66) {// Down
                            line = (line == inputs.size() - 1) ? 0 : line + 1;
                        }
                    }
                }
            } else {// 'Normal' character
                answers[inputs[line]] += current;
            }

            // Redraw inputs
            cout << utils::move_left(1000) << (previous == 0 ? "" : utils::move_up(previous));
            for (unsigned int i = 0; i < inputs.size(); i++) {
                cout << utils::clear_line(utils::EOL);
                string indicator = (answers[inputs[i]].empty() ? color::grey + "⊙ " : color::green + "⦿ ");
                if (i == line) {
                    utils::print_question(color::cyan + utils::lfill(inputs[i], width), indicator);
                } else {
                    utils::print_question(utils::lfill(inputs[i], width), indicator);
                }
                cout << answers[inputs[i]] << endl;
            }
            cout << utils::move_up(inputs.size() - line)
                 << utils::move_right(width + 5 + answers[inputs[line]].length());
        }

        // Print resume
        cout << utils::move_left(1000) << (line == 0 ? "" : utils::move_up(line))
             << utils::move_up()
             << utils::clear_line(utils::EOL);
        utils::print_answer(question);
        cout << endl;
        for (const auto &input: inputs) {
            cout << utils::clear_line(utils::EOL);
            utils::print_question(utils::lfill(input, width), color::green + "⦿ ");
            cout << answers[input] << endl;
        }

        return answers;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Input

    auto input(const string &question,
               const string &default_value = "") -> string {
        // Print question
        utils::print_question(question);

        // Print default value
        cout << color::grey << default_value << color::reset;
        cout << utils::move_left(default_value.size());

        // Get answer
        string answer;
        char current;
        utils::enable_raw_mode();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    cout << endl;
                    break;
                } else if (current == 127) {// Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                        cout << utils::move_left(1);
                        cout << utils::clear_line(utils::EOL);
                    }
                } else if (current == 9) {// Tab
                    if (answer == default_value.substr(0, answer.length())) {
                        cout << default_value.substr(answer.length());
                        answer = default_value;
                    }
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else {// 'Normal' character
                answer += current;
                cout << current;
            }

            // Check default_value
            if (answer == default_value.substr(0, answer.length())) {
                cout << color::grey << default_value.substr(answer.length()) << color::reset;
                if (answer != default_value) {
                    cout << utils::move_left(default_value.size() - answer.size());
                }
            } else if (answer != default_value) {
                cout << utils::clear_line(utils::EOL);
            }
        }
        utils::disable_raw_mode();

        // Print resume
        cout << utils::move_up()
             << utils::move_left(1000);
        utils::print_answer(question);
        cout << color::cyan << answer << color::reset << endl;

        return answer;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Invisible

    auto invisible(const string &question) -> string {
        // Print question
        utils::print_question(question);

        // Get answer
        string answer;
        char current;
        utils::enable_raw_mode();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    cout << endl;
                    break;
                } else if (current == 127) {// Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                    }
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else {// 'Normal' character
                answer += current;
            }
        }
        utils::disable_raw_mode();

        // Print resume
        cout << utils::move_up()
             << utils::move_left(1000);
        utils::print_answer(question);
        cout << endl;

        return answer;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // List

    auto list(const string &question) -> vector<string> {
        // Print question
        utils::print_question(question);

        // Get answer
        string answer;
        char current;
        utils::enable_raw_mode();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    cout << endl;
                    break;
                } else if (current == 127) {// Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                        cout << utils::move_left(1);
                        cout << utils::clear_line(utils::EOL);
                    }
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else {// 'Normal' character
                answer += current;
                cout << current;
            }
        }
        utils::disable_raw_mode();

        // Print resume
        cout << utils::move_up()
             << utils::move_left(1000);
        utils::print_answer(question);
        auto items = utils::split(answer, ',');
        for (auto it = items.begin(); it != items.end(); it++) {
            cout << color::cyan << *it << color::reset;
            if (it + 1 != items.end()) {
                cout << ", ";
            }
        }
        cout << endl;

        return items;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // MultiSelect

    auto multi_select(const string &question,
                      const vector<string> &choices) -> vector<string> {
        // Print question
        utils::print_question(question);
        cout << endl;

        unsigned int selected = 0;
        bool choice[choices.size()];
        for (unsigned int i = 0; i < choices.size(); i++) {
            choice[i] = false;
        }

        // Print choices
        for (unsigned int i = 0; i < choices.size(); i++) {
            if (choice[i]) {
                cout << color::bold << color::green << "✔ " << color::reset;
            } else {
                cout << color::grey << "✔ " << color::reset;
            }
            if (i == selected) {
                cout << color::cyan << color::underline << choices[i] << color::reset << endl;
            } else {
                cout << choices[i] << endl;
            }
        }

        // Get answer
        char current;
        utils::enable_raw_mode();
        cout << utils::hide_cursor();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    break;
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 65) {// Up
                            selected = (selected == 0) ? choices.size() - 1 : selected - 1;
                        } else if (current == 66) {// Down
                            selected = (selected == choices.size() - 1) ? 0 : selected + 1;
                        } else if (current == 67) {// Right
                            choice[selected] = true;
                        } else if (current == 68) {// Left
                            choice[selected] = false;
                        }
                    }
                }
            }

            // Redraw choices
            cout << utils::move_up(choices.size())
                 << utils::move_left(1000);
            for (unsigned int i = 0; i < choices.size(); i++) {
                if (choice[i]) {
                    cout << color::bold << color::green << "✔ " << color::reset;
                } else {
                    cout << color::grey << "✔ " << color::reset;
                }
                if (i == selected) {
                    cout << color::cyan << color::underline << choices[i] << color::reset << endl;
                } else {
                    cout << choices[i] << endl;
                }
            }
        }
        cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        for (unsigned int i = 0; i < choices.size(); i++) {// Clear choices
            cout << utils::move_up() << utils::clear_line(utils::EOL);
        }
        cout << utils::move_up()
             << utils::move_left(1000);
        utils::print_answer(question);
        vector<string> items;
        for (unsigned int i = 0; i < choices.size(); i++) {
            if (choice[i]) {
                items.push_back(choices[i]);
            }
        }
        for (auto it = items.begin(); it != items.end(); it++) {
            cout << color::cyan << *it << color::reset;
            if (it + 1 != items.end()) {
                cout << ", ";
            }
        }
        cout << endl;

        return items;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Password

    auto password(const string &question,
                  char mask = '*') -> string {
        // Print question
        utils::print_question(question);

        // Get answer
        string answer;
        char current;
        utils::enable_raw_mode();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    cout << endl;
                    break;
                } else if (current == 127) {// Backspace
                    if (!answer.empty()) {
                        answer.pop_back();
                        cout << utils::move_left(1);
                        cout << utils::clear_line(utils::EOL);
                    }
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        // Ignore arrow keys
                    }
                }
            } else {// 'Normal' character
                answer += current;
                cout << mask;
            }
        }
        utils::disable_raw_mode();

        // Print resume
        cout << utils::move_up()
             << utils::move_left(1000);
        utils::print_answer(question);
        cout << color::cyan << string(answer.size(), mask) << color::reset << endl;

        return answer;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Quiz

    auto quiz(const string &question,
              const vector<string> &choices,
              const string &correct) -> bool {
        // Print question
        utils::print_question(question);
        cout << endl;

        unsigned int choice = 0;

        // Print choices
        for (unsigned int i = 0; i < choices.size(); i++) {
            if (i == choice) {
                cout << color::cyan << color::bold << "> " << color::reset
                     << color::cyan << color::underline << choices[i] << color::reset << endl;
            } else {
                cout << "  " << choices[i] << endl;
            }
        }

        // Get answer
        char current;
        utils::enable_raw_mode();
        cout << utils::hide_cursor();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    break;
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 65) {// Up
                            choice = (choice == 0) ? choices.size() - 1 : choice - 1;
                        } else if (current == 66) {// Down
                            choice = (choice == choices.size() - 1) ? 0 : choice + 1;
                        }
                    }
                }
            }

            // Redraw choices
            cout << utils::move_up(choices.size())
                 << utils::move_left(1000);
            for (unsigned int i = 0; i < choices.size(); i++) {
                if (i == choice) {
                    cout << color::cyan << color::bold << "> " << color::reset
                         << color::cyan << color::underline << choices[i] << color::reset << endl;
                } else {
                    cout << "  " << choices[i] << endl;
                }
            }
        }
        cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        for (unsigned int i = 0; i < choices.size(); i++) {// Clear choices
            cout << utils::move_up() << utils::clear_line(utils::EOL);
        }
        cout << utils::move_up()
             << utils::move_left(1000);
        utils::print_answer(question);
        bool result = (choices[choice] == correct);
        cout << (result ? color::green : color::red) << choices[choice] << color::reset << endl;

        return result;
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Select

    auto select(const string &question,
                const vector<string> &choices) -> string {
        // Print question
        utils::print_question(question);
        cout << endl;

        unsigned int choice = 0;

        // Print choices
        for (unsigned int i = 0; i < choices.size(); i++) {
            if (i == choice) {
                cout << color::cyan << color::bold << "> " << color::reset
                     << color::cyan << color::underline << choices[i] << color::reset << endl;
            } else {
                cout << "  " << choices[i] << endl;
            }
        }

        // Get answer
        char current;
        utils::enable_raw_mode();
        cout << utils::hide_cursor();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    break;
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 65) {// Up
                            choice = (choice == 0) ? choices.size() - 1 : choice - 1;
                        } else if (current == 66) {// Down
                            choice = (choice == choices.size() - 1) ? 0 : choice + 1;
                        }
                    }
                }
            }

            // Redraw choices
            cout << utils::move_up(choices.size())
                 << utils::move_left(1000);
            for (unsigned int i = 0; i < choices.size(); i++) {
                if (i == choice) {
                    cout << color::cyan << color::bold << "> " << color::reset
                         << color::cyan << color::underline << choices[i] << color::reset << endl;
                } else {
                    cout << "  " << choices[i] << endl;
                }
            }
        }
        cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        for (unsigned int i = 0; i < choices.size(); i++) {// Clear choices
            cout << utils::move_up() << utils::clear_line(utils::EOL);
        }
        cout << utils::move_up()
             << utils::move_left(1000);
        utils::print_answer(question);
        cout << color::cyan << choices[choice] << color::reset << endl;

        return choices[choice];
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
    // Toggle

    auto toggle(const string &question,
                const string &enable,
                const string &disable,
                bool default_value = false) -> bool {
        // Print question
        utils::print_question(question);

        // Print choices
        bool toggled = default_value;
        cout << (toggled ? color::cyan + color::underline : "") << enable << color::reset << "/"
             << (toggled ? "" : color::cyan + color::underline) << disable << color::reset;

        // Get answer
        char current;
        utils::enable_raw_mode();
        cout << utils::hide_cursor();
        while (cin.get(current)) {
            if (iscntrl(current)) {
                if (current == 10) {// Enter
                    break;
                } else if (current == 27) {// Escape
                    cin.get(current);
                    if (current == 91) {
                        cin.get(current);
                        if (current == 68) {// Left
                            toggled = true;
                        } else if (current == 67) {// Right
                            toggled = false;
                        }
                    }
                }
            }

            // Redraw choices
            cout << utils::move_left(enable.length() + disable.length() + 1)
                 << utils::clear_line(utils::EOL)
                 << (toggled ? color::cyan + color::underline : "") << enable << color::reset << "/"
                 << (toggled ? "" : color::cyan + color::underline) << disable << color::reset;
        }
        cout << utils::show_cursor();
        utils::disable_raw_mode();

        // Print resume
        cout << utils::move_left(1000);
        utils::print_answer(question);
        cout << (toggled ? color::green : color::red) << (toggled ? enable : disable) << color::reset << endl;

        return toggled;
    }
}// namespace enquirer
