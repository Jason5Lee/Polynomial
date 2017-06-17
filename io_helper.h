#ifndef PROJECT3_IO_HELPER_H
#define PROJECT3_IO_HELPER_H

#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

//用于返回主菜单的异常。
class BackToMenu {
};

//删除字符串首尾的空白字符
inline void Trim(std::string &str) {
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}

//反复读入字符串，直到遇到合法的输入。
//parse为将字符串转换成类型T的函数，并在转换失败时抛出异常。
//当输入->_->时抛出BackToMenu异常返回主菜单。
template<typename T, typename U>
T Read(const std::string &prompt, U parse) {
    for (;;) {
        std::string str;
        std::cout << prompt;
        getline(std::cin, str);
        Trim(str);
        if (str.empty())
            continue;
        if (str == "->_->")
            throw BackToMenu();
        try {
            return parse(str);
        }
        catch (const std::runtime_error &e) {
            std::cout << "错误：" << e.what() << "!" << std::endl << std::endl;
        }
    }
}

//格式错误异常
struct FormatException: std::runtime_error {
    FormatException(const std::string &str, const std::string &typeName)
            : runtime_error("\"" + str + "\"" + "不是合法的" + typeName) {}
};

//将字符串转化为指定类型的数据的仿函数(functor）
template<typename T>
class GenericParse {
public:
    GenericParse(const std::string &typeName) : typeName(typeName) {}

    T operator()(const std::string &str) {
        std::stringstream ss;
        ss << str;
        T ret;
        if (ss >> ret && ss.eof())
            return ret;
        else
            throw FormatException(str, typeName);
    }

private:
    const std::string typeName;
};

#endif //PROJECT3_IO_HELPER_H
