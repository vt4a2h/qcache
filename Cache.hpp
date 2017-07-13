#pragma once

#include <cstddef>
#include <unordered_map>
#include <memory>

namespace sc {

    template <class Key, class Value, class ValuePtr = std::shared_ptr<Value>>
    class Cache
    {
    public:
        Cache(std::size_t totalCost = 100)
            : m_TotalCost(totalCost)
        {}

        ValuePtr putValue(Key const& key, Value * v, std::size_t cost = 1)
        {
            m_CurrentCost += cost;
            // if (m_CurrentCost >= m_Costs) cleanup...
            return (m_Values[key] = {cost, ValuePtr(v)});
        }

        ValuePtr takeValue(Key const& key)
        {
            ValuePtr result;
            auto it = m_Values.find(key);
            if (it != std::end(m_Values))
            {
                result = it->value;
                m_CurrentCost -= it->cost;

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

    private:
        std::size_t m_TotalCost;
        std::size_t m_CurrentCost;
        std::unordered_map<Key, ValueData> m_Values;
    };

} // sc
