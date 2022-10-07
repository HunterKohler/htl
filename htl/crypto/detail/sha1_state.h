#ifndef HTL_CRYPTO_DETAIL_SHA1_STATE_H_
#define HTL_CRYPTO_DETAIL_SHA1_STATE_H_

#include <bit>
#include <cstdint>
#include <htl/unaligned.h>

namespace htl::detail {

struct SHA1State {
    static constexpr std::size_t block_size = 64;
    static constexpr std::endian byte_order = std::endian::big;

    std::uint32_t a, b, c, d, e;

    void reset() noexcept
    {
        a = 0x67452301;
        b = 0xEFCDAB89;
        c = 0x98BADCFE;
        d = 0x10325476;
        e = 0xC3D2E1F0;
    }

    void transform(const std::uint8_t *data) noexcept
    {
        std::uint32_t aa = a, bb = b, cc = c, dd = d, ee = e, w[80];

        for (int i = 0; i < 15; i++) {
            w[i] = load_unaligned_be64(data + 4 * i);
        }

        for (int i = 16; i < 80; i++) {
            w[i] = std::rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        step<0>(w);
        step<1>(w);
        step<2>(w);
        step<3>(w);
        step<4>(w);
        step<5>(w);
        step<6>(w);
        step<7>(w);
        step<8>(w);
        step<9>(w);
        step<10>(w);
        step<11>(w);
        step<12>(w);
        step<13>(w);
        step<14>(w);
        step<15>(w);
        step<16>(w);
        step<17>(w);
        step<18>(w);
        step<19>(w);
        step<20>(w);
        step<21>(w);
        step<22>(w);
        step<23>(w);
        step<24>(w);
        step<25>(w);
        step<26>(w);
        step<27>(w);
        step<28>(w);
        step<29>(w);
        step<30>(w);
        step<31>(w);
        step<32>(w);
        step<33>(w);
        step<34>(w);
        step<35>(w);
        step<36>(w);
        step<37>(w);
        step<38>(w);
        step<39>(w);
        step<40>(w);
        step<41>(w);
        step<42>(w);
        step<43>(w);
        step<44>(w);
        step<45>(w);
        step<46>(w);
        step<47>(w);
        step<48>(w);
        step<49>(w);
        step<50>(w);
        step<51>(w);
        step<52>(w);
        step<53>(w);
        step<54>(w);
        step<55>(w);
        step<56>(w);
        step<57>(w);
        step<58>(w);
        step<59>(w);
        step<60>(w);
        step<61>(w);
        step<62>(w);
        step<63>(w);
        step<64>(w);
        step<65>(w);
        step<66>(w);
        step<67>(w);
        step<68>(w);
        step<69>(w);
        step<70>(w);
        step<71>(w);
        step<72>(w);
        step<73>(w);
        step<74>(w);
        step<75>(w);
        step<76>(w);
        step<77>(w);
        step<78>(w);
        step<79>(w);

        a += aa;
        b += bb;
        c += cc;
        d += dd;
        e += ee;
    }

    void digest(std::uint8_t *dest) noexcept
    {
        store_unaligned_be32(dest, a);
        store_unaligned_be32(dest + 4, b);
        store_unaligned_be32(dest + 8, c);
        store_unaligned_be32(dest + 12, d);
        store_unaligned_be32(dest + 16, e);
    }

    static std::uint32_t Ch(
        std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
    {
        return (x & y) ^ (~x & z);
    }

    static std::uint32_t Parity(
        std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
    {
        return x ^ y ^ z;
    }

    static std::uint32_t Maj(
        std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
    {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    template <std::size_t R>
    static constexpr std::uint32_t
    F(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
    {
        if constexpr (R < 20) {
            return Ch(x, y, z);
        } else if constexpr (R < 40) {
            return Parity(x, y, z);
        } else if constexpr (R < 60) {
            return Maj(x, y, z);
        } else {
            return Parity(x, y, z);
        }
    }

    template <std::size_t R>
    static constexpr std::uint32_t K() noexcept
    {
        if constexpr (R < 20) {
            return 0x5A827999;
        } else if constexpr (R < 40) {
            return 0x6ED9EBA1;
        } else if constexpr (R < 60) {
            return 0x8F1BBCDC;
        } else {
            return 0xCA62C1D6;
        }
    }

    template <std::size_t R>
    void step(std::uint32_t *w)
    {
        std::uint32_t tmp = std::rotl(a, 5) + F<R>(b, c, d) + e + K<R>() + w[R];
        e = d;
        d = c;
        c = std::rotl(b, 30);
        b = a;
        a = tmp;
    }
};

} // namespace htl::detail

#endif
