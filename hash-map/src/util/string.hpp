#pragma once
#pragma clang diagnostic ignored "-Wunused-function"

#include <string>
#include <sstream>
#include <vector>
#include "error.hpp"
#include "result.hpp"

namespace Util {
    
    static Result<std::vector<std::string>> split(std::string inp, char delimiter) {
        if (inp.empty()) {
            Error err(ErrorLevel::Warning, "Util::split", "input must not be empty");
            return Result<std::vector<std::string>>(err);
        }

        std::istringstream iss(inp);
        std::string buffer;
        std::vector<std::string> result;

        while (std::getline(iss, buffer, delimiter))
            result.push_back(buffer);

        return Result<std::vector<std::string>>(result);
    }

    static bool isDigitStr(std::string str) {
        for (auto iter = str.begin(); iter != str.end(); iter++)
            if(!isdigit(*iter))
                return false;

        return true;
    }

} // namespace Util
