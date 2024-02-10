#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "polynomial.hpp"
#include "storage.hpp"

class AbortOperationException : public std::runtime_error {
public:
    AbortOperationException() : std::runtime_error("Operation aborted") {}
};

class StandardInputEOFException : public std::runtime_error {
public:
    StandardInputEOFException() : std::runtime_error("Standard Input reaches EOF") {}
};

std::string concat_strings(const char *before, std::string_view str, const char *after) {
    std::string ret;
    ret.reserve(strlen(before) + str.size() + strlen(after));
    ret.append(before);
    ret.append(str.data(), str.size());
    ret.append(after);
    return ret;
}

// Returns a string with leading and trailing whitespace characters removed.
std::string_view trim(const std::string_view str) {
    // Characters considered as whitespace
    const std::string WHITESPACE = " \n\r\t\f\v";

    // Find the first character position after excluding leading whitespace characters
    size_t start = str.find_first_not_of(WHITESPACE);

    // If the string is all whitespace characters, return an empty string
    if (start == std::string::npos) return "";

    // Find the last character position after excluding trailing whitespace characters
    size_t end = str.find_last_not_of(WHITESPACE);

    // Return the substring with leading and trailing whitespace characters removed
    return str.substr(start, end - start + 1);
}

// Read line repeatedly until it is a valid input.
// `parse` is the functor to convert the string to the desired type.
//      It should throw a runtime_error if the string is invalid.
// When `*main menu*` is entered, AbortException is thrown.
template <typename T, typename U>
T readInput(const std::string_view prompt, U parse) {
    while (!std::cin.eof()) {
        std::string line;
        std::cout << prompt;
        getline(std::cin, line);
        std::string_view str = trim(line);
        if (str.empty()) continue;
        if (str == "*main menu*")
            // If the user wants to return to the main menu, abort the current operation.
            throw AbortOperationException();
        try {
            return parse(str);
        } catch (const std::runtime_error &e) {
            std::cout << e.what() << std::endl << std::endl;
        }
    }
    throw StandardInputEOFException();
}

// A functor that converts a string to data of the specified type via stringstream.
template <typename T>
class GenericParse {
public:
    explicit GenericParse(std::string &&typeName) : typeName(std::move(typeName)) {}

    T operator()(const std::string_view str) {
        std::stringstream ss;
        ss << str;
        T ret;
        if (ss >> ret && ss.eof()) {
            return ret;
        } else {
            throw std::runtime_error(concat_strings("Please enter a valid ", typeName, "."));
        }
    }

private:
    const std::string typeName;
};

// Convert a string to a polynomial and process assignment statements within it.
Polynomial polynomialParse(Storage<Polynomial> &storage, const std::string_view str) {
    auto equalSign = str.find('=');
    if (equalSign != std::string_view::npos) {  // Assignment statement.
        auto varName = trim(str.substr(0, equalSign));
        auto polynomial = trim(str.substr(equalSign + 1));
        Polynomial tmp = polynomialParse(storage, polynomial);
        storage.assign(std::string{varName}, tmp);
        return tmp;
    }
    // else
    try {
        return storage.getReference(str);  // string is a variable
    } catch (const std::runtime_error &) {
        Polynomial poly;
        std::stringstream ss;
        ss << str;
        if ((ss >> poly).fail()) {
            throw std::runtime_error("Please input a valid polynomial with assignment, or an existing identifier");
        }
        return poly;
    }
}

void printHelp() {
    std::cout
        << "1.Polynomial Addition" << std::endl
        << "2.Polynomial Subtraction" << std::endl
        << "3.Multiplying a Polynomial with a Constant" << std::endl
        << "4.Polynomial and Polynomial Multiplication" << std::endl
        << "5.Evaluate the Polynomial" << std::endl
        << "6.Check if two polynomials are equal" << std::endl
        << "7.Polynomial Derivation" << std::endl
        << "8.Show all stored polynomials" << std::endl
        << "9.Help" << std::endl
        << "0.Exit" << std::endl
        << "Example polynomial: -x^3 + 2x^4 - x^6 + x - 24." << std::endl
        << "  You can use an assignment statement to store the polynomial to a variable when entering a polynomial,"
        << std::endl
        << "  for example, `a = x^3 + 2`." << std::endl
        << "  You can use the variable identifier as the input to use the stored polynomial." << std::endl
        << "  The identifier used for storage consists of letters and is not more than 10 in length." << std::endl
        << "At any time you can enter `*main menu*` to return to the main menu." << std::endl;
}

int main() {
    std::cout << "Welcome to Polynomial Calculator 1.0" << std::endl;
    printHelp();

    Storage<Polynomial> storage;
    auto parsePoly = [&](const std::string_view str) { return polynomialParse(storage, str); };
    try {
        for (;;) {
            try {
                std::cout << std::endl;
                int choice = readInput<int>("Please enter an action: ", [](const std::string_view str) {
                    int a = GenericParse<int>("integer")(str);
                    if (a < 0 || a > 9) {
                        throw std::runtime_error("Please enter a number between 0 and 9.");
                    }
                    return a;
                });
                if (choice == 0)
                    break;
                else if (choice == 9)
                    printHelp();
                else if (choice == 5) {
                    Polynomial pol = readInput<Polynomial>("Please enter a polynomial:", parsePoly);
                    auto factor =
                        readInput<double>("Please enter the value of x:", GenericParse<double>("real number"));
                    std::cout << "Evaluated value: " << pol(factor) << std::endl;
                } else if (choice == 8) {
                    std::cout << "The currently stored polynomials:" << std::endl
                              << "Variable    Polynomial" << std::endl;
                    std::cout.flags(std::ios::left);
                    for (const auto &p : storage) {
                        std::cout << std::setw(13) << p.first << p.second << std::endl;
                    }
                } else if (choice == 6) {
                    Polynomial lhs = readInput<Polynomial>("Please enter the first polynomial:", parsePoly),
                               rhs = readInput<Polynomial>("Please enter a second polynomial:", parsePoly);
                    std::cout << "The polynomials" << (lhs == rhs ? "" : " not") << " equal.";
                } else {
                    Polynomial result;
                    if (choice == 3) {
                        auto factor =
                            readInput<double>("Please enter a constant:", GenericParse<double>("real number"));
                        Polynomial rhs = readInput<Polynomial>("Please enter a polynomial:", parsePoly);
                        result = factor * rhs;
                    } else if (choice == 7) {
                        Polynomial pol = readInput<Polynomial>("Please enter a polynomial:", parsePoly);
                        result = pol.getDerivative();
                    } else {
                        Polynomial lhs = readInput<Polynomial>("Please enter the first polynomial:", parsePoly),
                                   rhs = readInput<Polynomial>("Please enter a second polynomial:", parsePoly);
                        if (choice == 1)
                            result = lhs + rhs;
                        else if (choice == 2)
                            result = lhs - rhs;
                        else if (choice == 4)
                            result = lhs * rhs;
                    }
                    std::cout << "Result: " << result << std::endl;
                    bool save = readInput<bool>("Store the result?(y/n)", [](const std::string_view str) {
                        if (str == "y" || str == "Y") return true;
                        if (str == "n" || str == "N") return false;
                        throw std::runtime_error("Please enter y or n.");
                    });
                    if (save) {
                        readInput<void>("Please enter variable name:",
                                        [&](const std::string_view str) { storage.assign(str, result); });
                    }
                }
            } catch (AbortOperationException &) {
            }
        }
    } catch (StandardInputEOFException &) {
        std::cout << "Standard Input reaches EOF" << std::endl;
    }

    return 0;
}
