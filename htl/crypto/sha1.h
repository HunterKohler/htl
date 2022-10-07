#ifndef HTL_CRYPTO_SHA1_H_
#define HTL_CRYPTO_SHA1_H_

#include <cstdint>
#include <ranges>
#include <algorithm>
#include <htl/crypto/detail/sha1_state.h>
#include <htl/crypto/detail/mdx_hash.h>

namespace htl {

class SHA1 {
public:
    static constexpr std::size_t key_size = 0;
    static constexpr std::size_t block_size = 64;
    static constexpr std::size_t digest_size = 20;

    static void hash(const std::uint8_t *data, std::size_t data_size,
                     std::uint8_t *dest) noexcept
    {
        SHA1 context;

        context.update(data, data_size);
        context.finalize(dest);
    }

    SHA1() noexcept = default;

    void reset() noexcept
    {
        _hash.reset();
    }

    void update(const std::uint8_t *data, std::size_t data_size) noexcept
    {
        _hash.update(data, data_size);
    }

    void finalize(std::uint8_t *dest) noexcept
    {
        _hash.finalize(dest);
    }

private:
    detail::MDXHash<detail::SHA1State> _hash;
};

} // namespace htl

#endif
