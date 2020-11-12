/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_MODEL_HPP_
#define ACE3X_TREE_MODEL_HPP_

#include <QAbstractItemModel>
#include <memory>

struct FileInfo;
class TreeEntry;

class TreeModel : public QAbstractItemModel {
public:
    TreeModel();
    ~TreeModel();

    void addTopLevelEntry(TreeEntry *entry);
    void clear();
    TreeEntry *itemFromIndex(const QModelIndex &index) const;

    /* Returns the number of VPP's loaded */
    int load(const QString &path);
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
    static std::vector<FileInfo> TreeModel::gather_level_vpps_in_dir(const std::string &dir, bool load_data);

    std::unique_ptr<TreeEntry> mInvisibleRoot;
};

#endif    // ACE3X_TREE_MODEL_HPP_
