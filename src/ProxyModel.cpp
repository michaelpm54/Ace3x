/* SPDX-License-Identifier: GPLv3-or-later */

#include "ProxyModel.hpp"

#include "Entry.hpp"

SortProxy::SortProxy(QObject *parent)
        : QSortFilterProxyModel(parent)
{}

bool SortProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
        auto entryLeft  = static_cast<Entry *>(left.internalPointer());
        auto entryRight = static_cast<Entry *>(right.internalPointer());

        switch (left.column()) {
                case 0:
                        return entryLeft->getFilename() < entryRight->getFilename();
                case 1:
                        return entryLeft->getSize() < entryRight->getSize();
                default:
                        break;
        }

        return false;
}
