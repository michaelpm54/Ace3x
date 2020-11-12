/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entries/tree-entry.hpp"

#include <QDebug>
#include <QFileInfo>

TreeEntry::TreeEntry() = default;

TreeEntry::TreeEntry(const FileInfo &fileInfo)
    : file_info_(fileInfo)
{
}

TreeEntry::~TreeEntry() = default;

void TreeEntry::removeAllChildren()
{
    children_.clear();
}

int TreeEntry::getIndex() const
{
    return file_info_.index_in_parent;
}

void TreeEntry::addChild(TreeEntry *entry)
{
    children_.push_back(std::unique_ptr<TreeEntry>(entry));
    entry->parent_ = this;
}

TreeEntry *TreeEntry::getChild(std::uint16_t index) const
{
    if (index >= children_.size())
        return nullptr;
    return children_[index].get();
}

std::string TreeEntry::getPath() const
{
    if (parent_) {
        if (!parent_->getPath().empty())
            return parent_->getPath() + '/' + file_info_.file_name;
    }

    return file_info_.file_name;
}

std::string TreeEntry::getFilename() const
{
    return file_info_.file_name;
}

std::string TreeEntry::getName() const
{
    return file_info_.file_name;
}

std::string TreeEntry::getExtension() const
{
    return file_info_.extension;
}

TreeEntry *TreeEntry::getParent() const
{
    return parent_;
}

const std::uint8_t *TreeEntry::getData() const
{
    return file_info_.file_data.data();
}

std::uint64_t TreeEntry::getSize() const
{
    return file_info_.file_data.size();
}

int TreeEntry::get_num_children() const
{
    return static_cast<int>(children_.size());
}
