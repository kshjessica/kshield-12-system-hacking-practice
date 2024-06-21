#include "command.hpp"

namespace CLI {

    Command::Command() {
        this->op = Operator::None;
    }

    Command::Command(std::string op, std::vector<std::string> args) {
        if (!op.compare("create"))
            this->op = Operator::Create;
        else if (!op.compare("object"))
            this->op = Operator::Object;
        else if (!op.compare("hash_map"))
            this->op = Operator::HashMap;
        else if (!op.compare("list"))
            this->op = Operator::List;
        else if (!op.compare("quit"))
            this->op = Operator::Quit;
        else
            this->op = Operator::None;

        this->args = args;
    }

    bool Command::is_valid() {
        if (this->op == Operator::None)
            return false;
        
        if (this->op == Operator::Quit || this->op == Operator::List) {
            if (this->args.empty())
                return true;
            return false;
        }

        if (this->args.empty())
            return false;
        
        size_t args_sz = this->args.size();

        switch (this->op) {
            case Operator::Create:
                return (!args[0].compare("object") ||
                        !args[0].compare("integer_array") ||
                        !args[0].compare("object_array") ||
                        !args[0].compare("hash_map"));

            case Operator::Object:
                if(!isDigitStr(args[0]))
                    return false;

                if (args_sz < 3 || args_sz > 5)
                    return false;

                if (args[1].compare("set") && args[1].compare("get"))
                    return false;
                
                if (args[2].compare("property") && args[2].compare("item"))
                    return false;

                if (!isDigitStr(args[3]))
                    return false;

                if (args_sz > 4)
                    if (!isDigitStr(args[4]))
                        return false;

                break;
                
            case Operator::HashMap:
                if (args_sz < 2 || 3 < args_sz)
                    return false;

                if (args[0].compare("set") && args[0].compare("get") && args[0].compare("delete"))
                    return false;

                if (!isDigitStr(args[1]))
                        return false;

                if (args_sz > 2)
                    if (!isDigitStr(args[2]))
                        return false;
                break;
                
            default :
                return false;
        }

        return true;
    }

    Result<uint64_t> Command::run() {
        switch(this->op) {
            case Operator::Create:
                return create(this->args);

            case Operator::Object:
                return object(this->args);

            case Operator::HashMap:
                return hash_map(this->args);

            case Operator::List:
                return list();

            case Operator::Quit:
                return Result<uint64_t>(-1);

            default:
                return Result<uint64_t>(0);
        }
    }

} // namespace CLI
