#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "../data-structure/BloomFilter.hpp"

// Custom hash for std::string that correctly hashes the string's content.
// The BloomFilter implementation passes `&data` and `sizeof(data)`.
// For `std::string`, `data` is a pointer to the string object, and `sizeof(data)` is not the string's length.
// This custom hash works around that by accessing the string's data and length through the pointer.
struct StringHash
{
    static std::pair<uint64_t, uint64_t> hash(const std::string *data, std::size_t len)
    {
        (void)len; // ignore the incorrect length
        std::pair<uint64_t, uint64_t> hash_value;
        MurmurHash3_x64_128(data->c_str(), data->length(), 0, &hash_value.first);
        return hash_value;
    }
};

void test_int()
{
    std::cout << "Testing BloomFilter with int..." << std::endl;
    Atok::BloomFilter<int> bf(1000, 10);
    bf.add(42);
    bf.add(123);

    assert(bf.pcontain(42));
    assert(bf.pcontain(123));
    assert(!bf.pcontain(999));
    std::cout << "Test with int PASSED." << std::endl;
}

void test_string()
{
    std::cout << "Testing BloomFilter with std::string..." << std::endl;
    Atok::BloomFilter<std::string, StringHash> bf(1000, 10);
    
    std::string s1 = "hello";
    std::string s2 = "world";
    std::string s3 = "test";
    
    bf.add(s1);
    bf.add(s2);

    assert(bf.pcontain(s1));
    assert(bf.pcontain("hello"));
    assert(bf.pcontain(s2));
    assert(bf.pcontain("world"));
    assert(!bf.pcontain(s3));
    assert(!bf.pcontain("test"));

    std::cout << "Test with std::string PASSED." << std::endl;
}

struct Point
{
    int x, y;
};

void test_struct()
{
    std::cout << "Testing BloomFilter with a struct..." << std::endl;
    Atok::BloomFilter<Point> bf(1000, 10);
    Point p1 = {1, 2};
    Point p2 = {3, 4};
    Point p3 = {5, 6};

    bf.add(p1);
    bf.add(p2);

    assert(bf.pcontain(p1));
    assert(bf.pcontain({1, 2}));
    assert(bf.pcontain(p2));
    assert(!bf.pcontain(p3));
    std::cout << "Test with struct PASSED." << std::endl;
}

int main()
{
    test_int();
    test_string();
    test_struct();

    std::cout << "\nAll BloomFilter tests passed!" << std::endl;

    return 0;
}
