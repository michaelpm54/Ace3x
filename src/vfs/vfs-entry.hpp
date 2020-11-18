#ifndef ACE3X_VFS_VFS_ENTRY_HPP_
#define ACE3X_VFS_VFS_ENTRY_HPP_

#include <string>
#include <vector>

struct VfsEntry {
    int index;
    std::uintmax_t size;
    std::size_t offset_in_parent;
    std::size_t offset_in_root;
    std::string name;
    std::string absolute_path;
    std::string relative_path;
    std::string extension;
    std::vector<VfsEntry*> entries;
    VfsEntry* root;
    VfsEntry* parent;
    const unsigned char* data;
};

#endif    // ACE3X_VFS_VFS_ENTRY_HPP_
