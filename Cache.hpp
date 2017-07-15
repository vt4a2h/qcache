#pragma once

#include <cstddef>
#include <cassert>
#include <unordered_map>
#include <memory>
#include <functional>

namespace sc {

    namespace details {

        template <class Key, class Value, class ValuePtr, auto maker>
        class Cache
        {
        public:
            Cache(std::size_t totalCost = 100)
                : m_TotalCost(totalCost)
            {}

            ValuePtr putValue(Key const& key, Value * v, std::size_t cost = 1)
            {
                return addValueImpl(key, [&v]{ return ValuePtr(v); }, cost);
            }

            ValuePtr makeValue(Key const& key, std::size_t cost= 1)
            {
                return addValueImpl(key, maker, cost);
            }

            ValuePtr takeValue(Key const& key)
            {
                ValuePtr result;
                auto it = m_Values.find(key);
                if (it != std::end(m_Values))
                {
                    result = it->second.value;
                    m_CurrentCost -= it->second.cost;

                    m_Values.erase(it);
                }

                return result;
            }

            std::size_t totalCost() const
            {
                return m_TotalCost;
            }

            void setTotalCost(std::size_t totalCost)
            {
                m_TotalCost = totalCost;
            }

        private: // Types
            struct ValueData
            {
                std::size_t cost;
                ValuePtr value;
            };

        private: // Functions
            template <class F>
            ValuePtr addValueImpl(Key const& key, F func, std::size_t cost)
            {
                m_CurrentCost += cost;

                // TODO:
                // if (m_CurrentCost >= m_Costs) cleanup...

                if (m_Values.find(key) != std::end(m_Values))
                    m_Values.erase(key);

                auto result = m_Values.emplace(key, ValueData{cost, func()});
                assert(result.second);

                return result.first->second.value;
            }

        private:
            std::size_t m_TotalCost;
            std::size_t m_CurrentCost;
            std::unordered_map<Key, ValueData> m_Values;
        };

    } // namespace details

    template<class Key, class Value>
    using Cache = details::Cache<Key, Value, std::shared_ptr<Value>, &std::make_shared<Value>>;

} // namespace sc
