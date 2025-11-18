#include <iostream>
#include "include/memory_resource.h"
#include "forward_list.h"

struct ComplexType {
    int a;
    double b;
    char c;

    ComplexType(int a, double b, char c) : a(a), b(b), c(c) {}

    friend std::ostream& operator<<(std::ostream& os, const ComplexType& obj) {
        os << "(" << obj.a << ", " << obj.b << ", " << obj.c << ")";
        return os;
    }
};

void demo_simple_types() {
    std::cout << "=== Demo with simple types (int) ===" << std::endl;

    DynamicListMemoryResource mem_res;
    forward_list<int> int_list(&mem_res);

    int_list.push_front(10);
    int_list.push_front(20);
    int_list.push_front(30);
    int_list.push_front(40);

    std::cout << "List contents: ";
    for (auto it = int_list.begin(); it != int_list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Range-based for: ";
    for (const auto& value : int_list) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

void demo_complex_types() {
    std::cout << "\n=== Demo with complex types ===" << std::endl;

    DynamicListMemoryResource mem_res;
    forward_list<ComplexType> complex_list(&mem_res);

    complex_list.push_front(ComplexType{1, 1.1, 'A'});
    complex_list.push_front(ComplexType{2, 2.2, 'B'});
    complex_list.push_front(ComplexType{3, 3.3, 'C'});
    complex_list.push_front(ComplexType{4, 4.4, 'D'});

    std::cout << "Complex list contents: ";
    for (auto it = complex_list.begin(); it != complex_list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Accessing fields: ";
    for (auto it = complex_list.begin(); it != complex_list.end(); ++it) {
        std::cout << "{" << it->a << "|" << it->b << "|" << it->c << "} ";
    }
    std::cout << std::endl;
}

void demo_multiple_containers() {
    std::cout << "\n=== Demo multiple containers sharing memory resource ===" << std::endl;

    DynamicListMemoryResource shared_mem_res;

    forward_list<int> list1(&shared_mem_res);
    forward_list<int> list2(&shared_mem_res);

    list1.push_front(100);
    list1.push_front(200);
    list2.push_front(300);
    list2.push_front(400);

    std::cout << "List 1: ";
    for (auto val : list1) std::cout << val << " ";
    std::cout << std::endl;

    std::cout << "List 2: ";
    for (auto val : list2) std::cout << val << " ";
    std::cout << std::endl;

}

int main() {
    demo_simple_types();
    demo_complex_types();
    demo_multiple_containers();

    std::cout << "\nAll demonstrations completed successfully!" << std::endl;
    return 0;
}
