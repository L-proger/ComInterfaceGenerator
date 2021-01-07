#pragma once

#include "NumericConstant.h"
#include <sstream>

class DoubleConstant : public NumericConstant {
public:
    double value;
    DoubleConstant(std::shared_ptr<Type> type, double value ) : NumericConstant(type), value(value){

    }
    DoubleConstant(std::shared_ptr<Type> type, const std::string& valueStr ) : NumericConstant(type){
        value = std::stod(valueStr);
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
        return std::make_shared<DoubleConstant>(type, value);
    }
    std::string toString() override {
        std::stringstream ss;
        ss << value ;
        return ss.str();
    }
};
