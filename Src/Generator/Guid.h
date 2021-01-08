#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

namespace ComInterfaceGenerator {
struct Guid {
    std::uint32_t data1;
    std::uint16_t data2;
    std::uint16_t data3;
    std::uint8_t  data4[8];

    //6B751459-0E3C-4FE1-860D-8F75D15FBF66

    static bool fromString(const std::string& guidStr, Guid& result) {
        if(guidStr.size() != (16 * 2) + 4){
            return false;
        }
        result.data1 = std::stoul(guidStr.substr(0, 8), nullptr, 16);
        result.data2 = std::stoul(guidStr.substr(9, 4), nullptr, 16);
        result.data3 = std::stoul(guidStr.substr(14, 4), nullptr, 16);
        result.data4[0] = std::stoul(guidStr.substr(19, 2), nullptr, 16);
        result.data4[1] = std::stoul(guidStr.substr(21, 2), nullptr, 16);

        for(int i = 2; i < 8; ++i){
            result.data4[i] = std::stoul(guidStr.substr(24 + ((i - 2) * 2), 2), nullptr, 16);
        }
        return true;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << std::setfill('0') << std::hex;
        ss << std::setw(8) << data1;
        ss << "-";
        ss << std::setw(4) << data2;
        ss << "-";
        ss << std::setw(4) << data3;
        ss << "-";
        ss << std::setw(2) << +data4[0];
        ss << std::setw(2) << +data4[1];
        ss << "-";

        for(int i = 2; i < 8; ++i){
             ss << std::setw(2) << +data4[i];
        }
        return ss.str();
    }


    constexpr bool operator ==(const Guid& rhs) const {
        return
                (data1 == rhs.data1) &&
                (data2 == rhs.data2) &&
                (data3 == rhs.data3) &&
                (*reinterpret_cast<const std::uint64_t*>(data4) == *reinterpret_cast<const std::uint64_t*>(rhs.data4));
    }

    constexpr bool operator !=(const Guid& rhs) const {
        return !(this->operator ==(rhs));
    }
};

}
