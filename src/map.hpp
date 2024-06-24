#pragma once
#include "value.hpp"
#include "storage.hpp"

namespace Memory {
    // Interface for SecureMap
    class ISecureMap {
    public:
        // constructor / destructor
        ISecureMap() = default;
        virtual ~ISecureMap() = default;
    };

    // SecureMap
    template<typename K, typename V>
    class SecureMap : public ISecureMap {
    public:
        // constructor / destructor
        SecureMap() = default;
        ~SecureMap() = default;

        // emplace
        template<typename... Args>
        void emplace(const K& key, Args&&... args) {
            unique_lock lock(m_mapMutex);
            // ASSERT(!m_map.contains(key));
            m_map[key].writeLock()->construct(forward<Args>(args)...);
        }
        // erase
        void erase(const K& key) {
            destroy(key);
            unique_lock lock(m_mapMutex);
            // ASSERT(m_map.contains(key));
            m_map.erase(key);
        }
        // clear
        void clear() {
            {
                shared_lock lock(m_mapMutex);
                for (auto it = m_map.begin(); it != m_map.end(); ++it) {
                    it->second.writeLock()->destroy();
                }
            }
            unique_lock lock(m_mapMutex);
            m_map.clear();
        }

        // destroy
        void destroy(const K& key) {
            shared_lock lock(m_mapMutex);
            // ASSERT(m_map.contains(key));
            m_map.at(key).writeLock()->destroy();
        }
        // clean
        void clean() {
            unique_lock lock(m_mapMutex);
            auto it = m_map.begin();
            while (it != m_map.end()) {
                if (!it->second.readLock()->isValid()) {
                    it = m_map.erase(it);
                }
                else {
                    ++it;
                }
            }
        } 

        // read / write lock
        ReadLocked<V> readLock(const K& key) const {
            shared_lock lock(m_mapMutex);
            if (m_map.contains(key)) {
                return m_map.at(key).readLock();
            }
            return {};
        }
        WriteLocked<V> writeLock(const K& key) {
            shared_lock lock(m_mapMutex);
            if (m_map.contains(key)) {
                return m_map.at(key).writeLock();
            }
            return {};
        }
        
        // iterate
        // :: foreach
        void forEach(function<void(const K&, WriteLocked<V>&)> func) {
            typename Map<K, SecureValue<Storage<V>>>::iterator it;
            WriteLocked<Storage<V>> locked;
            {
                shared_lock lock(m_mapMutex);
                if (m_map.empty()) {
                    return;
                }
                it = m_map.begin();
                locked = it->second.writeLock();
            }
            while (true) {
                WriteLocked<V> locked_value;
                if (locked->isValid()) {
                    locked_value = move(locked);
                    func(it->first, locked_value);
                }
                shared_lock lock(m_mapMutex);
                if (++it == m_map.end()) {
                    return;
                }
                else {
                    locked = it->second.writeLock();
                }
            }
        }
        void forEach(function<void(const K&, ReadLocked<V>&)> func) const {
            typename Map<K, SecureValue<Storage<V>>>::const_iterator it;
            ReadLocked<Storage<V>> locked;
            {
                shared_lock lock(m_mapMutex);
                if (m_map.empty()) {
                    return;
                }
                it = m_map.begin();
                locked = it->second.readLock();
            }
            while (true) {
                ReadLocked<V> locked_value;
                if (locked->isValid()) {
                    locked_value = move(locked);
                    func(it->first, locked_value);
                }
                shared_lock lock(m_mapMutex);
                if (++it == m_map.end()) {
                    return;
                }
                else {
                    locked = it->second.readLock();
                }
            }
        }
    private:
        // map
        Map<K, SecureValue<Storage<V>>> m_map;
        mutable shared_mutex m_mapMutex;
    };
}