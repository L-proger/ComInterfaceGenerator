#pragma once

#include "Attribute.h"
#include <stdexcept>
#include <string>

class InAttribute : public Attribute  {
public:
    InAttribute(std::shared_ptr<AttributeType> type) : Attribute("In") {
        if(type->arguments.size() != 0){
            throw std::runtime_error("InAttribute constructor failed: expected 0 arguments but received " + std::to_string(type->arguments.size()));
        }
    }
};
