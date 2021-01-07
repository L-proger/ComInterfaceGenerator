#pragma once

#include <memory>
#include <Type/Type.h>

class Constant {
public:
    virtual ~Constant() = default;
    std::shared_ptr<Type> type;
    Constant(std::shared_ptr<Type> type):type(type){

    }
    virtual std::shared_ptr<Constant> copy() = 0;
    virtual std::string toString() = 0;
};
