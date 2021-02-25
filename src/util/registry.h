#pragma once

#include <limits>
#include <compare>
#include <cassert>
#include "memory/allocator.h"

namespace entler {
    enum class RegistryLocality {
        spacial,
        temporal,
    };

    class RegistryKey {
    public:
        RegistryKey()
            : index_(invalid_index_)
            , nonce_(0)
        {
        }

        uint32_t index() const {
            assert(*this);
            return index_;
        }

        uint32_t nonce() const {
            assert(*this);
            return nonce_;
        }

        explicit operator bool() const {
            return index_ == std::numeric_limits<decltype(index_)>::max();
        }

        auto operator<=>(const RegistryKey&) const = default;

    private:
        template<typename T, RegistryLocality>
        friend class Registry;

        RegistryKey(uint32_t index, uint32_t nonce)
            : index_(index)
            , nonce_(nonce)
        {
        }

        static constexpr uint32_t invalid_index_ = std::numeric_limits<uint32_t>::max();
        uint32_t index_;
        uint32_t nonce_;
    };

    template<typename T, RegistryLocality locality>
    class Registry {
    public:
        Registry(size_t capacity, Allocator& allocator = GlobalAllocator::instance());
        ~Registry() {
        }

    private:
        using Row = std::aligned_storage<sizeof(T), alignof(T)>;

        Allocator& allocator_;
        Row*       rows_;
        uint32_t*  unused_rows_; // stack or a heap depending on locality
        size_t     unused_row_count_;
        uint32_t*  nonces_;
    };
}
