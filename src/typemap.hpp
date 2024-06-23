#pragma once
#include "common.hpp"
#include "common_thread.hpp"

namespace Safe {
    // TypeMap
    template<typename V>
    class TypeMap {
    public:
        // emplace
        template<typename T, typename... Args>
        void emplace(Args&&... args) {
            unique_lock lock(m_mapMutex);
            m_map.try_emplace(typeid(T).hash_code(), forward<Args>(args)...);
        }
        // erase
        template<typename T>
        void erase() {
            unique_lock lock(m_mapMutex);
            m_map.erase(typeid(T).hash_code());
        }
        // clear
        void clear() {
            unique_lock lock(m_mapMutex);
            m_map.clear();
        }

        // access
        // :: at
        template<typename T>
        T& at() {
            shared_lock lock(m_mapMutex);
            return m_map.at(typeid(T).hash_code());
        }
        // :: contains
        template<typename T>
        bool contains() {
            shared_lock lock(m_mapMutex);
            return m_map.contains(typeid(T).hash_code());
        }
        
        // iterate
        // :: begin
        auto begin() {
            shared_lock lock(m_mapMutex);
            return m_map.begin();
        }
        auto begin() const {
            shared_lock lock(m_mapMutex);
            return m_map.begin();
        }
        // :: end
        auto end() {
            shared_lock lock(m_mapMutex);
            return m_map.end();
        }
        auto end() const {
            shared_lock lock(m_mapMutex);
            return m_map.end();
        }
    private:
        // map
        Map<address, V> m_map;
        mutable shared_mutex m_mapMutex;
    };
}