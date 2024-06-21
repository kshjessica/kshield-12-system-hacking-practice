#pragma once

#include <iostream>
#include <map>
#include <string>
#include "memory.hpp"
#include "../util/result.hpp"

namespace Core {

    static const uint64_t BUCKET_PAGE_SIZE = 0x6;

    class Allocator {

        private:
            uint8_t *aligned_mem;
            uint32_t idx;
            std::map<const char *, Memory *> mapped_area;

        public:
            Allocator() {
                this->aligned_mem = (uint8_t*)Memory::get_aligned_memory();
                this->idx = 0;

                alloc("Bucket", 0);
            };
            ~Allocator() {
                for (auto iter = this->mapped_area.begin(); iter != this->mapped_area.end(); iter++)
                    delete iter->second;
            }

            Result<void *> alloc(const char *zone, size_t len) {
                auto area = this->mapped_area.find(zone);
                Memory *mem = nullptr;
                if (area == mapped_area.end()) {
                    if (!strcmp(zone, "Bucket")) {
                        mem = new Memory(&this->aligned_mem[MY_PAGE_SIZE * this->idx], MY_PAGE_SIZE * BUCKET_PAGE_SIZE);
                        this->idx += BUCKET_PAGE_SIZE;
                    } else
                        mem = new Memory(&this->aligned_mem[MY_PAGE_SIZE * this->idx++], MY_PAGE_SIZE);
                    this->mapped_area.insert({ zone, mem });
                } else
                    mem = area->second;

                void *p = mem->alloc(len);

                if (p == nullptr) {
                    Error err(ErrorLevel::Warning, "Allocator::alloc", "fail to allocate");
                    return Result<void *>(err);
                }

                return Result<void *>(p);
            }
            Result<bool> dealloc(const char *zone, void *ptr) {
                auto area = this->mapped_area.find(zone);
                if (area == this->mapped_area.end()) {
                    Error err(ErrorLevel::Warning, "Allocator::dealloc", "zone is not exists");
                    return Result<bool>(err);
                }

               // TODO?

                return Result<bool>(false); // Not implemented
            }

            // for HashMapBuffer
            Result<void *> increase(const char *zone, size_t len) {
                auto area = this->mapped_area.find(zone);
                Memory *mem = nullptr;
                if (area == mapped_area.end()) {
                    mem = new Memory(&this->aligned_mem[MY_PAGE_SIZE * this->idx++], MY_PAGE_SIZE);
                    this->mapped_area.insert({ zone, mem });
                } else
                    mem = area->second;

                void *p = mem->increase(len);

                if (p == nullptr) {
                    Error err(ErrorLevel::Warning, "Allocator::increase", "fail to increase");
                    return Result<void *>(err);
                }

                return Result<void *>(p);
            }

    }; // class Allocator

} // namespace Core
