#pragma once

#include <map>
#include <vector>
#include "allocator.hpp"
#include "../object/object.hpp"
#include "../object/hash_map.hpp"
#include "../util/option.hpp"

namespace Core {

    class Context {

        private:
            Context() {
                this->allocator = new Allocator();
                this->object_list = new std::vector<Object::Object *>();
                this->map = new Util::Option<Object::HashMap *>();
            }

        public:
            std::vector<Object::Object *> *object_list;
            Util::Option<Object::HashMap *> *map;

            static Context &get_instance() {
                static Context ctx;
                return ctx;
            }

            Allocator *allocator;

    }; // class Context

} // namespace Core
