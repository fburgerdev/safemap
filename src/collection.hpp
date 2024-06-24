#pragma once
#include "map.hpp"

namespace Memory {
    // Collection
    template<typename K>
    class Collection {
    public:
        // emplace
        template<typename V, typename... Args>
        void emplace(const K& key, Args&&... args) {
            get<V>().emplace(key, forward<Args>(args)...);
        }
        // erase
        template<typename V>
        void erase(const K& key) {
            get<V>().erase(key);
        }
        // clear
        template<typename V>
        void clear() {
            get<V>().clear();
        }

        // destroy
        template<typename V>
        void destroy(const K& key) {
            get<V>().destroy(key);
        }
        // clean
        template<typename V>
        void clean() {
            get<V>().clean();
        } 

        // read / write lock
        template<typename V>
        ReadLocked<V> readLock(const K& key) const {
            return get<V>().readLock(key);
        }
        template<typename V>
        WriteLocked<V> writeLock(const K& key) {
            return get<V>().writeLock(key);
        }
        
        // addType
        template<typename V>
        void addType() {
            unique_lock lock(m_mapMutex);
            m_map.try_emplace(typeid(V).hash_code(), static_cast<ISecureMap*>(new SecureMap<K, V>()));
        }

        // get
        template<typename V>
        SecureMap<K, V>& get() {
            shared_lock lock(m_mapMutex);
            return *reinterpret_cast<SecureMap<K, V>*>(m_map.at(typeid(V).hash_code()).get());
        }
        template<typename V>
        const SecureMap<K, V>& get() const {
            shared_lock lock(m_mapMutex);
            return *reinterpret_cast<SecureMap<K, V>*>(m_map.at(typeid(V).hash_code()).get());
        }

        // // iterate
        // // :: begin
        // auto begin() {
        //     shared_lock lock(m_mapMutex);
        //     return m_map.begin();
        // }
        // auto begin() const {
        //     shared_lock lock(m_mapMutex);
        //     return m_map.begin();
        // }
        // // :: end
        // auto end() {
        //     shared_lock lock(m_mapMutex);
        //     return m_map.end();
        // }
        // auto end() const {
        //     shared_lock lock(m_mapMutex);
        //     return m_map.end();
        // }
    private:
        // map
        Map<address, unique_ptr<ISecureMap>> m_map;
        mutable shared_mutex m_mapMutex;
    };
}