#pragma once

#include <string>

namespace StringResolve {

inline std::string resolve(const std::string& source){
    return source.substr(1, source.size() - 2);
}

}
