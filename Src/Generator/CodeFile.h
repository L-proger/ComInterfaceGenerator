#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <Type/Type.h>
#include <Type/Module.h>
#include <Generator/ExternalModule.h>

class CodeFile {
public:
    std::uint8_t identSize = 4;
    CodeFile& ident() {
        _ident++;
        return *this;
    }
    CodeFile& unident() {
        _ident--;
        return *this;
    }
    CodeFile& writeLine() {
        finalizeLine();
        return *this;
    }
    CodeFile& write(const std::string& text){
        if(_textLine.tellp() == 0){
            _textLine << getIdentString();
        }
        _textLine << text;
        return *this;
    }
    CodeFile& writeLine(const std::string& text){
       write(text);
       finalizeLine();
       return *this;
    }
    std::string toString() {
        finalizeLine();
        return _text.str();
    }

    CodeFile& space(std::size_t count = 1){
        for(std::size_t i = 0; i < count; ++i){
            write(" ");
          
        }
        return *this;
    }

    virtual void writeModule(std::shared_ptr<Module> module) = 0;

    virtual std::string getFileName(std::shared_ptr<Module> module) = 0;
private:

    void finalizeLine() {
        _text << _textLine.str() << "\r\n";
        _textLine.str("");
    }
    std::string getIdentString() {
        std::string result;
        for(int i = 0; i < (_ident * identSize); ++i){
            result += " ";
        }
        return result;
    }
    int _ident = 0;
    std::stringstream _textLine;
    std::stringstream _text;
};
