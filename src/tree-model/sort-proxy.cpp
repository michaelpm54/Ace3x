/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-model/sort-proxy.hpp"

#include "vfs/vfs-entry.hpp"

TreeEntrySortProxy::TreeEntrySortProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool TreeEntrySortProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    auto entryLeft = static_cast<VfsEntry *>(left.internalPointer());
    auto entryRight = static_cast<VfsEntry *>(right.internalPointer());

    if (!entryLeft || !entryRight) {
        return true;
    }

    switch (left.column()) {
        case 0: {
            return (QString::fromStdString(entryLeft->name).toLower() > QString::fromStdString(entryRight->name).toLower());
        }
        case 1: {
            return (entryLeft->size < entryRight->size);
        }
        default: {
            break;
        }
    }

    return false;
}
