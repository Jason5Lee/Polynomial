#include <iostream>
#include "polynomial.h"
#include "storage.h"
#include "polynomial_helper.h"
#include "io_helper.h"
using namespace std;

void printHelp() {
    cout << "1.多项式相加" << endl
         << "2.多项式相减" << endl
         << "3.多项式与常数相乘" << endl
         << "4.多项式与多项式相乘" << endl
         << "5.求多项式代入某点的值" << endl
         << "6.判断两个多项式是否相等" << endl
         << "7.多项式求导" << endl
         << "8.显示所有储存多项式" << endl
         << "9.帮助(查看功能列表)" << endl
         << "0.退出程序" << endl
         << "注意：在输入多项式时，可以使用赋值语句去储存多项式。" << endl
         << "储存所用的标识符由字母构成，且长度不超过10" << endl
         << "在任何时候可以输入\"->_->\"（不包含双引号）返回主菜单。" << endl;
}

int main() {
    cout << "欢迎使用多项式计算器1.0" << endl;
    printHelp();
    for (;;) {
        try {
            cout << endl;
            int choice = Read<int>("请输入操作：", [](const string &str) {
                int a = GenericParse<int>("整数")(str);
                if (a < 0 || a > 9)
                    throw runtime_error("选项" + str + "不存在!");
                return a;
            });
            if (choice == 0)
                break;
            else if (choice == 9)
                printHelp();
            else if (choice == 5) {
                Polynomial pol = Read<Polynomial>("请输入多项式:", PolynomialHelper::process);
                double factor = Read<double>("请输入代入的x的值:", GenericParse<double>("实数"));
                cout << "代入后计算的值:" << pol(factor) << endl;
            } else if (choice == 8) {
                PolynomialHelper::printStorageTo(cout);
            } else if (choice == 6) {
                Polynomial lhs = Read<Polynomial>("请输入第一个多项式:", PolynomialHelper::process),
                        rhs = Read<Polynomial>("请输入第二个多项式:", PolynomialHelper::process);
                cout << "这两个多项式" << (lhs == rhs ? "" : "不") << "相等.";
            } else {
                Polynomial result;
                if (choice == 3) {
                    double factor = Read<double>("请输入常数：", GenericParse<double>("实数"));
                    Polynomial rhs = Read<Polynomial>("请输入多项式：", PolynomialHelper::process);
                    result = factor * rhs;
                } else if (choice == 7) {
                    Polynomial pol = Read<Polynomial>("请输入多项式：", PolynomialHelper::process);
                    result = pol.getDerivative();
                } else {
                    Polynomial lhs = Read<Polynomial>("请输入第一个多项式:", PolynomialHelper::process),
                            rhs = Read<Polynomial>("请输入第二个多项式:", PolynomialHelper::process);
                    if (choice == 1)
                        result = lhs + rhs;
                    else if (choice == 2)
                        result = lhs - rhs;
                    else if (choice == 4)
                        result = lhs * rhs;
                }
                cout << "运算结果：" << result << endl;
                bool save = Read<bool>("是否储存运算结果？(y/n)", [](const string &str) {
                    if (str == "y" || str == "Y")
                        return true;
                    if (str == "n" || str == "N")
                        return false;
                    throw runtime_error("请输入y或n.");
                });
                if (save) {
                    Read<void>("请输入变量名：", [&](const string &str) {
                        PolynomialHelper::store(str, result);
                    });
                }
            }
        } catch (BackToMenu &) {}
    }

    return 0;
}