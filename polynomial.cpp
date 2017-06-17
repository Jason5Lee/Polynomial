#include "polynomial.h"
#include<algorithm>
#include<initializer_list>
#include<cmath>
#include<sstream>
#include<cstdio>

using namespace std;

//判断x是否足够小可以认为为0
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
    return isZero(coefficient - other.coefficient)
           && degree == other.degree;
}

Term Term::getDerivative() const {
    if (degree == 0)
        return Term(0);
    else
        return Term(coefficient * degree, degree - 1);
}

bool Term::useless() const {
    return isZero(coefficient);
}

Polynomial::Polynomial(double num) {
    if (!isZero(num))
        poly.push_back(Term(num, 0));
}

Polynomial::Polynomial(initializer_list<Term> terms) : poly(terms) {
    arrange();
}

Polynomial Polynomial::parse(const string &expr) {
    const char *ptr = expr.c_str();
    Polynomial result;
    for (;;) {
        double coefficient;
        int degree;
        char end;
        switch (sscanf(ptr, " (%lf,%d %c", &coefficient, &degree, &end)) {
            case 3:
                if (end != ')')
                    throw PolynomialFormatException();
                result.poly.push_back(Term(coefficient, degree));
                while (*(ptr++) != ')');
                break;
            case EOF:
                result.arrange();
                return result;
            default:
                throw PolynomialFormatException();
        }
    }
}

bool Polynomial::operator==(const Polynomial &p) const {
    return poly == p.poly;
}

Polynomial operator+(const Polynomial &a, const Polynomial &b) {
    Polynomial sum;
    for (auto &term : a.poly)
        sum.poly.push_back(term);
    for (auto &term : b.poly)
        sum.poly.push_back(term);
    sum.arrange();
    return sum;
}

Polynomial operator-(const Polynomial &a, const Polynomial &b) {
    Polynomial sub;
    for (auto &term : a.poly)
        sub.poly.push_back(term);
    for (auto &term : b.poly)
        sub.poly.push_back(Term(-term.getCoefficient(), term.getDegree()));
    sub.arrange();
    return sub;
}

Polynomial operator*(const Polynomial &a, const Polynomial &b) {
    Polynomial mul;
    for (auto &term1 : a.poly)
        for (auto &term2 : b.poly)
            mul.poly.push_back(Term(
                    term1.getCoefficient() * term2.getCoefficient(),
                    term1.getDegree() + term2.getDegree()));
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

ostream &operator<<(ostream &out, const Polynomial &p) {
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
            if (!isZero(absCoefficient - 1) || term.getDegree() == 0)
                out << absCoefficient;
            if (term.getDegree() > 0) {
                out << "x";
                if (term.getDegree() != 1)
                    out << "^" << term.getDegree();
            }
            first = false;
        }
    }
    return out;
}

void Polynomial::arrange() {
    if (!empty()) {
        //将多项式按照次数从大到小排序
        sort(poly.begin(), poly.end(), [](const Term &a, const Term &b) {
            return a.getDegree() > b.getDegree();
        });
        auto head = poly.begin();
        auto tail = head + 1;
        while (tail < poly.end()) {
            if (head->tryMergeWith(*tail)) {}
            else {
                if (!head->useless())
                    ++head;
                *head = *tail;
            }
            ++tail;
        }
        if (!head->useless())
            ++head;
        poly.erase(head, poly.end());
    }
}

bool Polynomial::empty() const {
    return poly.empty();
}

Polynomial Polynomial::getDerivative() const {
    Polynomial pol;
    for (auto &term : poly)
        pol.poly.push_back(term.getDerivative());
    pol.arrange();
    return pol;
}