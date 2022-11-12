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
        inline std::string move_up(int n = 1) {
            return "\033[" + std::to_string(n) + "A";
        }

        /**
         * Move the cursor n times downward
         */
        inline std::string move_down(int n = 1) {
            return "\033[" + std::to_string(n) + "B";
        }

        /**
         * Move the cursor n times to the left
         */
        inline std::string move_left(int n = 1) {
            return "\033[" + std::to_string(n) + "D";
        }

        /**
         * Move the cursor n times to the right
         */
        inline std::string move_right(int n = 1) {
            return "\033[" + std::to_string(n) + "C";
        }

        /**
         * Clear entire line
         */
        inline std::string clear_line() {
            return "\033[K";
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
         * @param str
         */
        inline void print_question(const std::string &question) {
            std::cout << clear_line();
            std::cout << color::cyan << "? "
                      << color::reset << question
                      << color::grey << " > "
                      << color::reset;
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

    bool confirm(const std::string &question) {
        return false; // TODO
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
        return ""; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// Invisible

    std::string invisible(const std::string &question) {
        return ""; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// List

    std::vector<std::string> list(const std::string &question) {
        return {}; // TODO
    }

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
// MultiSelect

    std::vector<std::string> multi_select(const std::string &question,
                                          const std::vector<std::string> &choices) {
        return {}; // TODO;
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
                       const std::string choices[]) {
        return ""; // TODO
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
