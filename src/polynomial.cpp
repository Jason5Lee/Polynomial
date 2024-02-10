#include "polynomial.hpp"

#include <algorithm>
#include <cmath>
#include <initializer_list>

// Determine if x is small enough to be considered 0.
bool isZero(double x) {
    const double eps = 1e-7;
    return fabs(x) < eps;
}

bool Term::tryMergeWith(const Term &other) {
    if (degree == other.degree) {
        coefficient += other.coefficient;
        return true;
    }
    return false;
}

bool Term::operator==(const Term &other) const {
    return isZero(coefficient - other.coefficient) && degree == other.degree;
}

Term Term::getDerivative() const {
    if (degree == 0)
        return Term(0);
    else
        return Term(coefficient * degree, degree - 1);
}

bool Term::useless() const { return isZero(coefficient); }

Polynomial::Polynomial(double num) {
    if (!isZero(num)) poly.push_back(Term(num, 0));
}

Polynomial::Polynomial(std::initializer_list<Term> terms) : poly(terms) { arrange(); }

bool Polynomial::operator==(const Polynomial &p) const { return poly == p.poly; }

Polynomial operator+(const Polynomial &a, const Polynomial &b) {
    Polynomial sum;
    for (auto &term : a.poly) sum.poly.push_back(term);
    for (auto &term : b.poly) sum.poly.push_back(term);
    sum.arrange();
    return sum;
}

Polynomial operator-(const Polynomial &a, const Polynomial &b) {
    Polynomial sub;
    for (auto &term : a.poly) sub.poly.push_back(term);
    for (auto &term : b.poly) sub.poly.push_back(Term(-term.getCoefficient(), term.getDegree()));
    sub.arrange();
    return sub;
}

Polynomial operator*(const Polynomial &a, const Polynomial &b) {
    Polynomial mul;
    for (auto &term1 : a.poly)
        for (auto &term2 : b.poly)
            mul.poly.push_back(
                Term(term1.getCoefficient() * term2.getCoefficient(), term1.getDegree() + term2.getDegree()));
    mul.arrange();
    return mul;
}

double Polynomial::operator()(double x) const {
    double yield = 0;
    for (auto &term : poly) {
        yield += term.getCoefficient() * pow(x, term.getDegree());
    }
    return yield;
}

std::istream &operator>>(std::istream &in, Polynomial &p) {
    double coe, deg;
    const auto tryParseXDeg = [&]() -> bool {
        if (in.peek() == 'x') {
            in.ignore();
            in >> std::ws;
            if (in.peek() == '^') {
                in.ignore();
                auto expectExp = in.tellg();
                if (!(in >> deg) || deg < 0) {
                    in.seekg(expectExp);
                    in.setstate(std::ios::failbit);
                    return false;
                }
            } else {
                if (in.eof()) {
                    in.clear();
                }
                deg = 1;
            }
            return true;
        }
        if (in.eof()) {
            in.clear();
        }
        return false;
    };

    const auto parseTerm = [&]() {
        auto start = in.tellg();
        if (in >> coe) {
            if (!tryParseXDeg()) {
                if (in.fail()) {
                    return;
                }
                deg = 0;
            }
        } else {
            in.clear();
            in.seekg(start);
            if (in.peek() == 'x') {
                coe = 1;
            } else if (in.peek() == '+') {
                coe = 1;
                in.ignore();
            } else if (in.peek() == '-') {
                coe = -1;
                in.ignore();
            } else {
                in.setstate(std::ios::failbit);
                return;
            }

            if (!tryParseXDeg()) {
                in.setstate(std::ios::failbit);
                return;
            }
        }
    };

    p = Polynomial();
    parseTerm();
    if (!in) {
        return in;
    }
    p.poly.push_back(Term(coe, deg));
    while (true) {
        in >> std::ws;
        char op;
        if (!(in >> op)) {
            if (in.eof()) {
                in.clear();
            }
            break;
        }

        if (op != '+' && op != '-') {
            in.setstate(std::ios::failbit);
            break;
        }
        double opNum = op == '+' ? 1 : -1;
        in >> std::ws;
        parseTerm();
        if (!in) {
            break;
        }
        p.poly.push_back(Term(coe * opNum, deg));
    }
    p.arrange();
    return in;
}

std::ostream &operator<<(std::ostream &out, const Polynomial &p) {
    if (p.empty())
        out << '0';
    else {
        bool first = true;
        for (auto &term : p.poly) {
            if (term.getCoefficient() < 0)
                out << "-";
            else if (term.getCoefficient() > 0 && !first)
                out << "+";
            double absCoefficient = fabs(term.getCoefficient());
            if (!isZero(absCoefficient - 1) || term.getDegree() == 0) out << absCoefficient;
            if (term.getDegree() > 0) {
                out << "x";
                if (term.getDegree() != 1) out << "^" << term.getDegree();
            }
            first = false;
        }
    }
    return out;
}

void Polynomial::arrange() {
    if (!empty()) {
        // Sort polynomials in descending order of degree.
        sort(poly.begin(), poly.end(), [](const Term &a, const Term &b) { return a.getDegree() > b.getDegree(); });
        auto head = poly.begin();
        auto tail = head + 1;
        while (tail < poly.end()) {
            if (head->tryMergeWith(*tail)) {
            } else {
                if (!head->useless()) ++head;
                *head = *tail;
            }
            ++tail;
        }
        if (!head->useless()) ++head;
        poly.erase(head, poly.end());
    }
}

bool Polynomial::empty() const { return poly.empty(); }

Polynomial Polynomial::getDerivative() const {
    Polynomial pol;
    for (auto &term : poly) pol.poly.push_back(term.getDerivative());
    pol.arrange();
    return pol;
}
