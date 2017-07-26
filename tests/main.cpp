#include <gtest/gtest.h>

#ifdef TEST_WITH_BOOST_SHARED_POINTER
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif

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

TEST(CacheTest, Resize)
{
    const std::size_t maxCost = 42;
    const std::size_t extraElements = 10;

    sc::Cache<std::size_t, double> cache(maxCost);
    ASSERT_EQ(cache.maxCost(), maxCost);

    for (std::size_t i = 0; i < maxCost + extraElements; ++i)
    {
        *cache.makeValue(i) = i * 2;
    }
    ASSERT_EQ(cache.elementsCount(), maxCost);
    ASSERT_EQ(cache.totalCost()    , maxCost);

    for (std::size_t i = 0; i < extraElements; ++i)
    {
       ASSERT_FALSE(!!cache[i]);
    }

    cache.setMaxCost(maxCost - 2);
    ASSERT_EQ(cache.elementsCount(), maxCost - 2);
    ASSERT_EQ(cache.totalCost()    , maxCost - 2);
    for (std::size_t i = 0; i < extraElements + 2; ++i)
    {
       ASSERT_FALSE(!!cache[i]);
    }
}

TEST(CacheTest, CustomMaker)
{
    int count = 0;
    auto counter = [&count]{ return ++count, std::make_shared<double>(); };

    using ExtendedCache = sc::Cache<std::size_t, double,
                                   std::shared_ptr<double>,
                                   decltype(counter)>;

    const std::size_t maxCost = 10;
    ExtendedCache cache(maxCost, counter);
    for (std::size_t i = 0; i < maxCost; ++i)
    {
        *cache.makeValue(i) = i * 2;
        ASSERT_TRUE(!!cache[i]);
    }
    ASSERT_EQ(cache.elementsCount(), maxCost);
    ASSERT_EQ(cache.elementsCount(), count  );
}

#ifdef TEST_WITH_BOOST_SHARED_POINTER
TEST(CacheTest, CustomSharedPointer)
{
    using BoostedCache = sc::Cache<std::size_t, double,
                                   boost::shared_ptr<double>,
                                   decltype(&boost::make_shared<double>)>;

    const std::size_t maxCost = 10;
    BoostedCache cache(maxCost, &boost::make_shared<double>);
    for (std::size_t i = 0; i < maxCost; ++i)
    {
        *cache.makeValue(i) = i * 2;
        ASSERT_TRUE(!!cache[i]);
    }
    ASSERT_EQ(cache.elementsCount(), maxCost);
}
#endif

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

