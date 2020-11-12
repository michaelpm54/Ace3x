/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entry/tree-entry-sort-proxy.hpp"

#include "tree-entry/tree-entry.hpp"

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
            return (entryLeft->getFilename().toLower() > entryRight->getFilename().toLower());
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
