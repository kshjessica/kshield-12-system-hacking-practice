#include "hash_map.hpp"
#include "../core/context.hpp"

namespace Object {

    void *HashMap::operator new(size_t size) {
        Context ctx = Context::get_instance();
        Result<void *> alloc_res = ctx.allocator->alloc("HashMap", size);
        if (alloc_res.get_stats() != ResultStats::Ok)
            throw size;

        void *mem = alloc_res.get_ok();
        return mem;
    }
    void HashMap::operator delete(void *p) {
        Context ctx = Context::get_instance();
        Result<bool> dealloc_res = ctx.allocator->dealloc("HashMap", p);
    }

    HashMap::HashMap() {
        set_buffer(INIT_CAPACITY);
        ((uint32_t *)this->properties)[2] = INIT_CAPACITY;
    }

    Result<bool> HashMap::set_buffer(uint32_t capacity) {
        Context ctx = Context::get_instance();

        Result<void *> increase_res = ctx.allocator->increase("HashMapBuffer", sizeof(void *) * capacity);

        if (increase_res.get_stats() != ResultStats::Ok) {
            Error err(ErrorLevel::Warning, "HashMap::set_buffer", "failed to increase buffer");
            return Result<bool>(err);
        }

        if (this->get_property(0) == nullptr)
            this->set_property(0, increase_res.get_ok());

        return Result<bool>(true);
    }

    uint32_t HashMap::next_capacity(uint32_t capacity, uint32_t key_count) {
        if (!capacity)
            return INIT_CAPACITY;

        if (3 * key_count <= capacity && capacity > 64)
            return capacity;

        return capacity * 4;
    }

    bool HashMap::should_rehash(uint32_t capacity, uint32_t key_count, uint32_t delete_count) {
        return 2 * (key_count + delete_count) >= capacity;
    }

    Result<uint64_t> HashMap::get(uint32_t key) {
        Bucket **mem = (Bucket **)this->get_property(0);

        if (mem == NULL) {
            Error err(ErrorLevel::Warning, "HashMap::get", "HashMapBuffer must not be empty");
            return Result<uint64_t>(err);
        }

        uint32_t mask = ((uint32_t *)this->properties)[2] - 1; // capacity - 1
        uint32_t hashed_key = hash_key(key);

        Bucket *bucket = nullptr;
        
        for (int i = 0; i <= mask; i++) {
            Bucket *tmp = mem[i];
            if (tmp != NULL && tmp->get_key() == hashed_key) {
                bucket = tmp;
                break;
            }
        }

        if (bucket == nullptr) {
            Error err(ErrorLevel::Warning, "HashMap::get", "key is not exist");
            return Result<uint64_t>(err);
        }

        return Result<uint64_t>(bucket->get_value());
    }

    Result<bool> HashMap::set(uint32_t key, uint64_t value) {
        Bucket **mem = (Bucket **)this->get_property(0);

        if (mem == NULL) {
            Error err(ErrorLevel::Warning, "HashMap::set", "HashMapBuffer must not be empty");
            return Result<bool>(err);
        }

        uint32_t mask = ((uint32_t *)this->properties)[2] - 1; // capacity - 1
        uint32_t hashed_key = hash_key(key);

        int empty_idx = -1;

        for (int i = 0; i <= mask; i++) {
            Bucket *tmp = mem[i];

            if (empty_idx == -1 && tmp == nullptr)
                empty_idx = i;

            if (tmp != nullptr && tmp->get_key() == hashed_key) {
                Error err(ErrorLevel::Warning, "HashMap::set", "key is already exist");
                return Result<bool>(err);
            }
        }

        Bucket *bucket = new Bucket(hashed_key, value);
        mem[empty_idx] = bucket;

        ((uint32_t *)this->properties)[3] += 1; // key_count

        uint32_t capacity = ((uint32_t *)this->properties)[2];
        uint32_t key_count = ((uint32_t *)this->properties)[3];
        uint32_t delete_count = ((uint32_t *)this->properties)[4];

        if (should_rehash(capacity, key_count, delete_count)) {
            uint32_t new_capacity = next_capacity(capacity, key_count);

            if (new_capacity > capacity) {
                Result<bool> set_buffer_res = set_buffer(new_capacity);

                ((uint32_t *)this->properties)[2] = new_capacity; // vuln

                if (set_buffer_res.get_stats() != ResultStats::Ok) {
                    return set_buffer_res;
                }
            }
        }

        return Result<bool>(true);
    }

    Result<bool> HashMap::del(uint32_t key) {
        Bucket **mem = (Bucket **)this->get_property(0);

        if (mem == NULL) {
            Error err(ErrorLevel::Warning, "HashMap::del", "HashMapBuffer must not be empty");
            return Result<bool>(err);
        }

        uint32_t mask = ((uint32_t *)this->properties)[2] - 1; // capacity - 1
        uint32_t hashed_key = hash_key(key);

        int target_idx = -1;
        
        for (int i = 0; i <= mask; i++) {
            Bucket *tmp = mem[i];
            if (tmp != NULL && tmp->get_key() == hashed_key) {
                target_idx = i;
                break;
            }
        }
        
        if (target_idx == -1) {
            Error err(ErrorLevel::Warning, "HashMap::del", "key is not exist");
            return Result<bool>(err);
        }

        mem[target_idx]->unlink();
        delete mem[target_idx];
        mem[target_idx] = nullptr;

        ((uint32_t *)this->properties)[4] += 1; // delete_count
        ((uint32_t *)this->properties)[3] -= 1; // key_count

        return Result<bool>(true);
    }

    static uint32_t hash_key(uint32_t key) {
        uint64_t tmp = key;
        tmp += (tmp << 32) ^ 0xffffffffffffffff;
        tmp ^= tmp >> 22;
        tmp += (tmp << 13) ^ 0xffffffffffffffff;
        tmp ^= tmp >> 8;
        tmp += tmp << 3;
        tmp ^= (tmp >> 15);
        tmp += (tmp << 27) ^ 0xffffffffffffffff;
        tmp ^= tmp >> 31;

        return tmp;
    }

} // namespace Object
