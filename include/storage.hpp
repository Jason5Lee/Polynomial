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
    struct IllegalIdentifierException : std::runtime_error {
        IllegalIdentifierException(): std::runtime_error("Identifier invalid") {}
    };

    struct IdentifierNotFoundException : std::runtime_error {
        IdentifierNotFoundException(): std::runtime_error("Identifier not found") {}
    };

    // Get the reference based on the identifier.
    T &getReference(const std::string &id) {
        if (!isLegalIdentifier(id))
            throw IllegalIdentifierException();
        auto iter = mp.find(id);
        if (iter == mp.end())
            throw IdentifierNotFoundException();
        else
            return iter->second;
    }

    // Set the value of an identifier.
    void assign(const std::string &id, const T &value) {
        if (!isLegalIdentifier(id))
            throw IllegalIdentifierException();
        mp[id] = value;
    }

    decltype(mp.begin()) begin() { return mp.begin(); }
    decltype(mp.end()) end() { return mp.end(); }

protected:
    // Determine whether an identifier is legal.
    // Can be overridden to implement custom identifier rules.
    virtual bool isLegalIdentifier(const std::string &id) {
        if (id.size() <= 0 || id.size() > 10) return false;
        for (char ch : id)
            if (!isalpha(ch))
                return false;
        return true;
    }
};

#endif //PROJECT3_STORAGE_H
