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

void construct_test(Test &test) {
    test.describe("Auth", []() {
        return SKIP;

        return PASS;
    });

    test.describe("Autocomplete", []() {
        return SKIP;

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

            ss << "J" << '\t' << endl;
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