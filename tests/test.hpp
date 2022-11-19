#ifndef TEST_HPP
#define TEST_HPP

#include <string>
#include <functional>
#include <map>
#include <unistd.h>
#include <termios.h>
#include <iostream>

/*
 * Simple c++ test framework
 *
 * Usage:
 * // Instantiate a test object
 * Test test;
 * // Describe some tests
 * test.describe("Test 1", []() {
 *      // Test some things
 *      Test::Result res = it("1 is equal to 1", []() {
 *          assert_equal(1, 1);
 *
 *          return Test::PASS;
 *      });
 *
 *      // You can use assert (or should directly)
 *      should_not_equal(1, 2);
 *
 *      return res;
 * });
 * // Run tests
 * bool success = test.run();
 *
 * // It will display that (with colors)
 * Run 1 test
 * Test 1
 * ✔ 1 is equal to 1
 *  PASS
 *
 *  All tests passed
 *
 *  // describe()
 *  method describe takes 2 arguments :
 *  - name of the test
 *  - function of the test. This function should return a test::Result
 *
 *  // it()
 *  macro it takes 2 arguments :
 *  - name of the test
 *  - function of the test. This function should return a test::Result
 *
 *  // assert_* and should_*
 *  For each assert, there is an equivalent should (and vice versa)
 *  If assert failed, the test fail. If should failed, the test is skipped
 *  List of assert_* and should_*
 *  - equal
 *  - not_equal
 *  - less
 *  - great
 *  - less_equal
 *  - great_equal
 *  - true
 *  - false
 *  - empty
 *  - not_empty
 *
 *  // it_<a>_<b>()
 *  These macros are some helpers for tests, it runs a test and return b if not a.
 *  In fact, it's just a shortcut : it_<a>_fail() is assert_equal(<a>, it()) and
 *                                  it_<a>_skip() is should_equal(<a>, it())
 *  List of it_<a>_<b>()
 *  - it_pass_fail
 *  - it_fail_fail
 *  - it_skip_fail
 *  - it_pass_skip
 *  - it_fail_skip
 *  - it_skip_skip
 */

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * Colors used for displaying tests
 */
namespace color {
    const std::string black = "\033[30m";
    const std::string red = "\033[31m";
    const std::string green = "\033[32m";
    const std::string yellow = "\033[33m";
    const std::string blue = "\033[34m";
    const std::string cyan = "\033[36m";
    const std::string grey = "\033[90m";

    const std::string bg_red = "\033[41m";
    const std::string bg_green = "\033[42m";
    const std::string bg_yellow = "\033[43m";

    const std::string reset = "\033[0m";
    const std::string bold = "\033[1m";
}

/**
 * Utility functions for tests
 */
namespace utils {
    inline std::string move_left(uint n = 1) {
        return "\033[" + std::to_string(n) + "D";
    }

    inline std::string clear_line() {
        return "\033[0K";
    }

    inline std::string hide_cursor() {
        return "\033[?25l";
    }

