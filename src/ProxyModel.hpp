/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef PROXYMODEL_HPP
#define PROXYMODEL_HPP

#include <QSortFilterProxyModel>

class SortProxy : public QSortFilterProxyModel {
public:
    SortProxy(QObject *parent = nullptr);

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif    // PROXYMODEL_HPP
