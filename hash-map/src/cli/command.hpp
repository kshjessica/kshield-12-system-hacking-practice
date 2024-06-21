#pragma once
#pragma clang diagnostic ignored "-Wunused-function"

#include <iostream>
#include <string>
#include <vector>
#include "../core/context.hpp"
#include "../object/object.hpp"
#include "../object/array.hpp"
#include "../object/hash_map.hpp"
#include "../util/error.hpp"
#include "../util/result.hpp"
#include "../util/string.hpp"
#include "../util/vector.hpp"

using namespace Core;
using namespace Util;

namespace CLI {

    enum class Operator {
        
        None,
        Create,
        Object,
        HashMap,
        List,
        Quit,

    }; // enum class Operand

    class Command {

        private:
            Operator op;
            std::vector<std::string> args;

            static Result<Object::Object *> get_object_from_context(size_t idx) {
                Context ctx = Context::get_instance();

                if (ctx.object_list->size() <= idx) {
                    Error err(ErrorLevel::Warning, "Command::get_object_from_context", "index is invalid");
                    return Result<Object::Object *>(err);
                }

                Object::Object *obj = ctx.object_list->at(idx);
                if (obj == nullptr) {
                    Error err(ErrorLevel::Warning, "Command::get_object_from_context", "object must not be empty");
                    return Result<Object::Object *>(err);
                }

                return Result<Object::Object *>(obj);
            }

            static Result<size_t> create_object() {
                Context ctx = Context::get_instance();

                try {
                    Object::Object *obj = new Object::Object();
                    size_t idx = ctx.object_list->size();
                    ctx.object_list->push_back(obj);
                    return Result<size_t>(idx);
                } catch (int i) {
                    Error err(ErrorLevel::Warning, "Command::create_object", "failed to allocate Object");
                    return Result<size_t>(err);
                }
            }

            static Result<size_t> create_integer_array() {
                Context ctx = Context::get_instance();

                try {
                    Object::IntegerArray *int_arr = new Object::IntegerArray();
                    size_t idx = ctx.object_list->size();
                    ctx.object_list->push_back(int_arr);
                    return Result<size_t>(idx);
                } catch (int i) {
                    Error err(ErrorLevel::Warning, "Command::create_integer_array", "failed to allocate IntegerArray");
                    return Result<size_t>(err);
                }
            }

            static Result<size_t> create_object_array() {
                Context ctx = Context::get_instance();

                try {
                    Object::ObjectArray *obj_arr = new Object::ObjectArray();
                    size_t idx = ctx.object_list->size();
                    ctx.object_list->push_back(obj_arr);
                    return Result<size_t>(idx);
                } catch (int i) {
                    Error err(ErrorLevel::Warning, "Command::create_object_array", "failed to allocate ObjectArray");
                    return Result<size_t>(err);
                }
            }

            static Result<bool> create_hash_map() {
                Context ctx = Context::get_instance();

                if (ctx.map->get_stats() == OptionStats::None) {
                    Object::HashMap *map = new Object::HashMap();
                    ctx.map->set_value(map);
                    return Result<bool>(ctx.map->set_value(map));
                }

                return Result<bool>(true);
            }

            static Result<size_t> create(std::vector<std::string> args) {
                if (args.size() != 1) {
                    Error err(ErrorLevel::Warning, "Command::create", "argc is invalid");
                    return Result<size_t>(err);
                }

                std::string object_kind = args[0];

                if (!object_kind.compare("object"))
                    return create_object();
                else if (!object_kind.compare("integer_array"))
                    return create_integer_array();
                else if (!object_kind.compare("object_array"))
                    return create_object_array();
                else if (!object_kind.compare("hash_map")) {
                    auto res = create_hash_map();
                    if (res.get_stats() == ResultStats::Err)
                        return Result<size_t>(res.get_err());
                    return Result<size_t>(0);
                }

                Error err(ErrorLevel::Warning, "Command::create", "object_kind is invalid");
                return Result<size_t>(err);
            }

            Result<bool> hash_map_set(Object::HashMap *map, std::vector<std::string> args){
                uint32_t key = std::stoi(args[0]);
                uint64_t value = std::stol(args[1]);
                return map->set(key, value);
            }
            Result<size_t> hash_map_get(Object::HashMap *map, std::vector<std::string> args){
                uint32_t key = std::stoi(args[0]);
                return map->get(key);
            }
            Result<bool> hash_map_delete(Object::HashMap *map, std::vector<std::string> args){
                uint32_t key = std::stoi(args[0]);
                return map->del(key);
            }

