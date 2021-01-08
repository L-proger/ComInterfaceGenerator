#pragma once

#pragma once

#include <Generator/CodeFile.h>
#include <Type/InterfaceType.h>
#include <Type/StructType.h>
#include <Type/EnumType.h>

class CppCodeFile : public CodeFile {
public:
    CppCodeFile() {

    }

    std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }

    CppCodeFile& beginNamespaceScope(std::string name) {
        name = replaceAll(name, ".", "::");
        write("namespace " + name);
        beginScope(name);
        return *this;
    }

    CppCodeFile& beginScope(std::string name) {
        _currentScopeName = name;
        writeLine("{").ident();
        return *this;
    }
    CppCodeFile& endScope(std::string textAfter) {
        std::string text = "}" + textAfter;
        if(_currentScopeName.size() != 0){
            text += " //" + _currentScopeName;
            _currentScopeName.clear();
        }
        unident().writeLine(text);
        return *this;
    }

    CppCodeFile& endScope() {
        return endScope("");
    }


    CppCodeFile& comment(const std::string& text) {
        write("//" + text);
        return *this;
    }

    void writeModule(std::shared_ptr<Module> module) override {
        _module = module;

        for(auto& dep : module->dependencies){
            comment("imports: " + dep->module->name).writeLine();
        }

        for(auto& type : module->types){
            writeType(type);
        }

    }
private:
    std::shared_ptr<Module> _module;
    void writeType(std::shared_ptr<Type> type) {
        auto interfaceType = std::dynamic_pointer_cast<InterfaceType>(type);
        if(interfaceType != nullptr){
            writeInterface(interfaceType);
            return;
        }

        auto structType = std::dynamic_pointer_cast<StructType>(type);
        if(structType != nullptr){
            writeStruct(structType);
            return;
        }

        auto enumType = std::dynamic_pointer_cast<EnumType>(type);
        if(enumType != nullptr){
            writeEnum(enumType);
            return;
        }
    }

    std::string fullName(std::shared_ptr<Type> type){
        auto result = type->moduleName + "." + type->name;
        return replaceAll(result, ".", "::");
    }

    void writeInterface(std::shared_ptr<InterfaceType> type) {
        beginNamespaceScope(_module->name);
        //Tag declaration
        write("class ").write(type->name).writeLine(";");
        endScope();


        //Lframework types specialization
        beginNamespaceScope("LFramework");

        //Write ABI
        writeLine("template<>");

        write("struct ").write("InterfaceAbi<").write(fullName(type)).write(">");
        if(type->baseInterfaceType != nullptr){
            write(" : public InterfaceAbi<").write(fullName(type->baseInterfaceType)).write(">");
        }
        beginScope(type->name);//ABI
        if(type->baseInterfaceType != nullptr){
            write("using Base = InterfaceAbi<").write(fullName(type->baseInterfaceType)).writeLine(">;");\
        }

        //TODO: write ID()

        //Write abi methods
        for(auto& method : type->methods){
            writeAbiMethod(method);
        }

        endScope(";"); //ABI
        endScope(); //LFramework
        //struct InterfaceAbi<MicroNetwork::Host::INetwork> : public InterfaceAbi<IUnknown> {
    }

    void writeStruct(std::shared_ptr<StructType> type) {
        beginNamespaceScope(_module->name);
        //Tag declaration
        write("struct ").write(type->name);
        beginScope(type->name);
        for(auto& field : type->fields){
            writeStructField(field);
        }
        endScope(";");
        endScope();
    }

    void writeStructField(StructType::Field field){
        write(fullName(field.type)).write(" ").write(field.name).writeLine(";");
    }

    void writeEnum(std::shared_ptr<EnumType> type) {
        beginNamespaceScope(_module->name);
        //Tag declaration
        write("enum ").write(type->name).write(" : ").write(fullName(type->fieldType));
        beginScope(type->name);
        for(auto& field : type->fields){
            writeEnumField(field);
        }
        endScope(";");
        endScope();
    }

    void writeEnumField(EnumType::Field field){
        write(field.name).write(" = ").write(field.value->toString()).writeLine(",");
    }

    void writeAbiMethod(MethodDesc desc) {
        write("virtual Result ").write(desc.name).write("(");
        for(std::size_t i = 0; i < desc.args.size(); ++i){
            writeAbiArg(desc.args[i]);
            if(desc.returnType->name == "void"){
                if(i < desc.args.size() - 1){
                     write(", ");
                }
            }else{
                write(", ");
            }
        }

        if(desc.returnType->name != "void"){
            MethodArg arg;
            arg.name = "result";
            arg.reference = true;
            arg.type = desc.returnType;
            writeAbiArg(arg);
        }

        writeLine(") = 0;");
    }

    void writeAbiArg(MethodArg arg) {
        write(fullName(arg.type));
        if(arg.reference){
            write("&");
        }
        write(" ");
        write(arg.name);
    }

    std::string _currentScopeName;
};