#pragma once

#include "GuidAttribute.h"
#include "ConstAttribute.h"
#include "InAttribute.h"
#include "OutAttribute.h"
#include <stdexcept>

class AttributeList {
public:
    std::vector<std::shared_ptr<Attribute>> attributes;
    static AttributeList parse(std::vector<std::shared_ptr<AttributeType>> attributesList){
        AttributeList result;
        for(auto attType : attributesList){
            if(attType->name == "Guid"){
                result.attributes.push_back(std::make_shared<GuidAttribute>(attType));
            }else if(attType->name == "Const"){
                result.attributes.push_back(std::make_shared<ConstAttribute>(attType));
            }else if(attType->name == "In"){
                result.attributes.push_back(std::make_shared<InAttribute>(attType));
            }else if(attType->name == "Out"){
                result.attributes.push_back(std::make_shared<OutAttribute>(attType));
            }else{
                throw std::runtime_error("Unknown attribute type: " + attType->name);
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

    template<typename TAttribute>
    std::shared_ptr<TAttribute> getAttribute() {
        for(auto& attibute : attributes){
            auto castedAttribute = std::dynamic_pointer_cast<TAttribute>(attibute);
            if(castedAttribute != nullptr){
                return castedAttribute;
            }
        }
        return nullptr;
    }
};
