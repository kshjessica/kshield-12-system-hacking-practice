#include "array.hpp"
#include "../core/context.hpp"

namespace Object {

    // class Array
    void *Array::operator new(size_t size) {
        Context ctx = Context::get_instance();
        Result<void *> alloc_res = ctx.allocator->alloc("Array", size);
        if (alloc_res.get_stats() != ResultStats::Ok)
            throw size;

        void *mem = alloc_res.get_ok();
        return mem;
    }
    void Array::operator delete(void *p) {
        Context ctx = Context::get_instance();
        Result<bool> dealloc_res = ctx.allocator->dealloc("Array", p);
    }

    Array::Array() {
        Context ctx = Context::get_instance();

        Result<void *> alloc_res = ctx.allocator->alloc("ArrayBuffer", sizeof(uint64_t) * ARRAY_BUFFER_SIZE);
        if (alloc_res.get_stats() != ResultStats::Ok) {
            Error err(ErrorLevel::Warning, "Array::Array", "failed to allocate array buffer");
            return;
        }
        this->set_inline_data(alloc_res.get_ok());

        return;
    }

} // namespace Object
