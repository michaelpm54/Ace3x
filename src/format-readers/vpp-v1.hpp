/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMAT_READERS_VPP_V1_HPP_
#define ACE3X_FORMAT_READERS_VPP_V1_HPP_

#include <vector>

#include "format-readers/vpp-archive.hpp"

struct FileInfo;

class VppV1 : public VppArchive {
public:
    void read(const FileInfo &info) override;
    const std::vector<FileInfo> &get_entries() const;

private:
    std::vector<FileInfo> entries_;
};

#endif    // ACE3X_FORMAT_READERS_VPP_V1_HPP_
