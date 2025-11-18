#include <gtest/gtest.h>
#include <memory>
#include "../include/memory_resource.h"
#include "../include/forward_list.h"

using namespace std;

TEST(MemoryResourceTest, BasicAllocation) {
    DynamicListMemoryResource mem_res;
    
    void* ptr1 = mem_res.allocate(100, alignof(max_align_t));
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = mem_res.allocate(200, alignof(max_align_t));
    EXPECT_NE(ptr2, nullptr);
    
    mem_res.deallocate(ptr1, 100, alignof(max_align_t));
    mem_res.deallocate(ptr2, 200, alignof(max_align_t));
}

TEST(MemoryResourceTest, MemoryReuse) {
    DynamicListMemoryResource mem_res;
    
    void* ptr1 = mem_res.allocate(100, alignof(max_align_t));
    mem_res.deallocate(ptr1, 100, alignof(max_align_t));
    
    void* ptr2 = mem_res.allocate(100, alignof(max_align_t));
    EXPECT_NE(ptr2, nullptr);
    
    mem_res.deallocate(ptr2, 100, alignof(max_align_t));
}

TEST(MemoryResourceTest, MultipleAllocations) {
    DynamicListMemoryResource mem_res;
    
    const int count = 10;
    void* pointers[count];
    
    for (int i = 0; i < count; ++i) {
        pointers[i] = mem_res.allocate(50 * (i + 1), alignof(max_align_t));
        EXPECT_NE(pointers[i], nullptr);
    }
    
    for (int i = count - 1; i >= 0; --i) {
        mem_res.deallocate(pointers[i], 50 * (i + 1), alignof(max_align_t));
    }
}

TEST(ForwardListTest, BasicOperationsInt) {
    DynamicListMemoryResource mem_res;
    forward_list<int> list(&mem_res);
    
    EXPECT_TRUE(list.empty());
    
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    
    EXPECT_FALSE(list.empty());
    
    auto it = list.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, list.end());
}

TEST(ForwardListTest, RangeBasedForInt) {
    DynamicListMemoryResource mem_res;
    forward_list<int> list(&mem_res);
    
    list.push_front(10);
    list.push_front(20);
    list.push_front(30);
    
    int sum = 0;
    int count = 0;
    for (const auto& item : list) {
        sum += item;
        count++;
    }
    
    EXPECT_EQ(sum, 60);
    EXPECT_EQ(count, 3);
}

TEST(ForwardListTest, ClearOperations) {
    DynamicListMemoryResource mem_res;
    forward_list<int> list(&mem_res);
    
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    
    EXPECT_FALSE(list.empty());
    
    list.clear();
    
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.begin(), list.end());
}

struct TestComplexType {
    int id;
    double value;
    std::string name;
    
    TestComplexType(int i, double v, const std::string& n) 
        : id(i), value(v), name(n) {}
    
    bool operator==(const TestComplexType& other) const {
        return id == other.id && value == other.value && name == other.name;
    }
};

TEST(ForwardListTest, ComplexTypeOperations) {
    DynamicListMemoryResource mem_res;
    forward_list<TestComplexType> list(&mem_res);
    
    list.push_front(TestComplexType(1, 3.14, "first"));
    list.push_front(TestComplexType(2, 2.71, "second"));
    list.push_front(TestComplexType(3, 1.41, "third"));
    
    auto it = list.begin();
    EXPECT_EQ(it->id, 3);
    EXPECT_EQ(it->value, 1.41);
    EXPECT_EQ(it->name, "third");
    
    ++it;
    EXPECT_EQ(it->id, 2);
    EXPECT_EQ(it->value, 2.71);
    EXPECT_EQ(it->name, "second");
    
    ++it;
    EXPECT_EQ(it->id, 1);
    EXPECT_EQ(it->value, 3.14);
    EXPECT_EQ(it->name, "first");
}

TEST(ForwardListTest, ComplexTypeModification) {
    DynamicListMemoryResource mem_res;
    forward_list<TestComplexType> list(&mem_res);
    
    list.push_front(TestComplexType(1, 1.0, "original"));
    
    auto it = list.begin();
    it->value = 2.5;
    it->name = "modified";
    
    EXPECT_EQ(list.begin()->value, 2.5);
    EXPECT_EQ(list.begin()->name, "modified");
}

TEST(IteratorTest, ForwardIteratorRequirements) {
    DynamicListMemoryResource mem_res;
    forward_list<int> list(&mem_res);
    
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    
    using iterator_type = decltype(list)::iterator;
    using category = typename std::iterator_traits<iterator_type>::iterator_category;
    static_assert(std::is_same<category, std::forward_iterator_tag>::value, 
                  "Iterator should be forward iterator");
    
    auto it1 = list.begin();
    auto it2 = list.begin();
    
    ++it1;
    EXPECT_EQ(*it1, 2);
    
    auto it3 = it1++;
    EXPECT_EQ(*it3, 2);
    EXPECT_EQ(*it1, 1);
    
    EXPECT_EQ(it2, list.begin());
    EXPECT_NE(it1, it2);
}

TEST(IteratorTest, IteratorTraits) {
    using iterator_type = forward_list<int>::iterator;
    
    using value_type = typename std::iterator_traits<iterator_type>::value_type;
    using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
    using pointer = typename std::iterator_traits<iterator_type>::pointer;
    using reference = typename std::iterator_traits<iterator_type>::reference;
    
    static_assert(std::is_same<value_type, int>::value, "Value type should be int");
    static_assert(std::is_same<difference_type, std::ptrdiff_t>::value, 
                  "Difference type should be ptrdiff_t");
}

TEST(MemoryTest, MultipleListsSameResource) {
    DynamicListMemoryResource shared_mem_res;
    
    forward_list<int> list1(&shared_mem_res);
    forward_list<std::string> list2(&shared_mem_res);
    
    list1.push_front(100);
    list1.push_front(200);
    list2.push_front("hello");
    list2.push_front("world");
    
    EXPECT_FALSE(list1.empty());
    EXPECT_FALSE(list2.empty());
    
    int int_sum = 0;
    for (const auto& item : list1) {
        int_sum += item;
    }
    EXPECT_EQ(int_sum, 300);
    
    std::string concatenated;
    for (const auto& item : list2) {
        concatenated += item + " ";
    }
    EXPECT_EQ(concatenated, "world hello ");
}

TEST(MemoryTest, StressTest) {
    DynamicListMemoryResource mem_res;
    forward_list<int> list(&mem_res);
    
    const int num_elements = 1000;
    
    for (int i = 0; i < num_elements; ++i) {
        list.push_front(i);
    }
    
    int count = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        count++;
    }
    EXPECT_EQ(count, num_elements);
    
    list.clear();
    EXPECT_TRUE(list.empty());
    
    for (int i = 0; i < num_elements; ++i) {
        list.push_front(i * 2);
    }
    
    count = 0;
    int sum = 0;
    for (const auto& item : list) {
        count++;
        sum += item;
    }
    
    EXPECT_EQ(count, num_elements);
    EXPECT_GT(sum, 0);
}