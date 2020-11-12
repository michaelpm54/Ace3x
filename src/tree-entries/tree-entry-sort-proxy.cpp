/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entries/tree-entry-sort-proxy.hpp"

#include "tree-entries/tree-entry.hpp"

TreeEntrySortProxy::TreeEntrySortProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool TreeEntrySortProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    auto entryLeft = static_cast<TreeEntry *>(left.internalPointer());
    auto entryRight = static_cast<TreeEntry *>(right.internalPointer());

    if (!entryLeft || !entryRight) {
        return true;
    }

    switch (left.column()) {
        case 0: {
            return (QString::fromStdString(entryLeft->getFilename()).toLower() > QString::fromStdString(entryRight->getFilename()).toLower());
        }
        case 1: {
            return (entryLeft->getSize() < entryRight->getSize());
        }
        default: {
            break;
        }
    }

    return (false);
}
