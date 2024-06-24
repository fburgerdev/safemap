#pragma once
// #include <...> (HPPMERGE)
#include <ranges>
#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <unordered_set>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>
#include <optional>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <atomic>


// #include "common.hpp" (HPPMERGE)
namespace Safe {
    // ranges
    namespace stdr = std::ranges;

    // types
    // types :: address
    using address = std::size_t;
    // types :: int
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;
    // types :: uint
    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;
    // types :: string
    using string = std::string;
    using string_view = std::string_view;

    // container
    // container :: sequential
    template<typename T, address N>
    using Array = std::array<T, N>;
    template<typename T>
    using List = std::vector<T>;
    template<typename T>
    using Stack = std::stack<T>;
    template<typename T>
    using Queue = std::queue<T>;
    template<typename T>
    using Deque = std::deque<T>;
    // container :: tree
    template<typename T>
    using Set = std::set<T>;
    template<typename Key, typename Value>
    using Map = std::map<Key, Value>;
    // container :: hash
    template<typename T>
    using HashSet = std::unordered_set<T>;
    template<typename Key, typename Value>
    using HashMap = std::unordered_map<Key, Value>;

    // move semantics
    using std::forward;
    using std::move;

    // smart pointers
    using std::make_unique;
    using std::make_shared;
    using std::unique_ptr;
    using std::shared_ptr;
    using std::weak_ptr;

    // function
    using std::function;

    // templates
    template<typename T>
    using Opt = std::optional<T>;

    // debug
    using std::cout;
    using std::endl;
}

// #include "common_thread.hpp" (HPPMERGE)
namespace Safe {
    // lock
    using std::unique_lock;
    using std::shared_lock;
    // mutex
    using std::mutex;
    using std::shared_mutex;
    using std::recursive_mutex;
    // atomic
    // :: address
    using atomic_address = std::atomic_size_t;
    // :: int
    using atomic_int8 = std::atomic_int8_t;
    using atomic_int16 = std::atomic_int16_t;
    using atomic_int32 = std::atomic_int32_t;
    using atomic_int64 = std::atomic_int64_t;
    // :: uint
    using atomic_uint8 = std::atomic_uint8_t;
    using atomic_uint16 = std::atomic_uint16_t;
    using atomic_uint32 = std::atomic_uint32_t;
    using atomic_uint64 = std::atomic_uint64_t;
}

// #include "lock.hpp" (HPPMERGE)
namespace Safe {
    // Locked
    template<typename T, typename TLock>
    class Locked {
    public:
        // constructor
        Locked()
            : m_ptr(nullptr) {}
        Locked(T* ptr, shared_mutex& mutex)
            : m_ptr(ptr), m_lock(mutex) {}
        // destructor
        ~Locked() = default;
        // copy
        Locked(const Locked<T, TLock>&) = delete;
        // copy assign
        Locked<T, TLock>& operator=(const Locked<T, TLock>&) = delete;
        // move
        template<typename U>
        Locked(Locked<U, TLock>&& other)
            : m_ptr((T*)other.m_ptr), m_lock(move(other.m_lock)) {}
        // move assign
        template<typename U>
        Locked<T, TLock>& operator=(Locked<U, TLock>&& other) {
            m_ptr = (T*)other.m_ptr;
            m_lock = move(other.m_lock);
            return *this;
        }

        // operator->
        T* operator->() {
            return m_ptr;
        }
        // release
        void release() {
            if (m_lock) {
                m_lock.unlock();
                m_lock.release();
            }
        }

        // isValid
        operator bool() const {
            return m_ptr != nullptr;
        }

        // friend
        template<typename U, typename ULock>
        friend class Locked;
    private:
        // pointer
        T* m_ptr;
        TLock m_lock;
    };
    // ReadLocked / WriteLocked
    template<typename T>
    using ReadLocked = Locked<const T, shared_lock<shared_mutex>>;
    template<typename T>
    using WriteLocked = Locked<T, unique_lock<shared_mutex>>;
}

// #include "value.hpp" (HPPMERGE)
namespace Safe {
    // SecureValue
    template<typename T>
    class SecureValue {
    public:
        // constructor
        template<typename... Args>
        SecureValue(Args&&... args)
            : m_value(forward<Args>(args)...) {}
        // read / write lock
        ReadLocked<T> readLock() const {
            return { &m_value, m_mutex };
        }
        WriteLocked<T> writeLock() {
            return { &m_value, m_mutex };
        }
    private:
        // value
        T m_value;
        mutable shared_mutex m_mutex;
    };
}

// #include "storage.hpp" (HPPMERGE)
namespace Safe {
    // Storage
    template<typename T>
    class Storage {
    public:
        // construct
        template<typename... Args>
        void construct(Args&&... args) {
            if (m_isValid) {
                destroy();
            }
            std::construct_at(std::bit_cast<T*>(&m_storage), forward<Args>(args)...);
            m_isValid = true;
        }
        // destroy
        void destroy() {
            if (m_isValid) {
                m_isValid = false;
                std::destroy_at(std::bit_cast<T*>(&m_storage));
            }
        }
        // isValid
        bool isValid() const {
            return m_isValid;
        }
        
        // get
        T& get() {
            return *std::bit_cast<T*>(&m_storage);
        }
        const T& get() const {
            return *std::bit_cast<const T*>(&m_storage);
        }
    private:
        // storage
        std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;
        bool m_isValid = false;
    };
}

// #include "map.hpp" (HPPMERGE)
namespace Safe {
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

// #include "view.hpp" (HPPMERGE)
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

// #include "collection.hpp" (HPPMERGE)
namespace Safe {
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