/* SPDX-License-Identifier: GPLv3-or-later */

#include "Entry.hpp"

#include <QDebug>
#include <QFileInfo>

bool IsArchive(QString ext)
{
	if ((ext == "vpp") || (ext == "peg"))
		return true;
	return false;
}

Entry::Entry(QString name)
        : mName(name)
        , mFilename(QFileInfo(name).fileName())
        , mExtension(QFileInfo(name).suffix().toLower())
        , mIsArchive(IsArchive(mExtension))
{
}

Entry::Entry(QString name, std::uint16_t index, std::uint32_t size)
        : mName(name)
        , mFilename(QFileInfo(name).fileName())
        , mExtension(QFileInfo(name).suffix().toLower())
        , mIsArchive(IsArchive(mExtension))
        , mIndex(index)
        , mSize(size)
{}

Entry::~Entry() = default;

void Entry::removeAllChildren()
{
        mChildren.clear();
}

std::uint16_t Entry::getIndex() const
{
        return mIndex;
}

void Entry::addChild(Entry *entry)
{
        mChildren.push_back(std::unique_ptr<Entry>(entry));
        entry->mParent = this;
}

Entry *Entry::getChild(std::uint16_t index) const
{
        if (index >= mChildren.size())
                return nullptr;
        return mChildren[index].get();
}

QString Entry::getPath() const
{
        if (mParent) {
                if (!mParent->getPath().isEmpty())
                        return mParent->getPath() + '/' + mFilename;
	}

        return mName;
}

QString Entry::getFilename() const
{
        return mFilename;
}

QString Entry::getName() const
{
        return mName;
}

QString Entry::getExtension() const
{
        return mExtension;
}

std::uint16_t Entry::count() const
{
        return mChildren.size();
}

bool Entry::isArchive() const
{
        return mIsArchive;
}

Entry *Entry::getParent() const
{
        return mParent;
}

const std::uint8_t *Entry::getData() const
{
        return mData.data();
}

void Entry::read(std::vector<std::uint8_t> data, QTextEdit *log)
{
        mData = data;
        mSize = data.size();
}

qint64 Entry::getSize() const
{
        return mSize;
}
