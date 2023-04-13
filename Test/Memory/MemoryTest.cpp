#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <vector>

#include "Memory/BaseAllocator.h"
#include "Memory/StompMalloc.h"

template<typename T>
using StompAllocator = BaseAllocator<T, StompMalloc>;

class StompMallocTest
{
public:
    static void TestFront()
    {
        const int numElements = 100;
        int* arr = (int*)StompMalloc::AllocProtectFront(sizeof(int) * numElements);
        //int a = arr[-1];
        StompMalloc::FreeProtectFront(arr);
    }

    static void TestBack()
    {
        const int numElements = 100;
        int* arr = (int*)StompMalloc::AllocProtectBack(sizeof(int) * numElements);
        int a = arr[100];
        StompMalloc::FreeProtectBack(arr);
    }
};

TEST_CASE("Memory")
{
    std::vector<int, StompAllocator<int>> vec;
    vec.reserve(4);
    for (int i = 0; i < 100; ++i)
    {
        vec.push_back(i + 1);
    }

    int a = vec.data()[0];

    StompMallocTest::TestFront();
    StompMallocTest::TestBack();
}
