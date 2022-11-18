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
        it_pass_fail("default false", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            bool result = enquirer::confirm("test", false);

            cin.rdbuf(old);

            assert_false(result);

            return PASS;
        })

        it_pass_fail("default true", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            bool result = enquirer::confirm("test", true);

            cin.rdbuf(old);

            assert_true(result);

            return PASS;
        })

        it_pass_fail("can confirm true from false", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_left << endl;
            bool result = enquirer::confirm("test", false);

            cin.rdbuf(old);

            assert_true(result);

            return PASS;
        })

        it_pass_fail("can confirm false from true", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_right << endl;
            bool result = enquirer::confirm("test", true);

            cin.rdbuf(old);

            assert_false(result);

            return PASS;
        })

        return PASS;
    });

    test.describe("Form", []() {
        it_pass_fail("can take input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "test" << utils_char::arrow_down
               << "test2" << utils_char::arrow_down
               << "test3" << endl;
            auto answers = enquirer::form("Please provide some informations:", {
                    "Firstname",
                    "Lastname",
                    "Username"
            });

            cin.rdbuf(old);

            assert_equal("test", answers["Firstname"]);
            assert_equal("test2", answers["Lastname"]);
            assert_equal("test3", answers["Username"]);

            return PASS;
        })

        it_pass_fail("can take input in reverse order", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_up << "test3"
               << utils_char::arrow_up << "test2"
               << utils_char::arrow_up << "test" << endl;
            auto answers = enquirer::form("Please provide some informations:", {
                    "Firstname",
                    "Lastname",
                    "Username"
            });

            cin.rdbuf(old);

            assert_equal("test", answers["Firstname"]);
            assert_equal("test2", answers["Lastname"]);
            assert_equal("test3", answers["Username"]);

            return PASS;
        })

        it_pass_fail("fill first and last, then enter and fill second", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "test" << utils_char::arrow_down << utils_char::arrow_down
               << "test3" << endl
               << "test2" << endl;
            auto answers = enquirer::form("Please provide some informations:", {
                    "Firstname",
                    "Lastname",
                    "Username"
            });

            cin.rdbuf(old);

            assert_equal("test", answers["Firstname"]);
            assert_equal("test2", answers["Lastname"]);
            assert_equal("test3", answers["Username"]);

            return PASS;
        })

        it_pass_fail("can complete only with enter", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "test" << endl
               << "test2" << endl
               << "test3" << endl;
            auto answers = enquirer::form("Please provide some informations:", {
                    "Firstname",
                    "Lastname",
                    "Username"
            });

            cin.rdbuf(old);

            assert_equal("test", answers["Firstname"]);
            assert_equal("test2", answers["Lastname"]);
            assert_equal("test3", answers["Username"]);

            return PASS;
        })

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
        it_pass_fail("return the input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "Hello world!" << endl;
            string res = enquirer::invisible("Type something");

            cin.rdbuf(old);

            assert_equal(res, "Hello world!")

            return PASS;
        })

        return PASS;
    });

    test.describe("List", []() {
        it_pass_fail("can take simple input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a" << endl;
            auto res = enquirer::list("Type something");

            cin.rdbuf(old);

            assert_equal(1, res.size());
            assert_equal("a", res[0]);

            return PASS;
        })

        it_pass_fail("can take multiple input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a, b, c" << endl;
            auto res = enquirer::list("Type something");

            cin.rdbuf(old);

            assert_equal(3, res.size());
            assert_equal("a", res[0]);
            assert_equal("b", res[1]);
            assert_equal("c", res[2]);

            return PASS;
        })

        it_pass_fail("can take input with more spaces", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a   ,       b,      c      ,d,e,f" << endl;
            auto res = enquirer::list("Type something");

            cin.rdbuf(old);

            assert_equal(6, res.size());
            assert_equal("a", res[0]);
            assert_equal("b", res[1]);
            assert_equal("c", res[2]);
            assert_equal("d", res[3]);
            assert_equal("e", res[4]);
            assert_equal("f", res[5]);

            return PASS;
        })

        it_pass_fail("can take empty input at begin", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << ", b, c" << endl;
            auto res = enquirer::list("Type something");

            cin.rdbuf(old);

            assert_equal(3, res.size());
            assert_empty(res[0]);
            assert_equal("b", res[1]);
            assert_equal("c", res[2]);

            return PASS;
        })

        it_pass_fail("can take empty input at middle", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a, , c" << endl;
            auto res = enquirer::list("Type something");

            cin.rdbuf(old);

            assert_equal(3, res.size());
            assert_equal("a", res[0]);
            assert_empty(res[1]);
            assert_equal("c", res[2]);

            return PASS;
        })

        it_pass_fail("can take empty input at end", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "a, b, " << endl;
            auto res = enquirer::list("Type something");

            cin.rdbuf(old);

            assert_equal(3, res.size());
            assert_equal("a", res[0]);
            assert_equal("b", res[1]);
            assert_empty(res[2]);

            return PASS;
        })

        return PASS;
    });

    test.describe("MultiSelect", []() {
        it_pass_fail("can take no input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            auto res = enquirer::multi_select("Choose", {
                    "a",
                    "b",
                    "c"
            });

            cin.rdbuf(old);

            assert_empty(res);

            return PASS;
        })

        it_pass_fail("can take several input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_right << utils_char::arrow_up
               << utils_char::arrow_right << endl;
            auto res = enquirer::multi_select("Choose", {
                    "a",
                    "b",
                    "c"
            });

            cin.rdbuf(old);

            assert_equal(2, res.size());
            assert_equal("a", res[0]);
            assert_equal("c", res[1]);

            return PASS;
        })

        it_pass_fail("can take several input in reverse order", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_up << utils_char::arrow_right
               << utils_char::arrow_up << utils_char::arrow_right
               << utils_char::arrow_up << utils_char::arrow_right
               << endl;
            auto res = enquirer::multi_select("Choose", {
                    "a",
                    "b",
                    "c"
            });

            cin.rdbuf(old);

            assert_equal(3, res.size());
            assert_equal("a", res[0]);
            assert_equal("b", res[1]);
            assert_equal("c", res[2]);

            return PASS;
        })

        return PASS;
    });

    test.describe("Number", []() {
        it_pass_fail("can take simple input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "1" << endl;
            auto res = enquirer::number<int>("Type something");

            cin.rdbuf(old);

            assert_equal(1, res);

            return PASS;
        })

        return PASS;
    });

    test.describe("Password", []() {
        it_pass_fail("can take simple input", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << "p@ssw0rd" << endl;
            auto res = enquirer::password("Type something");

            cin.rdbuf(old);

            assert_equal("p@ssw0rd", res);

            return PASS;
        })

        return PASS;
    });

    test.describe("Quiz", []() {
        it_pass_fail("good answer", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            bool res = enquirer::quiz("Which is yellow?", {"Banana", "Coconut", "Strawberry"}, "Banana");

            cin.rdbuf(old);

            assert_true(res);

            return PASS;
        })

        it_pass_fail("wrong answer", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_up << endl;
            bool res = enquirer::quiz("Which is yellow?", {"Banana", "Coconut", "Strawberry"}, "Banana");

            cin.rdbuf(old);

            assert_false(res);

            return PASS;
        })

        return PASS;
    });

    test.describe("Slider", []() {
        it_pass_fail("return initial value", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            int res = enquirer::slider<int>("Choose", 1, 10, 1, 5);

            cin.rdbuf(old);

            assert_equal(5, res);

            return PASS;
        })

        it_pass_fail("return min value", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_left
               << utils_char::arrow_left
               << utils_char::arrow_left
               << utils_char::arrow_left
               << endl;
            int res = enquirer::slider<int>("Choose", 1, 10, 1, 5);

            cin.rdbuf(old);

            assert_equal(1, res);

            return PASS;
        })

        it_pass_fail("return max value", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_right
               << utils_char::arrow_right
               << utils_char::arrow_right
               << utils_char::arrow_right
               << utils_char::arrow_right
               << endl;
            int res = enquirer::slider<int>("Choose", 1, 10, 1, 5);

            cin.rdbuf(old);

            assert_equal(10, res);

            return PASS;
        })

        return PASS;
    });

    test.describe("Select", []() {
        it_pass_fail("return first value", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            string res = enquirer::select("Choose", {"a", "b", "c"});

            cin.rdbuf(old);

            assert_equal("a", res);

            return PASS;
        })

        it_pass_fail("return last value", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_up
               << endl;
            string res = enquirer::select("Choose", {"a", "b", "c"});

            cin.rdbuf(old);

            assert_equal("c", res);

            return PASS;
        })

        return PASS;
    });

    test.describe("Toggle", []() {
        it_pass_fail("return default true", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            bool res = enquirer::toggle("Choose", "", "", true);

            cin.rdbuf(old);

            assert_true(res);

            return PASS;
        })

        it_pass_fail("return default false", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << endl;
            bool res = enquirer::toggle("Choose", "", "", false);

            cin.rdbuf(old);

            assert_false(res);

            return PASS;
        })

        it_pass_fail("return true", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_left
               << endl;
            bool res = enquirer::toggle("Choose", "", "", false);

            cin.rdbuf(old);

            assert_true(res);

            return PASS;
        })

        it_pass_fail("return false", []() {
            stringstream ss;
            streambuf *old = cin.rdbuf(ss.rdbuf());

            ss << utils_char::arrow_right
               << endl;
            bool res = enquirer::toggle("Choose", "", "", true);

            cin.rdbuf(old);

            assert_false(res);

            return PASS;
        })

        return PASS;
    });
}

int main() {
    cout << "Enquirer test, version " << ENQUIRER_VERSION << endl << endl;

    Test test;

    construct_test(test);

    return test.run() ? 0 : 1;
}