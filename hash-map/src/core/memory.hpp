#pragma once

#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include "../util/result.hpp"

using namespace Util;

namespace Core {

    static const uint64_t MY_ALIGNE_SIZE = 0x100000000;
    static const uint64_t MY_PAGE_SIZE = 0x1000;

    class Memory {

        private:
            uint8_t *mem;
            uint64_t size;

        public:
            uint32_t idx;
            Memory(void *mem, uint64_t size) {
                this->mem = (uint8_t *)mem;
                this->size = size;
                this->idx = 0;
            }
            ~Memory() {
                munmap(this->mem, this->size);
            }

            void *alloc(size_t len) {
                if (this->idx + len > this->size) {
                    // TODO?
                    // OOM
                    return nullptr;
                }

                void *p = (void *) &this->mem[this->idx];
                this->idx += len;
                return p;
            }

            void *increase(size_t len) {
                if (len > this->size)
                    return nullptr;
                return (void *)this->mem;
            }

            static void *get_aligned_memory() {
                void *mem = mmap(NULL, MY_ALIGNE_SIZE * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

                size_t r_trim_size = (uint64_t)mem & (MY_ALIGNE_SIZE - 1);
                size_t l_trim_size = MY_ALIGNE_SIZE - r_trim_size;
                void *aligned_mem = (void *)((uint64_t)mem + l_trim_size);

                munmap(mem, l_trim_size);
                munmap((void *)((uint64_t)aligned_mem + MY_ALIGNE_SIZE), r_trim_size);

                return aligned_mem;
            }

    }; // class Memory

} // namespace Core