    inline std::string show_cursor() {
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

namespace test {
    /**
     * Result of a test
     */
    typedef enum {
        PASS = 0,
        FAIL = 1,
        SKIP = 2
    } Result;

    /**
     * Class Test used to describe and run tests
     */
    class Test {
    public:
        Test() = default;

        /**
         * Describe a new test
         *
         * @param name Name of the test
         * @param func The test itself
         */
        void describe(const std::string &name, const std::function<Result()> &func) {
            _tests[name] = func;
        }

        /**
         * Run all described tests
         *
         * @return Result of tests
         */
        bool run() {
            utils::enable_raw_mode();
            std::cout << utils::hide_cursor();

            std::cout << color::bold << color::blue << " Run " << _tests.size() << " test"
                      << (_tests.size() > 1 ? "s" : "") << color::reset
                      << std::endl;

            int passed = 0;
            before_all();
            for (const auto &test: _tests) {
                std::cout << color::cyan << test.first << color::reset << std::endl;

                before_each();
                auto res = test.second();
                after_each();

                switch (res) {
                    case PASS:
                        std::cout << "  " << color::bg_green << color::black << " PASS " << color::reset << std::endl;
                        passed++;
                        break;
                    case FAIL:
                        std::cout << "  " << color::bg_red << color::black << " FAIL " << color::reset << std::endl;
                        break;
                    case SKIP:
                        std::cout << "  " << color::bg_yellow << color::black << " SKIP " << color::reset << std::endl;
                        passed++;
                        break;
                }
                std::cout << std::endl;
            }
            after_all();

            // Resume
            bool result = passed == _tests.size();
            if (result) {
                std::cout << color::bold << color::green << " All tests passed " << color::reset << std::endl;
            } else {
                std::cout << color::bold << color::red << " " << _tests.size() - passed << " tests failed "
                          << color::reset << std::endl;
            }

            std::cout << utils::show_cursor();
            utils::disable_raw_mode();

            return result;
        }

        /**
         * Function called before each tests
         */
        std::function<void()> before_each = []() {};
        /**
         * Function called after each tests
         */
        std::function<void()> after_each = []() {};
        /**
         * Function called before all tests
         */
        std::function<void()> before_all = []() {};
        /**
         * Function called after all tests
         */
        std::function<void()> after_all = []() {};

    private:
        std::map<std::string, std::function<Result()>> _tests;
    };

// ====================

/**
 * Run a simple test and return the result
 */
#define it(name, func) ([]() {                                                   \
    std::cout << color::grey << "⏲ " << name << color::reset;                    \
    std::cout.flush();                                                           \
    std::streambuf *old = std::cout.rdbuf(nullptr);                              \
    test::Result res = func();                                                   \
    std::cout.rdbuf(old);                                                        \
    std::cout << utils::move_left(1000) << utils::clear_line();                  \
    switch (res) {                                                               \
    case test::PASS:                                                             \
        std::cout << color::green << "✔ " << name << color::reset << std::endl;  \
        break;                                                                   \
    case test::SKIP:                                                             \
        std::cout << color::yellow << "⚠ " << name << color::reset << std::endl; \
        break;                                                                   \
    case test::FAIL:                                                             \
        std::cout << color::red << "✘ " << name << color::reset << std::endl;    \
        break;                                                                   \
    }                                                                            \
    return res;                                                                  \
    })()

/**
 * Derivation of it() that fail if the test is FAIL or SKIP
 */
#define it_pass_fail(name, func) assert_equal(test::PASS, it(name, func))

/**
 * Derivation of it() that fail if the test is PASS or SKIP
 */
#define it_fail_fail(name, func) assert_equal(test::FAIL, it(name, func))

/**
 * Derivation of it() that fail if the test is PASS or FAIL
 */
#define it_skip_fail(name, func) assert_equal(test::SKIP, it(name, func))

/**
 * Derivation of it() that skip if the test is FAIL or SKIP
 */
#define it_pass_skip(name, func) should_equal(test::PASS, it(name, func))

/**
 * Derivation of it() that skip if the test is PASS or SKIP
 */
#define it_fail_skip(name, func) should_equal(test::FAIL, it(name, func))

/**
 * Derivation of it() that skip if the test is PASS or FAIL
 */
#define it_skip_skip(name, func) should_equal(test::SKIP, it(name, func))

// ====================
// Asserts (FAIL if false)

/**
 * Assert that a is equal to b
 */
#define assert_equal(a, b) if (a != b) { return test::FAIL; }
/**
 * Assert that a is not equal to b
 */
#define assert_not_equal(a, b) if (a == b) { return test::FAIL; }
/**
 * Assert that a is less than b
 */
#define assert_less(a, b) if (a >= b) { return test::FAIL; }
/**
 * Assert that a is greater than b
 */
#define assert_great(a, b) if (a <= b) { return test::FAIL; }
/**
 * Assert that a is less or equal to b
 */
#define assert_less_equal(a, b) if (a > b) { return test::FAIL; }
/**
 * Assert that a is greater or equal to b
 */
#define assert_great_equal(a, b) if (a < b) { return test::FAIL; }
/**
 * Assert that a is true
 */
#define assert_true(a) if (!a) { return test::FAIL; }
/**
 * Assert that a is false
 */
#define assert_false(a) if (a) { return test::FAIL; }
/**
 * Assert that a is empty
 */
#define assert_empty(a) if (!a.empty()) { return test::FAIL; }
/**
 * Assert that a is not empty
 */
#define assert_not_empty(a) if (!a.empty()) { return test::FAIL; }

// ====================
// Should (SKIP if false)

/**
 * a should be equal to b
 */
#define should_equal(a, b) if (a != b) { return test::SKIP; }
/**
 * a should not be equal to b
 */
#define should_not_equal(a, b) if (a == b) { return test::SKIP; }
/**
 * a should be less than b
 */
#define should_less(a, b) if (a >= b) { return test::SKIP; }
/**
 * a should be greater than b
 */
#define should_great(a, b) if (a <= b) { return test::SKIP; }
/**
 * a should be less or equal to b
 */
#define should_less_equal(a, b) if (a > b) { return test::SKIP; }
/**
 * a should be greater or equal to b
 */
#define should_great_equal(a, b) if (a < b) { return test::SKIP; }
/**
 * a should be true
 */
#define should_true(a) if (!a) { return test::SKIP; }
/**
 * a should be false
 */
#define should_false(a) if (a) { return test::SKIP; }
/**
 * a should be empty
 */
#define should_empty(a) if (!a.empty()) { return test::SKIP; }
/**
 * a should be not empty
 */
#define should_not_empty(a) if (a.empty()) { return test::SKIP; }

}

#endif // TEST_HPP
