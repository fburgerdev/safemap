#pragma once
#include "map.hpp"

namespace Safe {
    // WriteView
    template<typename K, typename V>
    class WriteView {
    public:
        // constructor
        WriteView() = default;
        WriteView(const K& key, SecureMap<K, V>& map)
            : m_key(key), m_map(&map) {}

        // key
        const K& key() const {
            return m_key;
        }
        // operator->
        auto operator->() {
            return m_map->writeLock(m_key); 
        }
        // compare
        bool operator==(const WriteView<K, V>& other) const {
            return m_key == other.m_key;
        }
        bool operator<(const WriteView<K, V>& other) const {
            return m_key < other.m_key;
        }
        bool operator>(const WriteView<K, V>& other) const {
            return m_key > other.m_key;
        }

        // friend
        template<typename K2, typename V2>
        friend class ReadView;
    private:
        // member
        K m_key;
        SecureMap<K, V>* m_map = nullptr;
    };
    // ReadView
    template<typename K, typename V>
    class ReadView {
    public:
        // constructor
        ReadView() = default;
        ReadView(const WriteView<K, V>& view)
            : m_key(view.m_key), m_map(view.m_map) {}
        ReadView(const K& key, const SecureMap<K, V>& map)
            : m_key(key), m_map(&map) {}

        // key
        const K& key() const {
            return m_key;
        }
        // operator->
        auto operator->() {
            return m_map->readLock(m_key); 
        }
        // compare
        bool operator==(const ReadView<K, V>& other) const {
            return m_key == other.m_key;
        }
        bool operator<(const ReadView<K, V>& other) const {
            return m_key < other.m_key;
        }
        bool operator>(const ReadView<K, V>& other) const {
            return m_key > other.m_key;
        }
    private:
        // member
        K m_key;
        const SecureMap<K, V>* m_map = nullptr;
    };
}