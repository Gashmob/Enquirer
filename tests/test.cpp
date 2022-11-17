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
}

int main() {
    cout << "Enquirer test, version " << ENQUIRER_VERSION << endl << endl;

    Test test;

    construct_test(test);

    return test.run() ? 0 : 1;
}