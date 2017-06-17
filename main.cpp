// Project3.cpp : Defines the entry point for the console application.
//e
//#if false
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cctype>
#include <iomanip>
#include "polynomial.h"
#include "storage.h"

using namespace std;

//用于返回主菜单的异常。
class BackToMenuException {
};

//删除字符串首尾的空白字符
void Trim(string &str) {
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}

//反复读入字符串，直到遇到合法的输入。
//parse为将字符串转换成类型T的函数，并在转换失败时抛出异常。
//当输入->_->时抛出BackToMenuException异常返回主菜单。
template<typename T, typename U>
T Read(const string &prompt, U parse) {
    for (;;) {
        string str;
        cout << prompt;
        getline(cin, str);
        Trim(str);
        if (str.empty())
            continue;
        if (str == "->_->")
            throw BackToMenuException();
        try {
            return parse(str);
        }
        catch (const runtime_error &e) {
            cout << "错误：" << e.what() << "!" << endl << endl;
        }
    }
}

//将字符串转化为指定类型的数据的仿函数(functor）
template<typename T>
class GenericParse {
public:
    GenericParse(const string &typeName) : typeName(typeName) {}

    T operator()(const string &str) {
        stringstream ss;
        ss << str;
        T ret;
        if (ss >> ret && ss.eof())
            return ret;
        else
            throw runtime_error("\"" + str + "\"" + "不是一个合法的" + typeName);
    }

private:
    const string typeName;
};

Storage<Polynomial> global; //储存多项式

//将字符串转化成多项式，并处理其中的赋值语句。
Polynomial PolynomialParse(const string &str) {
    auto equalSign = str.find('=');
    if (equalSign != string::npos) { //输入包含复制语句
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
        return global.getReference(str); //输入为变量名
    }
    catch (const runtime_error &) {
        try {
            return Polynomial::parse(str); //输入为多项式
        }
        catch (const runtime_error &) {
            throw runtime_error(str + "不是正确的多项式或标识符");
        }
    }
}

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
                Polynomial pol = Read<Polynomial>("请输入多项式:", PolynomialParse);
                double factor = Read<double>("请输入代入的x的值:", GenericParse<double>("实数"));
                cout << "代入后计算的值:" << pol(factor) << endl;
            } else if (choice == 8) {
                cout << "当前储存的多项式如下：" << endl
                     << "多项式名    多项式" << endl;
                cout.flags(ios::left);
                for (const auto &p : global) {
                    cout << setw(11) << p.first << p.second << endl;
                }
            } else if (choice == 6) {
                Polynomial lhs = Read<Polynomial>("请输入第一个多项式:", PolynomialParse),
                        rhs = Read<Polynomial>("请输入第二个多项式:", PolynomialParse);
                cout << "这两个多项式" << (lhs == rhs ? "" : "不") << "相等.";
            } else {
                Polynomial result;
                if (choice == 3) {
                    double factor = Read<double>("请输入常数：", GenericParse<double>("实数"));
                    Polynomial rhs = Read<Polynomial>("请输入多项式：", PolynomialParse);
                    result = factor * rhs;
                } else if (choice == 7) {
                    Polynomial pol = Read<Polynomial>("请输入多项式：", PolynomialParse);
                    result = pol.getDerivative();
                } else {
                    Polynomial lhs = Read<Polynomial>("请输入第一个多项式:", PolynomialParse),
                            rhs = Read<Polynomial>("请输入第二个多项式:", PolynomialParse);
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
                        global.assign(str, result);
                    });
                }
            }
        } catch (BackToMenuException &) {}
    }

    return 0;
}
//#endif
