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

int main() {
    cout << "Enquirer test, version " << ENQUIRER_VERSION << endl;

    return run_tests() ? 0 : 1;
}