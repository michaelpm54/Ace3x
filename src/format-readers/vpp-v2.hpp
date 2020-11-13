/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMAT_READERS_VPP_V2_HPP_
#define ACE3X_FORMAT_READERS_VPP_V2_HPP_

#include <vector>

#include "format-readers/vpp-archive.hpp"
#include "mio.hpp"

struct FileInfo;

class VppV2 : public VppArchive {
public:
    void read(const FileInfo &info) override;
    const std::vector<FileInfo> &get_entries() const;
    bool is_compressed() const;

private:
    mio::shared_mmap_source decompressed_mmap_;
    std::vector<FileInfo> entries_;
    bool compressed_;
};

#endif    // ACE3X_FORMAT_READERS_VPP_V2_HPP_
