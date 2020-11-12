/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entry/tree-entry.hpp"

#include <QDebug>
#include <QFileInfo>

TreeEntry::TreeEntry(QString name)
    : mName(name)
    , mFilename(QFileInfo(name).fileName())
    , mExtension(QFileInfo(name).suffix().toLower())
{
}

TreeEntry::TreeEntry(QString name, std::uint16_t index, std::uint32_t size)
    : mName(name)
    , mFilename(QFileInfo(name).fileName())
    , mExtension(QFileInfo(name).suffix().toLower())
    , mIndex(index)
    , mSize(size)
{
}

TreeEntry::~TreeEntry() = default;

void TreeEntry::removeAllChildren()
{
    mChildren.clear();
}

std::uint16_t TreeEntry::getIndex() const
{
    return mIndex;
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

QString TreeEntry::getPath() const
{
    if (mParent) {
        if (!mParent->getPath().isEmpty())
            return mParent->getPath() + '/' + mFilename;
    }

    return mName;
}

QString TreeEntry::getFilename() const
{
    return mFilename;
}

QString TreeEntry::getName() const
{
    return mName;
}

QString TreeEntry::getExtension() const
{
    return mExtension;
}

TreeEntry *TreeEntry::getParent() const
{
    return mParent;
}

const std::uint8_t *TreeEntry::getData() const
{
    return mData.data();
}

void TreeEntry::read(std::vector<std::uint8_t> data, QTextEdit *log)
{
    mData = data;
    mSize = data.size();
}

qint64 TreeEntry::getSize() const
{
    return mSize;
}

unsigned int TreeEntry::get_num_children() const
{
    return mChildren.size();
}
