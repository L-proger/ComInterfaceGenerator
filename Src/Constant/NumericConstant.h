#pragma once

#include "Constant.h"

class NumericConstant : public Constant {
public:
    NumericConstant(std::shared_ptr<Type> type) : Constant(type){}
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual void negate() = 0;
};
