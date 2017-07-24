#include <gtest/gtest.h>

#include "Cache.hpp"

TEST(CacheTest, Create)
{
    sc::Cache<std::string, double> cache;
    ASSERT_EQ(cache.totalCost(), 0);
    ASSERT_EQ(cache.maxCost(), 100);
}

TEST(CacheTest, AddElement)
{
    sc::Cache<std::string, double> cache;

    auto v1 = cache.putValue("foo", new double(10));
    ASSERT_TRUE(!!v1);
    ASSERT_EQ(*v1, 10);
    ASSERT_EQ(cache.totalCost(), 1);
    ASSERT_EQ(cache["foo"], v1);

    auto v2 = cache.makeValue("foo", 2);
    *v2 = 11;
    ASSERT_TRUE(!!v2);
    ASSERT_NE(v1, v2);
    ASSERT_EQ(*v2, 11);
    ASSERT_EQ(cache.totalCost(), 2);
    ASSERT_EQ(cache["foo"], v2);
}

TEST(CacheTest, RemoveElement)
{
    sc::Cache<std::string, double> cache;
    cache.putValue("foo", new double(10));
    cache.putValue("bar", new double(12), 42);
    ASSERT_EQ(cache.totalCost(), 43);

    auto v1 = cache.takeValue("foo");
    ASSERT_FALSE(!!cache["foo"]);
    ASSERT_EQ(cache.totalCost(), 42);

    cache.removeValue("bar");
    ASSERT_FALSE(!!cache["bar"]);
    ASSERT_EQ(cache.totalCost(), 0);
}

TEST(CacheTest, GetElement)
{
    sc::Cache<std::string, double> cache;
    auto v1 = cache.putValue("foo", new double(10));
    ASSERT_EQ(v1, cache["foo"]);
    *v1 = 23;
    ASSERT_EQ(*v1, *cache["foo"]);

    ASSERT_FALSE(!!cache["bar"]);
}

// TODO: add tests for max cost
// TODO: add tests for genaral use case

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

