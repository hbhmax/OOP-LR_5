#include "../include/memory_resource.h"
#include <memory>

void* DynamicListMemoryResource::do_allocate(size_t bytes, size_t alignment) {
    auto it = free_blocks.begin();
    while (it != free_blocks.end()) {
        if (it->size >= bytes && it->alignment >= alignment) {
            void* ptr = it->ptr;
            allocated_blocks.splice(allocated_blocks.end(), free_blocks, it);
            return ptr;
        }
        ++it;
    }

    void* ptr = ::operator new(bytes, std::align_val_t{alignment});
    allocated_blocks.push_back({ptr, bytes, alignment});
    return ptr;
}

void DynamicListMemoryResource::do_deallocate(void* ptr, size_t bytes, size_t alignment) {
    auto it = allocated_blocks.begin();
    while (it != allocated_blocks.end()) {
        if (it->ptr == ptr) {
            free_blocks.splice(free_blocks.end(), allocated_blocks, it);
            return;
        }
        ++it;
    }
}

bool DynamicListMemoryResource::do_is_equal(const memory_resource& other) const noexcept {
    return this == &other;
}

DynamicListMemoryResource::~DynamicListMemoryResource() {
    for (const auto& block : allocated_blocks) {
        ::operator delete(block.ptr, std::align_val_t{block.alignment});
    }
}
