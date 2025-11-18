#ifndef MEMORY_RESOURCE_H
#define MEMORY_RESOURCE_H

#include <memory_resource>
#include <list>

class DynamicListMemoryResource : public std::pmr::memory_resource {
private:
    struct Block {
        void* ptr;
        size_t size;
        size_t alignment;
    };
    
    std::list<Block> allocated_blocks;
    std::list<Block> free_blocks;

public:
    DynamicListMemoryResource() = default;
    ~DynamicListMemoryResource();

    DynamicListMemoryResource(const DynamicListMemoryResource&) = delete;
    DynamicListMemoryResource& operator=(const DynamicListMemoryResource&) = delete;

protected:
    void* do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override;
    bool do_is_equal(const memory_resource& other) const noexcept override;
};

#endif