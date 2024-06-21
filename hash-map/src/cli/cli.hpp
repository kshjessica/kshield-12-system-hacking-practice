#pragma once

#include <string>
#include <vector>
#include "command.hpp"
#include "../util/error.hpp"
#include "../util/result.hpp"
#include "../util/string.hpp"
#include "../util/vector.hpp"

using namespace Util;

namespace CLI {

    static Result<Command> parse(std::vector<std::string> args) {
        if (args.empty()) {
            Error err(ErrorLevel::Warning, "CLI::parse", "args must not be empty");
            return Result<Command>(err);
        }

        std::string op = pop_front(args);
        Command cmd = Command(op, args);
        if (!cmd.is_valid()) {
            Error err(ErrorLevel::Warning, "CLI::parse", "command is not valid");
            return Result<Command>(err);
        }

        return Result<Command>(cmd);
    }

    static Result<uint64_t> run(std::string inp) {

        Result<std::vector<std::string>> split_res = split(inp, ' ');
        if (split_res.get_stats() != ResultStats::Ok)
            return Result<uint64_t>(split_res.get_err());

        std::vector<std::string> args = split_res.get_ok();
        Result<Command> parse_res = parse(args);
        if (parse_res.get_stats() != ResultStats::Ok)
            return Result<uint64_t>(parse_res.get_err());

        Command cmd = parse_res.get_ok();
        return cmd.run();
    }

} // namespace CLI
