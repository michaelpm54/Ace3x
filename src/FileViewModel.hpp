/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef FILEVIEWMODEL_HPP
#define FILEVIEWMODEL_HPP

#include <QAbstractItemModel>
#include <memory>

class TreeEntry;

class FileViewModel : public QAbstractItemModel {
public:
    FileViewModel();
    ~FileViewModel();

    void addRoots(QList<TreeEntry *> list);
    void clear();
    TreeEntry *itemFromIndex(const QModelIndex &index) const;

    QModelIndex index(int row, int col, const QModelIndex &parent) const;

protected:
    QModelIndex parent(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool hasChildren(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &) const;
    bool canFetchMore(const QModelIndex &index) const;
    void fetchMore(const QModelIndex &index);

private:
    void populate(TreeEntry *parent, const TreeEntry &entry);
    void addChildren(TreeEntry *parent, TreeEntry *entry);

    std::unique_ptr<TreeEntry> mInvisibleRoot;
};

#endif    // FILEVIEWMODEL_HPP
