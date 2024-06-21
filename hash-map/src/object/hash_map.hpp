#pragma once
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"

#include <string>
#include <utility>
#include <cstring>
#include "object.hpp"
#include "bucket.hpp"
#include "../util/result.hpp"

using namespace Core;
using namespace Util;

namespace Object {

    // key @ uint32_t
    // value @ uint64_t
    class HashMap: public Object {

        static const uint32_t INIT_CAPACITY = 4;

        private:
            // properties[0] = *HashMapBuffer
            // ((uint32_t *)properties)[2] = capacity -- capacity-1 is mask
            // ((uint32_t *)properties)[3] = key_count
            // ((uint32_t *)properties)[4] = delete_count

            Result<bool> set_buffer(uint32_t capacity);
            uint32_t next_capacity(uint32_t capacity, uint32_t key_count);
            bool should_rehash(uint32_t capacity, uint32_t key_count, uint32_t delete_count);

        public:
            void *operator new(size_t size);
            void operator delete(void *p);

            HashMap();

            Result<uint64_t> get(uint32_t key);
            Result<bool> set(uint32_t key, uint64_t value);
            Result<bool> del(uint32_t key);

            virtual const char *get_type() {
                return "HashMap";
            }

    }; // class HashMap

    static uint32_t hash_key(uint32_t key);

} // namespace Object
