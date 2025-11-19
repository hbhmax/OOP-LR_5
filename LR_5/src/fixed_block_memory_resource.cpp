#include "../include/fixed_block_memory_resource.h"
#include <stdexcept>
#include <iostream>

std::list<MemoryBlock>::iterator FixedBlockMemoryResource::find_free_block(std::size_t bytes) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->is_free && it->size >= bytes) {
            return it;
        }
    }
    return blocks.end();
}

FixedBlockMemoryResource::FixedBlockMemoryResource(std::size_t size)
    : pool(new char[size]), pool_size(size) {
    blocks.emplace_back(pool, pool_size, true);
}

FixedBlockMemoryResource::~FixedBlockMemoryResource() {
    delete[] pool;
}

void* FixedBlockMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    auto free_block_it = find_free_block(bytes);
    if (free_block_it == blocks.end()) {
        throw std::bad_alloc();
    }

    void* allocated_ptr = free_block_it->ptr;
    
    std::size_t remaining = free_block_it->size - bytes;
    if (remaining > 0) {
        blocks.emplace(std::next(free_block_it), static_cast<char*>(allocated_ptr) + bytes, remaining, true);
    }

    free_block_it->size = bytes;
    free_block_it->is_free = false;

    return allocated_ptr;
}

void FixedBlockMemoryResource::do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) {
    for (auto& block : blocks) {
        if (block.ptr == ptr && !block.is_free) {
            block.is_free = true;
            merge_free_blocks();
            return;
        }
    }
    throw std::invalid_argument("Invalid pointer to deallocate");
}

bool FixedBlockMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}

void FixedBlockMemoryResource::merge_free_blocks() {
    auto it = blocks.begin();
    while (it != blocks.end()) {
        auto next_it = std::next(it);
        if (next_it != blocks.end() && it->is_free && next_it->is_free &&
            static_cast<char*>(it->ptr) + it->size == next_it->ptr) {
            it->size += next_it->size;
            blocks.erase(next_it);
        } else {
            ++it;
        }
    }
}