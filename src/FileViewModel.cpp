/* SPDX-License-Identifier: GPLv3-or-later */

#include "FileViewModel.hpp"

#include <QDebug>

#include "tree-entry/tree-entry.hpp"

FileViewModel::FileViewModel()
    : mInvisibleRoot(new TreeEntry(""))
{
    clear();
}

FileViewModel::~FileViewModel() = default;

TreeEntry *FileViewModel::itemFromIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return mInvisibleRoot.get();

    return static_cast<TreeEntry *>(index.internalPointer());
}

QModelIndex FileViewModel::index(int row, int col, const QModelIndex &parent) const
{
    TreeEntry *parentItem = itemFromIndex(parent);
    if (!parentItem)
        return QModelIndex();

    if (row >= static_cast<int>(parentItem->count()))
        return QModelIndex();

    TreeEntry *item = parentItem->getChild(row);

    if (!item)
        return QModelIndex();

    return createIndex(row, col, item);
}

QModelIndex FileViewModel::parent(const QModelIndex &index) const
{
    auto parent = itemFromIndex(index)->getParent();

    if (parent == mInvisibleRoot.get())
        return QModelIndex();

    return createIndex(parent->getIndex(), 0, parent);
}

QVariant FileViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeEntry *item = itemFromIndex(index);

    if (role == Qt::DisplayRole)
        switch (index.column()) {
            case 0:
                return item->getFilename();
            case 1:
                return QLocale::system().formattedDataSize(item->getSize(), 2, nullptr);
            default:
                break;
        }

    return QVariant();
}

bool FileViewModel::hasChildren(const QModelIndex &index) const
{
    if (!index.isValid())
        return true;

    const auto item = itemFromIndex(index);
    return item->isArchive() && item->count() != 0;
}

QVariant FileViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
        switch (section) {
            case 0:
                return "Filename";
            case 1:
                return "Size";
            default:
                break;
        }

    return QVariant();
}

int FileViewModel::rowCount(const QModelIndex &parent) const
{
    return itemFromIndex(parent)->count();
}

int FileViewModel::columnCount(const QModelIndex &) const
{
    return 2;
}

void FileViewModel::clear()
{
    beginResetModel();

    mInvisibleRoot->removeAllChildren();

    endResetModel();
}

bool FileViewModel::canFetchMore(const QModelIndex &index) const
{
    return itemFromIndex(index)->isArchive();
}

void FileViewModel::fetchMore(const QModelIndex &index)
{
    if (!index.isValid())
        return;
}

void FileViewModel::addRoots(QList<TreeEntry *> list)
{
    clear();

    beginInsertRows(QModelIndex(), 0, list.size() - 1);
    for (auto &e : list)
        mInvisibleRoot->addChild(e);
    endInsertRows();
}
