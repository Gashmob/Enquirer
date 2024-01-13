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
#include <enquirer.h>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std;
using namespace ::testing;

namespace utils_char {
    const string arrow_up    = "\e[A";
    const string arrow_down  = "\e[B";
    const string arrow_right = "\e[C";
    const string arrow_left  = "\e[D";

    const string tab = "\t";
    const string del = "\x7F";
}// namespace utils_char

auto execWithCinRedirected(const function<void(stringstream &stream)> &function) -> void {
    stringstream input_stream;
    streambuf *old = cin.rdbuf(input_stream.rdbuf());
    function(input_stream);
    cin.rdbuf(old);
}

TEST(enquirer, auth_1) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "username" << utils_char::arrow_down << "password" << endl;
        auto result = enquirer::auth();

        ASSERT_STREQ("username", result.first.c_str());
        ASSERT_STREQ("password", result.second.c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_up << "password" << utils_char::arrow_up << "username" << endl;
        auto result = enquirer::auth();

        ASSERT_STREQ("username", result.first.c_str());
        ASSERT_STREQ("password", result.second.c_str());
    });
}

TEST(enquirer, auth_2) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "admin" << utils_char::arrow_down << "admin" << endl;
        bool result = enquirer::auth([](const pair<string, string> &crd) {
            return crd.first == "admin" && crd.second == "admin";
        });
        ASSERT_TRUE(result);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "abcde" << utils_char::arrow_down << "abcde" << endl;
        bool result = enquirer::auth([](const pair<string, string> &crd) {
            return crd.first == "admin" && crd.second == "admin";
        });
        ASSERT_FALSE(result);
    });
}

TEST(enquirer, autocomplete) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "a" << utils_char::tab << endl;
        string result = enquirer::autocomplete("", {"abcde"});
        ASSERT_STREQ("abcde", result.c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "a" << utils_char::arrow_down << utils_char::tab << endl;
        string result = enquirer::autocomplete("", {"abcde", "abcde2"});
        ASSERT_STREQ("abcde2", result.c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "a" << utils_char::del << "bonjour" << endl;
        string result = enquirer::autocomplete("", {"abcde", "abcde2"});
        ASSERT_STREQ("bonjour", result.c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "a" << utils_char::arrow_down << utils_char::arrow_down << utils_char::tab << endl;
        string result = enquirer::autocomplete("", {"abcde", "abcdef"});
        ASSERT_STREQ("abcde", result.c_str());
    });
}

TEST(enquirer, confirm) {
    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        bool result = enquirer::confirm("test", false);
        ASSERT_FALSE(result);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        bool result = enquirer::confirm("test", true);
        ASSERT_TRUE(result);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_left << endl;
        bool result = enquirer::confirm("test", false);
        ASSERT_TRUE(result);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_right << endl;
        bool result = enquirer::confirm("test", true);
        ASSERT_FALSE(result);
    });
}

TEST(enquirer, form) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "test" << utils_char::arrow_down
               << "test2" << utils_char::arrow_down
               << "test3" << endl;
        auto answers = enquirer::form(
                "Please provide some informations:",
                {"Firstname",
                 "Lastname",
                 "Username"});
        ASSERT_STREQ("test", answers["Firstname"].c_str());
        ASSERT_STREQ("test2", answers["Lastname"].c_str());
        ASSERT_STREQ("test3", answers["Username"].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_up << "test3"
               << utils_char::arrow_up << "test2"
               << utils_char::arrow_up << "test" << endl;
        auto answers = enquirer::form(
                "Please provide some informations:",
                {"Firstname",
                 "Lastname",
                 "Username"});
        ASSERT_STREQ("test", answers["Firstname"].c_str());
        ASSERT_STREQ("test2", answers["Lastname"].c_str());
        ASSERT_STREQ("test3", answers["Username"].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "test" << utils_char::arrow_down << utils_char::arrow_down
               << "test3" << endl
               << "test2" << endl;
        auto answers = enquirer::form(
                "Please provide some informations:",
                {"Firstname",
                 "Lastname",
                 "Username"});
        ASSERT_STREQ("test", answers["Firstname"].c_str());
        ASSERT_STREQ("test2", answers["Lastname"].c_str());
        ASSERT_STREQ("test3", answers["Username"].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "test" << endl
               << "test2" << endl
               << "test3" << endl;
        auto answers = enquirer::form(
                "Please provide some informations:",
                {"Firstname",
                 "Lastname",
                 "Username"});
        ASSERT_STREQ("test", answers["Firstname"].c_str());
        ASSERT_STREQ("test2", answers["Lastname"].c_str());
        ASSERT_STREQ("test3", answers["Username"].c_str());
    });
}

TEST(enquirer, input) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "Hello world!" << endl;
        string res = enquirer::input("Type something");
        ASSERT_STREQ("Hello world!", res.c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "J" << utils_char::tab << endl;
        string res = enquirer::input("Type something", "John");
        ASSERT_STREQ("John", res.c_str());
    });
}

TEST(enquirer, invisible) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "Hello world!" << endl;
        string res = enquirer::invisible("Type something");
        ASSERT_STREQ("Hello world!", res.c_str());
    });
}

