#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include "fixed_block_memory_resource.h"
#include <memory>
#include <stdexcept>

template<typename T, typename Allocator = std::pmr::polymorphic_allocator<T>>
class SinglyLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        
        Node(const T& value, Node* nxt = nullptr)
            : data(value), next(nxt) {}
            
        Node(T&& value, Node* nxt = nullptr)
            : data(std::move(value)), next(nxt) {}
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    
    Node* head;
    Node* tail;
    size_t size_;
    NodeAllocator alloc;

public:
    explicit SinglyLinkedList(const Allocator& alloc = Allocator()) 
        : head(nullptr), tail(nullptr), size_(0), alloc(alloc) {}
    
    SinglyLinkedList(const SinglyLinkedList& other)
        : head(nullptr), tail(nullptr), size_(0), alloc(other.alloc) {
        for (auto it = other.head; it != nullptr; it = it->next) {
            push_back(it->data);
        }
    }
    
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head(other.head), tail(other.tail), size_(other.size_), alloc(std::move(other.alloc)) {
        other.head = nullptr;
        other.tail = nullptr;
        other.size_ = 0;
    }
    
    ~SinglyLinkedList() {
        destroy_list();
    }
    
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            destroy_list();
            for (auto it = other.head; it != nullptr; it = it->next) {
                push_back(it->data);
            }
        }
        return *this;
    }
    
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            destroy_list();
            head = other.head;
            tail = other.tail;
            size_ = other.size_;
            
            other.head = nullptr;
            other.tail = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
 
    void push_back(const T& value) {
        Node* new_node = allocate_node(value);
        if (tail) {
            tail->next = new_node;
            tail = new_node;
        } else {
            head = tail = new_node;
        }
        size_++;
    }
    
    void push_back(T&& value) {
        Node* new_node = allocate_node(std::move(value));
        if (tail) {
            tail->next = new_node;
            tail = new_node;
        } else {
            head = tail = new_node;
        }
        size_++;
    }
    
    void push_front(const T& value) {
        Node* new_node = allocate_node(value);
        new_node->next = head;
        head = new_node;
        if (size_ == 0) {
            tail = new_node;
        }
        size_++;
    }
    
    void push_front(T&& value) {
        Node* new_node = allocate_node(std::move(value));
        new_node->next = head;
        head = new_node;
        if (size_ == 0) {
            tail = new_node;
        }
        size_++;
    }
    
    void pop_front() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        Node* to_delete = head;
        head = head->next;
        if (head == nullptr) {
            tail = nullptr;
        }
        destroy_node(to_delete);
        size_--;
    }
    
    void pop_back() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        if (head == tail) {
            destroy_node(head);
            head = tail = nullptr;
        } else {
            Node* current = head;
            while (current->next != tail) {
                current = current->next;
            }
            destroy_node(tail);
            tail = current;
            tail->next = nullptr;
        }
        size_--;
    }
    
    T& front() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }
    
    const T& front() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }
    
    T& back() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail->data;
    }
    
    const T& back() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail->data;
    }
    
    bool empty() const {
        return size_ == 0;
    }
    
    size_t size() const {
        return size_;
    }
    
    void clear() {
        destroy_list();
    }
    
private:
    void destroy_list() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            destroy_node(current);
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        size_ = 0;
    }
    
    Node* allocate_node(const T& value) {
        Node* new_node = alloc.allocate(1);
        try {
            std::allocator_traits<NodeAllocator>::construct(alloc, new_node, value);
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
        return new_node;
    }
    
    Node* allocate_node(T&& value) {
        Node* new_node = alloc.allocate(1);
        try {
            std::allocator_traits<NodeAllocator>::construct(alloc, new_node, std::move(value));
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
        return new_node;
    }
    
    void destroy_node(Node* node) {
        std::allocator_traits<NodeAllocator>::destroy(alloc, node);
        alloc.deallocate(node, 1);
    }

public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;

    class const_iterator {
    protected:
        Node* current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        constexpr const_iterator() noexcept : current(nullptr) {}
        constexpr const_iterator(Node* node) noexcept : current(node) {}

        const_iterator& operator++() noexcept {
            if (current) current = current->next;
            return *this;
        }

        const_iterator operator++(int) noexcept {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        reference operator*() const noexcept {
            return current->data;
        }

        pointer operator->() const noexcept {
            return &current->data;
        }

        bool operator==(const const_iterator& other) const noexcept {
            return current == other.current;
        }

        bool operator!=(const const_iterator& other) const noexcept {
            return !(*this == other);
        }
    };

    class iterator : public const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        using const_iterator::const_iterator;

        iterator& operator++() noexcept {
            if (this->current) this->current = this->current->next;
            return *this;
        }

        iterator operator++(int) noexcept {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        reference operator*() noexcept {
            return const_cast<reference>(const_iterator::operator*());
        }

        pointer operator->() noexcept {
            return const_cast<pointer>(const_iterator::operator->());
        }
    };

    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }
    const_iterator begin() const { return const_iterator(head); }
    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator cbegin() const { return const_iterator(head); }
    const_iterator cend() const { return const_iterator(nullptr); }
};

#endif