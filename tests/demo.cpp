/**
 *     === Enquirer ===
 * Created by Kevin Traini
 *      Under GPL-3.0
 * -----------------------
 */
#include <iostream>
#include "enquirer.hpp"

using namespace std;

int main() {
    cout << "Test Enquirer included version " << ENQUIRER_VERSION << endl;

    do {
        string choice = enquirer::select("Choose a function to test:", {
                "auth",
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
                "toggle"
        });

        if (choice == "auth") {
            bool authed = enquirer::auth([](const pair<string, string> &crd) {
                return crd.first == "admin" && crd.second == "admin";
            });
            cout << "'" << authed << "'" << endl;
        } else if (choice == "autocomplete") {
            string answer = enquirer::autocomplete("What is you favorite fruit", {
                    "Apple",
                    "Banana",
                    "Blueberry",
                    "Cherry",
                    "Orange",
                    "Pear",
                    "Raspberry",
                    "Strawberry"
            });
            cout << "'" << answer << "'" << endl;
        } else if (choice == "confirm") {
            bool answer = enquirer::confirm("Do you like this library?", true);
            cout << "'" << answer << "'" << endl;
        } else if (choice == "form") {
            auto answers = enquirer::form("Please provide some informations:", {
                    "Firstname",
                    "Lastname",
                    "Username"
            });
        } else if (choice == "input") {
            string answer = enquirer::input("What is you name?", "John Doe");
            cout << "'" << answer << "'" << endl;
        } else if (choice == "invisible") {
            string answer = enquirer::invisible("What is you secret?");
            cout << "'" << answer << "'" << endl;
        } else if (choice == "list") {
            auto answer = enquirer::list("Type comma separated keywords");
            for (const auto &keyword: answer) {
                cout << "'" << keyword << "' ";
            }
            cout << endl;
        } else if (choice == "multiselect") {
            auto answer = enquirer::multi_select("Choose your favorite colors:", {
                    "red",
                    "green",
                    "blue",
                    "yellow",
                    "orange",
                    "purple",
                    "black",
                    "white",
            });
            for (const auto &keyword: answer) {
                cout << "'" << keyword << "' ";
            }
            cout << endl;
        } else if (choice == "number") {
            auto n = enquirer::number<float>("Enter a number");
            cout << "'" << n << "'" << endl;
        } else if (choice == "password") {
            string pwd = enquirer::password("Enter your password");
            cout << "'" << pwd << "'" << endl;
        } else if (choice == "quiz") {
            bool answer = enquirer::quiz("Which is yellow?", {"Banana", "Coconut", "Strawberry"}, "Banana");
            cout << "'" << answer << "'" << endl;
        } else if (choice == "slider") {
            auto n = enquirer::slider<int>("Choose a number between 1 and 1000", 1, 1000, 1, 500);
            cout << "'" << n << "'" << endl;
        } else if (choice == "select") {
            string answer = enquirer::select("What is your favorite color?", {"Red", "Green", "Blue"});
            cout << "'" << answer << "'" << endl;
        } else if (choice == "toggle") {
            bool answer = enquirer::toggle("Light?", "On", "Off", true);
            cout << "Switch " << (answer ? "on" : "off") << " the light" << endl;
        }
    } while (enquirer::confirm("Do you want to test another function?", true));

    return 0;
}