            Result<uint64_t> hash_map(std::vector<std::string> args){
                Context ctx = Context::get_instance();
                if (ctx.map->get_stats() == OptionStats::None) {
                    Error err(ErrorLevel::Warning, "Command::hash_map", "hash_map must not be empty");
                    return Result<uint64_t>(err);
                }

                Object::HashMap *map = ctx.map->get_value();

                std::string cmd = pop_front(args);
                if (!cmd.compare("set")) {
                    if (args.size() != 2) {
                        Error err(ErrorLevel::Warning, "Command::hash_map", "argc is invalid");
                        return Result<uint64_t>(err);
                    }
                    auto res = hash_map_set(map, args);
                    if (res.get_stats() != ResultStats::Ok)
                        return Result<uint64_t>(res.get_err());
                    return Result<uint64_t>(0);
                } else if (!cmd.compare("get")) {
                    if (args.size() != 1) {
                        Error err(ErrorLevel::Warning, "Command::hash_map", "argc is invalid");
                        return Result<uint64_t>(err);
                    }
                    return hash_map_get(map, args);
                } else if (!cmd.compare("delete")) {
                    if (args.size() != 1) {
                        Error err(ErrorLevel::Warning, "Command::hash_map", "argc is invalid");
                        return Result<uint64_t>(err);
                    }
                    auto res = hash_map_delete(map, args);
                    if (res.get_stats() != ResultStats::Ok)
                        return Result<uint64_t>(res.get_err());
                    return Result<uint64_t>(0);
                }

                Error err(ErrorLevel::Warning, "Command::hash_map", "method is invalid");
                return Result<uint64_t>(err);
            }

            static Result<size_t> list() {
                Context ctx = Context::get_instance();
                puts("================= List =================");
                auto iter = ctx.object_list->begin();
                for (int i = 0; iter != ctx.object_list->end(); i++, iter++)
                    printf("[%i] %s\n", i, (*iter)->get_type());
                puts("========================================");
                return Result<size_t>(0);
            }

            static Result<uint64_t> object_action(Object::Object *obj) {
                const char *obj_type = obj->get_type();
                std::string inp;

                if (!std::strcmp(obj_type, "Object"))
                    std::cout << "    Object > ";
                else if (!std::strcmp(obj_type, "IntegerArray"))
                    std::cout << "    IntegerArray > ";
                else if (!std::strcmp(obj_type, "ObjectArray"))
                    std::cout << "    ObjectArray > ";
                else {
                    Error err(ErrorLevel::Warning, "Command::object_action", "object is invalid");
                    return Result<uint64_t>(err);
                }

                std::getline(std::cin, inp);
                Result<std::vector<std::string>> split_res = split(inp, ' ');
                if (split_res.get_stats() != ResultStats::Ok)
                    return Result<uint64_t>(split_res.get_err());

                std::vector<std::string> args = split_res.get_ok();
                if (!args[0].compare("get")) {
                    if (args.size() != 3) {
                        Error err(ErrorLevel::Warning, "Command::object_action", "argc is invalid");
                        return Result<uint64_t>(err);
                    }

                    size_t elem_idx = std::stoll(args[2]);

                    return object_get(obj, args[1], elem_idx);
                } else if (!args[0].compare("set")) {
                    if (args.size() != 4) {
                        Error err(ErrorLevel::Warning, "Command::object_action", "argc is invalid");
                        return Result<uint64_t>(err);
                    }

                    size_t elem_idx = std::stoll(args[2]);
                    uint64_t value = std::stoll(args[3]);

                    return object_set(obj, args[1], elem_idx, value);
                } else {
                    Error err(ErrorLevel::Warning, "Command::object_action", "method is invalid");
                    return Result<uint64_t>(err);
                }
            }

