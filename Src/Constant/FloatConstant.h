#pragma once

#include "NumericConstant.h"
#include <sstream>

class FloatConstant : public NumericConstant {
public:
    float value;
    FloatConstant(std::shared_ptr<Type> type, float value ) : NumericConstant(type), value(value){

    }
    FloatConstant(std::shared_ptr<Type> type, const std::string& valueStr ) : NumericConstant(type){
        value = std::stof(valueStr);
    }
    void increment() override {
        value++;
    }
    void decrement() override {
        value--;
    }
    void negate() override {
        value = -value;
    }
    std::shared_ptr<Constant> copy() override {
        return std::make_shared<FloatConstant>(type, value);
    }
    std::string toString() override {
        std::stringstream ss;
        ss << value << "f";
        return ss.str();
    }
};
