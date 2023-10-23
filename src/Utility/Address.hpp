#pragma once

#include <string>
#include <cstdint>

namespace Utility {
    // sizeof(Address) should always be sizeof(void*).
    class Address {
    public:
        Address();
        Address(void* ptr);
        Address(uintptr_t addr);

        void Set(Address ptr) {
            m_Ptr = ptr.Ptr();
        }

        template <typename T>
        Address Get(T offset) const {
            return Address((uintptr_t)m_Ptr + offset);
        }

        template <typename T>
        Address Add(T offset) const {
            return Address((uintptr_t)m_Ptr + offset);
        }

        template <typename T>
        Address Sub(T offset) const {
            return Address((uintptr_t)m_Ptr - offset);
        }

        template <typename T>
        T& As() const {
            return *(T*)&m_Ptr;
        }

        // To is like As but dereferences that shit.
        template <typename T>
        T& To() const {
            return *(T*)m_Ptr;
        }

        Address Deref() const {
            return To<void*>();
        }

        void* Ptr() const {
            return m_Ptr;
        }

        operator uintptr_t& () const {
            return *(uintptr_t*)&m_Ptr;
        }

        operator void* () const {
            return m_Ptr;
        }

		bool operator ==(Address rhs) {
			return this->m_Ptr == rhs.m_Ptr;
		}

        bool operator ==(bool val) {
            return ((m_Ptr && val) || (!m_Ptr && !val));
        }

        bool operator !=(bool val) {
            return !(*this == val);
        }

        bool operator ==(uintptr_t val) {
            return ((uintptr_t)m_Ptr == val);
        }

        bool operator !=(uintptr_t val) {
            return !(*this == val);
        }

        bool operator ==(void* val) {
            return (m_Ptr == val);
        }

        bool operator !=(void* val) {
            return !(*this == val);
        }

    private:
        void* m_Ptr;

        static_assert(sizeof(m_Ptr) == sizeof(uintptr_t), "[Address] Size mismatch!");
    };

    typedef Address Offset;

    static_assert(sizeof(Address) == sizeof(uintptr_t), "[Address] Size mismatch!");
}