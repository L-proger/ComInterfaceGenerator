#pragma once

#include "AttributeListParser.h"
#include <Type/Method.h>
#include "TypeCache.h"

class MethodParser : public CidlBaseListener {
public:
    MethodDesc result;

    void enterMethod(CidlParser::MethodContext* ctx) override {
        result.name = ctx->method_name()->getText();
        result.returnType = TypeCache::findType(TypeNameParser::parse(ctx->local_or_imported_type()));


        auto paramList = ctx->method_parameter_list();
        if(paramList != nullptr){
            for(auto& paramCtx : paramList->method_parameter()){
                MethodArg arg;
                arg.name = paramCtx->local_type()->getText();

                if(paramCtx->attribute_list() != nullptr){
                    AttributeListParser parser;
                    paramCtx->attribute_list()->enterRule(&parser);
                    arg.attributes = parser.result;
                }

                if(paramCtx->local_or_imported_type() != nullptr){
                    arg.type = TypeCache::findType(TypeNameParser::parse(paramCtx->local_or_imported_type()));
                    arg.reference = false;
                }else{
                    arg.type = TypeCache::findType(TypeNameParser::parse(paramCtx->reference_type_name()->local_or_imported_type()));
                    arg.reference = true;
                }

                //TODO: parse attributes

                result.args.push_back(arg);
            }
        }
    }
};
