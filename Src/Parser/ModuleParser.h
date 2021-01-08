#pragma once

#include "CidlBaseListener.h"

#include "EnumParser.h"
#include "StructParser.h"
#include "InterfaceParser.h"
#include <functional>
#include <Type/Module.h>

class ModuleParser : public CidlBaseListener {
public:
    ModuleParser(){

    }

    virtual void enterModule(CidlParser::ModuleContext * ctx) override {
        //Get module imports
       /* auto importList = ctx->importExprList();
        if(importList != nullptr){
            auto expr = importList->importExpr();
            for(auto & import : expr){
                imports.push_back(_importHandler(import->import_file_path()->getText()));
            }
        }*/

        //parse enums
        for(auto& enumType : ctx->enumDefinition()){
            EnumParser parser;
            enumType->enterRule(&parser);
            //enums.push_back(parser.result);
        }

        //parse structs
        for(auto& structType : ctx->structDefinition()){
            StructParser parser;
            structType->enterRule(&parser);
            //structs.push_back(parser.result);
        }

        //parse interfaces
        for(auto& interfaceType : ctx->interfaceDefinition()){
            InterfaceParser parser;
            interfaceType->enterRule(&parser);
            //interfaces.push_back(parser.result);
        }
    }
};
