#pragma once

#include <cstddef>
#include <cassert>
#include <unordered_map>
#include <list>
#include <memory>
#include <algorithm>

namespace sc {

    /// Convenient QCache. See https://doc.qt.io/qt-5/qcache.html for further details.
    /**
     * Pros:
     * 1) No raw pointers
     * 2) You can use any type of smart pointers (shared-like presumably)
     * 3) You can specify custom object maker
     */
    template <class Key, class Value, class ValuePtr = std::shared_ptr<Value>,
              class Maker = decltype(&std::make_shared<Value>)>
    class Cache
    {
    public:
        /// Create cache with specific size and custom objects maker
        Cache(std::size_t maxCost = 100, Maker maker = std::make_shared<Value>);

        /// Add value to the cache
        ValuePtr putValue(Key const& key, Value *v, std::size_t cost = 1);
        /// Create value and put it to the cache
        ValuePtr makeValue(Key const& key, std::size_t cost = 1);

        /// Find and remove value. Returns nullptr if value is not found
        ValuePtr takeValue(Key const& key);
        /// Remove value by key
        void removeValue(Key const& key);

        /// Get value by key. Returns nullptr if value is not found
        ValuePtr operator[] (Key const& key) const;

        /// Max cache size
        std::size_t maxCost() const;
        /// Change max cache size
        void setMaxCost(std::size_t maxCost);

        /// Cost of added elements
        std::size_t totalCost() const;

        /// Clear cache
        void clear();

    private: // Types
        struct KeyCost
        {
            std::reference_wrapper<const Key> key;
            std::size_t cost;
        };

        using ValuesMap = std::unordered_map<Key, ValuePtr>;
        using CostsList = std::list<KeyCost>;

    private: // Functions
        template <class F>
        ValuePtr addValueImpl(Key const& key, F func, std::size_t cost)
        {
            if (cost > m_MaxCost)
                return nullptr;

            freeSpace(m_MaxCost - cost);
            m_Values.erase(key);

            auto result = m_Values.emplace(key, func());
            assert(result.second);

            m_Costs.emplace_back(KeyCost{result.first->first, cost});
            m_TotalCost += cost;

            return result.first->second;
        }

        void removeImpl(typename ValuesMap::iterator it)
        {
            assert(it != std::end(m_Values));
            auto costsIt = std::find_if(std::begin(m_Costs), std::end(m_Costs),
                                        [&](auto && v) { return v.key == it->first; });
            if (costsIt != std::end(m_Costs)) {
                m_TotalCost -= costsIt->cost;
                m_Costs.erase(costsIt);
            }

            m_Values.erase(it);
        }

        void freeSpace(std::size_t newMaxCost)
        {
            while (m_TotalCost > newMaxCost) {
                assert(!m_Costs.empty());

                KeyCost cost = m_Costs.front();
                m_Costs.pop_front();

                m_Values.erase(cost.key);
                m_TotalCost -= cost.cost;
            }
        }

    private:
        Maker m_Maker;

        std::size_t m_MaxCost;
        std::size_t m_TotalCost;
        ValuesMap m_Values;
        CostsList m_Costs;
    };

    template<class Key, class Value, class ValuePtr, class Maker>
    Cache<Key, Value, ValuePtr, Maker>::Cache(std::size_t maxCost, Maker maker)
        : m_Maker(maker)
        , m_MaxCost(maxCost)
        , m_TotalCost(0)
    {}

    template<class Key, class Value, class ValuePtr, class Maker>
    ValuePtr Cache<Key, Value, ValuePtr, Maker>::putValue(Key const& key, Value *v, std::size_t cost)
    {
        return addValueImpl(key, [&v]{ return ValuePtr(v); }, cost);
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    ValuePtr Cache<Key, Value, ValuePtr, Maker>::makeValue(Key const& key, std::size_t cost)
    {
        return addValueImpl(key, m_Maker, cost);
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    ValuePtr Cache<Key, Value, ValuePtr, Maker>::takeValue(Key const& key)
    {
        ValuePtr result;

        auto it = m_Values.find(key);
        if (it != std::end(m_Values)) {
            result = it->second;
            removeImpl(it);
        }

        return result;
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    void Cache<Key, Value, ValuePtr, Maker>::removeValue(Key const& key)
    {
        auto it = m_Values.find(key);
        if (it != std::end(m_Values))
            removeImpl(it);
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    ValuePtr Cache<Key, Value, ValuePtr, Maker>::operator[](Key const& key) const
    {
        auto it = m_Values.find(key);
        return it != std::cend(m_Values) ? it->second : ValuePtr();
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    std::size_t Cache<Key, Value, ValuePtr, Maker>::maxCost() const
    {
        return m_MaxCost;
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    void Cache<Key, Value, ValuePtr, Maker>::setMaxCost(std::size_t totalCost)
    {
        m_MaxCost = totalCost;
        freeSpace(m_MaxCost);
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    std::size_t Cache<Key, Value, ValuePtr, Maker>::totalCost() const
    {
        return m_TotalCost;
    }

    template<class Key, class Value, class ValuePtr, class Maker>
    void Cache<Key, Value, ValuePtr, Maker>::clear()
    {
        m_Values.clear();
        m_Costs.clear();
        m_TotalCost = 0;
    }

} // namespace sc
