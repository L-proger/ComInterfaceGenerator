#include "TypeCache.h"
#include <filesystem>
#include <Generator/CSharp/CSharpGenerator.h>
#include <Generator/Cpp/CppGenerator.h>
#include <Generator/Cpp/CppInterfaceSerializerCodeFile.h>
#include <Generator/Cpp/CppIoStreamDescriptorCodeFile.h>
#include <iostream>
#include <Generator/Guid.h>
#include <Generator/Attribute/AttributeUtils.h>

#include <CommandLine/Parser.h>

enum class OutputLanguage {
    Cpp,
    CSharp
};

template<>
class CommandLine::ValueConverter<OutputLanguage> {
public:
    static OutputLanguage convert(const std::string& value) {
        if(value == "cpp" || value == "Cpp" || value == "CPP"){
            return OutputLanguage::Cpp;
        }
        if(value == "CS" || value == "cs" || value == "Cs" || value == "csharp" || value == "CSharp"){
            return OutputLanguage::CSharp;
        }
        throw CommandLine::Exception("Failed to convert OutputLanguage value");
    }
};


void init(const std::vector<std::string>& modules, const std::vector<std::string>& inputDirs) {
    if(modules.empty()){
        //throw std::runtime_error("No input files");
    }

    TypeCache::init();
    for(auto& dir : inputDirs){
        TypeCache::addSearchPath(dir);
    }

    for(auto& module : modules){
        TypeCache::parseModule(module);
    }
}

void run(OutputLanguage language, const std::string& outputDir, std::optional<bool> enableExceptions) {
    auto m = TypeCache::getModules();

    if(language == OutputLanguage::Cpp){
        CppGenerator generator;
        if (enableExceptions.has_value()) {
            generator.enableExceptions = enableExceptions.value();
        }
      
        generator.generate(outputDir);
    }else{
        throw std::runtime_error("Target language not supported");
    }
}

