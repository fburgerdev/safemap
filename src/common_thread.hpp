#pragma once
#include <mutex> // mutex, recursive_mutex, unique_lock
#include <shared_mutex> // shared_mutex, shared_lock
#include <atomic> // atomic dtypes...

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