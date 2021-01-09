#pragma once

#include "Attribute.h"
#include <stdexcept>
#include <string>

class OutAttribute : public Attribute  {
public:
    OutAttribute(std::shared_ptr<AttributeType> type) : Attribute("Out") {
        if(type->arguments.size() != 0){
            throw std::runtime_error("OutAttribute constructor failed: expected 0 arguments but received " + std::to_string(type->arguments.size()));
        }
    }
};
