#pragma once

#include <Generator/Generator.h>
#include <Generator/Cpp/CppCodeFile.h>
#include <Generator/ExternalModule.h>

class CppGenerator : public Generator {
public:
    CppGenerator(const std::string& outputPath) : Generator(outputPath){
        auto primitiveModule = TypeCache::findModule(TypeCache::primitiveModuleName());

        //replace integer types
        auto cstdintModule = TypeCache::addModule<ExternalModule>("cstdint");
        TypeCache::replaceType(primitiveModule->findType("uint8"), cstdintModule->addNewType("std::uint8_t"));
        TypeCache::replaceType(primitiveModule->findType("uint16"), cstdintModule->addNewType("std::uint16_t"));
        TypeCache::replaceType(primitiveModule->findType("uint32"), cstdintModule->addNewType("std::uint32_t"));
        TypeCache::replaceType(primitiveModule->findType("uint64"), cstdintModule->addNewType("std::uint64_t"));
        TypeCache::replaceType(primitiveModule->findType("int8"), cstdintModule->addNewType("std::int8_t"));
        TypeCache::replaceType(primitiveModule->findType("int16"), cstdintModule->addNewType("std::int16_t"));
        TypeCache::replaceType(primitiveModule->findType("int32"), cstdintModule->addNewType("std::int32_t"));
        TypeCache::replaceType(primitiveModule->findType("int64"), cstdintModule->addNewType("std::int64_t"));

        auto languagePrimitiveModule = TypeCache::addModule<ExternalModule>("LanguagePrimitive");
        TypeCache::replaceType(primitiveModule->findType("void"), languagePrimitiveModule->addNewType("void"));
        TypeCache::replaceType(primitiveModule->findType("float"), languagePrimitiveModule->addNewType("float"));
        TypeCache::replaceType(primitiveModule->findType("double"), languagePrimitiveModule->addNewType("double"));


        auto stringModule = TypeCache::addModule<ExternalModule>("string");
        TypeCache::replaceType(primitiveModule->findType("string"), stringModule->addNewType("std::string"));


        auto comCoreModule = TypeCache::addModule<ExternalModule>("LFramework/COM/ComObject.h");
        TypeCache::replaceType(primitiveModule->findType("IUnknown"), comCoreModule->addNewType("LFramework::IUnknown"));
    }

    std::shared_ptr<CodeFile> createCodeFile() override {
        return std::make_shared<CppCodeFile>();
    }
};
