#pragma once
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"

#include <iostream>
#include "object.hpp"
#include "../util/result.hpp"

using namespace Core;

namespace Object {

    class Bucket: public Object {

        private:
            // inline_data   = prev  (*Bucket)
            // properties[0] = next  (*Bucket)
            // properties[1] = key   (uint32_t)
            // properties[2] = value (uint64_t)

            inline void set_prev(Bucket *prev) {
                this->inline_data = prev;
            }
            inline void set_next(Bucket *next) {
                this->properties[0] = next;
            }

        public:
            void *operator new(size_t size);
            void operator delete(void *p);

            Bucket();
            Bucket(uint32_t key, uint64_t value);

            inline uint32_t get_key() {
                return ((uint32_t *)this->properties)[2];
            }
            inline uint64_t get_value() {
                return (uint64_t)this->get_property(2);
            }

            virtual const char *get_type() {
                return "Bucket";
            }

            void unlink();

    }; // class Bucket

} // namespace Object
