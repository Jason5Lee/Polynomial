
//
// Created by brian on 2017/5/28.
//

#ifndef PROJECT3_STORAGE_H
#define PROJECT3_STORAGE_H

#include <map>
#include <string>
#include <cctype>
#include <exception>

template<typename T>
class Storage {
private:
    std::map<std::string, T> mp;
public:
    //标识符不合法异常
    struct IllegalIdentifierException : std::runtime_error {
        IllegalIdentifierException(): std::runtime_error("标识符不合法") {}
    };

    //找不到标识符异常
    struct IdentifierNotFoundException : std::runtime_error {
        IdentifierNotFoundException(): std::runtime_error("找不到标识符") {}
    };

    //根据标识符获得指定对象引用
    T &getReference(const std::string &id) {
        if (!isLegalIdentifier(id))
            throw IllegalIdentifierException();
        auto iter = mp.find(id);
        if (iter == mp.end())
            throw IdentifierNotFoundException();
        else
            return iter->second;
    }

    //赋值运算
    void assign(const std::string &id, const T &value) {
        if (!isLegalIdentifier(id))
            throw IllegalIdentifierException();
        mp[id] = value;
    }

    //返回首尾迭代器，用于范围for语句
    decltype(mp.begin()) begin() { return mp.begin(); }
    decltype(mp.end()) end() { return mp.end(); }

protected:
    //判断标识符是否合法
    //可通过继承重写该函数以实现自定义标识符规则
    virtual bool isLegalIdentifier(const std::string &id) {
        if (id.size() <= 0 || id.size() > 10) return false;
        for (char ch : id)
            if (!isalpha(ch))
                return false;
        return true;
    }
};

#endif //PROJECT3_STORAGE_H
