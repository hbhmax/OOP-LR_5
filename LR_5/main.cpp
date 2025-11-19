#include <iostream>
#include "include/singly_linked_list.h"

struct ComplexType {
    int id;
    double value;
    std::string name;
    
    ComplexType(int i, double v, const std::string& n) : id(i), value(v), name(n) {}
    
    friend std::ostream& operator<<(std::ostream& os, const ComplexType& obj) {
        os << "ComplexType{id=" << obj.id << ", value=" << obj.value << ", name=\"" << obj.name << "\"}";
        return os;
    }
};

int main() {    
    FixedBlockMemoryResource pool(2048);

    std::cout << "\n--- Testing with int types ---" << std::endl;
    
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> intList(&pool);
    
    for (int i = 1; i <= 5; ++i) {
        intList.push_back(i * 10);
    }
    
    std::cout << "Using ++ iterator:" << std::endl;
    std::cout << "List contents: ";
    for (auto it = intList.begin(); it != intList.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Front element: " << intList.front() << std::endl;
    std::cout << "Back element: " << intList.back() << std::endl;
    
    std::cout << std::endl;
    
    std::cout << "\n--- Testing with ComplexType ---" << std::endl;
    
    SinglyLinkedList<ComplexType, std::pmr::polymorphic_allocator<ComplexType>> complexList(&pool);
    
    complexList.push_back(ComplexType(1, 3.14, "First"));
    complexList.push_back(ComplexType(2, 2.71, "Second"));
    complexList.push_back(ComplexType(3, 1.41, "Third"));
    complexList.push_back(ComplexType(4, 1.62, "Fourth"));
    
    std::cout << "\nUsing iterator:" << std::endl;
    for (auto it = complexList.begin(); it != complexList.end(); it++) {
        ComplexType item = *it;
        std::cout << item << std::endl;
    }
    
    std::cout << "\nUsing const_iterator:" << std::endl;
    const auto& constComplexList = complexList;
    for (auto it = constComplexList.begin(); it != constComplexList.end(); ++it) {
        std::cout << "ID: " << it->id << ", Name: " << it->name << std::endl;
    }
    
    std::cout << "\n--- List Sizes ---" << std::endl;
    std::cout << "intList size: " << intList.size() << std::endl;
    std::cout << "complexList size: " << complexList.size() << std::endl;
    
    std::cout << "\n--- Empty Check ---" << std::endl;
    std::cout << "intList is empty: " << (intList.empty() ? "true" : "false") << std::endl;
    std::cout << "complexList is empty: " << (complexList.empty() ? "true" : "false") << std::endl;
    
    std::cout << "\n--- Testing clear() ---" << std::endl;
    intList.clear();
    std::cout << "After clear - intList size: " << intList.size() << std::endl;
    std::cout << "intList is empty: " << (intList.empty() ? "true" : "false") << std::endl;
    
    return 0;
}