            static Result<uint64_t> object_get(Object::Object *obj, std::string target, size_t elem_idx) {
                const char *obj_type = obj->get_type();

                if (!std::strcmp(obj_type, "Object")) {
                    if (!target.compare("item")) {
                        Error err(ErrorLevel::Warning, "Command::object_get", "object cannot have item");
                        return Result<uint64_t>(err);
                    } else if (!target.compare("property")) {
                        if (4 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_get", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        return Result<uint64_t>((uint64_t)obj->get_property(elem_idx));
                    }

                    Error err(ErrorLevel::Warning, "Command::object_get", "target is invalid");
                    return Result<uint64_t>(err);
                } else if (!std::strcmp(obj_type, "IntegerArray")) {
                    Object::IntegerArray *int_arr = (Object::IntegerArray *)obj;

                    if (!target.compare("item")) {
                        if (8 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_get", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        return Result<uint64_t>(int_arr->get_inline_data(elem_idx));
                    } else if (!target.compare("property")) {
                        if (4 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_get", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        return Result<uint64_t>((uint64_t)int_arr->get_property(elem_idx));
                    }

                    Error err(ErrorLevel::Warning, "Command::object_get", "target is invalid");
                    return Result<uint64_t>(err);
                } else if (!std::strcmp(obj_type, "ObjectArray")) {
                    Object::ObjectArray *obj_arr = (Object::ObjectArray *)obj;

                    if (!target.compare("item")) {
                        if (8 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_get", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }

                        Object::Object *get = (Object::Object *)obj_arr->get_inline_data(elem_idx);
                        if (get == nullptr) {
                            Error err(ErrorLevel::Warning, "Command::object_get", "element must not be empty");
                            return Result<uint64_t>(err);
                        }

                        return object_action(get);

                    } else if (!target.compare("property")) {
                        if (4 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_get", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        return Result<uint64_t>((uint64_t)obj_arr->get_property(elem_idx));
                    }

                    Error err(ErrorLevel::Warning, "Command::object_get", "target is invalid");
                    return Result<uint64_t>(err);
                }

                printf("%p\n", obj);
                Error err(ErrorLevel::Warning, "Command::object_get", "object is invalid");
                return Result<uint64_t>(err);
            }

            static Result<uint64_t> object_set(Object::Object *obj, std::string target, size_t elem_idx, uint64_t value) {

                if (!std::strcmp(obj->get_type(), "Object")) {
                    if (!target.compare("item")) {
                        Error err(ErrorLevel::Warning, "Command::object_set", "object cannot have item");
                        return Result<uint64_t>(err);
                    } else if (!target.compare("property")) {
                        if (4 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_set", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        obj->set_property(elem_idx, (void *)value);
                        return Result<uint64_t>(0);
                    }

                    Error err(ErrorLevel::Warning, "Command::object_set", "target is invalid");
                    return Result<uint64_t>(err);
                } else if (!std::strcmp(obj->get_type(), "IntegerArray")) {
                    Object::IntegerArray *int_arr = (Object::IntegerArray *)obj;

                    if (!target.compare("item")) {
                        if (8 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_set", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        int_arr->set_inline_data(elem_idx, value);
                        return Result<uint64_t>(0);
                    } else if (!target.compare("property")) {
                        if (4 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_set", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        int_arr->set_property(elem_idx, (void *)value);
                        return Result<uint64_t>(0);
                    }

                    Error err(ErrorLevel::Warning, "Command::object_set", "target is invalid");
                    return Result<uint64_t>(err);
                } else if (!std::strcmp(obj->get_type(), "ObjectArray")) {
                    Object::ObjectArray *obj_arr = (Object::ObjectArray *)obj;

                    if (!target.compare("item")) {
                        if (8 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_set", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }

                        Result<Object::Object *> get_res = get_object_from_context(value);
                        if (get_res.get_stats() != ResultStats::Ok) {
                            return Result<uint64_t>(get_res.get_err());
                        }

                        obj_arr->set_inline_data(elem_idx, (void *)get_res.get_ok());
                        return Result<uint64_t>(0);

                    } else if (!target.compare("property")) {
                        if (4 <= elem_idx) {
                            Error err(ErrorLevel::Warning, "Command::object_set", "elem_idx is invalid");
                            return Result<uint64_t>(err);
                        }
                        obj_arr->set_property(elem_idx, (void *)value);
                        return Result<uint64_t>(0);
                    }

                    Error err(ErrorLevel::Warning, "Command::object_set", "target is invalid");
                    return Result<uint64_t>(err);
                }

                Error err(ErrorLevel::Warning, "Command::object_set", "object is invalid");
                return Result<uint64_t>(err);
            }

            static Result<uint64_t> object(std::vector<std::string> args) {
                if (!args[1].compare("get")) {
                    if (args.size() != 4) {
                        Error err(ErrorLevel::Warning, "Command::object", "argc is invalid");
                        return Result<uint64_t>(err);
                    }

                    size_t obj_idx = std::stoll(args[0]);
                    Result<Object::Object *> get_res = get_object_from_context(obj_idx);
                    if (get_res.get_stats() != ResultStats::Ok) {
                        return Result<uint64_t>(get_res.get_err());
                    }

                    Object::Object *obj = get_res.get_ok();
                    size_t elem_idx = std::stoll(args[3]);

                    return object_get(obj, args[2], elem_idx);

                } else if (!args[1].compare("set")) {
                    if (args.size() != 5) {
                        Error err(ErrorLevel::Warning, "Command::object", "argc is invalid");
                        return Result<uint64_t>(err);
                    }

                    size_t obj_idx = std::stoll(args[0]);
                    Result<Object::Object *> get_res = get_object_from_context(obj_idx);
                    if (get_res.get_stats() != ResultStats::Ok) {
                        return Result<uint64_t>(get_res.get_err());
                    }

                    Object::Object *obj = get_res.get_ok();
                    size_t elem_idx = std::stoll(args[3]);
                    uint64_t value = std::stoll(args[4]);

                    return object_set(obj, args[2], elem_idx, value);
                }

                Error err(ErrorLevel::Warning, "Command::object", "method is invalid");
                return Result<uint64_t>(err);
            }

        public:
            Command();
            Command(std::string op, std::vector<std::string> args);
            bool is_valid();
            Result<uint64_t> run();

    }; // class Command

} // namespace CLI
