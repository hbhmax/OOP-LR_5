#include <gtest/gtest.h>
#include "../include/singly_linked_list.h"

class SinglyLinkedListTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool = new FixedBlockMemoryResource(2048);
    }

    void TearDown() override {
        delete pool;
    }

    FixedBlockMemoryResource* pool;
    
    // Вспомогательная функция для получения аллокатора
    std::pmr::polymorphic_allocator<int> get_allocator() {
        return std::pmr::polymorphic_allocator<int>(pool);
    }
};

struct TestStruct {
    int id;
    double value;
    std::string name;

    TestStruct(int i = 0, double v = 0.0, const std::string& n = "") 
        : id(i), value(v), name(n) {}

    bool operator==(const TestStruct& other) const {
        return id == other.id && value == other.value && name == other.name;
    }
};

TEST(FixedBlockMemoryResourceTest, BasicAllocationDeallocation) {
    FixedBlockMemoryResource pool(1024);
    
    void* ptr1 = pool.allocate(64, 1);
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = pool.allocate(128, 1);
    EXPECT_NE(ptr2, nullptr);
    
    pool.deallocate(ptr1, 64, 1);
    pool.deallocate(ptr2, 128, 1);
}

TEST(FixedBlockMemoryResourceTest, OutOfMemoryThrows) {
    FixedBlockMemoryResource pool(100);
    
    void* ptr1 = pool.allocate(50, 1);
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = pool.allocate(40, 1);
    EXPECT_NE(ptr2, nullptr);
    
    // Исправлено: добавлено static_cast<void> чтобы избежать предупреждения
    EXPECT_THROW(static_cast<void>(pool.allocate(60, 1)), std::bad_alloc);
    
    pool.deallocate(ptr1, 50, 1);
    pool.deallocate(ptr2, 40, 1);
}

TEST_F(SinglyLinkedListTest, DefaultConstructor) {
    SinglyLinkedList<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(SinglyLinkedListTest, PushBackAndSize) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
}

TEST_F(SinglyLinkedListTest, PushFrontAndFront) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.size(), 3);
}

TEST_F(SinglyLinkedListTest, BackOperation) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    EXPECT_EQ(list.back(), 3);
}

TEST_F(SinglyLinkedListTest, PopFront) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    list.pop_front();
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.size(), 2);
}

TEST_F(SinglyLinkedListTest, PopBack) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    list.pop_back();
    EXPECT_EQ(list.back(), 2);
    EXPECT_EQ(list.size(), 2);
}

TEST_F(SinglyLinkedListTest, Clear) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(SinglyLinkedListTest, CopyConstructor) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list1(get_allocator());
    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list2(list1);
    
    EXPECT_EQ(list1.size(), list2.size());
    
    auto it1 = list1.begin();
    auto it2 = list2.begin();
    while (it1 != list1.end() && it2 != list2.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST_F(SinglyLinkedListTest, MoveConstructor) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list1(get_allocator());
    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    
    size_t original_size = list1.size();
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list2(std::move(list1));
    
    EXPECT_EQ(list2.size(), original_size);
    EXPECT_TRUE(list1.empty());
}

TEST_F(SinglyLinkedListTest, IteratorBasic) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, list.end());
}

TEST_F(SinglyLinkedListTest, IteratorPostfixIncrement) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    auto old_it = it++;
    EXPECT_EQ(*old_it, 1);
    EXPECT_EQ(*it, 2);
}

TEST_F(SinglyLinkedListTest, IteratorRangeBasedFor) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    int sum = 0;
    for (const auto& item : list) {
        sum += item;
    }
    EXPECT_EQ(sum, 6);
}

TEST_F(SinglyLinkedListTest, IteratorArrowOperator) {
    // Исправлено: используем фигурные скобки вместо круглых
    SinglyLinkedList<TestStruct, std::pmr::polymorphic_allocator<TestStruct>> complexList{
        std::pmr::polymorphic_allocator<TestStruct>(pool)};
    
    complexList.push_back(TestStruct(1, 3.14, "test1"));
    complexList.push_back(TestStruct(2, 2.71, "test2"));
    
    auto it = complexList.begin();
    EXPECT_EQ(it->id, 1);
    EXPECT_EQ(it->name, "test1");
    ++it;
    EXPECT_EQ(it->id, 2);
    EXPECT_EQ(it->name, "test2");
}

TEST_F(SinglyLinkedListTest, ConstIterator) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    const auto& const_list = list;
    auto cit = const_list.begin();
    EXPECT_EQ(*cit, 1);
    ++cit;
    EXPECT_EQ(*cit, 2);
    
    int sum = 0;
    for (auto it = const_list.cbegin(); it != const_list.cend(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6);
}

TEST_F(SinglyLinkedListTest, IteratorComparison) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    list.push_back(1);
    list.push_back(2);
    
    auto it1 = list.begin();
    auto it2 = list.begin();
    auto it3 = ++list.begin();
    
    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 == it3);
    EXPECT_TRUE(it1 != it3);
}

TEST_F(SinglyLinkedListTest, ComplexTypeOperations) {
    // Исправлено: используем фигурные скобки вместо круглых
    SinglyLinkedList<TestStruct, std::pmr::polymorphic_allocator<TestStruct>> list{
        std::pmr::polymorphic_allocator<TestStruct>(pool)};
    
    list.push_back(TestStruct(1, 1.1, "first"));
    list.push_back(TestStruct(2, 2.2, "second"));
    list.push_back(TestStruct(3, 3.3, "third"));
    
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.front().id, 1);
    EXPECT_EQ(list.back().id, 3);
    
    int id_sum = 0;
    for (const auto& item : list) {
        id_sum += item.id;
    }
    EXPECT_EQ(id_sum, 6);
}

TEST_F(SinglyLinkedListTest, MoveAssignment) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list1(get_allocator());
    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list2(get_allocator());
    list2 = std::move(list1);
    
    EXPECT_EQ(list2.size(), 3);
    EXPECT_TRUE(list1.empty());
    
    std::vector<int> elements;
    for (const auto& item : list2) {
        elements.push_back(item);
    }
    EXPECT_EQ(elements, std::vector<int>({1, 2, 3}));
}

TEST_F(SinglyLinkedListTest, CopyAssignment) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list1(get_allocator());
    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list2(get_allocator());
    list2 = list1;
    
    EXPECT_EQ(list1.size(), list2.size());
    
    list1.pop_front();
    EXPECT_NE(list1.size(), list2.size());
}

TEST_F(SinglyLinkedListTest, EmptyListExceptions) {
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(get_allocator());
    
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);
    EXPECT_THROW(list.pop_front(), std::out_of_range);
    EXPECT_THROW(list.pop_back(), std::out_of_range);
}

TEST_F(SinglyLinkedListTest, MemoryResourceUsage) {
    FixedBlockMemoryResource small_pool(256);
    auto small_allocator = std::pmr::polymorphic_allocator<int>(&small_pool);
    
    SinglyLinkedList<int, std::pmr::polymorphic_allocator<int>> list(small_allocator);
    
    for (int i = 0; i < 10; ++i) {
        list.push_back(i);
    }
    
    EXPECT_EQ(list.size(), 10);
    
    list.clear();
    EXPECT_TRUE(list.empty());
    
    for (int i = 0; i < 5; ++i) {
        list.push_back(i * 2);
    }
    
    EXPECT_EQ(list.size(), 5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}