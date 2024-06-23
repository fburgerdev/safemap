#pragma once
#include "common_thread.hpp"

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