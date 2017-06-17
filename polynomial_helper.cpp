#include "polynomial_helper.h"
#include "io_helper.h"
#include <iomanip>
using namespace std;

Storage<Polynomial> PolynomialHelper::global;

Polynomial PolynomialHelper::process(const string &str) {
    auto equalSign = str.find('=');
    if (equalSign != string::npos) { //输入包含复制语句
        string varName = str.substr(0, equalSign);
        string polynomial = str.substr(equalSign + 1);
        Trim(varName);
        Trim(polynomial);
        Polynomial tmp = process(polynomial);
        store(varName, tmp);
        return tmp;
    }
//else
    try {
        return global.getReference(str); //输入为变量名
    }
    catch (const runtime_error &) {
        try {
            return Polynomial::parse(str); //输入为多项式
        }
        catch (const runtime_error &) {
            throw FormatException(str, "多项式或标识符");
        }
    }
}
void PolynomialHelper::printStorageTo(ostream &out) {
    out << "当前储存的多项式如下：" << endl
         << "多项式名    多项式" << endl;
    out.flags(ios::left);
    for (const auto &p : global) {
        out << setw(11) << p.first << p.second << endl;
    }
}