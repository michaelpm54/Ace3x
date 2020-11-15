#ifndef ACE3X_VFS_MMAP_VFS_HPP_
#define ACE3X_VFS_MMAP_VFS_HPP_

#include <deque>
#include <filesystem>
#include <map>
#include <unordered_map>

#include "format-readers/vpp.hpp"
#include "vfs/mio.hpp"
#include "vfs/vfs-entry.hpp"
#include "vfs/vfs.hpp"

class MmapVfs : public Vfs {
private:
    struct VppFile {
        mio::mmap_source mmap;
        VfsEntry* entry;
        ace3x::vpp::VppInfo info;
    };

public:
    bool add_root_archive(const std::string& path) override;
    VfsEntry* get_entry(const std::string& absolute_path) override;

private:
    VfsEntry* add_entry(const VfsEntry& entry);
    bool remap_as_decompressed_vpp(mio::mmap_source& mmap, ace3x::vpp::VppInfo& info);
    bool map_file(mio::mmap_source& mmap, const std::filesystem::path& path);

private:
    std::unordered_map<std::string, VppFile> loaded_vpps_;
    std::deque<VfsEntry> entries_;
};

#endif    // ACE3X_VFS_MMAP_VFS_HPP_
