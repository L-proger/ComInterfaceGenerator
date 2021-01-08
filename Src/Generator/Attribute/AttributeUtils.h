#pragma once

#include "GuidAttribute.h"
#include <stdexcept>

class AttributeList {
public:
    std::vector<std::shared_ptr<Attribute>> attributes;
    static AttributeList parse(std::vector<std::shared_ptr<AttributeType>> attributesList){
        AttributeList result;
        for(auto attType : attributesList){
            if(attType->name == "Guid"){
                result.attributes.push_back(std::make_shared<GuidAttribute>(attType));
            }else{
                throw new std::runtime_error("Unknown attribute type: " + attType->name);
            }
        }
        return result;
    }

    template<typename TAttribute>
    std::vector<std::shared_ptr<TAttribute>> getAttributes() {
        std::vector<std::shared_ptr<TAttribute>> result;
        for(auto& attibute : attributes){
            auto castedAttribute = std::dynamic_pointer_cast<TAttribute>(attibute);
            if(castedAttribute != nullptr){
                result.push_back(castedAttribute);
            }
        }
        return result;
    }
};
