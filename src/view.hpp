#pragma once
#include "map.hpp"

namespace Memory {
    // GenericView
    template<typename K>
    class GenericView {
    public:
        // constructor
        GenericView() = default;
        template<typename V>
        GenericView(const K& key, SecureMap<K, V>& map)
            : m_key(key), m_map(static_cast<void*>(&map)) {}
        // key
        const K& key() const {
            return m_key;
        }
        // compare
        bool operator==(const GenericView<K>& other) const {
            return m_key == other.m_key;
        }
        bool operator<(const GenericView<K>& other) const {
            return m_key < other.m_key;
        }
        bool operator>(const GenericView<K>& other) const {
            return m_key > other.m_key;
        }
    
        // friend
        template<typename K2, typename V2>
        friend class ReadView;
        template<typename K2, typename V2>
        friend class WriteView;
    private:
        // member
        K m_key;
        void* m_map = nullptr;
    };

    // WriteView
    template<typename K, typename V>
    class WriteView {
    public:
        // constructor
        WriteView() = default;
        WriteView(const GenericView<K>& view)
            : m_key(view.m_key), m_map(reinterpret_cast<SecureMap<K, V>*>(view.m_map)) {}
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
        ReadView(const GenericView<K>& view)
            : m_key(view.m_key), m_map(reinterpret_cast<const SecureMap<K, V>*>(view.m_map)) {}
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