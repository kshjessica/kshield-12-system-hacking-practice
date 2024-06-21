#pragma once

#include "object.hpp"

namespace Object {

    static const uint32_t ARRAY_BUFFER_SIZE = 8;

    class Array: public Object {

        public:
            void *operator new(size_t size);
            void operator delete(void *p);

            Array();

            virtual const char *get_type() {
                return "Array";
            }

    }; // class Array

    class IntegerArray: public Array {

        public:
            virtual const char *get_type() {
                return "IntegerArray";
            }

            void set_inline_data(size_t index, uint64_t value) {
                ((uint64_t *)this->inline_data)[index] = value;
            }
            uint64_t get_inline_data(size_t index) {
                return ((uint64_t *)this->inline_data)[index];
            }

    }; // class IntegerArray

    class ObjectArray: public Array {

        public:
            virtual const char *get_type() {
                return "ObjectArray";
            }

            void set_inline_data(size_t index, void* value) {
                ((void **)this->inline_data)[index] = value;
            }
            void *get_inline_data(size_t index) {
                return ((void **)this->inline_data)[index];
            }

    }; // class ObjectArray

} // namespace Object
