#ifndef HTL_DETAIL_SIMPLE_HASH_H_
#define HTL_DETAIL_SIMPLE_HASH_H_

#include <type_traits>
#include <htl/unaligned.h>
#include <htl/crypto/siphash.h>

namespace htl::detail {

template <class... Args>
    requires((std::is_trivially_copyable_v<Args> &&
              std::has_unique_object_representations_v<Args>)&&...)
inline std::size_t simple_hash(const Args &...args) noexcept
{
    static constexpr std::uint8_t siphash_key[SipHash::key_size]{
        0xF1, 0x64, 0x5D, 0x48, 0x73, 0xC2, 0x45, 0x6B,
        0xE2, 0x97, 0x70, 0x6E, 0xC6, 0xE4, 0xA9, 0xF5,
    };

    std::uint8_t dest[SipHash::digest_size];

    SipHash context{ siphash_key };

    (context.update(
         reinterpret_cast<const std::uint8_t *>(std::addressof(args)),
         sizeof(args)),
     ...);

    context.finalize(dest);

    return load_unaligned_le64(dest);
}

} // namespace htl::detail

#endif
