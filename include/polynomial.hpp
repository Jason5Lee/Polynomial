#ifndef JASON5LEE_POLYNOMIAL_POLYNOMIAL_H
#define JASON5LEE_POLYNOMIAL_POLYNOMIAL_H

#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <vector>

// A polynomial term
class Term {
public:
    // Exception when trying to construct an term with a negative degree
    struct NegativeDegreeException : std::runtime_error {
        NegativeDegreeException() : std::runtime_error("The degree cannot be negative") {}
    };

    Term(double coe = 0, int deg = 0) : coefficient(coe), degree(deg) {
        if (deg < 0) throw NegativeDegreeException();
    }

    // getters
    double getCoefficient() const { return coefficient; }
    int getDegree() const { return degree; }

    // Determine whether two items are of the same type, and if so, combine the
    // two items.
    bool tryMergeWith(const Term &other);

    // Get the derivative of this term.
    Term getDerivative() const;

    bool operator==(const Term &) const;

    // Determine whether the coefficient is 0.
    bool useless() const;

private:
    double coefficient;
    int degree;
};

class Polynomial {
public:
    // Constructs an empty polynomial, respresenting the constant 0.
    Polynomial() = default;

    // Construct a constant.
    Polynomial(double);

    // Construct a general polynomial from an initialization list.
    Polynomial(std::initializer_list<Term>);

    // Determine if it is an empty polynomial.
    bool empty() const;

    // Get the derivative.
    Polynomial getDerivative() const;

    bool operator==(const Polynomial &) const;

    friend Polynomial operator+(const Polynomial &, const Polynomial &);
    friend Polynomial operator-(const Polynomial &, const Polynomial &);

    // Since constant (double value) can be implicitly converted to Polynomial
    // with the constructor, extra multiplication operator for multiplying
    // polynomials by constants is not needed.
    friend Polynomial operator*(const Polynomial &, const Polynomial &);

    // Evaluation.
    double operator()(double x) const;

    // Read the polynomial from the input stream.
    friend std::istream &operator>>(std::istream &, Polynomial &);
    // Output the polynomial to the output stream.
    friend std::ostream &operator<<(std::ostream &, const Polynomial &);

private:
    std::vector<Term> poly;
    typedef decltype(poly.size()) size_t;

    // Arrange up polynomials.
    // It sorts the terms in descending order of degree, merge the same-degree
    // terms, and delete the terms with a coefficient of 0.
    void arrange();
};

#endif  // JASON5LEE_POLYNOMIAL_POLYNOMIAL_H
