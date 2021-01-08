#pragma once

#include "Attribute.h"
#include <Generator/Guid.h>
#include <Constant/StringConstant.h>
#include <memory>

class GuidAttribute : public Attribute {
public:
    ComInterfaceGenerator::Guid guid;
    GuidAttribute(std::shared_ptr<AttributeType> type) : Attribute("Guid"){
        if(type->arguments.size() != 1){
            throw std::runtime_error("Failed to parse Guid attribute: invalid arguments count" );
        }

        auto arg = type->arguments[0];
        auto strArg = std::dynamic_pointer_cast<StringConstant>(arg);

        if(strArg == nullptr){
            throw std::runtime_error("Failed to parse Guid attribute: invalid argument type" );
        }

        if(!ComInterfaceGenerator::Guid::fromString(strArg->value, guid)){
            throw std::runtime_error("Failed to parse Guid attribute: invalid argument format" );
        }
    }
};
