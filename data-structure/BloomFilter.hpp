#pragma once
#include <bits/stdc++.h>
#include "../dependency/MurmurHash3.cpp"

namespace Atok
{
    // to generate multiple hash value, this should return 2 hash values
    template <class Key, class Hash>
    concept Hashable = requires(const Key *data, std::size_t len) {
        Hash::hash(data, len);
    };

    template <class Key>
    struct DefaultHash
    {
        static std::pair<uint64_t, uint64_t> hash(const Key *data, std::size_t len)
        {
            std::pair<uint64_t, uint64_t> hash_value;
            MurmurHash3_x64_128(data, len, 0, &hash_value.first);

            return hash_value;
        }
    };

    template <class Key, class Hash = DefaultHash<Key>>
        requires Hashable<Key, Hash>
    struct BloomFilter
    {
        BloomFilter(uint64_t size, uint8_t num_hashes);

        void add(const Key &data);
        bool pcontain(const Key &data) const;

    private:
        uint8_t m_num_hashes;
        std::vector<bool> m_bits;
        void add(const Key *data, std::size_t len);
        bool pcontain(const Key *data, std::size_t len) const;
    };

    template <class Key, class Hash>
        requires Hashable<Key, Hash>
    inline BloomFilter<Key, Hash>::BloomFilter(uint64_t size, uint8_t num_hashes) : m_bits(size), m_num_hashes(num_hashes)
    {
    }

    inline uint64_t nthHash(uint8_t n, uint64_t hashA, uint64_t hashB, uint64_t filterSize)
    {
        return (hashA + n * hashB) % filterSize;
    }

    template <class Key, class Hash>
        requires Hashable<Key, Hash>
    inline void BloomFilter<Key, Hash>::BloomFilter::add(const Key *data, std::size_t len)
    {
        auto [hash1, hash2] = Hash::hash(data, len);

        for (int n = 0; n < m_num_hashes; n++)
        {
            m_bits[nthHash(n, hash1, hash2, m_bits.size())] = true;
        }
    }

    template <class Key, class Hash>
        requires Hashable<Key, Hash>
    inline void BloomFilter<Key, Hash>::BloomFilter::add(const Key &data)
    {
        BloomFilter<Key, Hash>::BloomFilter::add(&data, sizeof(data));
    }

    template <class Key, class Hash>
        requires Hashable<Key, Hash>
    bool BloomFilter<Key, Hash>::BloomFilter::pcontain(const Key *data, std::size_t len) const
    {
        auto [hash1, hash2] = Hash::hash(data, len);

        for (int n = 0; n < m_num_hashes; n++)
        {
            if (!m_bits[nthHash(n, hash1, hash2, m_bits.size())])
            {
                return false;
            }
        }

        return true;
    }

    template <class Key, class Hash>
        requires Hashable<Key, Hash>
    bool BloomFilter<Key, Hash>::BloomFilter::pcontain(const Key &data) const
    {
        return BloomFilter<Key, Hash>::BloomFilter::pcontain(&data, sizeof(data));
    }
}
