#pragma once

#include <string>

class Type {
public:
    virtual ~Type() = default;
    std::string name;
};