TEST(enquirer, list) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "a" << endl;
        auto res = enquirer::list("Type something");
        ASSERT_THAT(res, SizeIs(1));
        ASSERT_STREQ("a", res[0].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "a, b, c" << endl;
        auto res = enquirer::list("Type something");
        ASSERT_THAT(res, SizeIs(3));
        ASSERT_STREQ("a", res[0].c_str());
        ASSERT_STREQ("b", res[1].c_str());
        ASSERT_STREQ("c", res[2].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "a   ,       b,      c      ,d,e,f" << endl;
        auto res = enquirer::list("Type something");
        ASSERT_THAT(res, SizeIs(6));
        ASSERT_STREQ("a", res[0].c_str());
        ASSERT_STREQ("b", res[1].c_str());
        ASSERT_STREQ("c", res[2].c_str());
        ASSERT_STREQ("d", res[3].c_str());
        ASSERT_STREQ("e", res[4].c_str());
        ASSERT_STREQ("f", res[5].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << ", b, c" << endl;
        auto res = enquirer::list("Type something");
        ASSERT_THAT(res, SizeIs(3));
        ASSERT_STREQ("", res[0].c_str());
        ASSERT_STREQ("b", res[1].c_str());
        ASSERT_STREQ("c", res[2].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "a, , c" << endl;
        auto res = enquirer::list("Type something");
        ASSERT_THAT(res, SizeIs(3));
        ASSERT_STREQ("a", res[0].c_str());
        ASSERT_STREQ("", res[1].c_str());
        ASSERT_STREQ("c", res[2].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << "a, b, " << endl;
        auto res = enquirer::list("Type something");
        ASSERT_THAT(res, SizeIs(3));
        ASSERT_STREQ("a", res[0].c_str());
        ASSERT_STREQ("b", res[1].c_str());
        ASSERT_STREQ("", res[2].c_str());
    });
}

TEST(enquirer, multiSelect) {
    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        auto res = enquirer::multi_select(
                "Choose",
                {"a",
                 "b",
                 "c"});
        ASSERT_THAT(res, IsEmpty());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_right << utils_char::arrow_up
               << utils_char::arrow_right << endl;
        auto res = enquirer::multi_select(
                "Choose",
                {"a",
                 "b",
                 "c"});
        ASSERT_THAT(res, SizeIs(2));
        ASSERT_STREQ("a", res[0].c_str());
        ASSERT_STREQ("c", res[1].c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_up << utils_char::arrow_right
               << utils_char::arrow_up << utils_char::arrow_right
               << utils_char::arrow_up << utils_char::arrow_right
               << endl;
        auto res = enquirer::multi_select(
                "Choose",
                {"a",
                 "b",
                 "c"});
        ASSERT_THAT(res, SizeIs(3));
        ASSERT_STREQ("a", res[0].c_str());
        ASSERT_STREQ("b", res[1].c_str());
        ASSERT_STREQ("c", res[2].c_str());
    });
}

TEST(enquirer, number) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "1" << endl;
        auto res = enquirer::number<int>("Type something");
        ASSERT_EQ(1, res);
    });
}

TEST(enquirer, password) {
    execWithCinRedirected([](stringstream &stream) {
        stream << "p@ssw0rd" << endl;
        auto res = enquirer::password("Type something");
        ASSERT_STREQ("p@ssw0rd", res.c_str());
    });
}

TEST(enquirer, quiz) {
    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        bool res = enquirer::quiz("Which is yellow?", {"Banana", "Coconut", "Strawberry"}, "Banana");
        ASSERT_TRUE(res);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_up << endl;
        bool res = enquirer::quiz("Which is yellow?", {"Banana", "Coconut", "Strawberry"}, "Banana");
        ASSERT_FALSE(res);
    });
}

TEST(enquirer, slider) {
    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        int res = enquirer::slider<int>("Choose", 1, 10, 1, 5);
        ASSERT_EQ(5, res);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_left
               << utils_char::arrow_left
               << utils_char::arrow_left
               << utils_char::arrow_left
               << endl;
        int res = enquirer::slider<int>("Choose", 1, 10, 1, 5);
        ASSERT_EQ(1, res);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_right
               << utils_char::arrow_right
               << utils_char::arrow_right
               << utils_char::arrow_right
               << utils_char::arrow_right
               << endl;
        int res = enquirer::slider<int>("Choose", 1, 10, 1, 5);
        ASSERT_EQ(10, res);
    });
}

TEST(enquirer, select) {
    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        string res = enquirer::select("Choose", {"a", "b", "c"});
        ASSERT_STREQ("a", res.c_str());
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_up << endl;
        string res = enquirer::select("Choose", {"a", "b", "c"});
        ASSERT_STREQ("c", res.c_str());
    });
}

TEST(enquirer, toggle) {
    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        bool res = enquirer::toggle("Choose", "", "", true);
        ASSERT_TRUE(res);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << endl;
        bool res = enquirer::toggle("Choose", "", "", false);
        ASSERT_FALSE(res);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_left
               << endl;
        bool res = enquirer::toggle("Choose", "", "", false);
        ASSERT_TRUE(res);
    });

    execWithCinRedirected([](stringstream &stream) {
        stream << utils_char::arrow_right
               << endl;
        bool res = enquirer::toggle("Choose", "", "", true);
        ASSERT_FALSE(res);
    });
}
