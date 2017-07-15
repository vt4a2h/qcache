#pragma once

#include <cstddef>
#include <cassert>
#include <unordered_map>
#include <list>
#include <memory>
#include <algorithm>

namespace sc {

    namespace details {

        template <class Key, class Value, class ValuePtr, auto maker>
        class Cache
        {
        public:
            Cache(std::size_t totalCost = 100)
                : m_MaxCost(totalCost)
            {}

            ValuePtr putValue(Key const& key, Value * v, std::size_t cost = 1)
            {
                return addValueImpl(key, [&v]{ return ValuePtr(v); }, cost);
            }

            ValuePtr makeValue(Key const& key, std::size_t cost = 1)
            {
                return addValueImpl(key, maker, cost);
            }

            ValuePtr takeValue(Key const& key)
            {
                ValuePtr result;
                auto it = m_Values.find(key);
                if (it != std::end(m_Values))
                {
                    result = it->second;
                    m_Values.erase(it);

                    m_Costs.erase(std::find_if(std::begin(m_Costs), std::end(m_Costs),
                                               [&](auto && v) { return v.key == key; }));
                }

                return result;
            }

            std::size_t totalCost() const
            {
                return m_MaxCost;
            }

            void setTotalCost(std::size_t totalCost)
            {
                m_MaxCost = totalCost;
            }

        private: // Types
            struct KeyCost
            {
                Key key;
                std::size_t cost;
            };

        private: // Functions
            template <class F>
            ValuePtr addValueImpl(Key const& key, F func, std::size_t cost)
            {
                if (cost > m_MaxCost)
                    return nullptr;

                freeSpace(cost);

                m_Values.erase(key);
                auto result = m_Values.emplace(key, func());
                assert(result.second);

                m_Costs.emplace_back(KeyCost{key, cost});
                m_CurrentCost += cost;

                return result.first->second;
            }

            void freeSpace(std::size_t cost)
            {
                std::size_t newCost = cost + m_CurrentCost;
                while (newCost > m_MaxCost) {
                    assert(!m_Costs.empty());

                    KeyCost cost = m_Costs.front();
                    m_Costs.pop_front();

                    m_Values.erase(cost.key);
                    newCost -= cost.cost;
                }
            }

        private:
            std::size_t m_MaxCost;
            std::size_t m_CurrentCost;
            std::unordered_map<Key, ValuePtr> m_Values;
            std::list<KeyCost> m_Costs;
        };

    } // namespace details

    template<class Key, class Value>
    using Cache = details::Cache<Key, Value, std::shared_ptr<Value>, &std::make_shared<Value>>;

} // namespace sc
