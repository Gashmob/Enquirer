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
                "input",
                "invisible",
                "select",
                "confirm"
        });

        if (choice == "input") {
            string answer = enquirer::input("What is you name?", "John Doe");
            cout << "'" << answer << "'" << endl;
        } else if (choice == "invisible") {
            string answer = enquirer::invisible("What is you secret?");
            cout << "'" << answer << "'" << endl;
        } else if (choice == "select") {
            string answer = enquirer::select("What is your favorite color?", {"Red", "Green", "Blue"});
            cout << "'" << answer << "'" << endl;
        } else if (choice == "confirm") {
            bool answer = enquirer::confirm("Do you like this library?", true);
            cout << "'" << answer << "'" << endl;
        }
    } while (enquirer::confirm("Do you want to test another function?", true));

    return 0;
}