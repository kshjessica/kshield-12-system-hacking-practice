#include "object.hpp"
#include "../core/context.hpp"

namespace Object {

    void *Object::operator new(size_t size) {
        Context ctx = Context::get_instance();
        Result<void *> alloc_res = ctx.allocator->alloc("Object", size);
        if (alloc_res.get_stats() != ResultStats::Ok)
            throw size;

        return alloc_res.get_ok();
    }
    void Object::operator delete(void *p) {
        Context ctx = Context::get_instance();
        Result<bool> dealloc_res = ctx.allocator->dealloc("Object", p);
    }

    void Object::set_inline_data(void* value) {
        this->inline_data = (void*)value;
    }
    void *Object::get_inline_data() {
        return this->inline_data;
    }
    void Object::set_property(uint32_t idx, void* value) {
        this->properties[idx] = (void*)value;
    }
    void *Object::get_property(uint32_t idx) {
        return this->properties[idx];
    }

} // namespace Object
