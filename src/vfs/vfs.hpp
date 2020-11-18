#ifndef ACE3X_VFS_VFS_HPP_
#define ACE3X_VFS_VFS_HPP_

#include <string>

struct VfsEntry;

class Vfs {
public:
    ~Vfs() = default;

    virtual bool add_root_archive(const std::string& path) = 0;
    virtual VfsEntry* get_entry(const std::string& absolute_path) = 0;
    virtual void clear() = 0;
};

#endif    // ACE3X_VFS_VFS_HPP_
