#include "antlr4-runtime.h"
#include <tree/IterativeParseTreeWalker.h>
#include "CidlLexer.h"
#include "CidlParser.h"
#include "CidlBaseListener.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include "tree/ParseTreeListener.h"
#include "tree/ParseTree.h"
#include "tree/ErrorNode.h"
#include <Type/Type.h>
#include "TypeCache.h"
#include <Parser/ModuleParser.h>



void parseModule(const std::string& path) {
    std::ifstream codeFile (path);
    if (codeFile.is_open()) {
        std::cout << "Code file is open!" << std::endl;
        antlr4::ANTLRInputStream input(codeFile);

        CidlLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);

        tokens.fill();
        /*for (auto token : tokens.getTokens()) {
            std::cout << token->toString() << std::endl;
        }*/

        CidlParser parser(&tokens);
        auto module = parser.module();

        ModuleParser listener;
        module->enterRule(&listener);

        std::cout << "Done" << std::endl;

    }else{
        std::cout << "Failed to read code file!" << std::endl;
    }
}

int main() {
    TypeCache::init();

    std::filesystem::path srcPath(SRC_DIR);
    auto apiPath = srcPath / "IDL";
    auto testFilePath = apiPath / "Core.cidl";

    parseModule(testFilePath.string());
    return 0;
}
