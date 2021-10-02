#include "TypeCache.h"
#include <filesystem>
#include <Generator/CSharp/CSharpGenerator.h>
#include <Generator/Cpp/CppGenerator.h>
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

void run(OutputLanguage language, const std::string& outputDir, const std::vector<std::string>& inputDirs, const std::vector<std::string>& modules, std::optional<bool> enableExceptions) {
    if(modules.empty()){
        throw std::runtime_error("No input files");
    }

    TypeCache::init();
    for(auto& dir : inputDirs){
        TypeCache::addSearchPath(dir);
    }

    for(auto& module : modules){
        TypeCache::parseModule(module);
    }

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

int main(int argc, const char* const* argv) {
    try{
        auto version = CommandLine::OptionDescription("--version", "Print version", CommandLine::OptionType::NoValue).alias("-v");
        auto exceptions = CommandLine::OptionDescription("--exceptions", "Enable exceptions handling in generated files", CommandLine::OptionType::SingleValue).alias("-e");
        auto language = CommandLine::OptionDescription("--language", "Output files language", CommandLine::OptionType::SingleValue).alias("-l");
        auto outputDir = CommandLine::OptionDescription("--output", "Output files directory", CommandLine::OptionType::SingleValue).alias("-o");
        auto inputDirs = CommandLine::OptionDescription("--input",  "Input files directory", CommandLine::OptionType::MultipleValues).alias("-I");
        CommandLine::ArgumentDescription inputModules("Module names",  "Modulae names to generate", CommandLine::ArgumentType::MultipleValues);

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
                run(languageOpt.value<OutputLanguage>(), outputDirOpt.value(), inputDirsOpt.values(), inputModulesArg.values(), exceptionsOpt.valueOptional<bool>());
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
