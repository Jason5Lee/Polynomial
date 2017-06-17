#ifndef PROJECT3_POLYNOMIAL_H
#define PROJECT3_POLYNOMIAL_H

#include<string>
#include<initializer_list>
#include<vector>
#include<iostream>
#include<stdexcept>
#include "io_helper.h"

//多项式中的某一项
class Term {
public:
    //尝试构造次数小于0的项时抛出该异常
    struct NegativeDegreeException: std::runtime_error {
        NegativeDegreeException(): std::runtime_error("项的次数不能为负") {}
    };

    Term(double coe = 0, int deg = 0)
            : coefficient(coe), degree(deg) {
        if (deg < 0)
            throw NegativeDegreeException();
    }

    //访问器
    double getCoefficient() const { return coefficient; }
    int getDegree() const { return degree; }

    //判断两项是否为同类项，如果是则合并这两项
    bool tryMergeWith(const Term &other);

    //求导
    Term getDerivative() const;

    //判断两项是否相等
    bool operator==(const Term &) const;

    //判断该项的系数是否为0
    bool useless() const;
private:
    double coefficient;
    int degree;
};

class Polynomial {
public:
    //字符串解析失败的异常
    struct PolynomialFormatException: FormatException {
        PolynomialFormatException(const std::string& str): FormatException(str, "多项式") {}
    };

    //无参构造函数，构造一个空的多项式，即常数0。
    Polynomial() = default;

    //构造常数项式。
    Polynomial(double);

    //通过初始化列表构造一般的多项式。
    Polynomial(std::initializer_list<Term>);

    //将数对形式的多项式字符串解析成多项式对象
    static Polynomial parse(const std::string &);

    //判断是否是空多项式
    bool empty() const;

    //求导
    Polynomial getDerivative() const;

    //判断两个多项式是否相等
    bool operator==(const Polynomial &) const;

    //多项式加法
    friend Polynomial operator+(const Polynomial &, const Polynomial &);

    //多项式减法
    friend Polynomial operator-(const Polynomial &, const Polynomial &);

    //多项式乘法
    //由于常数（double类型）能借助构造函数隐式转换为Polynomial类型
    //我们不需要额外的多项式乘以常数的乘法运算符
    friend Polynomial operator*(const Polynomial &, const Polynomial &);

    //代入求值
    double operator()(double x) const;

    //以友好的形式输出多项式
    friend std::ostream &operator<<(std::ostream &, const Polynomial &);

private:
    //储存整个多项式
    std::vector<Term> poly;
    typedef decltype(poly.size()) size_t;

    //对多项式进行整理。即按幂降序排序，合并同类型，并删除系数为0的项。
    void arrange();
};
#endif //PROJECT3_POLYNOMIAL_H