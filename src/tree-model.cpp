/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-model.hpp"

#include <QDebug>

#include "tree-entries/tree-entry.hpp"

TreeModel::TreeModel()
    : mInvisibleRoot(new TreeEntry())
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
    if (!hasIndex(row, col, parent))
        return QModelIndex();

    TreeEntry *parent_entry;

    if (!parent.isValid())
        parent_entry = mInvisibleRoot.get();
    else
        parent_entry = static_cast<TreeEntry *>(parent.internalPointer());

    // Create a new index for this item
    TreeEntry *childItem = parent_entry->getChild(row);
    if (childItem)
        return createIndex(row, col, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeEntry *child_entry = static_cast<TreeEntry *>(index.internalPointer());
    TreeEntry *parent_entry = child_entry->getParent();

    // Return an invalid index for the root as it has no parent
    if (parent_entry == mInvisibleRoot.get())
        return QModelIndex();

    // Create the parent index
    return createIndex(parent_entry->getIndex(), 0, parent_entry);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeEntry *item = itemFromIndex(index);

    if (role == Qt::DisplayRole)
        switch (index.column()) {
            case 0:
                return QString::fromStdString(item->getFilename());
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

    const auto *item = itemFromIndex(index);
    return item->get_num_children() != 0;
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
    if (parent.column() > 1) {
        return 0;
    }

    TreeEntry *parent_entry;
    if (!parent.isValid())
        parent_entry = mInvisibleRoot.get();
    else
        parent_entry = static_cast<TreeEntry *>(parent.internalPointer());

    return parent_entry->get_num_children();
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
    if (!index.isValid())
        return false;

    return itemFromIndex(index)->get_num_children() > 0;
}

void TreeModel::fetchMore(const QModelIndex &)
{
    // FIXME
    return;
}

void TreeModel::addTopLevelEntry(TreeEntry *entry)
{
    beginInsertRows(QModelIndex(), mInvisibleRoot->get_num_children(), mInvisibleRoot->get_num_children());

    mInvisibleRoot->addChild(entry);

    endInsertRows();
}
