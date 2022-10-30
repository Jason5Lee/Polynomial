#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <utility>
#include "polynomial.hpp"
#include "storage.hpp"

using namespace std;

// Exception for returning to the menu.
class BackToMenuException : exception {
};

// Remove leading and trailing whitespace characters from a string.
void Trim(string &str) {
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}

// Read line repeatedly until it is a valid input.
// `parse` is the functor to convert the string to the desired type.
//      It should throw a runtime_error if the string is invalid.
// When `*main menu*` is entered, BackToMenuException is thrown.
template<typename T, typename U>
T Read(const string &prompt, U parse) {
    for (;;) {
        string str;
        cout << prompt;
        getline(cin, str);
        Trim(str);
        if (str.empty())
            continue;
        if (str == "*main menu*")
            throw BackToMenuException();
        try {
            return parse(str);
        }
        catch (const runtime_error &e) {
            cout << "error: " << e.what() << "!" << endl << endl;
        }
    }
}

// A functor that converts a string to data of the specified type via stringstream.
template<typename T>
class GenericParse {
public:
    explicit GenericParse(string typeName) : typeName(std::move(typeName)) {}

    T operator()(const string &str) {
        stringstream ss;
        ss << str;
        T ret;
        if (ss >> ret && ss.eof())
            return ret;
        else
            throw runtime_error("\"" + str + "\"" + " is not a legal " + typeName);
    }

private:
    const string typeName;
};

Storage<Polynomial> global; // The stored polynomials.

// Convert a string to a polynomial and process assignment statements within it.
Polynomial PolynomialParse(const string &str) {
    auto equalSign = str.find('=');
    if (equalSign != string::npos) { // Assignment statement.
        string varName = str.substr(0, equalSign);
        string polynomial = str.substr(equalSign + 1);
        Trim(varName);
        Trim(polynomial);
        Polynomial tmp = PolynomialParse(polynomial);
        global.assign(varName, tmp);
        return tmp;
    }
    //else
    try {
        return global.getReference(str); // string is a variable
    }
    catch (const runtime_error &) {
        try {
            return Polynomial::parse(str); // string is a polynomial
        }
        catch (const runtime_error &) {
            throw runtime_error(str + "is not a valid polynomial or identifier");
        }
    }
}

void printHelp() {
    cout << "1.Polynomial Addition" << endl
         << "2.Polynomial Subtraction" << endl
         << "3.Multiplying a Polynomial with a Constant" << endl
         << "4.Polynomial and Polynomial Multiplication" << endl
         << "5.Evaluate the Polynomial" << endl
         << "6.Check if two polynomials are equal" << endl
         << "7.Polynomial Derivation" << endl
         << "8.Show all stored polynomials" << endl
         << "9.Help" << endl
         << "0.Exit" << endl
         << "When entering a polynomial, it should in the format of `(coefficient,degree) ...`," << endl
         << "  for example, x^3+2x-2 should be entered as `(1,3)(2,1)(-2,0)`. Whitespaces can be added between numbers and symbols or between symbols." << endl
         << "  You can use an assignment statement to store the polynomial to a variable, for example, `a = (1, 3)(2, 0)`" << endl
         << "  You can use the variable identifier as the input to use the stored polynomial." << endl
         << "  The identifier used for storage consists of letters and is not more than 10 in length." << endl
         << "At any time you can enter `*main menu*` to return to the main menu." << endl;
}

int main() {
    cout << "Welcome to Polynomial Calculator 1.0" << endl;
    printHelp();
    for (;;) {
        try {
            cout << endl;
            int choice = Read<int>("Please enter an action: ", [](const string &str) {
                int a = GenericParse<int>("integer")(str);
                if (a < 0 || a > 9)
                    throw runtime_error("Option " + str + " does not exist!");
                return a;
            });
            if (choice == 0)
                break;
            else if (choice == 9)
                printHelp();
            else if (choice == 5) {
                Polynomial pol = Read<Polynomial>("Please enter a polynomial:", PolynomialParse);
                auto factor = Read<double>("Please enter the value of x:", GenericParse<double>("real number"));
                cout << "Evaluated value: " << pol(factor) << endl;
            } else if (choice == 8) {
                cout << "The currently stored polynomials:" << endl
                     << "Variable    Polynomial" << endl;
                cout.flags(ios::left);
                for (const auto &p : global) {
                    cout << setw(13) << p.first << p.second << endl;
                }
            } else if (choice == 6) {
                Polynomial lhs = Read<Polynomial>("Please enter the first polynomial:", PolynomialParse),
                        rhs = Read<Polynomial>("Please enter a second polynomial:", PolynomialParse);
                cout << "The polynomials" << (lhs == rhs ? "" : " not") << " equal.";
            } else {
                Polynomial result;
                if (choice == 3) {
                    auto factor = Read<double>("Please enter a constant:", GenericParse<double>("real number"));
                    Polynomial rhs = Read<Polynomial>("Please enter a polynomial:", PolynomialParse);
                    result = factor * rhs;
                } else if (choice == 7) {
                    Polynomial pol = Read<Polynomial>("Please enter a polynomial:", PolynomialParse);
                    result = pol.getDerivative();
                } else {
                    Polynomial lhs = Read<Polynomial>("Please enter the first polynomial:", PolynomialParse),
                            rhs = Read<Polynomial>("Please enter a second polynomial:", PolynomialParse);
                    if (choice == 1)
                        result = lhs + rhs;
                    else if (choice == 2)
                        result = lhs - rhs;
                    else if (choice == 4)
                        result = lhs * rhs;
                }
                cout << "Result: " << result << endl;
                bool save = Read<bool>("Store the result?(y/n)", [](const string &str) {
                    if (str == "y" || str == "Y")
                        return true;
                    if (str == "n" || str == "N")
                        return false;
                    throw runtime_error("Please enter y or n.");
                });
                if (save) {
                    Read<void>("Please enter variable name:", [&](const string &str) {
                        global.assign(str, result);
                    });
                }
            }
        } catch (BackToMenuException &) {}
    }

    return 0;
}
