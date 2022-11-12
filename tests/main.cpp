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

    string enquirer = "select";

    if (enquirer == "input") {
        string answer = enquirer::input("What is you name?", "John Doe");
        cout << "'" << answer << "'" << endl;
    } else if (enquirer == "invisible") {
        string answer = enquirer::invisible("What is you secret?");
        cout << "'" << answer << "'" << endl;
    } else if (enquirer == "select") {
        string answer = enquirer::select("What is your favorite color?", {"Red", "Green", "Blue"});
        cout << "'" << answer << "'" << endl;
    }

    return 0;
}