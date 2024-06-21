#pragma once
#pragma clang diagnostic ignored "-Wunused-variable"

#include <cstdio>
#include <cstdint>
#include "../core/allocator.hpp"
#include "../util/result.hpp"

namespace Core {
    class Context;
}

using namespace Core;
using namespace Util;

namespace Object {

    class Object {

        protected:
            void *inline_data;
            void *properties[4];

        public:
            void *operator new(size_t size);
            void operator delete(void *p);

            virtual const char *get_type() {
                return "Object";
            }

            void set_inline_data(void* value);
            void *get_inline_data();
            void set_property(uint32_t idx, void* value);
            void *get_property(uint32_t idx);

    }; // class Object

} // namespace Object
