#ifndef ACE3X_FORMAT_READERS_VPP_ARCHIVE_HPP_
#define ACE3X_FORMAT_READERS_VPP_ARCHIVE_HPP_

#include <cstdint>
#include <vector>

class VppArchive {
public:
    ~VppArchive() = default;
    virtual void read(const std::vector<std::uint8_t> &data) = 0;
};

#endif    // ACE3X_FORMAT_READERS_VPP_ARCHIVE_HPP_
