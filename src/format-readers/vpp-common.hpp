#ifndef ACE3X_FORMAT_READERS_VPP_COMMON_HPP_
#define ACE3X_FORMAT_READERS_VPP_COMMON_HPP_

namespace vpp_common {

inline constexpr std::uint16_t kChunkSize {0x800};

inline std::uint32_t align_to_chunk(std::uint32_t addr)
{
    if (addr == 0) {
        return addr;
    }

    int left = addr % kChunkSize;

    if (left > 0) {
        return (addr + (kChunkSize - left));
    }

    return addr;
}

}    // namespace vpp_common

#endif    // ACE3X_FORMAT_READERS_VPP_COMMON_HPP_