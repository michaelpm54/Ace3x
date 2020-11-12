#ifndef ACE3X_FORMAT_READERS_VPP_V2_HPP_
#define ACE3X_FORMAT_READERS_VPP_V2_HPP_

#include "format-readers/vpp-archive.hpp"

struct FileInfo;

class VppV2 : public VppArchive {
public:
    void read(const std::vector<std::uint8_t> &data) override;
    const std::vector<FileInfo> &get_entries() const;
    bool is_compressed() const;

private:
    std::vector<FileInfo> entries_;
    bool compressed_;
};

#endif    // ACE3X_FORMAT_READERS_VPP_V2_HPP_
