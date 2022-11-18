/**
 *     === Enquirer ===
 * Created by Kevin Traini
 *      Under GPL-3.0
 * -----------------------
 */
#include <iostream>
#include "enquirer.hpp"
#include "test.hpp"

using namespace std;
using namespace test;

namespace utils_char {
    const string arrow_up = "\e[A";
    const string arrow_down = "\e[B";
    const string arrow_right = "\e[C";
    const string arrow_left = "\e[D";

    const string tab = "\t";
    const string del = "\x7F";
}

void construct_test(Test &test) {
    test.describe("Auth 1", []() {
        it_pass_fail("return username and password", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "username" << utils_char::arrow_down << "password" << endl;
            auto result = enquirer::auth();

            cin.rdbuf(old);

            assert_equal("username", result.first);
            assert_equal("password", result.second);

            return PASS;
        })

        it_pass_skip("type password then username", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_up << "password" << utils_char::arrow_up << "username" << endl;
            auto result = enquirer::auth();

            cin.rdbuf(old);

            assert_equal("username", result.first);
            assert_equal("password", result.second);

            return PASS;
        })

        return PASS;
    });

    test.describe("Auth 2", []() {
        it_pass_fail("correct credentials", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "admin" << utils_char::arrow_down << "admin" << endl;
            bool result = enquirer::auth([](const pair<string, string> &crd) {
                return crd.first == "admin" && crd.second == "admin";
            });

            cin.rdbuf(old);

            assert_true(result);

            return PASS;
        })

        it_pass_fail("wrong credentials", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "abcde" << utils_char::arrow_down << "abcde" << endl;
            bool result = enquirer::auth([](const pair<string, string> &crd) {
                return crd.first == "admin" && crd.second == "admin";
            });

            cin.rdbuf(old);

            assert_false(result);

            return PASS;
        })

        return PASS;
    });

    test.describe("Autocomplete", []() {
        it_pass_fail("can complete first", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a" << utils_char::tab << endl;
            string result = enquirer::autocomplete("", {"abcde"});

            cin.rdbuf(old);

            assert_equal("abcde", result);

            return PASS;
        })

        it_pass_fail("can complete second", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a" << utils_char::arrow_down << utils_char::tab << endl;
            string result = enquirer::autocomplete("", {"abcde", "abcde2"});

            cin.rdbuf(old);

            assert_equal("abcde2", result);

            return PASS;
        })

        it_pass_fail("can take other input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a" << utils_char::del << "bonjour" << endl;
            string result = enquirer::autocomplete("", {"abcde", "abcde2"});

            cin.rdbuf(old);

            assert_equal("bonjour", result);

            return PASS;
        })

        it_pass_fail("can complete first passing by second", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a" << utils_char::arrow_down << utils_char::arrow_down << utils_char::tab << endl;
            string result = enquirer::autocomplete("", {"abcde", "abcdef"});

            cin.rdbuf(old);

            assert_equal("abcde", result);

            return PASS;
        })

        return PASS;
    });

    test.describe("Confirm", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Form", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Input", []() {
        it_pass_fail("return the input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "Hello world!" << endl;
            string res = enquirer::input("Type something");

            cin.rdbuf(old);

            assert_equal(res, "Hello world!")

            return PASS;
        })

        it_pass_fail("complete default value", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "J" << utils_char::tab << endl;
            string res = enquirer::input("Type something", "John");

            cin.rdbuf(old);

            assert_equal(res, "John")

            return PASS;
        })

        return PASS;
    });

    test.describe("Invisible", []() {
        return SKIP;

        return PASS;
    });

    test.describe("List", []() {
        return SKIP;

        return PASS;
    });

    test.describe("MultiSelect", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Number", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Password", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Quiz", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Slider", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Select", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Toggle", []() {
        return SKIP;

        return PASS;
    });
}

int main() {
    cout << "Enquirer test, version " << ENQUIRER_VERSION << endl << endl;

    Test test;

    construct_test(test);

    return test.run() ? 0 : 1;
}