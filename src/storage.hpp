#pragma once
#include "common.hpp"

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