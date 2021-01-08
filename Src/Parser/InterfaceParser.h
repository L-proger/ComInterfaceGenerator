#pragma once

#include "MethodParser.h"
#include "AttributeListParser.h"
#include <Type/InterfaceType.h>
#include "TypeCache.h"

class InterfaceParser : public CidlBaseListener {
public:
    std::shared_ptr<TypeRef> result;

    void enterInterfaceDefinition(CidlParser::InterfaceDefinitionContext* ctx) override {
        result = TypeCache::makeLocalType<InterfaceType>(ctx->local_type()->getText());

        auto interfaceType = std::dynamic_pointer_cast<InterfaceType>(result->type);

        interfaceType->baseInterfaceType = TypeCache::findOrDefineReferencedType(TypeNameParser::parse(ctx->interfaceInheritanceList()->local_or_imported_type()));

        auto attributesList = ctx->attribute_list();
        if(attributesList != nullptr){
            AttributeListParser attributesParser;
            attributesList->enterRule(&attributesParser);
            interfaceType->attributes = attributesParser.result;
        }

        for(auto& methodCtx : ctx->method()){
            MethodParser parser;
            methodCtx->enterRule(&parser);
            interfaceType->methods.push_back(parser.result);
        }
    }
};
