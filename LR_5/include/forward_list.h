#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include <memory_resource>
#include <iterator>

template<typename T>
class forward_list {
private:
    struct Node {
        T value;
        Node* next = nullptr;
        
        template<typename U>
        Node(U&& val, Node* n) : value(std::forward<U>(val)), next(n) {}
    };

    using allocator_type = std::pmr::polymorphic_allocator<Node>;
    allocator_type alloc;
    Node* head = nullptr;

public:
    using value_type = T;

    forward_list() : alloc(std::pmr::get_default_resource()) {}
    explicit forward_list(std::pmr::memory_resource* res) : alloc(res) {}

    ~forward_list() {
        clear();
    }

    forward_list(const forward_list&) = delete;
    forward_list& operator=(const forward_list&) = delete;

    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            std::allocator_traits<allocator_type>::destroy(alloc, temp);
            alloc.deallocate(temp, 1);
        }
    }

    class iterator {
        Node* current;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(Node* node = nullptr) : current(node) {}

        reference operator*() const { return current->value; }
        pointer operator->() const { return &current->value; }

        iterator& operator++() {
            current = current->next;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return current != other.current; }
    };

    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }

    template<typename U>
    void push_front(U&& value) {
        Node* new_node = alloc.allocate(1);
        std::allocator_traits<allocator_type>::construct(alloc, new_node, std::forward<U>(value), head);
        head = new_node;
    }

    bool empty() const { return head == nullptr; }
};

#endif