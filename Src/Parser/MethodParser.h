#pragma once

#include "AttributeListParser.h"
#include <Type/Method.h>
#include "TypeCache.h"

class MethodParser : public CidlBaseListener {
public:
    MethodDesc result;

    void enterMethod(CidlParser::MethodContext* ctx) override {
        result.name = ctx->method_name()->getText();
        result.returnType = TypeCache::findType(ctx->type_name()->getText());


        auto paramList = ctx->method_parameter_list();
        if(paramList != nullptr){
            for(auto& paramCtx : paramList->method_parameter()){
                MethodArg arg;
                arg.name = paramCtx->identifier()->getText();

                if(paramCtx->attribute_list() != nullptr){
                    AttributeListParser parser;
                    paramCtx->attribute_list()->enterRule(&parser);
                    arg.attributes = parser.result;
                }

                if(paramCtx->type_name() != nullptr){
                    arg.type = TypeCache::findType(paramCtx->type_name()->getText());
                    arg.reference = false;
                }else{
                    arg.type = TypeCache::findType(paramCtx->reference_type_name()->type_name()->getText());
                    arg.reference = true;
                }

                //TODO: parse attributes

                result.args.push_back(arg);
            }
        }
    }
};
