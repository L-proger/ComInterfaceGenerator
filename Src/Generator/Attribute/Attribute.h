#pragma once

#include <string>
#include <Type/AttributeType.h>

class Attribute {
public:
    virtual ~Attribute() = default;
    std::string name;
    Attribute(const std::string& name) : name(name) {

    }
};
