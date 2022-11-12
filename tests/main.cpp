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

    // Input
    string answer = enquirer::input("What is you name?", "John Doe");
    cout << "'" << answer << "'" << endl;

    return 0;
}