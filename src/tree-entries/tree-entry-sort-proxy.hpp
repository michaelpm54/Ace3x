/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRY_TREE_ENTRY_SORT_PROXY_HPP_
#define ACE3X_TREE_ENTRY_TREE_ENTRY_SORT_PROXY_HPP_

#include <QSortFilterProxyModel>

class TreeEntrySortProxy : public QSortFilterProxyModel {
public:
    TreeEntrySortProxy(QObject *parent = nullptr);

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif    // ACE3X_TREE_ENTRY_TREE_ENTRY_SORT_PROXY_HPP_
