#pragma once

#include "CidlBaseListener.h"
#include <Type/AttributeType.h>
#include "AttributeParser.h"

class AttributeListParser : public CidlBaseListener {
public:
    std::vector<std::shared_ptr<AttributeType>> result;

    void enterAttribute_list(CidlParser::Attribute_listContext * ctx) override {
        for(auto& attributeCtx : ctx->attribute()){
            AttributeParser parser;
            attributeCtx->enterRule(&parser);
            result.push_back(parser.result);
        }
    }
};