void generateInterfaceSerializer(OutputLanguage language, const std::string& outputDir, std::optional<bool> enableExceptions, const std::string& interfaceName, const std::string& direction) {
    auto m = TypeCache::getModules();
    auto splitPos = interfaceName.find_last_of('.');
    auto moduleName = interfaceName.substr(0, splitPos);
    std::filesystem::create_directories(outputDir);

    TypeCache::parseModule(moduleName);

    if(language == OutputLanguage::Cpp){
        CppGenerator generator;
        if (enableExceptions.has_value()) {
            generator.enableExceptions = enableExceptions.value();
        }
        
        auto moduleIt = std::find_if(generator.getWritableModules().begin(), generator.getWritableModules().end(), [moduleName](std::shared_ptr<Module> m){
            return m->name == moduleName;
        });

        auto codeFile = std::make_shared<CppInterfaceSerializerCodeFile>(generator.enableExceptions, interfaceName, direction == "out" ? true : false);
        codeFile->writeSerializer(*moduleIt);
        generator.saveCodeFile(outputDir, (*moduleIt)->name, codeFile);

    }else{
        throw std::runtime_error("Target language not supported");
    }
}
void generateIoStreamDescriptor(OutputLanguage language, const std::string& outputDir, std::optional<bool> enableExceptions, const std::string& interfaceName, bool invert) {
    auto m = TypeCache::getModules();
    auto splitPos = interfaceName.find_last_of('.');
    auto moduleName = interfaceName.substr(0, splitPos);
    std::filesystem::create_directories(outputDir);

    TypeCache::parseModule(moduleName);

    if(language == OutputLanguage::Cpp){
        CppGenerator generator;
        if (enableExceptions.has_value()) {
            generator.enableExceptions = enableExceptions.value();
        }
        
        auto moduleIt = std::find_if(generator.getWritableModules().begin(), generator.getWritableModules().end(), [moduleName](std::shared_ptr<Module> m){
            return m->name == moduleName;
        });

        auto codeFile = std::make_shared<CppIoStreamDescriptorCodeFile>(generator.enableExceptions, interfaceName, invert);
        codeFile->writeSerializer(*moduleIt);
        generator.saveCodeFile(outputDir, (*moduleIt)->name, codeFile);

    }else{
        throw std::runtime_error("Target language not supported");
    }
}
int main(int argc, const char* const* argv) {
    try{
        auto serializerDirection = CommandLine::OptionDescription("--direction", "Serialization direction [out/in]", CommandLine::OptionType::SingleValue).alias("-d");
        auto version = CommandLine::OptionDescription("--version", "Print version", CommandLine::OptionType::NoValue).alias("-v");
        auto exceptions = CommandLine::OptionDescription("--exceptions", "Enable exceptions handling in generated files", CommandLine::OptionType::SingleValue).alias("-e");
        auto language = CommandLine::OptionDescription("--language", "Output files language", CommandLine::OptionType::SingleValue).alias("-l");
        auto outputDir = CommandLine::OptionDescription("--output", "Output files directory", CommandLine::OptionType::SingleValue).alias("-o");
        auto inputDirs = CommandLine::OptionDescription("--input",  "Input files directory", CommandLine::OptionType::MultipleValues).alias("-I");
        auto invertStream = CommandLine::OptionDescription("--invert",  "Invert stream direction", CommandLine::OptionType::NoValue).alias("-i");

        CommandLine::ArgumentDescription inputModules("Module names",  "Module names to generate", CommandLine::ArgumentType::MultipleValues);

        CommandLine::ArgumentDescription inputInterface("Full interface name",  "Full interface name", CommandLine::ArgumentType::SingleValue);

        CommandLine::Command app("ComInterfaceGenerator", "Generate COM interfaces from .cidl files", [&](CommandLine::Command& cmd) {
            cmd.addHelpOption();
            auto& versionOpt = cmd.option(version);
            cmd.handler([&]() {
                if(versionOpt.isSet()){
                    std::cout << "0.1.0-alpha" << std::endl;
                }else{
                    std::cout << "No command selected!" << std::endl;
                }
            });
        });
        app.command("generate", "Generate interface files", [&](CommandLine::Command& cmd) {
            cmd.addHelpOption();
            auto& languageOpt = cmd.option(language);
            auto& outputDirOpt = cmd.option(outputDir);
            auto& inputDirsOpt = cmd.option(inputDirs);
            auto& exceptionsOpt = cmd.option(exceptions);
            auto& inputModulesArg = cmd.argument(inputModules);
            cmd.handler([&]() {

                init(inputModulesArg.values(), inputDirsOpt.values());
                run(languageOpt.value<OutputLanguage>(), outputDirOpt.value(), exceptionsOpt.valueOptional<bool>());
            });
        });

        app.command("generateInterfaceSerializer", "Generate interface data serializer", [&](CommandLine::Command& cmd) {
            cmd.addHelpOption();
            auto& serializerDirectionOpt = cmd.option(serializerDirection);
            auto& languageOpt = cmd.option(language);
            auto& outputDirOpt = cmd.option(outputDir);
            auto& inputDirsOpt = cmd.option(inputDirs);
            auto& exceptionsOpt = cmd.option(exceptions);
            auto& inputInterfaceArg = cmd.argument(inputInterface);
 
            cmd.handler([&]() {
                init({}, inputDirsOpt.values());
                generateInterfaceSerializer(languageOpt.value<OutputLanguage>(), outputDirOpt.value(), exceptionsOpt.valueOptional<bool>(), inputInterfaceArg.value(), serializerDirectionOpt.value());
            });
        });

        app.command("generateIoStreamDescriptor", "Generate IO serialization stream descriptor", [&](CommandLine::Command& cmd) {
            cmd.addHelpOption();
            auto& invertStreamOpt = cmd.option(invertStream);
            auto& languageOpt = cmd.option(language);
            auto& outputDirOpt = cmd.option(outputDir);
            auto& inputDirsOpt = cmd.option(inputDirs);
            auto& exceptionsOpt = cmd.option(exceptions);
            auto& inputInterfaceArg = cmd.argument(inputInterface);
 
            cmd.handler([&]() {
                init({}, inputDirsOpt.values());
                generateIoStreamDescriptor(languageOpt.value<OutputLanguage>(), outputDirOpt.value(), exceptionsOpt.valueOptional<bool>(), inputInterfaceArg.value(), invertStreamOpt.isSet());
            });
        });

        CommandLine::Parser commandLineParser;
        commandLineParser.parse(argc, argv, app);
        return 0;
    }catch(const CommandLine::Exception& ex){
        std::cerr << "CommandLine error: " << ex.what() << std::endl;
        return -1000;
    }catch(const std::exception& ex){
        std::cerr << "Generator error: " << ex.what() << std::endl;
        return -1;
    }catch(...){
        std::cerr << "Unexpected error" << std::endl;
        return -2;
    }
}
