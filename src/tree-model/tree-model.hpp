/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_MODEL_TREE_MODEL_HPP_
#define ACE3X_TREE_MODEL_TREE_MODEL_HPP_

#include <QAbstractItemModel>
#include <vector>

class Vfs;
struct VfsEntry;

using TreeRoot = std::vector<VfsEntry *>;

class TreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    TreeModel(Vfs *vfs);
    ~TreeModel();

    void addTopLevelEntry(VfsEntry *entry);
    void clear();
    VfsEntry *itemFromIndex(const QModelIndex &index) const;

    /* Returns the number of VPP's loaded */
    int load(const QString &path, Vfs *vfs);

protected:
    QModelIndex parent(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool hasChildren(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &) const;
    QModelIndex index(int row, int col, const QModelIndex &parent) const;

private:
    TreeRoot invisible_root_;
    Vfs *vfs_;
};

#endif    // ACE3X_TREE_MODEL_TREE_MODEL_HPP_
