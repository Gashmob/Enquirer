/**
 * MIT License
 *
 * Copyright (c) 2023 Kevin Traini
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
#include <iostream>

using namespace std;

auto main() -> int {
    cout << "Enquirer demo, version " << ENQUIRER_VERSION << endl;

    do {
        string choice = enquirer::select(
                "Choose a function to test:",
                {"auth",
                 "autocomplete",
                 "confirm",
                 "form",
                 "input",
                 "invisible",
                 "list",
                 "multiselect",
                 "number",
                 "password",
                 "quiz",
                 "slider",
                 "select",
                 "toggle"});

        if (choice == "auth") {
            bool is_valid = enquirer::auth([](const std::pair<std::string, std::string> &credentials) {
                return credentials.first == "admin" && credentials.second == "admin";
            });
            if (is_valid) {
                cout << "You are successfully authenticated!" << endl;
            } else {
                cout << "Invalid credentials!" << endl;
            }
        } else if (choice == "autocomplete") {
            string answer = enquirer::autocomplete(
                    "What is you favorite fruit",
                    {"Apple",
                     "Banana",
                     "Blueberry",
                     "Cherry",
                     "Orange",
                     "Pear",
                     "Raspberry",
                     "Strawberry"});
            cout << "Your favorite fruit is " << answer << endl;
        } else if (choice == "confirm") {
            bool quit = false;
            while (!quit) {
                quit = enquirer::confirm("Do you want to quit?");
            }
        } else if (choice == "form") {
            auto answers = enquirer::form("Please provide some informations:", {"Firstname",
                                                                                "Lastname",
                                                                                "Username"});
            cout << "Hi " << answers["Firstname"] << " " << answers["Lastname"] << ", also called "
                 << answers["Username"] << "!" << endl;
        } else if (choice == "input") {
            string answer = enquirer::input("What is your name?", "John Doe");
            cout << "Hi " << answer << "!" << endl;
        } else if (choice == "invisible") {
            string secret = enquirer::invisible("What is your secret?");
            cout << "Sorry, but I can't keep secrets! " << secret << endl;
        } else if (choice == "list") {
            auto keywords = enquirer::list("Type comma separated keywords");
            cout << "You typed " << keywords.size() << " keywords:" << endl;
            for (const auto &keyword: keywords) {
                cout << "'" << keyword << "' ";
            }
            cout << endl;
        } else if (choice == "multiselect") {
            auto choices = enquirer::multi_select(
                    "Choose some colors",
                    {"Red",
                     "Green",
                     "Blue",
                     "Yellow",
                     "Magenta",
                     "Cyan",
                     "White",
                     "Black"});
            cout << "You chose " << choices.size() << " colors:" << endl;
            for (const auto &c: choices) {
                cout << "'" << c << "' ";
            }
            cout << endl;
        } else if (choice == "number") {
            auto pi = enquirer::number<double>("What is the value of PI?");
            cout << "For you PI is " << pi << endl;
        } else if (choice == "password") {
            auto pwd = enquirer::password("What is your password?");
            cout << "Your password count " << pwd.size() << " characters" << endl;
        } else if (choice == "quiz") {
            if (enquirer::quiz("Which is yellow?", {"Banana", "Coconut", "Strawberry"}, "Banana"))
                cout << "Good answer!" << endl;
            else
                cout << "Bad answer!" << endl;
        } else if (choice == "slider") {
            int value = enquirer::slider<int>("How much do you want?", 0, 10, 1, 1);
            cout << "You want " << value << " potatoes" << endl;
        } else if (choice == "select") {
            auto language = enquirer::select("Which is the best one?", {"c++",
                                                                        "python",
                                                                        "java"});
            if (language == "c++")
                cout << "You are right!" << endl;
            else
                cout << "It's your choice, but I prefer c++" << endl;
        } else if (choice == "toggle") {
            bool light = enquirer::toggle("Light?", "On", "Off", true);
            cout << "Switch " << (light ? "on" : "off") << " the light" << endl;
        }
    } while (enquirer::confirm("Do you want to test another function?", true));

    return 0;
}