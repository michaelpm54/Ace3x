/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entries/tree-entry.hpp"

#include <QDebug>
#include <QFileInfo>

TreeEntry::TreeEntry() = default;

TreeEntry::TreeEntry(const FileInfo &fileInfo)
    : mFileInfo(fileInfo)
{
}

TreeEntry::~TreeEntry() = default;

void TreeEntry::removeAllChildren()
{
    mChildren.clear();
}

int TreeEntry::getIndex() const
{
    return mFileInfo.index_in_parent;
}

void TreeEntry::addChild(TreeEntry *entry)
{
    mChildren.push_back(std::unique_ptr<TreeEntry>(entry));
    entry->mParent = this;
}

TreeEntry *TreeEntry::getChild(std::uint16_t index) const
{
    if (index >= mChildren.size())
        return nullptr;
    return mChildren[index].get();
}

std::string TreeEntry::getPath() const
{
    if (mParent) {
        if (!mParent->getPath().empty())
            return mParent->getPath() + '/' + mFileInfo.file_name;
    }

    return mFileInfo.file_name;
}

std::string TreeEntry::getFilename() const
{
    return mFileInfo.file_name;
}

std::string TreeEntry::getName() const
{
    return mFileInfo.file_name;
}

std::string TreeEntry::getExtension() const
{
    return mFileInfo.extension;
}

TreeEntry *TreeEntry::getParent() const
{
    return mParent;
}

const std::uint8_t *TreeEntry::getData() const
{
    return mFileInfo.file_data.data();
}

std::uint64_t TreeEntry::getSize() const
{
    return mFileInfo.file_data.size();
}

unsigned int TreeEntry::get_num_children() const
{
    return mChildren.size();
}
