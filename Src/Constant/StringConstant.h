#pragma once

#include "Constant.h"

class StringConstant : public Constant {
public:
    std::string value;
    StringConstant(std::shared_ptr<Type> type, const std::string& value) : Constant(type), value(value){}
    std::shared_ptr<Constant> copy() override {
        return std::make_shared<StringConstant>(type, value);
    }
    std::string toString() override {

        return value;
    }
};
