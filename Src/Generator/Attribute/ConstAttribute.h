#pragma once

#include "Attribute.h"
#include <stdexcept>
#include <string>

class ConstAttribute : public Attribute  {
public:
    ConstAttribute(std::shared_ptr<AttributeType> type) : Attribute("Const") {
        if(type->arguments.size() != 0){
            throw std::runtime_error("ConstAttribute constructor failed: expected 0 arguments but received " + std::to_string(type->arguments.size()));
        }
    }
};
