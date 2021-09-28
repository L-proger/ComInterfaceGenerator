#pragma once

#include "AttributeListParser.h"
#include <Type/Method.h>
#include "TypeCache.h"

class MethodParser : public CidlBaseListener {
public:
    MethodDesc result;

    void enterMethod(CidlParser::MethodContext* ctx) override {
        result.name = ctx->method_name()->getText();

        //Parse return type
        MethodArg returnType;
        returnType.name = "result";

        returnType.reference = true;
        auto t = std::make_shared<AttributeType>();
        t->name = "Out";
        returnType.attributes.push_back(t);
        if(ctx->method_return_type()->array_type() != nullptr){
            returnType.type = TypeCache::findOrDefineReferencedType(TypeNameParser::parse(ctx->method_return_type()->array_type()->local_or_imported_type()));
            returnType.array = true;
        }else{
            returnType.type = TypeCache::findOrDefineReferencedType(TypeNameParser::parse(ctx->method_return_type()->local_or_imported_type()));
            returnType.array = false;
        }

        if (returnType.type->type->name == "void") {
            returnType.reference = false;
        }


        result.returnType = returnType;


        auto paramList = ctx->method_parameter_list();
        if(paramList != nullptr){
            for(auto& paramCtx : paramList->method_parameter()){
                MethodArg arg;

                //Parse arg attributes
                if(paramCtx->attribute_list() != nullptr){
                    AttributeListParser parser;
                    paramCtx->attribute_list()->enterRule(&parser);
                    arg.attributes = parser.result;
                }

                //Parse arg name
                arg.name = paramCtx->local_type()->getText();

                //Parse arg type
                if(paramCtx->array_type() != nullptr){
                    if(paramCtx->array_type()->local_or_imported_type() != nullptr){
                        arg.type = TypeCache::findOrDefineReferencedType(TypeNameParser::parse(paramCtx->array_type()->local_or_imported_type()));
                        arg.reference = false;
                        arg.array = true;
                    }
                }else{
                    if(paramCtx->local_or_imported_type() != nullptr){
                        arg.type = TypeCache::findOrDefineReferencedType(TypeNameParser::parse(paramCtx->local_or_imported_type()));
                        arg.reference = false;
                    }else{
                        arg.type = TypeCache::findOrDefineReferencedType(TypeNameParser::parse(paramCtx->reference_type_name()->local_or_imported_type()));
                        arg.reference = true;
                    }
                }

                result.args.push_back(arg);
            }
        }
    }
};
