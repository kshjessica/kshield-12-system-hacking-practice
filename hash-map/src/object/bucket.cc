#include "bucket.hpp"
#include "../core/context.hpp"

using namespace Core;

namespace Object {

    void *Bucket::operator new(size_t size) {
        Context ctx = Core::Context::get_instance();
        Result<void *> alloc_res = ctx.allocator->alloc("Bucket", sizeof(Bucket));
        if (alloc_res.get_stats() != ResultStats::Ok)
            throw size;

        void *mem = alloc_res.get_ok();
        return mem;
    }
    void Bucket::operator delete(void *p) {
        Context ctx = Core::Context::get_instance();
        Result<bool> dealloc_res = ctx.allocator->dealloc("Bucket", p);
    }

    Bucket::Bucket(uint32_t key, uint64_t value) {
        ((uint32_t *)this->properties)[2] = key;
        this->set_property(2, (void *)value);
    }

    void Bucket::unlink() {
        Bucket *prev = (Bucket *) this->get_inline_data();
        Bucket *next = (Bucket *) this->get_property(0);

        if (prev != nullptr && next != nullptr) {
            prev->set_next(next);
            next->set_prev(prev);
        }
    }

} // namespace Object
