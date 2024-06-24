#pragma once
#include "lock.hpp"

namespace Memory {
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