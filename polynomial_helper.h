#ifndef PROJECT3_POLYNOMIAL_HELPER_H
#define PROJECT3_POLYNOMIAL_HELPER_H

#include "polynomial.h"
#include "storage.h"
#include <string>
#include <iostream>

class PolynomialHelper
{
public:
    //将字符串转化成多项式，并处理其中的赋值语句。
    static Polynomial process(const std::string &str);

    //输出所有储存的多项式
    static void printStorageTo(std::ostream &out);

    //储存多项式
    static void store(const std::string& id, const Polynomial &pol) {
        global.assign(id, pol);
    }
private:
    static Storage<Polynomial> global; //储存多项式
};
#endif //PROJECT3_POLYNOMIAL_HELPER_H
