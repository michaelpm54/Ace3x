/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-model.hpp"

#include <QDebug>

#include "tree-entry/tree-entry.hpp"

TreeModel::TreeModel()
    : mInvisibleRoot(new TreeEntry(""))
{
    clear();
}

TreeModel::~TreeModel() = default;

TreeEntry *TreeModel::itemFromIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return mInvisibleRoot.get();

    return static_cast<TreeEntry *>(index.internalPointer());
}

QModelIndex TreeModel::index(int row, int col, const QModelIndex &parent) const
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

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    auto parent = itemFromIndex(index)->getParent();

    if (parent == mInvisibleRoot.get())
        return QModelIndex();

    return createIndex(parent->getIndex(), 0, parent);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
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

bool TreeModel::hasChildren(const QModelIndex &index) const
{
    if (!index.isValid())
        return true;

    const auto item = itemFromIndex(index);
    return item->isArchive() && item->count() != 0;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int TreeModel::rowCount(const QModelIndex &parent) const
{
    return itemFromIndex(parent)->count();
}

int TreeModel::columnCount(const QModelIndex &) const
{
    return 2;
}

void TreeModel::clear()
{
    beginResetModel();

    mInvisibleRoot->removeAllChildren();

    endResetModel();
}

bool TreeModel::canFetchMore(const QModelIndex &index) const
{
    return itemFromIndex(index)->isArchive();
}

void TreeModel::fetchMore(const QModelIndex &index)
{
    if (!index.isValid())
        return;
}

void TreeModel::addRoots(QList<TreeEntry *> list)
{
    clear();

    beginInsertRows(QModelIndex(), 0, list.size() - 1);
    for (auto &e : list)
        mInvisibleRoot->addChild(e);
    endInsertRows();
}
