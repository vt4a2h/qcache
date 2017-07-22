#pragma once

#include <cstddef>
#include <cassert>
#include <unordered_map>
#include <list>
#include <memory>
#include <algorithm>
#include <functional>

namespace sc {

    namespace details {

        template <class Key, class Value, class ValuePtr, auto maker>
        class Cache
        {
        public:
            Cache(std::size_t maxCost = 100);

            ValuePtr putValue(Key const& key, Value * v, std::size_t cost = 1);
            ValuePtr makeValue(Key const& key, std::size_t cost = 1);

            ValuePtr takeValue(Key const& key);
            void removeValue(Key const& key);

            ValuePtr operator[] (Key const& key) const;

            std::size_t totalCost() const;
            void setTotalCost(std::size_t totalCost);

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
            std::size_t m_MaxCost;
            std::size_t m_TotalCost;
            ValuesMap m_Values;
            CostsList m_Costs;
        };

        template<class Key, class Value, class ValuePtr, auto maker>
        Cache<Key, Value, ValuePtr, maker>::Cache(std::size_t maxCost)
            : m_MaxCost(maxCost)
            , m_TotalCost(0)
        {}

        template<class Key, class Value, class ValuePtr, auto maker>
        ValuePtr Cache<Key, Value, ValuePtr, maker>::putValue(const Key &key, Value *v, std::size_t cost)
        {
            return addValueImpl(key, [&v]{ return ValuePtr(v); }, cost);
        }

        template<class Key, class Value, class ValuePtr, auto maker>
        ValuePtr Cache<Key, Value, ValuePtr, maker>::makeValue(const Key &key, std::size_t cost)
        {
            return addValueImpl(key, maker, cost);
        }

        template<class Key, class Value, class ValuePtr, auto maker>
        ValuePtr Cache<Key, Value, ValuePtr, maker>::takeValue(const Key &key)
        {
            ValuePtr result;

            auto it = m_Values.find(key);
            if (it != std::end(m_Values)) {
                result = it->second;
                removeImpl(it);
            }

            return result;
        }

        template<class Key, class Value, class ValuePtr, auto maker>
        void Cache<Key, Value, ValuePtr, maker>::removeValue(const Key &key)
        {
            auto it = m_Values.find(key);
            if (it != std::end(m_Values))
                removeImpl(it);
        }

        template<class Key, class Value, class ValuePtr, auto maker>
        ValuePtr Cache<Key, Value, ValuePtr, maker>::operator[](const Key &key) const
        {
            auto it = m_Values.find(key);
            return it != std::cend(m_Values) ? it->second : ValuePtr();
        }

        template<class Key, class Value, class ValuePtr, auto maker>
        std::size_t Cache<Key, Value, ValuePtr, maker>::totalCost() const
        {
            return m_MaxCost;
        }

        template<class Key, class Value, class ValuePtr, auto maker>
        void Cache<Key, Value, ValuePtr, maker>::setTotalCost(std::size_t totalCost)
        {
            m_MaxCost = totalCost;
            freeSpace(m_MaxCost);
        }

        template<class Key, class Value, class ValuePtr, auto maker>
        void Cache<Key, Value, ValuePtr, maker>::clear()
        {
            m_Values.clear();
            m_Costs.clear();
            m_TotalCost = 0;
        }
    } // namespace details

    template<class Key, class Value>
    using Cache = details::Cache<Key, Value, std::shared_ptr<Value>, &std::make_shared<Value>>;

} // namespace sc
