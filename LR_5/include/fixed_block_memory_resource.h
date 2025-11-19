#ifndef FIXED_BLOCK_MEMORY_RESOURCE_H
#define FIXED_BLOCK_MEMORY_RESOURCE_H

#include <memory_resource>
#include <list>
#include <cstddef>

struct MemoryBlock {
    void* ptr;
    std::size_t size;
    bool is_free;

    MemoryBlock(void* p, std::size_t s, bool f) : ptr(p), size(s), is_free(f) {}
};

class FixedBlockMemoryResource : public std::pmr::memory_resource {
private:
    char* pool;
    std::size_t pool_size;
    std::list<MemoryBlock> blocks;

    std::list<MemoryBlock>::iterator find_free_block(std::size_t bytes);
    void merge_free_blocks();

public:
    explicit FixedBlockMemoryResource(std::size_t size);
    ~FixedBlockMemoryResource() override;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
};

#endif