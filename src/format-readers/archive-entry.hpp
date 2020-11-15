#ifndef ACE3X_FORMAT_READERS_ARCHIVE_ENTRY_HPP_
#define ACE3X_FORMAT_READERS_ARCHIVE_ENTRY_HPP_

#include <string>

namespace ace3x {

struct ArchiveEntry {
    std::string filename;
    int index;
    int size;
    int offset;
};

}    // namespace ace3x

#endif    // ACE3X_FORMAT_READERS_ARCHIVE_ENTRY_HPP_
