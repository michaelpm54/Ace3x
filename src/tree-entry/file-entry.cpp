#include "tree-entry/file-entry.hpp"

void FileEntry::read(std::vector<std::uint8_t> data, QTextEdit *log)
{
    mData = data;
    mSize = data.size();
}

bool FileEntry::is_archive() const
{
    return false;
}
