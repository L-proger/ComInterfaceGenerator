#pragma once

#include <Generator/CodeFile.h>
#include <Type/InterfaceType.h>
#include <Type/StructType.h>
#include <Type/EnumType.h>

class CSharpCodeFile : public CodeFile {
public:
    CSharpCodeFile() {

    }

    std::string getFileName(std::shared_ptr<Module> module) override {
        return module->name + ".cs";
    }

    CSharpCodeFile& beginNamespaceScope(std::string name) {
        write("namespace " + name);
        beginScope(name);
        return *this;
    }

    CSharpCodeFile& beginScope(std::string name) {
        _currentScopeName = name;
        writeLine("{").ident();
        return *this;
    }
    CSharpCodeFile& endScope() {
        std::string text = "}";
        if(_currentScopeName.size() != 0){
            text += " //" + _currentScopeName;
            _currentScopeName.clear();
        }
        unident().writeLine(text);
        return *this;
    }
    CSharpCodeFile& comment(const std::string& text) {
        write("//"+ text);
        return *this;
    }

    void writeModule(std::shared_ptr<Module> module) override {
       /* auto deps = module->getDependencies();

        for(auto& dep : deps){
            comment("imports: " + dep->name).writeLine();
        }

        beginNamespaceScope(module->name).writeLine();

        for(auto& type : module->getTypes()){
            writeType(type);
        }

        endScope();*/
    }



private:
    void writeType(std::shared_ptr<TypeRef> type) {
       /* auto interfaceType = std::dynamic_pointer_cast<InterfaceType>(type);
        if(interfaceType != nullptr){
            writeInterface(interfaceType);
            return;
        }

        auto structType = std::dynamic_pointer_cast<StructType>(type);
        if(structType != nullptr){
            writeStruct(structType);
            return;
        }

        auto enumType = std::dynamic_pointer_cast<StructType>(type);
        if(structType != nullptr){
            writeStruct(structType);
            return;
        }*/
    }


    std::string fullName(std::shared_ptr<Type> type){
        return type->moduleName + "." + type->name;
    }

    /*void writeInterface(std::shared_ptr<InterfaceType> type) {
        write("public interface ").write(type->name).write(" : ");

        if(type->baseInterfaceType != nullptr){
            write(fullName(type->baseInterfaceType));
        }

        beginScope(type->name);

        for(auto& method : type->methods){
            writeLine(method.name);
        }

        endScope();
    }

    void writeStruct(std::shared_ptr<StructType> type) {

    }

    void writeEnum(std::shared_ptr<EnumType> type) {

    }*/

    std::string _currentScopeName;
};
